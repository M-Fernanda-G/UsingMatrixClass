cmake_minimum_required(VERSION 3.16)

project(UsingMatrixClass VERSION 3.0.0 LANGUAGES CXX)

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

include(CTest)
include(FetchContent)

FetchContent_Declare(
    CustomMemoryAllocator
    GIT_REPOSITORY https://github.com/CSE-625-Fall-2026/CustomMemoryAllocator.git
    GIT_TAG v1.0.0
    GIT_SHALLOW TRUE
)

FetchContent_Declare(
    MatrixClassDemo
    GIT_REPOSITORY https://github.com/CSE-625-Fall-2026/MatrixClassDemo.git
    GIT_TAG v2.0.0
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(CustomMemoryAllocator MatrixClassDemo)

add_executable(using_matrix_class src/main.cpp)
target_link_libraries(
    using_matrix_class
    PRIVATE
        MatrixClassDemo::matrix
        CustomMemoryAllocator::global_new
)

add_executable(regular_new src/regular_new.cpp)
target_link_libraries(regular_new PRIVATE MatrixClassDemo::matrix)

add_executable(overloaded_new src/overloaded_new.cpp)
target_link_libraries(
    overloaded_new
    PRIVATE
        MatrixClassDemo::matrix
        CustomMemoryAllocator::global_new
)

if(BUILD_TESTING)
    add_test(NAME regular_new_benchmark COMMAND regular_new 3)
    add_test(NAME overloaded_new_benchmark COMMAND overloaded_new 3)
    add_subdirectory(test)
endif()
