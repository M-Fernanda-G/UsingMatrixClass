#include "NestedMatrixStress.hpp"

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
    return nested_matrix_stress::run(
        argument_count,
        arguments,
        "regular new",
        initializeMemory,
        shutdownMemory
    );
}
