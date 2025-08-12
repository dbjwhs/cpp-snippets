# C++20 Feature Detection Macros

C++20 feature detection macros are a standardized set of preprocessor definitions that allow developers to determine at
compile-time whether specific C++20 features are available in their compiler. This pattern was introduced as part of the C++20
standard through ISO/IEC Technical Specification P0941R2. The macros serve as a compiler-agnostic mechanism for feature
detection, replacing the traditional approach of checking compiler version numbers or vendor-specific macros. Each feature-test
macro is defined only when the corresponding feature is supported by the compiler implementation, with the value typically
indicating the year and month of the technical specification that introduced the feature (e.g., 201811L for features approved in
November 2018).

Feature detection macros address several critical problems in C++ development. They enable writing portable code that adapts to
different compiler capabilities without relying on compiler-specific detection mechanisms. This is particularly valuable for
library developers who need to support multiple compiler vendors and versions. The macros also facilitate graceful fallback
implementations when modern features aren't available, allowing code to take advantage of new features where possible while
maintaining compatibility with older toolchains. Additionally, they provide a clean, standardized way to ensure required
features are available before attempting to use them, improving code reliability and reducing obscure compilation errors. The
macros are especially useful during the transition period after a new standard is published, when compiler support is uneven
across vendors.

## Usage Examples

Here are some of the most commonly used C++20 feature detection macros:

- `__cpp_concepts`: Checks for language support for concepts
- `__cpp_lib_concepts`: Checks for library support for concepts
- `__cpp_coroutines`: Checks for language support for coroutines
- `__cpp_lib_coroutine`: Checks for library support for coroutines
- `__cpp_lib_ranges`: Checks for ranges library support
- `__cpp_lib_format`: Checks for std::format support
- `__cpp_lib_span`: Checks for std::span support
- `__cpp_modules`: Checks for modules support
- `__cpp_impl_three_way_comparison`: Checks for language support for three-way comparison
- `__cpp_lib_three_way_comparison`: Checks for library support for three-way comparison

### Basic Example

```cpp
#include <version>

#ifdef __cpp_concepts
    // Use concepts in your code
    template <std::integral T>
    T add(T a, T b) { return a + b; }
#else
    // Fallback implementation without concepts
    template <typename T>
    T add(T a, T b) { return a + b; }
#endif
```

### Comprehensive Usage

For more complex scenarios, you might check multiple features and adapt accordingly:

```cpp
#include <version>

// Check for ranges support
#ifdef __cpp_lib_ranges
    #include <ranges>
    // Use ranges-based implementation
#else
    // Use alternative algorithm implementation
#endif

// Check for format support
#ifdef __cpp_lib_format
    #include <format>
    std::string log_message(const std::string& msg, int code) {
        return std::format("[{}] {}", code, msg);
    }
#else
    #include <sstream>
    std::string log_message(const std::string& msg, int code) {
        std::ostringstream ss;
        ss << "[" << code << "] " << msg;
        return ss.str();
    }
#endif
```

## Best Practices

1. Always include the `<version>` header before checking feature-test macros
2. Provide graceful fallbacks for unsupported features
3. Use feature-test macros instead of compiler version checks when possible
4. Consider using feature-test macros in combination with CMake or other build systems to configure projects based on available features
5. Document which features your code requires and which are optional

## Common Pitfalls

- Don't confuse language feature macros (like `__cpp_concepts`) with library feature macros (like `__cpp_lib_concepts`)
- Be aware that some features might be partially implemented, so testing may still be needed
- Remember that the value of the macro indicates when the feature was standardized, not the quality of the implementation
- Some older compilers might claim support for features via these macros but have buggy implementations

## Further Reading

- ISO C++ Standard paper P0941R2: "Feature-test macros and implementation-specific predetermined macros"
- "Professional C++, 5th Edition" by Marc Gregoire (Chapter on Modern C++ Features)
- "C++20: The Complete Guide" by Nicolai M. Josuttis (Section on Feature Testing)
- "The C++ Programming Language, 4th Edition" by Bjarne Stroustrup (Background on C++ evolution)

## License
This code is provided under the MIT License. Feel free to use, modify, and distribute as needed.

## Contributing
Contributions are welcome! Please feel free to submit a Pull Request.
