# WIP

# C++ Code Snippets

A collection of self-contained C++ code snippets, examples, and mini-projects. Each snippet is independently buildable and includes its own CMake configuration.

## Repository Structure Goal *(this is evolving)*

```
.
├── algorithms/          # Algorithm implementations
├── data-structures/     # Custom data structure implementations
├── algorithms/          # Algorithm implementations
├── data-structures/     # Custom data structure implementations
├── design-patterns/     # Design pattern implementations
│   ├── creational/      # Object creation patterns
│   │   ├── abstract-factory/   # Creates families of related objects
│   │   ├── builder/            # Separates object construction from representation
│   │   ├── factory-method/     # Defines interface for creating objects
│   │   ├── prototype/          # Creates objects by cloning existing ones
│   │   └── singleton/          # Ensures a class has only one instance
│   ├── structural/     # Object composition patterns
│   │   ├── adapter/            # Makes incompatible interfaces compatible
│   │   ├── bridge/             # Separates abstraction from implementation
│   │   ├── composite/          # Composes objects into tree structures
│   │   ├── decorator/          # Adds responsibilities to objects dynamically
│   │   ├── facade/             # Provides unified interface to a set of interfaces
│   │   ├── flyweight/          # Shares common state between multiple objects
│   │   └── proxy/              # Provides surrogate for another object
│   └── behavioral/     # Object communication patterns
│       ├── chain-of-responsibility/  # Passes requests along a chain
│       ├── command/                  # Encapsulates a request as an object
│       ├── interpreter/              # Defines grammar for instructions
│       ├── iterator/                 # Accesses elements sequentially
│       ├── mediator/                 # Defines how objects interact
│       ├── memento/                  # Captures and restores object state
│       ├── observer/                 # Notifies dependents of changes
│       ├── state/                    # Alters object behavior when state changes
│       ├── strategy/                 # Encapsulates interchangeable algorithms
│       ├── template-method/          # Defines skeleton of an algorithm
│       └── visitor/                  # Separates algorithm from object structure
├── concurrency/        # Threading and concurrency examples
├── utilities/          # Helper functions and utility classes
└── headers/            # Common code used throughout collection
├── concurrency/        # Threading and concurrency examples
└── utilities/          # Helper functions and utility classes
└── headers/            # Common code used throughout collection
```

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
- [binary-search/](algorithms/binary-search/) - Binary search implementation with templated comparators
- [quick-sort/](algorithms/quick-sort/) - Quick sort implementation with custom partitioning
- [compression/](algorithms/compression/) - Compression; RLE, LZW, and Huffman
- [hash-table/](algorithms/hash-table/) - Hash Table; std::hash(), fnv-1a, murmur3
- [bloom-filter/](algorithms/bloom-filter/) - Bloom filter
- [hyperloglog/](algorithms/hyperloglog/) - Hyperloglog
- [merkle-tree/](algorithms/merkle-tree/) - Merkle tree

### Data Structures
- [linked-list/](data-structures/linked-list/) - Generic linked list implementation
- [binary-tree/](data-structures/binary-tree/) - Binary tree with traversal algorithms
- [lru-cache/](data-structures/lru-cache/) - My buddy JG asked me to write a simple C++ example of a LRU cache

### Design Patterns
- [creational/singleton/](design-patterns/creational/factory/) - Singleton pattern example
- [creational/prototype/](design-patterns/creational/factory/) - Prototype pattern example
- [creational/factory/](design-patterns/creational/factory/) - Factory pattern example
- [creational/abstract-factory/](design-patterns/creational/abstract-/) - Abstract factory pattern example
- [structural/flyweight/](design-patterns/structural/flyweight/) - Flyweight pattern example
- [structural/adapter/](design-patterns/structural/adapter/) - Adapter pattern example
- [structural/proxy/](design-patterns/structural/proxy/) - Proxy pattern example
- [structural/pimpl/](design-patterns/structural/pimpl/) - Proxy pattern example
- [behavioral/observer/](design-patterns/behavioral/observer/) - Observer pattern implementation
- [behavioral/command/](design-patterns/behavioral/command/) - Command pattern example
- [behavioral/memento/](design-patterns/behavioral/memento/) - Memento pattern example
- [behavioral/strategy/](design-patterns/behavioral/strategy/) - Strategy pattern example
- [behavioral/template-method/](design-patterns/behavioral/template-method/) - Template-method pattern example
- [behavioral/chain-of-responsibility/](design-patterns/behavioral/chain-of-responsibility/) - Chain-of-responsibility pattern example

### Concurrency
- [thread-pool/](concurrency/thread-pool/) - Thread pool implementation in C++
- [producer-consumer/](concurrency/producer-consumer/) - Producer-consumer pattern example in C++
- [readers-writers/](concurrency/readers-writers/) - A thread-safe Readers-Writers synchronization mechanism in C++
- [barrier-example/](concurrency/barrier-example/) - Example of the barrier pattern in C++
- [dining-philosophers/](concurrency/dining-philosophers/) - Example of the classic dining philosophers problem in C++
- [pipelining/](concurrency/pipelining/) - An example of pipelining in C++
- [active-object-pattern/](concurrency/active-object-pattern/) - An example of the active object pattern in C++
- [thread-safe-logging/](concurrency/thread-safe-logging/) - An thread-safe logging mechanism in C++
- [jthreads/](concurrency/jthreads/) - Since std::jthread doesn't seem to be part of OSX's C++20 support lets try to make one ourselves

### Utilities
- [string-utils/](utilities/string-utils/) - String manipulation utilities
- [timer/](utilities/timer/) - High-resolution timer class
- [dates/](utilities/dates/) - Date and time utilities
- [dir_scanner/](utilities/dir_scanner/) - A platform-agnostic C++ directory scanner that uses an iterative approach
- [mock-filesystem/](utilities/mock-filesystem/) - Simple Mock filesystem

### Headers
- [headers/](headers/) - C++ header (`.hpp`) files for various utilities used throughout the repository

### Odds and Ends
- [slicing/](odds-and-ends/slicing/) - Example of object slicing the has bit me in the past, simple but can be a pain in
complex codebases.

### Tooling
- [build_all.sh](tooling/build_all.sh/) - Builds all projects that contain CMake files
- [clean_all.sh/](tooling/clean_all.sh/) - Cleans all projects that contain CMake files
- [file_find_open.sh/](tooling/file_find_open.sh/) - Find all files containing and opens them
- [new_line.sh/](tooling/new_line.sh/) - Adds new line on any coding file, I prefer this


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
