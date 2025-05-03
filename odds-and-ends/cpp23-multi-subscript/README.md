# Multidimensional Subscript Operator (C++23)

The multidimensional subscript operator is a language feature introduced in C++23 as part of proposal [P2128R6](https://gcc.gnu.org/bugzilla/show_bug.cgi?id=102611). This feature
extends C++'s existing subscript operator to accept multiple indices, enabling more intuitive and elegant syntax for accessing
multi-dimensional data structures. Prior to C++23, developers had to use nested bracket operators (e.g., `matrix[i][j]`) or
design custom accessor methods (e.g., `matrix.at(i, j)`) to work with multidimensional structures. The new syntax allows for
direct, comma-separated indexing using a single bracket operator: `matrix[i, j]`. This approach has been common in languages
specifically designed for numerical computing like Fortran and MATLAB, and its inclusion in C++ represents a significant
improvement for mathematical and scientific programming workflows.

## Use Cases and Problem Solutions

The multidimensional subscript operator addresses several longstanding issues in C++ programming. It eliminates the need for
nested bracket operators, which can be error-prone and lead to pointer decay issues with built-in arrays. It provides a more
natural syntax for mathematical operations, making code more readable and maintainable, especially for complex algorithms in
scientific computing, machine learning, and graphics programming. The feature simplifies the implementation of tensor libraries,
matrix math operations, and multi-dimensional data structures by removing the need for custom accessor methods or overloaded
comma operators to achieve similar syntax. Additionally, when combined with other C++23 features like deducing this
parameters (explicit object parameters), it enables more flexible and powerful design patterns, including more elegant
implementations of the CRTP (Curiously Recurring Template Pattern) and the Overload Pattern.

## Examples

### Basic Matrix Example

```cpp
class Matrix {
private:
    int m_rows{0};
    int m_cols{0};
    std::vector<double> m_data{};

public:
    // Multidimensional subscript operator
    auto operator[](int row, int col) -> double& {
        return m_data[row * m_cols + col];
    }
};

// Usage:
Matrix m(3, 3);
m[0, 0] = 1.0;  // Direct 2D access
```

### With Explicit Object Parameter (Deducing This)

```cpp
class Matrix {
public:
    auto operator[](this const Matrix& self, int row, int col) const -> const double& {
        return self.m_data[row * self.m_cols + col];
    }
};
```

### 3D Tensor Example

```cpp
class Tensor {
public:
    auto operator[](int i, int j, int k) -> double& {
        return m_data[(i * m_dim2 * m_dim3) + (j * m_dim3) + k];
    }
};

// Usage:
Tensor t(2, 2, 2);
t[0, 0, 1] = 3.14;  // Direct 3D access
```

## Best Practices

1. **Always include bounds checking** in your implementation for safer code
2. **Provide both const and non-const versions** of the operator
3. **Use with deducing this parameter** for more flexible design patterns
4. **Document the indexing scheme** (row-major vs column-major) for clarity
5. **Consider providing an `at()` method** that does bounds checking as an alternative

## Common Pitfalls

- Forgetting bounds checking can lead to memory corruption
- Not providing both const and non-const versions limits flexibility
- Confusion with comma operator overloading (a different feature)
- Performance considerations when implementing bounds checking

## Books and References

- "C++23 - The Complete Guide" by Nicolai M. Josuttis
- "Modern C++ Design Patterns" by Dmitri Nesteruk
- ISO C++ Standard: [P2128R6](https://github.com/cplusplus/papers/issues/845) - "Multidimensional subscript operator"
- "Scientific Computing with C++" by Joe Pitt-Francis and Jonathan Whiteley
- "Effective Modern C++" by Scott Meyers (for general C++ best practices)

## License
This code is provided under the MIT License. Feel free to use, modify, and distribute as needed.

## Contributing
Contributions are welcome! Please feel free to submit a Pull Request.