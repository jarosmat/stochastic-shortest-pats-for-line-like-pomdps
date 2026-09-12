cmake_minimum_required(VERSION 4.0)
project(2026-jaros)

set(CMAKE_CXX_STANDARD 23)
set(TEST_DIRECTORY "${PROJECT_SOURCE_DIR}/Tests")
set(PARSER_DIRECTORY "${PROJECT_SOURCE_DIR}/pomdp-parser")
set(QUANTIFIER_DIRECTORY "${PROJECT_SOURCE_DIR}/performace-quantifier")
set(POLICIES_DIR "${PROJECT_SOURCE_DIR}/policies")
set(PLOTTER_DIR "${PROJECT_SOURCE_DIR}/strategy-plotter")
set(PROB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/performace-quantifier/probability")
set(BELIEF_DIR "${CMAKE_CURRENT_SOURCE_DIR}/performace-quantifier/belief")
set(LINE_LIKE_POMDP_DIR "${CMAKE_CURRENT_SOURCE_DIR}/performace-quantifier/pomdps")
set(GAME_SOLVERS_DIR "${PROJECT_SOURCE_DIR}/game-solvers")
set(PERIODIC_POL_DIR "${POLICIES_DIR}/periodic-policy")
set(EXPANDED_POL_DIR "${POLICIES_DIR}/expanded-policy")

include(FetchContent)
FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.4.0
)
FetchContent_MakeAvailable(Catch2)

find_package(Python3 REQUIRED COMPONENTS Interpreter Development)

include(FetchContent)
FetchContent_Declare(
        matplotlib-cpp
        GIT_REPOSITORY https://github.com/lava/matplotlib-cpp.git
        GIT_TAG master
)
FetchContent_MakeAvailable(matplotlib-cpp)

find_package(pybind11 REQUIRED)

execute_process(
        COMMAND ${Python3_EXECUTABLE} -c "import numpy; print(numpy.get_include())"
        OUTPUT_VARIABLE NUMPY_INCLUDE_DIR
        OUTPUT_STRIP_TRAILING_WHITESPACE
)
if (NOT NUMPY_INCLUDE_DIR)
    message(FATAL_ERROR "NumPy headers not found. Install numpy: pip install numpy")
endif ()

pybind11_add_module(pypomdp python_bindings.cpp
        "${PROB_DIR}/prob.cpp"
        "${BELIEF_DIR}/belief.cpp"
        "${LINE_LIKE_POMDP_DIR}/lineLikePomdp.cpp"
)
target_link_libraries(pypomdp PRIVATE
        Python3::Python
        Prob
        Belief
        LinePOMDP
        MaxProbTTriesSolver
        MinMovesBeforeThresholdSolver
        PeriodicPolicy
        ExpandedPolicy
)
target_include_directories(pypomdp PRIVATE
        ${NUMPY_INCLUDE_DIR}
        ${matplotlib-cpp_SOURCE_DIR}
        ${PROB_DIR}
        ${BELIEF_DIR}
        ${LINE_LIKE_POMDP_DIR}
        ${GAME_SOLVERS_DIR}
        ${PERIODIC_POL_DIR}
        ${EXPANDED_POL_DIR}
)

add_subdirectory("${TEST_DIRECTORY}")
add_subdirectory("${PARSER_DIRECTORY}")
add_subdirectory("${QUANTIFIER_DIRECTORY}")
add_subdirectory("${POLICIES_DIR}")
add_subdirectory("${GAME_SOLVERS_DIR}")
add_subdirectory("${PLOTTER_DIR}")