cmake_minimum_required(VERSION 4.0)
project(2026-jaros)

set(CMAKE_CXX_STANDARD 23)

add_library(ExpandedPolicy expandedPolicy.cpp)
target_include_directories(ExpandedPolicy PUBLIC "${PROJECT_SOURCE_DIR}")
