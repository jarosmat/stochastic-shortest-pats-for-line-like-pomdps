cmake_minimum_required(VERSION 4.0)
project(2026-jaros)

set(CMAKE_CXX_STANDARD 23)

add_library(plotting plotting.cpp)
target_link_libraries(plotting PUBLIC LinePOMDP Quantifier PeriodicPolicy)

add_executable(plotter plotter.cpp)
target_link_libraries(plotter PUBLIC plotting)

add_executable(plot_table plot_table.cpp)
target_link_libraries(plot_table PUBLIC plotting)