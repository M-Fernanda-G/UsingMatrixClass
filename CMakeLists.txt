cmake_minimum_required(VERSION 3.16)

project(UsingMatrixClass LANGUAGES CXX)

if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
    set(
        CMAKE_BUILD_TYPE Debug
        CACHE STRING "Choose the build type."
        FORCE
    )
    set_property(
        CACHE CMAKE_BUILD_TYPE
        PROPERTY STRINGS Debug Release RelWithDebInfo MinSizeRel
    )
endif()

include(FetchContent)

FetchContent_Declare(
    MatrixClassDemo
    GIT_REPOSITORY https://github.com/CSE-625-Fall-2026/MatrixClassDemo.git
    GIT_TAG v1.0.0
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(MatrixClassDemo)

add_executable(using_matrix_class src/main.cpp)
target_link_libraries(using_matrix_class PRIVATE MatrixClassDemo::matrix)
