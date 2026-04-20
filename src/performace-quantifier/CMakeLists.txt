cmake_minimum_required(VERSION 4.0)
project(2026-jaros)

set(CMAKE_CXX_STANDARD 23)
set(POMDP_DIR "${PROJECT_SOURCE_DIR}/pomdps")
set(PROB_DIR "${PROJECT_SOURCE_DIR}/probability")
set(BELIEF_DIR "${PROJECT_SOURCE_DIR}/belief")


add_subdirectory("${POMDP_DIR}")
add_subdirectory("${PROB_DIR}")
add_subdirectory("${BELIEF_DIR}")


add_library(Quantifier quantifier.cpp)
target_include_directories(Quantifier PUBLIC "${POMDP_DIR}" "${PROB_DIR}" "${BELIEF_DIR}")
target_link_libraries(Quantifier PUBLIC Prob LinePOMDP Belief)