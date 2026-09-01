#ifndef USING_MATRIX_CLASS_NESTED_MATRIX_STRESS_HPP
#define USING_MATRIX_CLASS_NESTED_MATRIX_STRESS_HPP

#include <matrix/Matrix.hpp>

#include <cerrno>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iterator>
#include <limits>
#include <memory>
#include <new>

namespace nested_matrix_stress {

using Matrix = matrix::Matrix<double>;
using InitializeMemory = bool (*)(std::size_t) noexcept;
using ShutdownMemory = bool (*)() noexcept;

inline constexpr std::size_t pool_size = 512ULL * 1024 * 1024;
inline constexpr std::size_t three_level_outer_count = 6;
inline constexpr std::size_t four_level_outer_count = 4;
inline constexpr std::size_t multiplication_interval = 3;

inline constexpr std::size_t matrix_dimensions[] = {
    16, 24, 32, 48, 64, 80, 96, 128, 160, 192, 224, 256
};

class DeterministicGenerator final {
public:
    explicit DeterministicGenerator(std::uint64_t seed) noexcept
        : state_(seed) {
    }

    std::uint64_t next() noexcept {
        state_ ^= state_ << 13;
        state_ ^= state_ >> 7;
        state_ ^= state_ << 17;
        return state_;
    }

private:
    std::uint64_t state_;
};

struct ThreeLevelMatrices {
    std::size_t outer_count{0};
    Matrix*** values{nullptr};
    std::size_t* middle_counts{nullptr};
    std::size_t** matrix_counts{nullptr};
};

struct FourLevelMatrices {
    std::size_t outer_count{0};
    Matrix**** values{nullptr};
    std::size_t* second_counts{nullptr};
    std::size_t** third_counts{nullptr};
    std::size_t*** matrix_counts{nullptr};
};

inline bool parseRounds(
    int argument_count,
    char** arguments,
    std::size_t& rounds
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

    rounds = static_cast<std::size_t>(parsed);
    return true;
}

inline void initializeMatrix(
    Matrix& value,
    std::size_t dimension,
    std::uint64_t identifier,
    std::uint64_t& allocation_count
) {
    value = Matrix(dimension, dimension);
    allocation_count += dimension + 1;

    const double adjustment =
        static_cast<double>(identifier % 1009) * 0.000001;
    for (std::size_t index = 0; index < dimension; ++index) {
        value.set(index, index, 1.0 + adjustment);
        if (index + 1 < dimension) {
            value.set(index, index + 1, adjustment * 0.25);
        }
    }
}

inline Matrix* createMatrixArray(
    std::size_t& matrix_count,
    DeterministicGenerator& generator,
    std::uint64_t& next_identifier,
    std::uint64_t& allocation_count
) {
    const std::size_t triple_count = 2 + generator.next() % 5;
    matrix_count = triple_count * 3;
    auto values = std::make_unique<Matrix[]>(matrix_count);
    ++allocation_count;

    for (std::size_t triple = 0; triple < triple_count; ++triple) {
        const std::size_t dimension = matrix_dimensions[
            generator.next() % std::size(matrix_dimensions)
        ];
        for (std::size_t member = 0; member < 3; ++member) {
            initializeMatrix(
                values[triple * 3 + member],
                dimension,
                next_identifier++,
                allocation_count
            );
        }
    }
    return values.release();
}

inline void destroyThreeOuter(
    ThreeLevelMatrices& tree,
    std::size_t outer
) noexcept {
    if (tree.values != nullptr && tree.values[outer] != nullptr) {
        for (std::size_t middle = 0;
             middle < tree.middle_counts[outer];
             ++middle) {
            delete[] tree.values[outer][middle];
        }
        delete[] tree.values[outer];
        tree.values[outer] = nullptr;
    }
    if (tree.matrix_counts != nullptr) {
        delete[] tree.matrix_counts[outer];
        tree.matrix_counts[outer] = nullptr;
    }
    if (tree.middle_counts != nullptr) {
        tree.middle_counts[outer] = 0;
    }
}

inline void rebuildThreeOuter(
    ThreeLevelMatrices& tree,
    std::size_t outer,
    DeterministicGenerator& generator,
    std::uint64_t& next_identifier,
    std::uint64_t& allocation_count
) {
    destroyThreeOuter(tree, outer);

    const std::size_t middle_count = 3 + generator.next() % 4;
    tree.middle_counts[outer] = middle_count;
    tree.values[outer] = new Matrix*[middle_count]{};
    tree.matrix_counts[outer] = new std::size_t[middle_count]{};
    allocation_count += 2;

    for (std::size_t middle = 0; middle < middle_count; ++middle) {
        tree.values[outer][middle] = createMatrixArray(
            tree.matrix_counts[outer][middle],
            generator,
            next_identifier,
            allocation_count
        );
    }
}

inline void createThreeLevel(
    ThreeLevelMatrices& tree,
    DeterministicGenerator& generator,
    std::uint64_t& next_identifier,
    std::uint64_t& allocation_count
) {
    tree.outer_count = three_level_outer_count;
    tree.values = new Matrix**[tree.outer_count]{};
    tree.middle_counts = new std::size_t[tree.outer_count]{};
    tree.matrix_counts = new std::size_t*[tree.outer_count]{};
    allocation_count += 3;

    for (std::size_t outer = 0; outer < tree.outer_count; ++outer) {
        rebuildThreeOuter(
            tree,
            outer,
            generator,
            next_identifier,
            allocation_count
        );
    }
}

inline void destroyThreeLevel(ThreeLevelMatrices& tree) noexcept {
    for (std::size_t outer = 0; outer < tree.outer_count; ++outer) {
        destroyThreeOuter(tree, outer);
    }
    delete[] tree.values;
    delete[] tree.middle_counts;
    delete[] tree.matrix_counts;
    tree = {};
}

inline void destroyFourOuter(
    FourLevelMatrices& tree,
    std::size_t outer
) noexcept {
    if (tree.values != nullptr && tree.values[outer] != nullptr) {
        for (std::size_t second = 0;
             second < tree.second_counts[outer];
             ++second) {
            if (tree.values[outer][second] != nullptr) {
                const std::size_t third_count =
                    tree.third_counts != nullptr &&
                        tree.third_counts[outer] != nullptr
                    ? tree.third_counts[outer][second]
                    : 0;
                for (std::size_t third = 0;
                     third < third_count;
                     ++third) {
                    delete[] tree.values[outer][second][third];
                }
                delete[] tree.values[outer][second];
            }
            if (tree.matrix_counts != nullptr &&
                tree.matrix_counts[outer] != nullptr) {
                delete[] tree.matrix_counts[outer][second];
            }
        }
        delete[] tree.values[outer];
        tree.values[outer] = nullptr;
    }
    if (tree.third_counts != nullptr) {
        delete[] tree.third_counts[outer];
        tree.third_counts[outer] = nullptr;
    }
    if (tree.matrix_counts != nullptr) {
        delete[] tree.matrix_counts[outer];
        tree.matrix_counts[outer] = nullptr;
    }
    if (tree.second_counts != nullptr) {
        tree.second_counts[outer] = 0;
    }
}

inline void rebuildFourOuter(
    FourLevelMatrices& tree,
    std::size_t outer,
    DeterministicGenerator& generator,
    std::uint64_t& next_identifier,
    std::uint64_t& allocation_count
) {
    destroyFourOuter(tree, outer);

    const std::size_t second_count = 3 + generator.next() % 3;
    tree.second_counts[outer] = second_count;
    tree.values[outer] = new Matrix**[second_count]{};
    tree.third_counts[outer] = new std::size_t[second_count]{};
    tree.matrix_counts[outer] = new std::size_t*[second_count]{};
    allocation_count += 3;

    for (std::size_t second = 0; second < second_count; ++second) {
        const std::size_t third_count = 2 + generator.next() % 3;
        tree.third_counts[outer][second] = third_count;
        tree.values[outer][second] = new Matrix*[third_count]{};
        tree.matrix_counts[outer][second] = new std::size_t[third_count]{};
        allocation_count += 2;

        for (std::size_t third = 0; third < third_count; ++third) {
            tree.values[outer][second][third] = createMatrixArray(
                tree.matrix_counts[outer][second][third],
                generator,
                next_identifier,
                allocation_count
            );
        }
    }
}

inline void createFourLevel(
    FourLevelMatrices& tree,
    DeterministicGenerator& generator,
    std::uint64_t& next_identifier,
    std::uint64_t& allocation_count
) {
    tree.outer_count = four_level_outer_count;
    tree.values = new Matrix***[tree.outer_count]{};
    tree.second_counts = new std::size_t[tree.outer_count]{};
    tree.third_counts = new std::size_t*[tree.outer_count]{};
    tree.matrix_counts = new std::size_t**[tree.outer_count]{};
    allocation_count += 4;

    for (std::size_t outer = 0; outer < tree.outer_count; ++outer) {
        rebuildFourOuter(
            tree,
            outer,
            generator,
            next_identifier,
            allocation_count
        );
    }
}

inline void destroyFourLevel(FourLevelMatrices& tree) noexcept {
    for (std::size_t outer = 0; outer < tree.outer_count; ++outer) {
        destroyFourOuter(tree, outer);
    }
    delete[] tree.values;
    delete[] tree.second_counts;
    delete[] tree.third_counts;
    delete[] tree.matrix_counts;
    tree = {};
}

inline void calculateMatrixArray(
    Matrix* values,
    std::size_t matrix_count,
    std::uint64_t& operation_count,
    std::uint64_t& allocation_count,
    double& checksum
) {
    for (std::size_t index = 0; index < matrix_count; index += 3) {
        const std::size_t dimension = values[index].getNumRows();
        if (operation_count % multiplication_interval == 0) {
            values[index + 2] = values[index] * values[index + 1];
        } else {
            values[index + 2] = values[index] + values[index + 1];
        }
        allocation_count += dimension + 1;
        checksum += values[index + 2].get(
            operation_count % dimension,
            (operation_count * 7) % dimension
        );
        ++operation_count;
    }
}

inline void calculateThreeLevel(
    ThreeLevelMatrices& tree,
    std::uint64_t& operation_count,
    std::uint64_t& allocation_count,
    double& checksum
) {
    for (std::size_t outer = 0; outer < tree.outer_count; ++outer) {
        for (std::size_t middle = 0;
             middle < tree.middle_counts[outer];
             ++middle) {
            calculateMatrixArray(
                tree.values[outer][middle],
                tree.matrix_counts[outer][middle],
                operation_count,
                allocation_count,
                checksum
            );
        }
    }
}

inline void calculateFourLevel(
    FourLevelMatrices& tree,
    std::uint64_t& operation_count,
    std::uint64_t& allocation_count,
    double& checksum
) {
    for (std::size_t outer = 0; outer < tree.outer_count; ++outer) {
        for (std::size_t second = 0;
             second < tree.second_counts[outer];
             ++second) {
            for (std::size_t third = 0;
                 third < tree.third_counts[outer][second];
                 ++third) {
                calculateMatrixArray(
                    tree.values[outer][second][third],
                    tree.matrix_counts[outer][second][third],
                    operation_count,
                    allocation_count,
                    checksum
                );
            }
        }
    }
}

inline int run(
    int argument_count,
    char** arguments,
    const char* memory_name,
    InitializeMemory initialize_memory,
    ShutdownMemory shutdown_memory
) {
    std::size_t rounds = 0;
    if (!parseRounds(argument_count, arguments, rounds)) {
        std::fprintf(stderr, "usage: %s ROUNDS\n", arguments[0]);
        return 1;
    }
    if (!initialize_memory(pool_size)) {
        std::fprintf(stderr, "unable to initialize stress-test memory\n");
        return 1;
    }

    DeterministicGenerator generator{0x625C0FFEE1234567ULL};
    ThreeLevelMatrices three_level;
    FourLevelMatrices four_level;
    std::uint64_t next_identifier = 1;
    std::uint64_t operation_count = 0;
    std::uint64_t allocation_count = 0;
    double checksum = 0.0;
    bool completed = true;

    try {
        createThreeLevel(
            three_level,
            generator,
            next_identifier,
            allocation_count
        );
        createFourLevel(
            four_level,
            generator,
            next_identifier,
            allocation_count
        );

        for (std::size_t round = 0; round < rounds; ++round) {
            calculateThreeLevel(
                three_level,
                operation_count,
                allocation_count,
                checksum
            );
            calculateFourLevel(
                four_level,
                operation_count,
                allocation_count,
                checksum
            );

            rebuildThreeOuter(
                three_level,
                generator.next() % three_level.outer_count,
                generator,
                next_identifier,
                allocation_count
            );
            rebuildFourOuter(
                four_level,
                generator.next() % four_level.outer_count,
                generator,
                next_identifier,
                allocation_count
            );
        }
    } catch (const std::bad_alloc&) {
        std::fprintf(stderr, "stress-test memory exhausted\n");
        completed = false;
    }

    destroyThreeLevel(three_level);
    destroyFourLevel(four_level);
    if (!shutdown_memory()) {
        std::fprintf(stderr, "unable to release stress-test memory\n");
        return 1;
    }
    if (!completed) {
        return 1;
    }

    std::printf(
        "%s nested stress: rounds=%zu calculations=%llu new_calls=%llu "
        "delete_calls=%llu checksum=%.17g\n",
        memory_name,
        rounds,
        static_cast<unsigned long long>(operation_count),
        static_cast<unsigned long long>(allocation_count),
        static_cast<unsigned long long>(allocation_count),
        checksum
    );
    return 0;
}

}

#endif
