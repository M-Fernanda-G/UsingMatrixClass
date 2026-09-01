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

include(CTest)
include(FetchContent)

set(
    CUSTOM_MEMORY_ALLOCATOR_VERSION
    v7.0.0
    CACHE STRING "CustomMemoryAllocator release tag"
)

FetchContent_Declare(
    CustomMemoryAllocator
    GIT_REPOSITORY https://github.com/CSE-625-Fall-2026/CustomMemoryAllocator.git
    GIT_TAG ${CUSTOM_MEMORY_ALLOCATOR_VERSION}
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

add_executable(regular_uniform_nodes src/regular_uniform_nodes.cpp)
target_link_libraries(regular_uniform_nodes PRIVATE MatrixClassDemo::matrix)

add_executable(overloaded_uniform_nodes src/overloaded_uniform_nodes.cpp)
target_link_libraries(
    overloaded_uniform_nodes
    PRIVATE
        MatrixClassDemo::matrix
        CustomMemoryAllocator::global_new
)

add_executable(regular_linked_list src/regular_linked_list.cpp)
target_compile_features(regular_linked_list PRIVATE cxx_std_17)

add_executable(overloaded_linked_list src/overloaded_linked_list.cpp)
target_compile_features(overloaded_linked_list PRIVATE cxx_std_17)
target_link_libraries(
    overloaded_linked_list
    PRIVATE CustomMemoryAllocator::global_new
)

add_executable(regular_memory_stress src/regular_memory_stress.cpp)
target_link_libraries(regular_memory_stress PRIVATE MatrixClassDemo::matrix)

add_executable(overloaded_memory_stress src/overloaded_memory_stress.cpp)
target_link_libraries(
    overloaded_memory_stress
    PRIVATE
        MatrixClassDemo::matrix
        CustomMemoryAllocator::global_new
)

if(BUILD_TESTING)
    add_test(NAME regular_new_benchmark COMMAND regular_new 3)
    add_test(NAME overloaded_new_benchmark COMMAND overloaded_new 3)
    add_test(
        NAME regular_uniform_nodes_test
        COMMAND regular_uniform_nodes 3 2
    )
    add_test(
        NAME overloaded_uniform_nodes_test
        COMMAND overloaded_uniform_nodes 3 2
    )
    add_test(NAME regular_linked_list_test COMMAND regular_linked_list 25)
    add_test(
        NAME overloaded_linked_list_test
        COMMAND overloaded_linked_list 25
    )
    add_test(NAME regular_memory_stress_test COMMAND regular_memory_stress 1)
    add_test(
        NAME overloaded_memory_stress_test
        COMMAND overloaded_memory_stress 1
    )
    add_subdirectory(test)
endif()
