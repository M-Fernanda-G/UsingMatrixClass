#include <matrix/Matrix.hpp>
#include <custom_memory/GlobalNew.hpp>

#include <cstddef>
#include <random>

int main() {
    constexpr std::size_t square_size = 1000;
    constexpr std::size_t pool_size = 64 * 1024 * 1024;

    if (!custom_memory::initialize(pool_size)) {
        return 1;
    }

    bool dimensions_are_correct = false;
    {
        matrix::Matrix<int> first(square_size, square_size);
        matrix::Matrix<int> second(square_size, square_size);
        std::mt19937 generator(std::random_device{}());
        std::uniform_int_distribution<int> distribution(0, 255);

        for (std::size_t row = 0; row < square_size; ++row) {
            for (std::size_t col = 0; col < square_size; ++col) {
                first.set(row, col, distribution(generator));
                second.set(row, col, distribution(generator));
            }
        }

        const matrix::Matrix<int> result = first * second;
        dimensions_are_correct = result.getNumRows() == square_size;
    }

    const bool released_everything =
        custom_memory::statistics().live_allocations == 0;
    const bool shutdown_succeeded = custom_memory::shutdown();
    return dimensions_are_correct && released_everything && shutdown_succeeded ? 0 : 1;
}
