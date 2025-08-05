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

## Monadic Chaining

One of the most powerful features of `std::expected` is its support for monadic operations, which enable clean composition
of fallible operations without explicit error checking at each step. This pattern, borrowed from functional programming,
allows you to build processing pipelines where errors automatically propagate through the chain.

### Core Monadic Operations

- **`and_then()`**: Chains operations that may fail, automatically propagating errors
- **`or_else()`**: Provides error recovery by offering alternative operations
- **`transform()`**: Modifies successful values while preserving errors unchanged

### Railway-Oriented Programming

The monadic interface enables "railway-oriented programming" where your code follows two tracks: a success track and an
error track. Operations automatically switch to the error track when failures occur, eliminating the need for explicit
error checking at each step.

#### The Two-Track System

In traditional error handling, you must explicitly check for errors after each operation:

```cpp
// Traditional approach - explicit error checking
auto file_result = read_file();                    // Returns std::expected<std::string, FileError>
if (!file_result.has_value()) {                    // Check if operation failed
    return handle_file_error(file_result.error()); // Handle the error
}

auto parse_result = parse_data(file_result.value());  // Returns std::expected<Data, ParseError>
if (!parse_result.has_value()) {                     // Check if parsing failed
    return handle_parse_error(parse_result.error()); // Handle the error
}

auto final_result = process_data(parse_result.value()); // Returns std::expected<Result, ProcessError>
if (!final_result.has_value()) {                       // Check if processing failed
    return handle_process_error(final_result.error()); // Handle the error
}

return final_result.value(); // Finally get the actual result
```

With railway-oriented programming, errors automatically propagate through the success/error tracks:

```cpp
// Railway approach - automatic error propagation  
return read_file()
    .and_then(parse_data)
    .and_then(process_data);
```

#### Visual Flow Diagram

```
Input ──→ read_file() ──┐
                        │
                        ├─ Success ──→ parse_data() ──┐
                        │                             │
                        │                             ├─ Success ──→ process_data() ──┐
                        │                             │                               │
                        │                             │                               ├─ Success ──→ RESULT
                        │                             │                               │
                        │                             │                               └─ Error ──────→ ERROR
                        │                             │
                        │                             └─ Error ──────────────────────────────────────→ ERROR
                        │
                        └─ Error ──────────────────────────────────────────────────────────────────────→ ERROR
```

#### Step-by-Step Error Propagation

Consider this monadic chain:
```cpp
std::expected<int, std::string> process_pipeline() {
    return file_to_string()                    // Step 1: std::expected<std::string, std::string>
        .and_then(parse_to_string)             // Step 2: std::expected<std::vector<int>, std::string>  
        .and_then([](const std::vector<int>& numbers) -> std::expected<int, std::string> {
            return calculate_sum(numbers);      // Step 3: std::expected<int, std::string>
        });
}
```

**Scenario 1: All Steps Succeed**
- Step 1 returns `std::expected<std::string, std::string>` with file content
- Step 2 executes, receives the string, returns `std::expected<std::vector<int>, std::string>` with parsed numbers
- Step 3 executes, receives the vector, returns `std::expected<int, std::string>` with calculated sum
- Final result: Success with the sum value

**Scenario 2: Step 1 Fails (File Error)**
- Step 1 returns `std::unexpected{"file not found"}`
- Step 2 **never executes** - error propagates automatically
- Step 3 **never executes** - error propagates automatically
- Final result: `std::unexpected{"file not found"}`

**Scenario 3: Step 2 Fails (Parse Error)**
- Step 1 succeeds, returns file content
- Step 2 executes but returns `std::unexpected{"invalid format"}`
- Step 3 **never executes** - error propagates automatically
- Final result: `std::unexpected{"invalid format"}`

**Scenario 4: Step 3 Fails (Processing Error)**
- Step 1 succeeds, returns file content
- Step 2 succeeds, returns parsed numbers
- Step 3 executes but returns `std::unexpected{"calculation failed"}`
- Final result: `std::unexpected{"calculation failed"}`

#### Contrast with Traditional Error Handling

**Traditional Approach Problems:**
- **Verbose**: Explicit error checking after every operation
- **Error-prone**: Easy to forget error checks or handle them inconsistently
- **Deeply nested**: Success path gets buried in nested if statements
- **Scattered logic**: Error handling code mixed with business logic

**Railway-Oriented Benefits:**
- **Concise**: Error propagation is automatic and implicit
- **Safe**: Impossible to forget error handling - it's built into the type system
- **Linear**: Success path reads as a clean sequence of operations
- **Separated concerns**: Business logic flows naturally, error handling is orthogonal

**Before (Traditional):**
```cpp
Result process_data(const Input& input) {
    auto step1 = validate_input(input);
    if (!step1.success) {
        log_error("Validation failed: " + step1.error);
        return Result::failure(step1.error);
    }
    
    auto step2 = transform_data(step1.data);
    if (!step2.success) {
        log_error("Transform failed: " + step2.error);
        return Result::failure(step2.error);
    }
    
    auto step3 = save_data(step2.data);
    if (!step3.success) {
        log_error("Save failed: " + step3.error);
        return Result::failure(step3.error);
    }
    
    return Result::success(step3.data);
}
```

**After (Railway-Oriented):**
```cpp
std::expected<Data, std::string> process_data(const Input& input) {
    return validate_input(input)
        .and_then(transform_data)
        .and_then(save_data);
}
```

The railway pattern transforms error handling from an explicit, repetitive concern into an implicit, composable feature
of the type system, leading to more maintainable and less error-prone code.

```cpp
auto process_pipeline(const std::string& input) {
    return validate_input(input)           // std::expected<Input, ValidationError>
        .and_then(parse_data)              // std::expected<Data, ValidationError>  
        .and_then(transform_data)          // std::expected<Result, ValidationError>
        .and_then(save_result);            // std::expected<void, ValidationError>
}
```

### Type Consistency Requirements

For monadic chaining to work, all operations in the chain must have the same error type. When integrating functions
with different error types, use adapter functions to convert to a common error type:

```cpp
// Original functions with different error types
std::expected<std::string, FileError> read_file();
std::expected<Data, ParseError> parse_data(const std::string&);

// Adapter functions for type consistency
auto read_file_adapted = []() -> std::expected<std::string, std::string> {
    auto result = read_file();
    return result ? result : std::unexpected{format_file_error(result.error())};
};

auto parse_adapted = [](const std::string& content) -> std::expected<Data, std::string> {
    auto result = parse_data(content);
    return result ? result : std::unexpected{format_parse_error(result.error())};
};

// Now chainable with consistent error types
auto final_result = read_file_adapted()
    .and_then(parse_adapted)
    .and_then(process_data);
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
- **[C++ standardization papers](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2022/p0323r12.html)**: P0323R12 (std::expected) and related proposals provide detailed rationale
- **["Railway Oriented Programming" by Scott Wlaschin](https://fsharpforfunandprofit.com/rop/)** - Seminal article on composable error handling patterns

## Key Contributors and Implementations

**Sy Brand (TartanLlama)** deserves special recognition for his foundational contributions to `std::expected`. His
[TartanLlama/expected](https://github.com/TartanLlama/expected) repository provided a crucial pre-standard
implementation that significantly influenced the C++23 standardization process. Written in 2017, this single-header
C++11/14/17 implementation included the monadic operations (`and_then`, `or_else`, `transform`) and functional-style
extensions that became integral to the final standard. Brand's work not only demonstrated the practical viability of
the expected pattern but also educated the C++ community through comprehensive documentation and real-world usage
examples. His implementation served as a reference for developers wanting to use expected-style error handling before
C++23 adoption, and his contributions helped prove to the standards committee that these monadic patterns were
valuable and should be included in the official specification. The railway-oriented programming concepts and monadic
chaining patterns demonstrated in our examples trace directly back to the foundational work established in his
implementation.

The pattern has also been extensively discussed in C++ conference talks, particularly CppCon presentations on
functional programming techniques and modern error handling strategies.

## License

This code is provided under the MIT License. Feel free to use, modify, and distribute as needed.

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.
