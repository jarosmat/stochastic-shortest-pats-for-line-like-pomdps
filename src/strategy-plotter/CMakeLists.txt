cmake_minimum_required(VERSION 4.0)
project(2026-jaros)

set(CMAKE_CXX_STANDARD 23)

add_executable(plotter plotter.cpp)
target_link_libraries(plotter PUBLIC LinePOMDP Quantifier PeriodicPolicy)