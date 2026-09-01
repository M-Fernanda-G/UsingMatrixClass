#include "UniformMatrixNodes.hpp"

#include <cstddef>

namespace {

bool initializeMemory(std::size_t) noexcept {
    return true;
}

bool shutdownMemory() noexcept {
    return true;
}

}

int main(int argument_count, char** arguments) {
    return uniform_matrix_nodes::run(
        argument_count,
        arguments,
        initializeMemory,
        shutdownMemory,
        "regular new"
    );
}
