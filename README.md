# UsingMatrixClass

## Build and test

```sh
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

The benchmarks keep 256 square matrices and replace all of them 640 times.
Every 32nd result uses multiplication and the rest use addition. This creates about
`164,096 * (matrix_size + 1)` matrix-storage allocation/deallocation pairs.

```sh
time ./build-release/regular_new 400
time ./build-release/overloaded_new 400
```
