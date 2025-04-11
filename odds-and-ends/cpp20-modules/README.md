# C++20 Modules Example

![Under Construction](https://img.shields.io/badge/Status-Under%20Construction-yellow) ![Requires C++20](https://img.shields.io/badge/Requires-C%2B%2B20-blue)

⚠️ **COMPILER SUPPORT NOTICE** ⚠️

This project demonstrates C++20 Modules, but **current compiler support is limited**. As of April 2025, full module support requires:
- GCC 14+ with `-fmodules-ts`
- Clang 16+ with `-fmodules -fbuiltin-module-map`
- MSVC from VS 2022 17.4+ with `/experimental:module`
- CMake 3.28+ for proper build integration

This code is checked in for future implementation when compiler support becomes more widely available and standardized.

## Overview

C++20 Modules represent one of the most significant changes to the C++ language since its inception. They were designed to
address the fundamental limitations of the traditional header file inclusion model that has been part of C and C++ since their
beginnings. The concept of modules was first proposed around 2004 by Daveed Vandevoorde, but it took over 15 years of design,
experimentation, and standardization work before they were finally included in the C++20 standard. The development involved
extensive collaboration between compiler vendors, library developers, and the C++ standards committee to ensure that modules
could integrate well with existing codebases and build systems while providing significant benefits in terms of compilation
speed, encapsulation, and interface clarity.

Modules solve several critical problems in C++ development. First, they dramatically improve compilation times by eliminating
redundant parsing of headers and enabling better parallelization of the build process. Second, they provide true encapsulation,
allowing precise control over which symbols are exported and which remain internal implementation details. Third, they eliminate
common issues with header files such as include guard problems, macro pollution, and include order dependencies. Modules also
help with package management and library distribution by providing clear boundaries between components and explicit interfaces.
This makes modules particularly valuable for large-scale projects, performance-critical applications where build times matter,
and library development where API boundaries need to be clearly defined and maintained.

## Key Features of C++20 Modules

- **Explicit Export Control**: Only symbols explicitly marked with `export` are visible to importers.
- **No Textual Inclusion**: Modules are compiled once and stored in binary form, eliminating redundant parsing.
- **Elimination of Include Guards**: No need for include guards or pragma once directives.
- **No Macro Leakage**: Macros defined in a module don't affect code that imports the module.
- **Improved Build Times**: Modules can significantly reduce compilation times in large projects.
- **Better Encapsulation**: Clear separation between interface and implementation.
- **Module Partitions**: Ability to split a module into logical parts while maintaining a unified interface.

## Basic Syntax and Usage

### Module Declaration and Export

```cpp
// Define a module interface
export module my_module;

// Export declarations
export int add(int a, int b) {
    return a + b;
}

// Non-exported declarations (private to the module)
int helper_function() {
    return 42;
}
```

### Importing Modules

```cpp
// Import a module
import my_module;

int main() {
    // Use exported symbols
    int result = add(2, 3);
    
    // This would not compile because helper_function is not exported
    // int x = helper_function();
    
    return 0;
}
```

### Module Partitions

```cpp
// Primary module interface
export module my_module;

// Declare a partition
export import :partition1;
export import :partition2;

// Partition definition
export module my_module:partition1;

export void function_in_partition1() {
    // Implementation
}
```

## Build System Integration

Compiling modules typically involves these steps:

1. Compile module interfaces to produce Binary Module Interface (BMI) files
2. Compile module implementations using the BMI files
3. Compile code that imports modules, also using the BMI files
4. Link everything together

Most major compilers now support modules, including GCC, Clang, and MSVC, though build system integration is still evolving.

## Best Practices

- Use modules to encapsulate logical components of your codebase
- Explicitly mark what should be part of the public interface with `export`
- Consider using module partitions for large modules
- Be aware of build system requirements for modules
- Gradually migrate existing code to modules rather than all at once

## Limitations and Considerations

- Build system support is still maturing
- Not all libraries have been updated to use or support modules
- Mixing modules with traditional headers requires careful consideration
- Some advanced template techniques may require adaptation for modules

## Books and Resources

- "Professional C++, 5th Edition" by Marc Gregoire covers C++20 modules
- "C++20: The Complete Guide" by Nicolai M. Josuttis includes detailed coverage of modules
- "C++ Best Practices" by Jason Turner has been updated with module recommendations
- The C++ reference documentation at cppreference.com provides technical details on modules

## License

This code is provided under the MIT License. Feel free to use, modify, and distribute as needed.

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.