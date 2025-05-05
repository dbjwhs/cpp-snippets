# C++23 `if consteval` Pattern

🚧 🚧 🚧

The `if consteval` feature introduced in C++23 represents a significant advancement in C++'s compile-time programming
capabilities. This pattern allows functions to execute different code paths depending on whether they're being evaluated at
compile time or at runtime. It builds upon the evolution of C++'s constexpr features, which began with the introduction of
`constexpr` in C++11, extended in C++14, complemented by `if constexpr` in C++17, and further enhanced with `consteval` in
C++20. The `if consteval` construct provides a clean, intuitive way to branch based on the evaluation context without
resorting to complex template metaprogramming techniques.

This pattern addresses several key use cases in modern C++ development. It enables more efficient algorithms by allowing
specialized implementations for compile-time evaluation, which can take advantage of the compiler's optimization
capabilities. It simplifies error handling by enabling early detection of errors at compile time when possible, while still
providing runtime fallbacks. The pattern integrates seamlessly with other modern C++ patterns like CRTP (Curiously Recurring
Template Pattern) and the Overload Pattern, enhancing their utility and flexibility. Additionally, it reduces code
duplication by allowing developers to maintain a single function with context-specific implementations rather than separate
functions for compile-time and runtime evaluation.

## Syntax and Basic Usage

The basic syntax of `if consteval` is straightforward:

```cpp
constexpr int example(int n) {
    if consteval {
        // Code executed during compile-time evaluation
        return compile_time_implementation(n);
    } else {
        // Code executed during runtime evaluation
        return runtime_implementation(n);
    }
}
```

The `if consteval` branch is taken when the function is being evaluated at compile time (e.g., in a `constexpr` context),
while the `else` branch is taken during normal runtime execution.

## Integration with Modern C++ Patterns

### CRTP (Curiously Recurring Template Pattern)

The `if consteval` pattern can enhance CRTP by allowing base classes to provide optimized implementations based on the
evaluation context:

```cpp
template<typename Derived>
class CRTPBase {
public:
    constexpr void method(this CRTPBase& self) {
        if consteval {
            // Optimized compile-time implementation
        } else {
            // Standard runtime implementation
        }
        static_cast<Derived&>(self).derived_method();
    }
};
```

### Overload Pattern

Combined with the Overload Pattern (using C++17's class template argument deduction), `if consteval` allows for
context-specific overload sets:

```cpp
template<typename T>
constexpr auto process(T value) {
    if consteval {
        auto handler = Overload{
            [](int x) { /* compile-time int handling */ },
            [](double x) { /* compile-time double handling */ }
        };
        return handler(value);
    } else {
        auto handler = Overload{
            [](int x) { /* runtime int handling */ },
            [](double x) { /* runtime double handling */ }
        };
        return handler(value);
    }
}
```

## Benefits and Limitations

Benefits:
- Cleaner code compared to template metaprogramming alternatives
- Reduced code duplication
- Better error messages
- Integration with modern C++ features
- Performance optimization opportunities

Limitations:
- Requires C++23 compiler support
- Can increase compile times for large codebases
- May introduce subtle bugs if compile-time and runtime behaviors differ significantly

## Common Use Cases

- Mathematical functions with specialized compile-time algorithms
- Serialization/deserialization with compile-time schema validation
- Optimized container operations
- Resource management with compile-time safety checks
- Type trait implementations with improved diagnostics

## Further Reading

For more information about this pattern and related C++ features, consider these resources:

- "C++23 Complete Guide" by Nicolai Josuttis
- "Template Metaprogramming with C++" by Marius Bancila
- "Modern C++ Design" by Andrei Alexandrescu (for background on template patterns)
- C++ Standards Committee papers: P1938R3, P1073R3

## License
This code is provided under the MIT License. Feel free to use, modify, and distribute as needed.

## Contributing
Contributions are welcome! Please feel free to submit a Pull Request.