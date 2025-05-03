// MIT License
// Copyright (c) 2025 dbjwhs

#include <iostream>
#include <vector>
#include <cassert>
#include <format>
#include <string>
#include <type_traits>
#include "../../../headers/project_utils.hpp"

// multidimensional subscript operator (c++23)
// history: the multidimensional subscript operator was introduced in c++23 as part of proposal p2128r6.
// prior to c++23, accessing multidimensional arrays or matrices required nested bracket operators or custom
// accessor functions. this feature allows direct multidimensional access using operator[](...) syntax, making
// matrix-like classes more intuitive and cleaner to use. this pattern was inspired by similar functionality in
// languages like fortran and matlab, which have long supported direct multidimensional indexing.
//
// the feature enables classes to define operator[](auto... indices) to accept multiple indices in a single
// call, similar to how functions accept multiple parameters. this is particularly useful for mathematical
// libraries, linear algebra, tensor operations, and scientific computing.

// forward declarations
class Matrix;

// crtp base class for demonstrating multidimensional subscript with crtp pattern
template <typename Derived>
class MatrixBase {
public:
    // explicit object parameter (this parameter) syntax from c++23
    // this allows us to make 'this' an explicit parameter in member functions
    auto get_derived(this MatrixBase const& self) -> Derived const& {
        // cast the base class reference to the derived type using crtp pattern
        return static_cast<Derived const&>(self);
    }

    auto get_derived(this MatrixBase& self) -> Derived& {
        // cast the base class reference to the derived type using crtp pattern
        return static_cast<Derived&>(self);
    }

    // demonstration of accessing multidimensional elements via crtp
    auto at(this MatrixBase const& self, int row, int col) -> double {
        // delegate to the derived class's operator[]
        return self.get_derived()[row, col];
    }
};

// matrix class for demonstrating multidimensional subscript operator
class Matrix : public MatrixBase<Matrix> {
private:
    // member variables using braced initialization
    int m_rows{0};
    int m_cols{0};
    std::vector<double> m_data{};

public:
    // constructor
    Matrix(int rows, int cols) : m_rows{rows}, m_cols{cols} {
        // using reserve for known container size
        m_data.reserve(rows * cols);

        // initialize with default values
        for (int ndx = 0; ndx < rows * cols; ++ndx) {
            m_data.emplace_back(0.0);
        }

        LOG_INFO(std::format("Matrix created with dimensions {} x {}", rows, cols));
    }

    // multidimensional subscript operator for const access
    auto operator[](this Matrix const& self, int row, int col) -> double const& {
        // check bounds
        if (row < 0 || row >= self.m_rows || col < 0 || col >= self.m_cols) {
            LOG_ERROR(std::format("Index out of bounds: [{}, {}]", row, col));
            throw std::out_of_range("Matrix indices out of bounds");
        }

        // calculate linear index from 2d coordinates
        return self.m_data[row * self.m_cols + col];
    }

    // multidimensional subscript operator for non-const access
    auto operator[](this Matrix& self, int row, int col) -> double& {
        // check bounds
        if (row < 0 || row >= self.m_rows || col < 0 || col >= self.m_cols) {
            LOG_ERROR(std::format("Index out of bounds: [{}, {}]", row, col));
            throw std::out_of_range("Matrix indices out of bounds");
        }

        // calculate linear index from 2d coordinates
        return self.m_data[row * self.m_cols + col];
    }

    // at() method for bounds checking (const version)
    auto at(this Matrix const& self, int row, int col) -> double const& {
        // check bounds - throws if out of range
        if (row < 0 || row >= self.m_rows || col < 0 || col >= self.m_cols) {
            LOG_ERROR(std::format("Matrix::at() - Index out of bounds: [{}, {}]", row, col));
            throw std::out_of_range(std::format("Matrix::at() - Index out of bounds: [{}, {}]", row, col));
        }

        // calculate linear index from 2d coordinates
        return self.m_data[row * self.m_cols + col];
    }

    // at() method for bounds checking (non-const version)
    auto at(this Matrix& self, int row, int col) -> double& {
        // check bounds - throws if out of range
        if (row < 0 || row >= self.m_rows || col < 0 || col >= self.m_cols) {
            LOG_ERROR(std::format("Matrix::at() - Index out of bounds: [{}, {}]", row, col));
            throw std::out_of_range(std::format("Matrix::at() - Index out of bounds: [{}, {}]", row, col));
        }

        // calculate linear index from 2d coordinates
        return self.m_data[row * self.m_cols + col];
    }

    // getter for rows
    auto rows(this Matrix const& self) -> int {
        return self.m_rows;
    }

    // getter for columns
    auto cols(this Matrix const& self) -> int {
        return self.m_cols;
    }

    // print the matrix contents
    auto print(this Matrix const& self) -> void {
        for (int ndx_row = 0; ndx_row < self.m_rows; ++ndx_row) {
            std::string row_str;
            for (int ndx_col = 0; ndx_col < self.m_cols; ++ndx_col) {
                row_str += std::format("{:.1f} ", self[ndx_row, ndx_col]);
            }
            LOG_INFO(row_str);
        }
    }
};

// tensor class demonstrating 3d subscript operator
class Tensor {
private:
    // member variables using braced initialization
    int m_dim1{0};
    int m_dim2{0};
    int m_dim3{0};
    std::vector<double> m_data{};

public:
    // constructor
    Tensor(int dim1, int dim2, int dim3)
        : m_dim1{dim1}, m_dim2{dim2}, m_dim3{dim3} {
        // using reserve for known container size
        m_data.reserve(dim1 * dim2 * dim3);

        // initialize with default values
        for (int ndx = 0; ndx < dim1 * dim2 * dim3; ++ndx) {
            m_data.emplace_back(0.0);
        }

        LOG_INFO(std::format("Tensor created with dimensions {}x{}x{}", dim1, dim2, dim3));
    }

    // multidimensional subscript operator for 3d access (const)
    auto operator[](this Tensor const& self, int i, int j, int k) -> double const& {
        // check bounds
        if (i < 0 || i >= self.m_dim1 || j < 0 || j >= self.m_dim2 || k < 0 || k >= self.m_dim3) {
            LOG_ERROR(std::format("Tensor index out of bounds: [{}, {}, {}]", i, j, k));
            throw std::out_of_range("Tensor indices out of bounds");
        }

        // calculate linear index from 3d coordinates
        return self.m_data[(i * self.m_dim2 * self.m_dim3) + (j * self.m_dim3) + k];
    }

    // multidimensional subscript operator for 3d access (non-const)
    auto operator[](this Tensor& self, int i, int j, int k) -> double& {
        // check bounds
        if (i < 0 || i >= self.m_dim1 || j < 0 || j >= self.m_dim2 || k < 0 || k >= self.m_dim3) {
            LOG_ERROR(std::format("Tensor index out of bounds: [{}, {}, {}]", i, j, k));
            throw std::out_of_range("Tensor indices out of bounds");
        }

        // calculate linear index from 3d coordinates
        return self.m_data[(i * self.m_dim2 * self.m_dim3) + (j * self.m_dim3) + k];
    }

    // at() method for bounds checking (const version)
    auto at(this Tensor const& self, int i, int j, int k) -> double const& {
        // check bounds - throws if out of range
        if (i < 0 || i >= self.m_dim1 || j < 0 || j >= self.m_dim2 || k < 0 || k >= self.m_dim3) {
            LOG_ERROR(std::format("Tensor::at() - Index out of bounds: [{}, {}, {}]", i, j, k));
            throw std::out_of_range(std::format("Tensor::at() - Index out of bounds: [{}, {}, {}]", i, j, k));
        }

        // calculate linear index from 3d coordinates
        return self.m_data[(i * self.m_dim2 * self.m_dim3) + (j * self.m_dim3) + k];
    }

    // at() method for bounds checking (non-const version)
    auto at(this Tensor& self, int i, int j, int k) -> double& {
        // check bounds - throws if out of range
        if (i < 0 || i >= self.m_dim1 || j < 0 || j >= self.m_dim2 || k < 0 || k >= self.m_dim3) {
            LOG_ERROR(std::format("Tensor::at() - Index out of bounds: [{}, {}, {}]", i, j, k));
            throw std::out_of_range(std::format("Tensor::at() - Index out of bounds: [{}, {}, {}]", i, j, k));
        }

        // calculate linear index from 3d coordinates
        return self.m_data[(i * self.m_dim2 * self.m_dim3) + (j * self.m_dim3) + k];
    }
};

// demonstration of an overload pattern with an explicit object parameter
class Printer {
public:
    // Note:
    // overload pattern with explicit object parameter (c++23 "deducing this") and type constraints
    // each print method handles a different type category using requires-clauses rather than
    // traditional function overloading based on explicit parameter types, very nice feature IMHO

    // print for integral types using explicit object parameter
    auto print(this Printer const&, auto value) -> void
        requires std::is_integral_v<decltype(value)> {
        LOG_INFO(std::format("Integral value: {}", value));
    }

    // print for floating point types using explicit object parameter
    auto print(this Printer const&, auto value) -> void
        requires std::is_floating_point_v<decltype(value)> {
        LOG_INFO(std::format("Floating point value: {:.4f}", value));
    }

    // print for string types using explicit object parameter
    auto print(this Printer const&, const std::string& value) -> void {
        LOG_INFO(std::format("String value: \"{}\"", value));
    }
};

// test functions
void test_matrix_basic() {
    LOG_INFO("Testing basic matrix operations");

    // create a 3x3 matrix
    Matrix matrix(3, 3);

    // set some values
    matrix[0, 0] = 1.0;
    matrix[0, 1] = 2.0;
    matrix[0, 2] = 3.0;
    matrix[1, 0] = 4.0;
    matrix[1, 1] = 5.0;
    matrix[1, 2] = 6.0;
    matrix[2, 0] = 7.0;
    matrix[2, 1] = 8.0;
    matrix[2, 2] = 9.0;

    // test accessing values - use if statements instead of asserting to avoid macro expansion issues
    if (matrix[0, 0] != 1.0) {
        LOG_ERROR("Test failed: m[0, 0] != 1.0");
    }

    if (matrix[1, 1] != 5.0) {
        LOG_ERROR("Test failed: m[1, 1] != 5.0");
    }

    if (matrix[2, 2] != 9.0) {
        LOG_ERROR("Test failed: m[2, 2] != 9.0");
    }

    LOG_INFO("Matrix contents:");
    matrix.print();

    // test the at method from crtp base class
    if (matrix.at(1, 1) != 5.0) {
        LOG_ERROR("Test failed: m.at(1, 1) != 5.0");
    }

    LOG_INFO("Basic matrix tests passed");
}

void test_tensor() {
    LOG_INFO("Testing tensor operations");

    // create a 2x2x2 tensor
    Tensor tensor(2, 2, 2);

    // set some values using the multidimensional subscript
    tensor[0, 0, 0] = 1.0;
    tensor[0, 0, 1] = 2.0;
    tensor[0, 1, 0] = 3.0;
    tensor[0, 1, 1] = 4.0;
    tensor[1, 0, 0] = 5.0;
    tensor[1, 0, 1] = 6.0;
    tensor[1, 1, 0] = 7.0;
    tensor[1, 1, 1] = 8.0;

    // test accessing values - use if statements instead of asserting to avoid macro expansion issues
    if (tensor[0, 0, 0] != 1.0) {
        LOG_ERROR("Test failed: t[0, 0, 0] != 1.0");
    }

    if (tensor[0, 1, 1] != 4.0) {
        LOG_ERROR("Test failed: t[0, 1, 1] != 4.0");
    }

    if (tensor[1, 1, 1] != 8.0) {
        LOG_ERROR("Test failed: t[1, 1, 1] != 8.0");
    }

    LOG_INFO("Tensor tests passed");
}

void test_bounds_checking() {
    LOG_INFO("Testing bounds checking");

    Matrix m(2, 2);

    // set valid values
    m[0, 0] = 1.0;
    m[0, 1] = 2.0;
    m[1, 0] = 3.0;
    m[1, 1] = 4.0;

    // test valid access
    if (m[0, 0] != 1.0) {
        LOG_ERROR("Test failed: m[0, 0] != 1.0");
    }

    if (m[1, 1] != 4.0) {
        LOG_ERROR("Test failed: m[1, 1] != 4.0");
    }

    // test invalid access (should throw)
    // note: you will see an "Index-out-of-bounds" error log before the exception is caught
    // this is expected behavior as the operator[] logs the error before throwing
    LOG_INFO("Intentionally testing out-of-bounds access - expect an error log next");
    bool exception_caught = false;
    try {
        Logger::StderrSuppressionGuard stderr_guard;
        auto val = m[2, 0]; // out of bounds - will log an error before throwing an exception
        LOG_ERROR("Test failed: Failed to catch out-of-bounds access");
    } catch (const std::out_of_range& e) {
        exception_caught = true;
        LOG_INFO(std::format("Successfully caught exception as expected: {}", e.what()));
    }

    if (!exception_caught) {
        LOG_ERROR("Test failed: Exception was not caught for out-of-bounds access");
    }

    LOG_INFO("Bounds checking tests passed");
}

void test_matrix_at_method() {
    LOG_INFO("Testing matrix at() method");

    // create a 2x2 matrix
    Matrix m(2, 2);

    // set some values using operator[]
    m[0, 0] = 1.0;
    m[0, 1] = 2.0;
    m[1, 0] = 3.0;
    m[1, 1] = 4.0;

    // test valid access with at()
    if (m.at(0, 0) != 1.0) {
        LOG_ERROR("Test failed: m.at(0, 0) != 1.0");
    }

    if (m.at(1, 1) != 4.0) {
        LOG_ERROR("Test failed: m.at(1, 1) != 4.0");
    }

    // test modification with at()
    m.at(0, 1) = 5.0;
    if (m[0, 1] != 5.0) {
        LOG_ERROR("Test failed: m[0, 1] != 5.0 after modifying with at()");
    }

    // test exception from at() for out-of-bounds
    LOG_INFO("Intentionally testing out-of-bounds access with at() - expect an error log next");

    bool exception_caught = false;
    try {
        Logger::StderrSuppressionGuard stderr_guard;
        auto val = m.at(2, 0); // out of bounds
        LOG_ERROR("Test failed: Failed to catch out-of-bounds access with at()");
    } catch (const std::out_of_range& e) {
        exception_caught = true;
        LOG_INFO(std::format("Successfully caught exception as expected: {}", e.what()));
    }

    if (!exception_caught) {
        LOG_ERROR("Test failed: Exception was not caught for out-of-bounds access with at()");
    }

    LOG_INFO("Matrix at() method tests passed");
}

void test_tensor_at_method() {
    LOG_INFO("Testing tensor at() method");

    // create a 2x2x2 tensor
    Tensor t(2, 2, 2);

    // set some values using operator[]
    t[0, 0, 0] = 1.0;
    t[0, 0, 1] = 2.0;
    t[0, 1, 0] = 3.0;
    t[0, 1, 1] = 4.0;
    t[1, 0, 0] = 5.0;
    t[1, 0, 1] = 6.0;
    t[1, 1, 0] = 7.0;
    t[1, 1, 1] = 8.0;

    // test valid access with at()
    if (t.at(0, 0, 0) != 1.0) {
        LOG_ERROR("Test failed: t.at(0, 0, 0) != 1.0");
    }

    if (t.at(1, 1, 1) != 8.0) {
        LOG_ERROR("Test failed: t.at(1, 1, 1) != 8.0");
    }

    // test modification with at()
    t.at(0, 1, 1) = 9.0;
    if (t[0, 1, 1] != 9.0) {
        LOG_ERROR("Test failed: t[0, 1, 1] != 9.0 after modifying with at()");
    }

    // test exception from at() for out-of-bounds
    LOG_INFO("Intentionally testing out-of-bounds access with at() - expect an error log next");

    bool exception_caught = false;
    try {
        Logger::StderrSuppressionGuard stderr_guard;
        auto val = t.at(0, 0, 2); // out of bounds in 3rd dimension
        LOG_ERROR("Test failed: Failed to catch out-of-bounds access with at()");
    } catch (const std::out_of_range& e) {
        exception_caught = true;
        LOG_INFO(std::format("Successfully caught exception as expected: {}", e.what()));
    }

    if (!exception_caught) {
        LOG_ERROR("Test failed: Exception was not caught for out-of-bounds access with at()");
    }

    LOG_INFO("Tensor at() method tests passed");
}

void test_overload_pattern() {
    LOG_INFO("Testing overload pattern with explicit object parameter");

    Printer printer;

    // test different types with the overloaded print method
    printer.print(42);        // integral
    printer.print(3.14159);   // floating point
    printer.print("Hello");   // string

    LOG_INFO("Overload pattern tests passed");
}

int main() {
    LOG_INFO("Starting multidimensional subscript operator tests");

    // run all tests
    test_matrix_basic();
    test_tensor();
    test_bounds_checking();
    test_matrix_at_method();
    test_tensor_at_method();
    test_overload_pattern();

    LOG_INFO("All tests completed");
    return 0;
}
