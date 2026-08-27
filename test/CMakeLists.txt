find_package(GTest CONFIG QUIET)

if(NOT GTest_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
        URL_HASH SHA256=40d4ec942217dcc84a9ebe2a68584ada7d4a33a8ee958755763278ea1c5e18ff
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
endif()

add_executable(
    using_matrix_class_tests
    Main.cpp
    MemoryIntegrationTests.cpp
)
target_link_libraries(
    using_matrix_class_tests
    PRIVATE
        MatrixClassDemo::matrix
        CustomMemoryAllocator::global_new
        GTest::gtest
)

include(GoogleTest)
gtest_discover_tests(using_matrix_class_tests)
