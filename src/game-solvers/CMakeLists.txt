cmake_minimum_required(VERSION 4.0)
project(2026-jaros)

set(CMAKE_CXX_STANDARD 23)

add_library(MaxProbTTriesSolver max_prob_t_tries_solver.cpp)
target_include_directories(MaxProbTTriesSolver PUBLIC "${LINE_LIKE_POMDP_DIR}" "${PROB_DIR}" "${BELIEF_DIR}" "${QUANTIFIER_DIRECTORY}" "${EXPANDED_POL_DIR}")
target_link_libraries(MaxProbTTriesSolver PUBLIC Prob LinePOMDP Belief Quantifier ExpandedPolicy)

add_library(MinMovesBeforeThresholdSolver min_moves_before_threshold_solver.cpp)
target_include_directories(MinMovesBeforeThresholdSolver PUBLIC "${LINE_LIKE_POMDP_DIR}" "${PROB_DIR}" "${BELIEF_DIR}" "${PERIODIC_POL_DIR}")
target_link_libraries(MinMovesBeforeThresholdSolver PUBLIC Prob LinePOMDP Belief PeriodicPolicy)
