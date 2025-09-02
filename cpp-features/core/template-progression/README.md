# C++ Templates Guide

C++ templates are a powerful feature of the language that enables generic programming. Templates were first introduced in
C++98 and have evolved significantly with each major language update. They allow developers to write code that works with
any data type by defining a template that the compiler instantiates for specific types at compile time. This approach
provides both flexibility and performance, as template code is generated during compilation, resulting in no runtime
overhead compared to hand-written type-specific implementations.

The history of C++ templates traces back to the early design of C++ when Bjarne Stroustrup was seeking ways to implement
generic programming features. Inspired by Ada's generics, templates were initially designed to support parameterized
types. The feature was formally introduced in the first C++ standard in 1998. Since then, templates have evolved
significantly with C++11 adding variadic templates and template aliases, C++14 adding return type deduction, C++17
introducing class template argument deduction (CTAD) and fold expressions, and C++20 bringing concepts for better
template constraints.

## Use Cases and Problems Solved

Templates solve numerous problems in modern C++ programming:

1. **Type-Independent Code**: Write algorithms and data structures that work with any type, promoting code reuse and
   reducing duplication.

2. **Compile-Time Polymorphism**: Achieve polymorphic behavior without the runtime overhead of virtual functions through
   techniques like CRTP (Curiously Recurring Template Pattern).

3. **Static Analysis and Correctness**: Enforce compile-time constraints on template parameters using techniques like
   SFINAE (Substitution Failure Is Not An Error) and concepts.

4. **Generic Containers**: Enable the creation of type-safe collection classes like `std::vector`, `std::map`, and
   `std::unordered_map`.

5. **Meta-Programming**: Perform complex computations at compile time, generating optimized code tailored for specific
   uses.

6. **Policy-Based Design**: Customize behavior of classes by providing different policy classes as template parameters.

7. **Function Objects and Callbacks**: Create type-safe callbacks and function objects without runtime overhead.

8. **Zero-Overhead Abstraction**: Provide high-level abstractions that get compiled down to efficient code with no
   runtime penalty.

## Basic Function Templates

The simplest form of templates are function templates that allow you to write a single function that can operate on
different types:

```cpp
template <typename T>
T max_value(T a, T b) {
    return (a > b) ? a : b;
}

// Usage:
int i = max_value(10, 20);          // T is int
double d = max_value(3.14, 2.71);   // T is double
```

## Class Templates

Class templates enable the creation of generic container classes and data structures:

```cpp
template <typename T>
class Stack {
private:
    std::vector<T> elements;
public:
    void push(const T& value) { elements.push_back(value); }
    T pop() { 
        T top = elements.back(); 
        elements.pop_back(); 
        return top;
    }
    bool empty() const { return elements.empty(); }
};

// Usage:
Stack<int> intStack;
Stack<std::string> stringStack;
```

## Template Specialization

Template specialization allows you to provide a specific implementation for certain types:

```cpp
// Primary template
template <typename T>
class DataProcessor {
public:
    void process(T data) {
        // Generic implementation
    }
};

// Full specialization for std::string
template <>
class DataProcessor<std::string> {
public:
    void process(std::string data) {
        // String-specific implementation
    }
};
```

## Advanced Template Techniques

### Variadic Templates

Introduced in C++11, variadic templates allow templates to accept any number of arguments of any type:

```cpp
template <typename... Args>
void print_all(Args... args) {
    (std::cout << ... << args) << std::endl;  // C++17 fold expression
}

// Usage:
print_all(1, "hello", 3.14, 'x');
```

### SFINAE (Substitution Failure Is Not An Error)

SFINAE enables template functions to be excluded from overload resolution if substituting template arguments fails:

```cpp
// Only available for types with a size() method
template <typename T>
auto get_size(const T& container) -> decltype(container.size(), size_t{}) {
    return container.size();
}

// Fallback for types without size()
template <typename T>
size_t get_size(...) {
    return 0;
}
```

### CRTP (Curiously Recurring Template Pattern)

CRTP is a technique where a class inherits from a template that takes the derived class as its template parameter:

```cpp
template <typename Derived>
class Base {
public:
    void interface() {
        static_cast<Derived*>(this)->implementation();
    }
};

class Derived : public Base<Derived> {
public:
    void implementation() {
        // Specific implementation
    }
};
```

### Template Meta-Programming

Template meta-programming involves performing computations at compile time using templates:

```cpp
// Compile-time factorial
template <unsigned N>
struct Factorial {
    static constexpr unsigned value = N * Factorial<N-1>::value;
};

template <>
struct Factorial<0> {
    static constexpr unsigned value = 1;
};

// Usage:
constexpr unsigned fact5 = Factorial<5>::value;  // 120
```

## Books and Resources

Several excellent books cover C++ templates in depth:

1. **"C++ Templates: The Complete Guide"** by David Vandevoorde, Nicolai M. Josuttis, and Douglas Gregor - This
   comprehensive guide covers everything from basic templates to advanced techniques.

2. **"Modern C++ Design"** by Andrei Alexandrescu - This book introduces policy-based design and other advanced template
   techniques.

3. **"Effective Modern C++"** by Scott Meyers - Includes several items on effective template usage.

4. **"C++ Template Metaprogramming"** by David Abrahams and Aleksey Gurtovoy - Focuses on advanced compile-time
   programming techniques.

5. **"C++ Core Guidelines"** - Contains many best practices for template usage.

## Best Practices

When working with templates, consider these best practices:

- **Keep templates simple**: Avoid overly complex templates that are hard to understand and maintain.
- **Use meaningful names**: Choose descriptive template parameter names like `ElementType` instead of just `T`.
- **Use concepts** (C++20) or type traits (pre-C++20) to constrain template parameters.
- **Provide clear error messages**: Use static_assert to provide meaningful error messages when template constraints are violated.
- **Minimize compile-time dependencies**: Templates are instantiated at compile time, so minimize dependencies to reduce build times.
- **Remember that template code must be in header files**: Template definitions need to be available at the point of instantiation.

## License
This code is provided under the MIT License. Feel free to use, modify, and distribute as needed.

## Contributing
Contributions are welcome! Please feel free to submit a Pull Request.
