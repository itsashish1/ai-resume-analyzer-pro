cmake_minimum_required(VERSION 3.15)
project(resume_analyzer_cpp)
set(CMAKE_CXX_STANDARD 17)
add_executable(resume_analyzer
  src/main.cpp
)
target_include_directories(resume_analyzer PRIVATE include)
