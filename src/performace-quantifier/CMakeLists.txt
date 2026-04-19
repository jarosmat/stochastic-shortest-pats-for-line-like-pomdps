cmake_minimum_required(VERSION 4.0)
project(2026-jaros)

set(CMAKE_CXX_STANDARD 23)
set(POMDP_DIR "${PROJECT_SOURCE_DIR}/pomdps")

add_subdirectory("${POMDP_DIR}")
