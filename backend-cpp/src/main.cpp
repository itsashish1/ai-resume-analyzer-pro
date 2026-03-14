#include <algorithm>
#include <cctype>
#include <iostream>
#include <regex>
#include <set>
#include <sstream>
#include <string>
#include <vector>
struct AnalysisResult {
  double score;
  std::vector<std::string> matched;
  std::vector<std::string> missing;
  std::vector<std::string> tips;
};
static std::string lower(const std::string& s) {
  std::string out = s;
  std::transform(out.begin(), out.end(), out.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
  return out;
}
static std::set<std::string> tokenize(const std::string& text) {
  static const std::set<std::string> stopwords = {
    "the", "is", "a", "an", "for", "and", "or", "to", "of", "in", "on", "with"
  };
  std::set<std::string> tokens;
  std::regex re("[A-Za-z+#.]+\");
  auto begin = std::sregex_iterator(text.begin(), text.end(), re);
  auto end = std::sregex_iterator();
  for (auto it = begin; it != end; ++it) {
    std::string w = lower(it->str());
    if (w.size() > 1 && !stopwords.count(w)) {
      tokens.insert(w);
    }
  }
  return tokens;
}
AnalysisResult analyzeResume(const std::string& resumeText, const std::string& jobDescription) {
  auto resumeTokens = tokenize(resumeText);
  auto jdTokens = tokenize(jobDescription);
  AnalysisResult result{};
  if (jdTokens.empty()) {
    result.score = 0.0;
    result.tips.push_back("Job description is empty.");
    return result;
  }
  for (const auto& t : jdTokens) {
    if (resumeTokens.count(t)) result.matched.push_back(t);
    else result.missing.push_back(t);
  }
  result.score = (static_cast<double>(result.matched.size()) / static_cast<double>(jdTokens.size())) * 100.0;
  std::string resumeLower = lower(resumeText);
  if (result.score < 50.0) result.tips.push_back("Add quantifiable project outcomes and core skills from JD.");
  if (resumeLower.find("project") == std::string::npos) result.tips.push_back("Include at least 2 detailed projects with tech stack and impact.");
  if (resumeLower.find("intern") == std::string::npos) result.tips.push_back("Add internship or practical experience section.");
  if (result.tips.empty()) result.tips.push_back("Strong alignment. Keep resume concise and impact-focused.");
  return result;
}
int main() {
  std::cout << "AI Resume Analyzer (C++)\n";
  std::cout << "Paste resume text (single line):\n";
  std::string resume;
  std::getline(std::cin, resume);
  std::cout << "Paste job description text (single line):\n";
  std::string jd;
  std::getline(std::cin, jd);
  AnalysisResult r = analyzeResume(resume, jd);
  std::cout << "\nScore: " << r.score << "%\n";
  std::cout << "Matched Skills: ";
  for (const auto& m : r.matched) std::cout << m << " ";
  std::cout << "\nMissing Skills: ";
  for (const auto& m : r.missing) std::cout << m << " ";
  std::cout << "\nTips:\n";
  for (const auto& t : r.tips) std::cout << "- " << t << "\n";
  return 0;
}
