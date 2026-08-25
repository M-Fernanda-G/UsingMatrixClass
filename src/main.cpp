#include <matrix/Matrix.hpp>

#include <cstddef>
#include <random>

int main() {
    constexpr std::size_t square_size = 1000;

    matrix::Matrix<double> first(square_size, square_size);
    matrix::Matrix<double> second(square_size, square_size);
    std::mt19937 generator(std::random_device{}());
    std::uniform_int_distribution<int> distribution(0, 255);

    for (std::size_t row = 0; row < square_size; ++row) {
        for (std::size_t col = 0; col < square_size; ++col) {
            first.set(row, col, distribution(generator));
            second.set(row, col, distribution(generator));
        }
    }

    const matrix::Matrix<double> result = first * second;

    return result.getNumRows() == square_size ? 0 : 1;
}
