# C++23 Preprocessor Directives: #elifdef, #elifndef, and #warning

## Overview

C++23 introduces three new preprocessor directives that enhance conditional compilation capabilities: `#elifdef`,
`#elifndef`, and `#warning`. These directives were added to the C++23 standard (ISO/IEC 14882:2024) through proposals
P2334R1 and P2437R1. The `#elifdef` and `#elifndef` directives originally came from the C23 standard and were adopted
into C++ to maintain compatibility between the two languages' preprocessors.

The `#elifdef` directive serves as shorthand for `#elif defined(MACRO)`, while `#elifndef` is shorthand for
`#elif !defined(MACRO)`. These directives significantly improve readability in complex preprocessor conditional chains,
reducing verbosity and making the code more maintainable. The `#warning` directive, which many compilers already
supported as an extension, is now standardized. It issues a compiler warning without halting compilation (unlike the
`#error` directive).

## Use Cases and Problem Solving

### Simplified Platform Detection

These directives streamline platform-specific code by making it easier to create clean, readable conditional compilation
chains. For example:

```cpp
#ifdef WINDOWS
    // Windows-specific code
#elifdef LINUX
    // Linux-specific code
#elifdef MACOS
    // macOS-specific code
#else
    // Generic fallback code
#endif
```

### Enhanced Feature Toggle Systems

When implementing feature toggles across different build configurations, the new directives reduce nesting complexity:

```cpp
#ifdef FEATURE_A
    // Feature A enabled
#elifdef FEATURE_B
    // Feature B enabled
#elifndef LEGACY_MODE
    // New behavior when not in legacy mode
#else
    // Default behavior
#endif
```

### Improved Warning System

The standardized `#warning` directive facilitates better communication of potential issues or deprecated features without
halting compilation:

```cpp
#if VERSION < 3
    #warning "This version is deprecated and will be removed in future releases"
#endif
```

### Backward Compatibility Handling

The directives simplify conditional code for backward compatibility with different standard versions:

```cpp
#ifdef CPP_23
    // Use newest features
#elifdef CPP_20
    // Use C++20 features
#elifndef LEGACY_COMPILER
    // Code for non-legacy compilers
#else
    // Fallback implementation
#endif
```

## Examples and Usage

### #elifdef Example

```cpp
#ifdef COMPILER_GCC
    // GCC specific code
#elifdef COMPILER_CLANG
    // Clang specific code
#elifdef COMPILER_MSVC
    // MSVC specific code
#else
    // Generic fallback
#endif
```

### #elifndef Example

```cpp
#ifdef DEBUG
    // Debug configuration
#elifndef NDEBUG
    // Not in debug but assertions enabled
#else
    // Release configuration
#endif
```

### #warning Example

```cpp
#if API_VERSION < 2
    #warning "Using deprecated API version, please upgrade to v2 or later"
#endif
```

### Good vs. Bad Usage

#### Good:
- Using these directives to create clean, readable conditional chains
- Using `#warning` to communicate important notices without breaking builds
- Organizing platform or feature-specific code in logical groups

#### Bad:
- Creating overly complex preprocessor logic that could be handled better with runtime checks
- Using these directives excessively when more modern C++ features like concepts or SFINAE might be more appropriate
- Relying too heavily on preprocessor conditionals instead of proper design patterns

## Reference Books

- "C++23 Complete Guide" by Nicolai M. Josuttis (upcoming)
- "Professional C++" by Marc Gregoire (latest edition covering C++23)
- "C++ Templates: The Complete Guide" by David Vandevoorde, Nicolai M. Josuttis, and Douglas Gregor

## License

This code is provided under the MIT License. Feel free to use, modify, and distribute as needed.

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.