# C++ Concepts Example

C++ Concepts is a language feature introduced in C++20 that provides a way to specify constraints on template parameters. The
feature has a long history, dating back to the early 2000s when it was first proposed for C++0x (which later became C++11).
However, due to its complexity and the need for more refinement, it was postponed and finally made its way into the C++20
standard. Concepts build upon and extend the type traits functionality from C++11, providing a more expressive and
user-friendly way to define requirements for template parameters.

Concepts solve several significant problems in C++ template programming. They improve template error messages by checking
requirements at the point of template instantiation rather than deep inside the template implementation. This results in more
readable and meaningful error messages. Concepts also enable function overloading based on constraints, which was previously
difficult to achieve without complex SFINAE (Substitution Failure Is Not An Error) techniques. Additionally, concepts make
template code more self-documenting by explicitly stating requirements in the interface rather than burying them in
implementation details or documentation.

## Key Features

- **Improved Template Error Messages**: Concepts provide clear error messages when template constraints are not satisfied.
- **Explicit Requirements**: Template requirements are specified in the interface, making code more self-documenting.
- **Overloading Based on Constraints**: Different implementations can be selected based on type properties.
- **Simplified Metaprogramming**: Reduces the need for complex SFINAE techniques.

## Basic Syntax

There are several ways to use concepts in C++20:

```cpp
// Define a concept
template <typename Type>
concept Numeric = std::is_arithmetic_v<Type>;

// Use a concept in a function template
template <Numeric Type>
Type add(Type a, Type b) {
    return a + b;
}

// Use a concept with requires clause
template <typename Type>
requires Numeric<Type>
Type subtract(Type a, Type b) {
    return a - b;
}

// Use auto with concepts
void process(Numeric auto value) {
    // ...
}
```

## Common Use Cases

1. **Type Constraints**: Ensuring template parameters meet specific requirements.
2. **API Documentation**: Making requirements explicit in the interface.
3. **Function Overloading**: Selecting different implementations based on type properties.
4. **Library Design**: Creating more user-friendly template libraries with better error messages.

## Best Practices

- Keep concepts focused and composable.
- Use standard library concepts when possible.
- Name concepts using adjectives or descriptive terms.
- Document the semantic requirements that cannot be expressed in code.

## Advanced Usage

Concepts can be combined using logical operators:

```cpp
template <typename T>
concept Sortable = Comparable<T> && Swappable<T>;
```

They can also use the `requires` expression for more complex constraints:

```cpp
template <typename Type>
concept Container = requires(Type a) {
    { a.begin() } -> std::same_as<typename Type::iterator>;
    { a.end() } -> std::same_as<typename Type::iterator>;
    { a.size() } -> std::convertible_to<std::size_t>;
};
```

## Recommended Books

- "C++20 - The Complete Guide" by Nicolai M. Josuttis
- "Professional C++" by Marc Gregoire
- "C++ Templates: The Complete Guide" by David Vandevoorde, Nicolai M. Josuttis, and Douglas Gregor (2nd Edition covers concepts)

## License
This code is provided under the MIT License. Feel free to use, modify, and distribute as needed.

## Contributing
Contributions are welcome! Please feel free to submit a Pull Request.
