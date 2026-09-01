#include "UniformMatrixNodes.hpp"

#include <custom_memory/GlobalNew.hpp>

#include <cstddef>

namespace {

bool initializeMemory(std::size_t pool_size) noexcept {
    return custom_memory::initialize(pool_size);
}

bool shutdownMemory() noexcept {
    return custom_memory::shutdown();
}

}

int main(int argument_count, char** arguments) {
    return uniform_matrix_nodes::run(
        argument_count,
        arguments,
        initializeMemory,
        shutdownMemory,
        "overloaded new"
    );
}
