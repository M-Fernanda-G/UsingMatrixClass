# UsingMatrixClass

## Purpose

MatrixClassDemo is a general matrix class for demonstration purposes. Its code
does not change when a different CustomMemoryAllocator version is selected.

UsingMatrixClass is the caller. The overloaded programs link the global `new`
target and initialize the custom allocator. After initialization, every
ordinary use of `new` and `new[]` in the caller and its dependencies uses the
custom allocator. Matching `delete` and `delete[]` calls return memory through
it. The regular programs do not link or initialize that target.

## Build and test

```sh
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

Use Release builds for profiling

```sh
cmake -S . -B build-release -DCMAKE_BUILD_TYPE=Release
cmake --build build-release
```

## Allocator versions

Each allocator version can use its own build directory. This keeps the fetched
dependency and compiled programs separate. It also makes profiling versions
with the same workload easier.

```sh
cmake -S . -B build-CMA-V1 -DCMAKE_BUILD_TYPE=Release -DCUSTOM_MEMORY_ALLOCATOR_VERSION=v1.0.0
cmake --build build-CMA-V1

cmake -S . -B build-CMA-V7 -DCMAKE_BUILD_TYPE=Release -DCUSTOM_MEMORY_ALLOCATOR_VERSION=v7.0.0
cmake --build build-CMA-V7
```

- v1.0.0 uses one ordered free list, first fit allocation, and immediate merging
- v2.0.0 searches for the smallest usable block to reduce wasted space
- v3.0.0 groups free blocks by size to reduce search work
- v4.0.0 caches small blocks per thread to avoid repeated global locking
- v5.0.0 merges adjacent blocks only when an allocation needs more space
- v6.0.0 refills caches in batches and creates reusable small block slabs
- v7.0.0 grows busy cache classes while limiting retained memory

## Test pairs

Each pair has one shared hpp file. Both cpp wrappers call the same workload.
The only intended difference is regular `new` compared with overloaded `new`.

### Matrix array

MatrixArrayWorkload.hpp keeps 256 same-size matrices in one array. It fills and
replaces matrix values through addition and multiplication. This pair shows
what happens when calculation time is much larger than allocation time.

```sh
time ./build-release/regular_new 140
time ./build-release/overloaded_new 140
```

### Uniform matrix nodes

UniformMatrixNodes.hpp keeps 256 separately allocated matrix nodes. It deletes
and recreates nodes through addition and multiplication. This pair adds object
lifetime activity while every matrix remains the same size.

```sh
time ./build-release/regular_uniform_nodes 100 2500
time ./build-release/overloaded_uniform_nodes 100 2500
```

### Nested matrix stress

NestedMatrixStress.hpp creates three-level and four-level arrays of matrices.
Matrix sizes vary. Complete subtrees are deleted and recreated. This pair
creates fragmentation and many different allocation sizes.

```sh
time ./build-release/regular_memory_stress 90
time ./build-release/overloaded_memory_stress 90
```

### Linked list

LinkedListWorkload.hpp creates nodes with integers, characters, booleans, and
dynamically sized integer arrays. A fixed seed controls appends, insertions,
deletions, and updates. The operation count is 200 times the maximum list size.

```sh
time ./build-release/regular_linked_list 750000
time ./build-release/overloaded_linked_list 750000
```

## Profiling results

These results use Release builds. Each regular and overloaded executable ran
sequentially with the same parameters and repetition count.

| Workload | Program parameters | Repetitions | Regular seconds | Overloaded seconds | Improvement |
| --- | --- | ---: | ---: | ---: | ---: |
| Matrix array | Matrix size 140 with 256 matrices and 640 replacement passes | 582 | 7007 | 5921 | 15.5 percent |
| Uniform matrix nodes | Matrix size 100 with 256 nodes and 2500 replacement passes | 455 | 7069 | 6646 | 6.0 percent |
| Nested matrix stress | 90 rounds with matrix sizes from 16 through 256 | 317 | 7189 | 6299 | 12.4 percent |
| Linked list | Maximum size 750000 with 150000000 operations and seed 6252026 | 355 | 6647 | 7004 | -5.4 percent |

## Questions

- What do you notice as the profiling scale increases in each test case?
- How could namespaces support multiple allocators based on scope and purpose?
- When could a known access pattern make an allocator heuristic effective?
- What tradeoffs come with creating and maintaining a custom allocator?
- Several threads update adjacent objects. How could padding each object to a
  cache-line boundary waste space yet improve performance by reducing false
  sharing and cache coherence traffic?
- Why can matrix calculation time hide allocation improvements?
- Why might the system allocator outperform a custom allocator for small nodes?
- How does fragmentation affect allocation search time and memory use?
- Why is a fixed random seed important for a fair comparison?
- How could execution order affect measured results?
- Why should several runs and median times be used for profiling?
- Which measurements should be collected in addition to elapsed time?
- How do canaries and allocation metadata affect speed and memory use?
- How should cache limits and size classes be selected for a workload?
- When could an allocator optimized for one workload harm another workload?
- Why do good design, separation of concerns, portability, and reusability
  matter when code is shared across projects?
- Explain the importance of `single source of truth` in sourcecode management.
