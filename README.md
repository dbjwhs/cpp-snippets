# C++ Code Snippets

A collection of self-contained C++ code snippets, examples, and mini-projects. Each snippet is independently buildable and includes its own CMake configuration.

## Building the Snippets

Each snippet is contained in its own directory with a CMakeLists.txt file. To build a specific snippet:

```bash
cd <snippet-directory>
mkdir build && cd build
cmake ..
make
```

Or you can elect to build everything at once.

1. `build_all.sh` that:
   - Automatically finds and builds all CMake projects in your directory structure
   - Creates build directories as needed
   - Provides colored output and detailed logging
   - Shows a comprehensive summary of successful and failed builds with executable paths
   - `--dry-run` | `-d` argument to perform a *dry run*
   - `--run` | `-r` argument to perform *run* executable after build

2. `clean_all.sh` that:
   - Finds and removes all build directories
   - Provides similar colored output and logging
   - Shows a summary of cleaned projects
   - `--dry-run` | `-d` argument to perform a *dry run*

### Prerequisites

- CMake 3.30 or higher
- C++20 compliant compiler (see gotcha's below for linux)
- Git

## Snippets Index

### Algorithms
- [binary-search/](algorithms/binary-search/)
- [bloom-filter/](algorithms/bloom-filter/)
- [compression/](algorithms/compression/)
- [hash-table/](algorithms/hash-table/)
- [hyperloglog/](algorithms/hyperloglog/)
- [merkle-tree/](algorithms/merkle-tree/)
- [quick-sort/](algorithms/quick-sort/)

### Concurrency
- [active-object-pattern/](concurrency/active-object-pattern/)
- [atomic/](concurrency/atomic/)
- [barrier-example/](concurrency/barrier-example/)
- [cpp20-latch/](concurrency/cpp20-latch/)
- [dining-philosophers/](concurrency/dining-philosophers/)
- [jthread-test/](concurrency/jthread-test/)
- [jthreads/](concurrency/jthreads/)
- [latch/](concurrency/latch/)
- [pipelining/](concurrency/pipelining/)
- [producer-consumer/](concurrency/producer-consumer/)
- [promise-future/](concurrency/promise-future/)
- [reader-writer/](concurrency/reader-writer/)
- [shared_mutex/](concurrency/shared_mutex/)
- [stop_token-source-callback/](concurrency/stop_token-source-callback/)
- [thread-local-storage/](concurrency/thread-local-storage/)
- [thread-pool/](concurrency/thread-pool/)
- [thread-pool-load-balance/](concurrency/thread-pool-load-balance/)
- [thread-safe-logging/](concurrency/thread-safe-logging/)
- [timed_mutex/](concurrency/timed_mutex/)

### Data Structures
- [binary-tree/](data-structures/binary-tree/)
- [flat_maps_explored/](data-structures/flat_maps_explored/)
- [linked-list/](data-structures/linked-list/)
- [lru-cache/](data-structures/lru-cache/)

### Design Patterns

#### Creational
- [abstract-factory/](design-patterns/creational/abstract-factory/)
- [builder/](design-patterns/creational/builder/)
- [factory/](design-patterns/creational/factory/)
- [prototype/](design-patterns/creational/prototype/)
- [singleton/](design-patterns/creational/singleton/)

#### Structural
- [adapter/](design-patterns/structural/adapter/)
- [bridge/](design-patterns/structural/bridge/)
- [composite/](design-patterns/structural/composite/)
- [decorator/](design-patterns/structural/decorator/)
- [facade/](design-patterns/structural/facade/)
- [flyweight/](design-patterns/structural/flyweight/)
- [pimpl/](design-patterns/structural/pimpl/)
- [proxy/](design-patterns/structural/proxy/)

#### Behavioral
- [chain-of-responsibility/](design-patterns/behavioral/chain-of-responsibility/)
- [command/](design-patterns/behavioral/command/)
- [fail-fast/](design-patterns/behavioral/fail-fast/)
- [finite-state-machine/](design-patterns/behavioral/finite-state-machine/)
- [interpreter/](design-patterns/behavioral/interpreter/)
- [iterator/](design-patterns/behavioral/iterator/)
- [mediator/](design-patterns/behavioral/mediator/)
- [memento/](design-patterns/behavioral/memento/)
- [observer/](design-patterns/behavioral/observer/)
- [state/](design-patterns/behavioral/state/)
- [strategy/](design-patterns/behavioral/strategy/)
- [template-method/](design-patterns/behavioral/template-method/)
- [visitor/](design-patterns/behavioral/visitor/)

### Utilities
- [dates/](utilities/dates/)
- [dir_scanner/](utilities/dir_scanner/)
- [mock-filesystem/](utilities/mock-filesystem/)
- [string-utils/](utilities/string-utils/)
- [timer/](utilities/timer/)

### Programming Paradigms
- [diamond-problem/](programming-paradigms/diamond-problem/)
- [negative-space/](programming-paradigms/negative-space/)

### Language Features

#### C++20 Features
- [concepts/](language-features/cpp20/concepts/)
- [coroutines/](language-features/cpp20/coroutines/)
- [counting-semaphore/](language-features/cpp20/counting-semaphore/)
- [designated-initializers/](language-features/cpp20/designated-initializers/)
- [modules/](language-features/cpp20/modules/)
- [ranges/](language-features/cpp20/ranges/)
- [spaceship/](language-features/cpp20/spaceship/)
- [test-macros/](language-features/cpp20/test-macros/)

#### C++23 Features
- [alias-labels/](language-features/cpp23/alias-labels/)
- [byteswap/](language-features/cpp23/byteswap/)
- [expected/](language-features/cpp23/expected/)
- [if-consteval/](language-features/cpp23/if-consteval/)
- [mdspan/](language-features/cpp23/mdspan/)
- [multi-subscript/](language-features/cpp23/multi-subscript/)
- [new-preprocessor/](language-features/cpp23/new-preprocessor/)
- [print-println/](language-features/cpp23/print-println/)
- [this/](language-features/cpp23/this/)
- [turing-machines/](language-features/cpp23/turing-machines/)

#### Core Language Features
- [const-constexpr/](language-features/core/const-constexpr/)
- [decltype/](language-features/core/decltype/)
- [slicing/](language-features/core/slicing/)
- [std-function/](language-features/core/std-function/)
- [std-move/](language-features/core/std-move/)
- [std-optional/](language-features/core/std-optional/)
- [std-regex/](language-features/core/std-regex/)
- [template-progression/](language-features/core/template-progression/)

### Networking
- [boost-asio/](networking/boost-asio/)
- [simple_client_server/](networking/simple_client_server/)
- [simple_subnet_ping/](networking/simple_subnet_ping/)

### System Programming
- [backtrace/](system-programming/backtrace/)

### Examples
- [hoare-logic/](examples/hoare-logic/)
- [pagination/](examples/pagination/)
- [phone-number-to-char/](examples/phone-number-to-char/)

## Contributing

Feel free to suggest improvements or report issues. Each snippet should:

1. Be self-contained in its own directory
2. Include a CMakeLists.txt file
3. Have a README.md explaining its purpose and usage
4. Include comments explaining complex logic
5. Follow the repository's coding style

## Snippet Directory Structure

Each snippet directory should follow this structure:

```
snippet-name/
├── CMakeLists.txt          # Build configuration
├── README.md               # Usage and explanation
├── include/                # Header files
│   └── snippet.hpp
└── src/                    # Source files
    └── snippet.cpp
```

## Gotchas and errata

I am currently evaluating JetBrain's CLion which creates a directory in each project file called `cmake-build-debug` which if removed becomes a **PITA**.

### Recreating the cmake-build-debug Directory

If you accidentally removed the `cmake-build-debug` directory, you can easily recreate it by following these steps:

#### 1. Regenerate the Build Directory

Navigate to the root directory of your CMake project in your terminal or command prompt, then run:

```bash
cmake -DCMAKE_BUILD_TYPE=Debug ..
```

**Note:** Replace `..` with the path to your CMakeLists.txt file if it's not in the same directory.

This command will reconfigure your project and generate the `cmake-build-debug` directory along with the necessary build files.

#### 2. Rebuild Your Project

You can either:

* Rebuild your project from within your IDE (Visual Studio or CLion)

OR

* Use the following command from your terminal:

```bash
cmake --build . --target all
```

This will rebuild all the targets in your project.

## Linux

As of this writing 2/17/2025 most of the projects are building on linux, see below for my setup instructions. However there still are six (6) project not building, I am working through issues on them currently.

### C++20 std::format Build Issues and Resolution

### Problem Description
Build failures occurred when attempting to use `std::format` in C++20 code. While the code built successfully in CLion, it failed during command line cmake/make builds with the error:
```
fatal error: format: No such file or directory
```

The system was initially running GCC 12.3.0 which had incomplete C++20 library support.

### Root Cause
GCC 12 implementations shipped with partial C++20 support. Specifically, the `std::format` header was not included in the standard library headers, which explained why the `<format>` header could not be found during compilation.

### Solution
The issue was resolved by upgrading to GCC 13, which provides complete C++20 support including the `std::format` implementation.

#### Step 1: Add Required Repository
```bash
sudo add-apt-repository ppa:ubuntu-toolchain-r/test
sudo apt update
```

#### Step 2: Install GCC 13
```bash
sudo apt install gcc-13 g++-13 libstdc++-13-dev
```

#### Step 3: Set System Defaults
```bash
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-13 130
sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-13 130
```

### Result
- Successfully upgraded to GCC 13.1.0 (Ubuntu 13.1.0-8ubuntu1~22.04)
- Full C++20 support including `std::format`
- Code now builds correctly both in CLion and command line environments

### Key Learning (saving you three hours of headaches)
When implementing C++20 features, particularly `std::format`, GCC 13 or later is required xfor complete standard library support. Earlier versions may have incomplete implementations of the C++20 standard library features.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

---
**Note**: This repository is actively maintained and growing. Check back regularly for new snippets and updates!
