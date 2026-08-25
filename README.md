# UsingMatrixClass

Fetches `MatrixClassDemo` version `v2.0.0` with CMake `FetchContent`.

## Build and run

With a single-config generator, the project defaults to a `Debug` build so the
compiler does not optimize the example:

```sh
cmake -S . -B build
cmake --build build
./build/using_matrix_class
```

You can select a different configuration explicitly when configuring:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

`MatrixClassDemo` is added to the same build tree by
`FetchContent_MakeAvailable`. Consequently, `CMAKE_BUILD_TYPE` applies to both
`UsingMatrixClass` and `MatrixClassDemo`. Setting the build type on the
top-level configure command forces the dependency to use the same
configuration:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

Multi-config generators such as Visual Studio and Xcode choose the shared
configuration at build time instead:

```sh
cmake -S . -B build
cmake --build build --config Debug
```
