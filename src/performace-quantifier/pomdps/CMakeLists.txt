cmake_minimum_required(VERSION 4.0)
project(2026-jaros)

set(CMAKE_CXX_STANDARD 23)

add_library(LinePOMDP lineLikePomdp.cpp)
target_link_libraries(LinePOMDP PUBLIC Prob Belief)
target_include_directories(LinePOMDP PUBLIC "${PROB_DIR}")

