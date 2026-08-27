#include "custom_memory/GlobalNew.hpp"

#include <gtest/gtest.h>

#include <cstddef>

int main(int argc, char** argv) {
    constexpr std::size_t pool_size = 64 * 1024 * 1024;
    if (!custom_memory::initialize(pool_size)) {
        return 1;
    }

    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
