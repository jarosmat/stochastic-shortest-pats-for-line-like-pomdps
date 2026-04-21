cmake_minimum_required(VERSION 4.0)
project(2026-jaros)

set(CMAKE_CXX_STANDARD 23)

include(CTest)

add_executable(QuantifierTest quantifierTest.cpp)
target_link_libraries(QuantifierTest Catch2::Catch2WithMain Quantifier Prob LinePOMDP Belief PeriodicPolicy)
add_test(NAME QuantifierTest COMMAND QuantifierTest)


include(Catch)
catch_discover_tests(QuantifierTest)

