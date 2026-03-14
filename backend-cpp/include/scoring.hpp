#pragma once
#include <string>
#include <vector>
struct AnalysisResult {
  double score;
  std::vector<std::string> matched;
  std::vector<std::string> missing;
  std::vector<std::string> tips;
};
AnalysisResult analyzeResume(const std::string& resumeText, const std::string& jobDescription);
