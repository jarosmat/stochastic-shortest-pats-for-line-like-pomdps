cmake_minimum_required(VERSION 4.0)
project(2026-jaros)

set(CMAKE_CXX_STANDARD 23)
set(TEST_DIRECTORY "${PROJECT_SOURCE_DIR}/Tests")
set(PARSER_DIRECTORY "${PROJECT_SOURCE_DIR}/pomdp-parser")
set(QUANTIFIER_DIRECTORY "${PROJECT_SOURCE_DIR}/performace-quantifier")
set(POLICIES_DIR "${PROJECT_SOURCE_DIR}/policies")

include(FetchContent)
FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.4.0
)

find_package(VTK REQUIRED COMPONENTS
        CommonCore
        FiltersCore
        RenderingCore
        InteractionStyle
)

FetchContent_MakeAvailable(Catch2)


add_subdirectory("${TEST_DIRECTORY}")
add_subdirectory("${PARSER_DIRECTORY}")
add_subdirectory("${QUANTIFIER_DIRECTORY}")
add_subdirectory("${POLICIES_DIR}")

add_executable(plot main.cpp)
target_link_libraries(plot PUBLIC)

target_link_libraries(plot PRIVATE
        ${VTK_LIBRARIES}
        VTK
)

# Include VTK headers
target_include_directories(plot PRIVATE ${VTK_INCLUDE_DIRS})