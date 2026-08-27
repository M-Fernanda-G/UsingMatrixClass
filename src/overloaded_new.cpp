#include <matrix/Matrix.hpp>
#include <custom_memory/GlobalNew.hpp>

#include <cerrno>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <limits>
#include <memory>

namespace {

constexpr std::size_t matrix_count = 256;
constexpr std::size_t replacement_passes = 640;
constexpr std::size_t multiplication_interval = 32;
constexpr char memory_name[] = "overloaded new";

bool initializeMemory(std::size_t pool_size) noexcept {
    return custom_memory::initialize(pool_size);
}

bool shutdownMemory() noexcept {
    return custom_memory::shutdown();
}

bool checkedAdd(
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

bool checkedMultiply(
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

bool parseMatrixSize(
    int argument_count,
    char** arguments,
    std::size_t& matrix_size
) noexcept {
    if (argument_count != 2) {
        return false;
    }

    errno = 0;
    char* end = nullptr;
    const unsigned long long parsed = std::strtoull(arguments[1], &end, 10);
    if (errno != 0 || end == arguments[1] || *end != '\0' || parsed == 0 ||
        parsed > std::numeric_limits<std::size_t>::max()) {
        return false;
    }

    matrix_size = static_cast<std::size_t>(parsed);
    return true;
}

bool calculatePoolSize(
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
        checkedMultiply(per_matrix, matrix_count + 3, matrices) &&
        checkedAdd(matrices, 64 * 1024 * 1024, pool_size);
}

int runBenchmark(std::size_t matrix_size) {
    std::size_t pool_size = 0;
    if (!calculatePoolSize(matrix_size, pool_size) ||
        !initializeMemory(pool_size)) {
        std::fprintf(stderr, "unable to initialize benchmark memory\n");
        return 1;
    }

    double checksum = 0.0;
    {
        using Matrix = matrix::Matrix<double>;
        Matrix addend(matrix_size, matrix_size);
        Matrix multiplier(matrix_size, matrix_size);
        std::unique_ptr<Matrix[]> matrices(new Matrix[matrix_count]);

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

        matrices[0] = addend;
        for (std::size_t index = 1; index < matrix_count; ++index) {
            if (index % multiplication_interval == 0) {
                matrices[index] = matrices[index - 1] * multiplier;
            } else {
                matrices[index] = matrices[index - 1] + addend;
            }
        }

        for (std::size_t pass = 0; pass < replacement_passes; ++pass) {
            for (std::size_t index = 0; index < matrix_count; ++index) {
                const std::size_t previous =
                    (index + matrix_count - 1) % matrix_count;
                if (index % multiplication_interval == 0) {
                    matrices[index] = matrices[previous] * multiplier;
                } else {
                    matrices[index] = matrices[previous] + addend;
                }
            }
        }

        for (std::size_t index = 0; index < matrix_count; ++index) {
            checksum += matrices[index].get(
                index % matrix_size,
                (index * 7) % matrix_size
            );
        }
    }

    if (!shutdownMemory()) {
        std::fprintf(stderr, "unable to release benchmark memory\n");
        return 1;
    }

    std::printf(
        "%s: size=%zu matrices=%zu checksum=%.17g\n",
        memory_name,
        matrix_size,
        matrix_count,
        checksum
    );
    return 0;
}

}

int main(int argument_count, char** arguments) {
    std::size_t matrix_size = 0;
    if (!parseMatrixSize(argument_count, arguments, matrix_size)) {
        std::fprintf(stderr, "usage: %s MATRIX_SIZE\n", arguments[0]);
        return 1;
    }
    return runBenchmark(matrix_size);
}
