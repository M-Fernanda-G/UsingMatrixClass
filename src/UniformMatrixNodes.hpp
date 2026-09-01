#ifndef USING_MATRIX_CLASS_UNIFORM_MATRIX_NODES_HPP
#define USING_MATRIX_CLASS_UNIFORM_MATRIX_NODES_HPP

#include <matrix/Matrix.hpp>

#include <cerrno>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <limits>
#include <memory>
#include <vector>

namespace uniform_matrix_nodes {

constexpr std::size_t node_count = 256;
constexpr std::size_t multiplication_interval = 32;

using InitializeMemory = bool (*)(std::size_t) noexcept;
using ShutdownMemory = bool (*)() noexcept;

inline bool checkedAdd(
    std::size_t left,
    std::size_t right,
    std::size_t& result
) noexcept {
    if (right > std::numeric_limits<std::size_t>::max() - left) {
        return false;
    }
    result = left + right;
    return true;
}

inline bool checkedMultiply(
    std::size_t left,
    std::size_t right,
    std::size_t& result
) noexcept {
    if (left != 0 &&
        right > std::numeric_limits<std::size_t>::max() / left) {
        return false;
    }
    result = left * right;
    return true;
}

inline bool parsePositive(const char* argument, std::size_t& value) noexcept {
    errno = 0;
    char* end = nullptr;
    const unsigned long long parsed = std::strtoull(argument, &end, 10);
    if (errno != 0 || end == argument || *end != '\0' || parsed == 0 ||
        parsed > std::numeric_limits<std::size_t>::max()) {
        return false;
    }
    value = static_cast<std::size_t>(parsed);
    return true;
}

inline bool calculatePoolSize(
    std::size_t matrix_size,
    std::size_t& pool_size
) noexcept {
    std::size_t elements = 0;
    std::size_t element_bytes = 0;
    std::size_t row_overhead = 0;
    std::size_t per_matrix = 0;
    std::size_t matrices = 0;

    return checkedMultiply(matrix_size, matrix_size, elements) &&
        checkedMultiply(elements, sizeof(double), element_bytes) &&
        checkedMultiply(matrix_size, 128, row_overhead) &&
        checkedAdd(element_bytes, row_overhead, per_matrix) &&
        checkedAdd(per_matrix, 4096, per_matrix) &&
        checkedMultiply(per_matrix, node_count + 6, matrices) &&
        checkedAdd(matrices, 64 * 1024 * 1024, pool_size);
}

inline void fillOperands(
    matrix::Matrix<double>& addend,
    matrix::Matrix<double>& multiplier
) {
    const std::size_t matrix_size = addend.getNumRows();
    for (std::size_t row = 0; row < matrix_size; ++row) {
        for (std::size_t col = 0; col < matrix_size; ++col) {
            const double identity = row == col ? 1.0 : 0.0;
            addend.set(
                row,
                col,
                identity + static_cast<double>((row + col) % 5) * 0.0001
            );
            multiplier.set(
                row,
                col,
                identity + static_cast<double>((3 * row + col) % 7) *
                    0.00005
            );
        }
    }
}

inline int run(
    int argument_count,
    char** arguments,
    InitializeMemory initialize_memory,
    ShutdownMemory shutdown_memory,
    const char* memory_name
) {
    std::size_t matrix_size = 0;
    std::size_t replacement_passes = 0;
    if (argument_count != 3 ||
        !parsePositive(arguments[1], matrix_size) ||
        !parsePositive(arguments[2], replacement_passes)) {
        std::fprintf(
            stderr,
            "usage: %s MATRIX_SIZE REPLACEMENT_PASSES\n",
            arguments[0]
        );
        return 1;
    }

    std::size_t pool_size = 0;
    if (!calculatePoolSize(matrix_size, pool_size) ||
        !initialize_memory(pool_size)) {
        std::fprintf(stderr, "unable to initialize benchmark memory\n");
        return 1;
    }

    double checksum = 0.0;
    {
        using Matrix = matrix::Matrix<double>;
        Matrix addend(matrix_size, matrix_size);
        Matrix multiplier(matrix_size, matrix_size);
        fillOperands(addend, multiplier);

        std::vector<std::unique_ptr<Matrix>> nodes(node_count);
        nodes[0] = std::make_unique<Matrix>(addend);
        for (std::size_t index = 1; index < node_count; ++index) {
            if (index % multiplication_interval == 0) {
                nodes[index] = std::make_unique<Matrix>(
                    *nodes[index - 1] * multiplier
                );
            } else {
                nodes[index] = std::make_unique<Matrix>(
                    *nodes[index - 1] + addend
                );
            }
        }

        for (std::size_t pass = 0; pass < replacement_passes; ++pass) {
            for (std::size_t index = 0; index < node_count; ++index) {
                const std::size_t previous =
                    (index + node_count - 1) % node_count;
                nodes[index].reset();
                if (index % multiplication_interval == 0) {
                    nodes[index] = std::make_unique<Matrix>(
                        *nodes[previous] * multiplier
                    );
                } else {
                    nodes[index] = std::make_unique<Matrix>(
                        *nodes[previous] + addend
                    );
                }
            }
        }

        for (std::size_t index = 0; index < node_count; ++index) {
            checksum += nodes[index]->get(
                index % matrix_size,
                (index * 7) % matrix_size
            );
        }
    }

    if (!shutdown_memory()) {
        std::fprintf(stderr, "unable to release benchmark memory\n");
        return 1;
    }

    std::printf(
        "%s uniform nodes: size=%zu passes=%zu nodes=%zu checksum=%.17g\n",
        memory_name,
        matrix_size,
        replacement_passes,
        node_count,
        checksum
    );
    return 0;
}

}

#endif
