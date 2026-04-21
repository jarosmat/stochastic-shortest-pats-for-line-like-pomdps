cmake_minimum_required(VERSION 4.0)
project(2026-jaros)

set(CMAKE_CXX_STANDARD 23)

add_library(PeriodicPolicy periodicPolicy.cpp)
target_include_directories(PeriodicPolicy PUBLIC "${PROJECT_SOURCE_DIR}")
