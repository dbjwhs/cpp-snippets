# std::print and std::println Pattern in C++23

The std::print and std::println functions introduced in C++23 represent a significant evolution in C++ formatted output
capabilities, building upon the std::format infrastructure from C++20. These functions provide a modern, type-safe, and
performance-oriented approach to formatted console output that addresses decades of limitations with printf-style
formatting and iostream verbosity. The pattern emerged from the need to combine the safety and expressiveness of C++20's
std::format with direct output capabilities, eliminating the intermediate string creation step that was previously
required. This design provides compile-time format string validation where possible, comprehensive Unicode support, and
optimized output paths that can significantly improve performance in output-heavy applications.

The std::print and std::println pattern addresses several critical problems that have plagued C++ developers for decades.
Traditional printf-family functions suffer from type safety issues, lack of compile-time validation, and potential
security vulnerabilities through format string attacks. C++ iostreams, while type-safe, introduce performance overhead,
verbose syntax, and unwanted locale dependencies that can affect program behavior unexpectedly. The C++20 std::format
function solved the formatting problem but still required an additional output step, creating unnecessary string
temporaries. The std::print pattern eliminates these intermediate allocations while maintaining all the safety and
expressiveness benefits. Additionally, this pattern provides consistent Unicode handling across platforms, automatic
newline management with std::println, and seamless integration with custom types through std::formatter specializations.

## Basic Usage Examples

```cpp
#include <print>

// Simple output without newline
std::print("Hello, world!");

// Formatted output with automatic newline
std::println("The answer is {}", 42);

// Multiple arguments with complex formatting
std::println("Name: {}, Age: {}, Score: {:.2f}", "Alice", 30, 95.67);
```

## Advanced Formatting Features

The pattern supports all std::format formatting specifications including numeric base conversion, floating-point
precision control, string alignment and padding, and custom fill characters. Container formatting is built-in for
standard library containers, ranges, and arrays. The system integrates seamlessly with chrono types, optional values,
and other standard library components.

```cpp
// Numeric formatting with different bases
std::println("Value: {} (hex: {:x}, binary: {:b})", 255, 255, 255);

// String alignment and padding
std::println("Left: '{:<10}', Center: '{:^10}', Right: '{:>10}'", "text", "text", "text");

// Container formatting
std::vector<int> numbers{1, 2, 3, 4, 5};
std::println("Numbers: {}", numbers);
```

## Performance Considerations

The std::print pattern provides significant performance advantages over traditional approaches by eliminating intermediate
string allocations and providing optimized output paths. Direct formatting to output streams reduces memory allocation
overhead and can improve cache locality. The pattern also enables better compiler optimizations through constexpr format
string evaluation when possible.

## Error Handling and Safety

Unlike printf-style functions, std::print provides compile-time format string validation where possible and runtime
validation otherwise. Type mismatches are caught at compile time, preventing common formatting errors and potential
security vulnerabilities. The pattern throws std::format_error for invalid format strings, providing clear error
messages for debugging.

## Integration with Custom Types

Custom types can be integrated with the std::print pattern through std::formatter specializations, following the same
approach as std::format. This provides consistent formatting behavior across string creation and direct output
operations.

```cpp
template<>
struct std::formatter<MyCustomType> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    auto format(const MyCustomType& obj, format_context& ctx) {
        return format_to(ctx.out(), "MyCustomType({})", obj.value());
    }
};
```

## Common Pitfalls and Best Practices

When migrating from printf or iostream code, developers should be aware of format string syntax differences and the
automatic newline behavior of std::println. Performance-critical code should prefer std::print over creating
intermediate std::format strings. Format strings should be compile-time constants when possible to enable optimization
and validation. The pattern works best when combined with consistent error handling strategies for format failures.

## Migration Strategies

Existing printf code can be migrated incrementally by replacing printf calls with std::print equivalents, adjusting
format specifiers to std::format syntax. Iostream code often becomes more concise when converted to std::print,
eliminating the need for stream manipulators and reducing verbosity. Mixed codebases can use both approaches during
transition periods without compatibility issues.

## Books and References

The pattern is extensively covered in "C++23 in Detail" by Bartłomiej Filipek, which provides comprehensive examples and
best practices. "The C++ Programming Language" (6th edition) by Bjarne Stroustrup includes detailed coverage of the
formatting library evolution. "Effective Modern C++" updates and C++23 supplement materials discuss the pattern in
context of modern C++ practices. The official C++ reference documentation (cppreference.com) provides authoritative
specification details and examples. Conference presentations from CppCon and other C++ conferences have extensively
covered the design rationale and implementation details of this pattern.

## License

This code is provided under the MIT License. Feel free to use, modify, and distribute as needed.

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.