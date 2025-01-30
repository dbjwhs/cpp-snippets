# WIP

# C++ Code Snippets

A collection of self-contained C++ code snippets, examples, and mini-projects. Each snippet is independently buildable and includes its own CMake configuration.

## Repository Structure

```
.
├── algorithms/          # Algorithm implementations
├── data-structures/     # Custom data structure implementations
├── design-patterns/     # Design pattern examples
├── concurrency/         # Threading and concurrency examples
└── utilities/           # Helper functions and utility classes
```

## Building the Snippets

Each snippet is contained in its own directory with a CMakeLists.txt file. To build a specific snippet:

```bash
cd <snippet-directory>
mkdir build && cd build
cmake ..
make
```

### Prerequisites

- CMake 3.15 or higher
- C++17 compliant compiler
- Git

## Snippets Index

### Algorithms
- [binary-search/](algorithms/binary-search/) - Binary search implementation with templated comparators
- [quick-sort/](algorithms/quick-sort/) - Quick sort implementation with custom partitioning

### Data Structures
- [linked-list/](data-structures/linked-list/) - Generic linked list implementation
- [binary-tree/](data-structures/binary-tree/) - Binary tree with traversal algorithms

### Design Patterns
- [observer/](design-patterns/observer/) - Observer pattern implementation
- [factory/](design-patterns/factory/) - Factory pattern example

### Concurrency
- [thread-pool/](concurrency/thread-pool/) - Thread pool implementation
- [producer-consumer/](concurrency/producer-consumer/) - Producer-consumer pattern example
- [readers-writers/](concurrency/readers-writers/) - A thread-safe Readers-Writers synchronization mechanism in C++

### Utilities
- [string-utils/](utilities/string-utils/) - String manipulation utilities
- [timer/](utilities/timer/) - High-resolution timer class
- [dates/](utilities/dates/) - Date and time utilities
- [dir_scanner/](utilities/dir_scanner/) - A platform-agnostic C++ directory scanner that uses an iterative approach

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

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

---
**Note**: This repository is actively maintained and growing. Check back regularly for new snippets and updates!
