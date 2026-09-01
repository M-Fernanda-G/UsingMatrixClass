#include "NestedMatrixStress.hpp"

#include <custom_memory/GlobalNew.hpp>

#include <cstddef>

namespace {

bool initializeMemory(std::size_t bytes) noexcept {
    return custom_memory::initialize(bytes);
}

bool shutdownMemory() noexcept {
    return custom_memory::shutdown();
}

}

int main(int argument_count, char** arguments) {
    return nested_matrix_stress::run(
        argument_count,
        arguments,
        "overloaded new",
        initializeMemory,
        shutdownMemory
    );
}
