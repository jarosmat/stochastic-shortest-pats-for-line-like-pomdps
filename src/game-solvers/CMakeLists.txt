cmake_minimum_required(VERSION 4.0)
project(2026-jaros)

set(CMAKE_CXX_STANDARD 23)

add_library(MaxProbTTriesSolver max_prob_t_tries_solver.cpp)
target_include_directories(MaxProbTTriesSolver PUBLIC "${POMDP_DIR}" "${PROB_DIR}" "${BELIEF_DIR}" "${PROJECT_SOURCE_DIR}" "${QUANTIFIER_DIRECTORY}")
target_link_libraries(MaxProbTTriesSolver PUBLIC Prob LinePOMDP Belief Quantifier)