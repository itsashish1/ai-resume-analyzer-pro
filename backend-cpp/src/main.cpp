#include <algorithm>
#include <cctype>
#include <iostream>
#include <regex>
#include <set>
#include <sstream>
#include <string>
#include <vector>
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#endif
struct AnalysisResult {
  double score;
  std::vector<std::string> matched;
  std::vector<std::string> missing;
  std::vector<std::string> tips;
};
static std::string toLower(std::string s) {
  std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return static_cast<char>(std::tolower(c)); });
  return s;
}
static std::set<std::string> tokenize(const std::string& text) {
  static const std::set<std::string> stopwords = {
    "the","is","a","an","for","and","or","to","of","in","on","with"
  };
  std::set<std::string> tokens;
  std::regex re("[A-Za-z+#.]+$");
  std::string word;
  for (char ch : text) {
    if (std::isalnum(static_cast<unsigned char>(ch)) || ch == '+' || ch == '#' || ch == '.') {
      word.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(ch))));
    } else if (!word.empty()) {
      if (word.size() > 1 && !stopwords.count(word)) tokens.insert(word);
      word.clear();
    }
  }
  if (!word.empty() && word.size() > 1 && !stopwords.count(word)) tokens.insert(word);
  return tokens;
}
static AnalysisResult analyzeResume(const std::string& resumeText, const std::string& jobDescription) {
  auto resumeTokens = tokenize(resumeText);
  auto jdTokens = tokenize(jobDescription);
  AnalysisResult result{};
  if (jdTokens.empty()) {
    result.score = 0;
    result.tips.push_back("Job description is empty.");
    return result;
  }
  for (const auto& t : jdTokens) {
    if (resumeTokens.count(t)) result.matched.push_back(t);
    else result.missing.push_back(t);
  }
  result.score = (static_cast<double>(result.matched.size()) / static_cast<double>(jdTokens.size())) * 100.0;
  std::string lowerResume = toLower(resumeText);
  if (result.score < 50.0) result.tips.push_back("Add quantifiable project outcomes and skills from the job description.");
  if (lowerResume.find("project") == std::string::npos) result.tips.push_back("Add at least 2 projects with measurable impact.");
  if (lowerResume.find("intern") == std::string::npos) result.tips.push_back("Include internship or practical industry experience.");
  if (result.tips.empty()) result.tips.push_back("Strong alignment. Keep concise and impact-focused bullets.");
  return result;
}
static std::string escapeJson(const std::string& s) {
  std::string out;
  for (char c : s) {
    if (c == '"') out += "\\\"";
    else if (c == '\\') out += "\\\\";
    else if (c == '\n') out += "\\n";
    else out += c;
  }
  return out;
}
static std::string jsonArray(const std::vector<std::string>& arr) {
  std::ostringstream os;
  os << "[";
  for (size_t i = 0; i < arr.size(); ++i) {
    os << "\"" << escapeJson(arr[i]) << "\"";
    if (i + 1 < arr.size()) os << ",";
  }
  os << "]";
  return os.str();
}
static std::string extractJsonString(const std::string& body, const std::string& key) {
  std::string pattern = "\"" + key + "\"\\s*:\\s*\"([\\s\\S]*?)\"";
  std::regex re(pattern);
  std::smatch m;
  if (std::regex_search(body, m, re) && m.size() > 1) {
    std::string value = m[1].str();
    std::string out;
    for (size_t i = 0; i < value.size(); ++i) {
      if (value[i] == '\\' && i + 1 < value.size()) {
        char n = value[i + 1];
        if (n == 'n') { out.push_back('\n'); ++i; continue; }
        out.push_back(n); ++i; continue;
      }
      out.push_back(value[i]);
    }
    return out;
  }
  return "";
}
static std::string response(const std::string& status, const std::string& body) {
  std::ostringstream os;
  os << "HTTP/1.1 " << status << "\r\n"
     << "Content-Type: application/json\r\n"
     << "Access-Control-Allow-Origin: *\r\n"
     << "Access-Control-Allow-Methods: GET,POST,OPTIONS\r\n"
     << "Access-Control-Allow-Headers: Content-Type\r\n"
     << "Content-Length: " << body.size() << "\r\n\r\n"
     << body;
  return os.str();
}
int main() {
#ifdef _WIN32
  WSADATA wsa;
  if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
    std::cerr << "WSAStartup failed\n";
    return 1;
  }
#endif
  int server_fd = static_cast<int>(socket(AF_INET, SOCK_STREAM, 0));
  if (server_fd < 0) {
    std::cerr << "Socket creation failed\n";
    return 1;
  }
  sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(8081);
  int opt = 1;
#ifdef _WIN32
  setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&opt), sizeof(opt));
#else
  setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
#endif
  if (bind(server_fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
    std::cerr << "Bind failed\n";
    return 1;
  }
  if (listen(server_fd, 10) < 0) {
    std::cerr << "Listen failed\n";
    return 1;
  }
  std::cout << "Resume Analyzer API running on http://localhost:8081\n";
  while (true) {
    sockaddr_in client_addr{};
#ifdef _WIN32
    int client_len = sizeof(client_addr);
#else
    socklen_t client_len = sizeof(client_addr);
#endif
    int client = static_cast<int>(accept(server_fd, reinterpret_cast<sockaddr*>(&client_addr), &client_len));
    if (client < 0) continue;
    char buffer[16384] = {0};
#ifdef _WIN32
    int bytes = recv(client, buffer, sizeof(buffer) - 1, 0);
#else
    int bytes = static_cast<int>(read(client, buffer, sizeof(buffer) - 1));
#endif
    if (bytes <= 0) {
#ifdef _WIN32
      closesocket(client);
#else
      close(client);
#endif
      continue;
    }
    std::string req(buffer, bytes);
    std::string res;
    if (req.rfind("OPTIONS ", 0) == 0) {
      res = response("200 OK", "{}");
    } else if (req.rfind("GET /health", 0) == 0) {
      res = response("200 OK", "{\"ok\":true,\"service\":\"resume-analyzer-cpp\"}");
    } else if (req.rfind("POST /analyze", 0) == 0) {
      size_t bodyPos = req.find("\r\n\r\n");
      std::string body = (bodyPos == std::string::npos) ? "" : req.substr(bodyPos + 4);
      std::string resume = extractJsonString(body, "resume_text");
      std::string jd = extractJsonString(body, "job_description");
      AnalysisResult a = analyzeResume(resume, jd);
      std::ostringstream out;
      out << "{"
          << "\"score\":" << a.score << ","
          << "\"matched\":" << jsonArray(a.matched) << ","
          << "\"missing\":" << jsonArray(a.missing) << ","
          << "\"tips\":" << jsonArray(a.tips)
          << "}";
      res = response("200 OK", out.str());
    } else {
      res = response("404 Not Found", "{\"message\":\"not found\"}");
    }
#ifdef _WIN32
    send(client, res.c_str(), static_cast<int>(res.size()), 0);
    closesocket(client);
#else
    write(client, res.c_str(), res.size());
    close(client);
#endif
  }
#ifdef _WIN32
  WSACleanup();
#endif
  return 0;
}
