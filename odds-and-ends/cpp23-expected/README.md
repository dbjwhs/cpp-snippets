# std::expected in C++23 - Error Handling Without Exceptions

The `std::expected<T, E>` template class, introduced in C++23, represents a fundamental shift in C++ error handling
paradigms. This pattern originated from functional programming languages, particularly Haskell's `Either` type and
Rust's `Result` type, and provides a composable, explicit way to handle operations that may fail without relying on
exceptions. The concept was extensively discussed in the C++ standardization committee, drawing inspiration from
boost::outcome and various third-party implementations that demonstrated the value of sum types for error handling.
Unlike traditional C++ error handling mechanisms such as exceptions, error codes, or optional types with separate
error reporting, `std::expected` makes error handling explicit in the type system while enabling elegant composition
of fallible operations through monadic interfaces.

This pattern addresses several critical problems in modern C++ development: the performance overhead and control flow
disruption of exceptions, the verbosity and error-prone nature of manual error code checking, the lack of
composability in traditional error handling approaches, and the difficulty of reasoning about error propagation in
complex systems. `std::expected` enables railway-oriented programming where operations can be chained together with
automatic error propagation, eliminating the need for explicit error checking at each step while maintaining type
safety. It's particularly valuable for file I/O operations, network programming, parsing and serialization,
mathematical computations that may fail, database operations, and any domain where failure is a normal part of
business logic rather than an exceptional circumstance. The pattern also promotes better API design by making
potential failures explicit in function signatures, leading to more robust and maintainable codebases.

## Basic Usage Examples

### Simple Success/Failure Operations

```cpp
std::expected<double, std::string> safe_divide(double numerator, double denominator) {
    if (denominator == 0.0) {
        return std::unexpected{"Division by zero"};
    }
    return numerator / denominator;
}

// Usage
auto result = safe_divide(10.0, 2.0);
if (result) {
    std::cout << "Result: " << *result << std::endl; // Result: 5
} else {
    std::cout << "Error: " << result.error() << std::endl;
}
```

### Chaining Operations with Monadic Interface

```cpp
auto chained_result = safe_divide(100.0, 4.0)
    .and_then([](double val) { return safe_sqrt(val); })
    .and_then([](double val) { return safe_round(val); });

if (chained_result) {
    std::cout << "Final result: " << *chained_result << std::endl;
}
```

### Error Recovery with or_else

```cpp
auto recovered = risky_operation()
    .or_else([](const Error& e) -> std::expected<int, Error> {
        log_warning("Operation failed, using default");
        return 42; // default value
    });
```

## Advanced Usage Patterns

### File Processing Pipeline

The example demonstrates a complete file processing pipeline using `std::expected` to handle multiple potential
failure points: file reading, content parsing, and data processing. Each operation can fail independently, and
errors are automatically propagated through the chain without requiring explicit checking at each step.

### Custom Error Types

Using enumerated error types provides type safety and enables pattern matching on different error conditions:

```cpp
enum class ValidationError {
    EMPTY_INPUT,
    INVALID_FORMAT,
    OUT_OF_RANGE
};

std::expected<User, ValidationError> validate_user_input(const std::string& input);
```

### Transforming Values and Errors

The `transform` and `transform_error` methods allow modification of successful values or error types without
affecting the overall expected structure:

```cpp
auto processed = parse_number(input)
    .transform([](int n) { return n * 2; })
    .transform_error([](ParseError e) { return format_error(e); });
```

## Good Practices

- **Explicit Error Types**: Use custom enumerated types for errors rather than generic strings when possible
- **Monadic Composition**: Leverage `and_then`, `or_else`, and `transform` for clean operation chaining
- **Early Returns**: Use the monadic interface to avoid deep nesting of error checks
- **Meaningful Error Messages**: Provide context-rich error information that aids in debugging
- **Consistent Error Handling**: Establish project-wide conventions for error types and handling patterns

## Anti-Patterns to Avoid

- **Ignoring Errors**: Always check the expected state before accessing values
- **Exception Throwing**: Don't throw exceptions from within expected operations
- **Overuse**: Not every function needs to return expected; use for operations with meaningful failure modes
- **Generic Error Types**: Avoid overly broad error types that don't provide actionable information
- **Mixed Paradigms**: Don't mix exceptions and expected in the same error handling path

## Performance Considerations

`std::expected` typically has minimal performance overhead compared to exceptions, especially in error-free execution
paths. The type uses a discriminated union internally, storing either the value or error with minimal memory overhead.
Error propagation through monadic operations is generally more efficient than exception unwinding, making it suitable
for performance-critical code paths where errors are not exceptional.

## Integration with Existing Code

When integrating `std::expected` into existing codebases, consider creating adapter functions that convert between
exceptions and expected types at API boundaries. This allows gradual adoption while maintaining compatibility with
existing exception-based code.

## Testing Strategies

Comprehensive testing should cover both success and failure paths, error propagation through chains, and edge cases
in monadic operations. The example code demonstrates assertion-based testing for all major scenarios, including
successful operations, various error conditions, and complex operation chaining.

## Comparison with Alternative Approaches

Unlike exceptions, `std::expected` makes errors explicit in the type system and doesn't disrupt normal control flow.
Compared to error codes, it provides automatic error propagation and composability. Compared to `std::optional`, it
carries error information explaining why an operation failed. The pattern is particularly advantageous over exceptions
in scenarios where failure is common and performance is critical.

## Books and Resources

Several influential books and resources have covered this pattern and its theoretical foundations:

- **"Functional Programming in C++" by Ivan Čukić** - Covers monadic error handling and railway-oriented programming
- **"Effective Modern C++" by Scott Meyers** - Discusses type safety and explicit error handling approaches
- **"C++ High Performance" by Björn Andrist and Viktor Sehr** - Performance implications of different error handling
- **"Programming Rust" by Jim Blandy and Jason Orendorff** - Excellent coverage of Result types that inspired C++
- **"Learn You a Haskell for Great Good!" by Miran Lipovača** - Foundational material on Either types and monads
- **C++ standardization papers**: P0323R12 (std::expected) and related proposals provide detailed rationale
- **"Railway Oriented Programming" by Scott Wlaschin** - Seminal article on composable error handling patterns

The pattern has also been extensively discussed in C++ conference talks, particularly CppCon presentations on
functional programming techniques and modern error handling strategies.

## License

This code is provided under the MIT License. Feel free to use, modify, and distribute as needed.

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.