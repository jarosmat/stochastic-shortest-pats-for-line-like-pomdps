cmake_minimum_required(VERSION 4.0)
project(2026-jaros)

set(CMAKE_CXX_STANDARD 23)
set(PERIODIC_POL_DIR "${PROJECT_SOURCE_DIR}/periodic-policy")

add_subdirectory("${PERIODIC_POL_DIR}")

