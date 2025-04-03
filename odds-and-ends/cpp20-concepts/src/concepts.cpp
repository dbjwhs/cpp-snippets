// MIT License
// Copyright (c) 2025 dbjwhs

#include <iostream>
#include <vector>
#include <string>
#include <type_traits>
#include <cassert>
#include <format>
#include "../../../headers/project_utils.hpp"

// c++ concepts history and usage
//
// concepts were introduced in c++20 as a major feature for template programming. they evolved from
// the c++11 type traits library and the original concepts proposal for c++11 that was postponed.
// concepts provide a way to express constraints on template parameters, making template code more
// readable, providing better error messages, and enabling function overloading based on constraints.
// common usage patterns include:
//  - constraining template parameters to specific types or behaviors
//  - creating distinct overloads based on type properties
//  - documenting template requirements in the interface
//  - enabling better compiler diagnostics
//  - simplifying template metaprogramming

// declare a concept for types that can be added
template <typename Type>
concept addable = requires(Type a, Type b) {
    // requires that a + b is a valid expression
    { a + b } -> std::convertible_to<Type>;
};

// declare a concept for types that are numeric
template <typename Type>
concept numeric = std::is_arithmetic_v<Type> && addable<Type>;

// declare a concept for types that are containers
template <typename Type>
concept container = requires(Type a) {
    // requires that container supports begin() and end() operations
    { a.begin() } -> std::same_as<typename Type::iterator>;
    { a.end() } -> std::same_as<typename Type::iterator>;
    // requires that the container has a size method returning size_t
    { a.size() } -> std::convertible_to<std::size_t>;
};

// a simple class template that is constrained with concepts
template <numeric Type>
class Calculator {
private:
    // member variable storing the accumulated value
    Type m_value;

public:
    // constructor initializing the calculator with a value
    explicit Calculator(Type initial_value) : m_value(initial_value) {}

    // add method that uses the addable concept
    void add(Type x) {
        m_value += x;
    }

    // getter for the current value
    [[nodiscard]] Type getValue() const {
        return m_value;
    }
};

// function that works with any container of numeric types
template <container C, numeric Type = typename C::value_type>
Type sum(const C& values) {
    Type result = Type{};
    for (int ndx = 0; ndx < values.size(); ndx++) {
        result += values[ndx];
    }
    return result;
}

// function to test a specific type with our concepts
template <typename Type>
void test_type() {
    // output the type name and whether it satisfies our concepts
    LOG_INFO(std::format("Testing type: {}", typeid(Type).name()));
    LOG_INFO(std::format("Is addable: {}", addable<Type> ? "Yes" : "No"));
    LOG_INFO(std::format("Is numeric: {}", numeric<Type> ? "Yes" : "No"));
}

// test the calculator class with different types
void test_calculator() {
    LOG_INFO("Testing Calculator with int");
    
    // create a calculator with int
    Calculator<int> calc_int(5);
    calc_int.add(10);
    
    // verify the result
    assert(calc_int.getValue() == 15 && "Calculator with int failed");
    LOG_INFO(std::format("Calculator value: {}", calc_int.getValue()));
    
    LOG_INFO("Testing Calculator with double");
    
    // create a calculator with double
    Calculator<double> calc_double(5.5);
    calc_double.add(10.5);
    
    // verify the result using approximate comparison for floating point
    constexpr double expected = 16.0;
    const double actual = calc_double.getValue();
    constexpr double epsilon = 0.001;
    assert(std::abs(actual - expected) < epsilon && "Calculator with double failed");
    LOG_INFO(std::format("Calculator value: {}", calc_double.getValue()));
    
    // the following line would cause a compilation error, uncomment to test
    // Calculator<std::string> calc_string("Hello"); // error: std::string doesn't satisfy the numeric concept
}

// test the sum function with different containers
void test_sum() {
    LOG_INFO("Testing sum with vector<int>");
    
    // create a vector of ints
    const std::vector<int> vec_int = {1, 2, 3, 4, 5};
    int sum_int = sum(vec_int);
    
    // verify the result
    assert(sum_int == 15 && "Sum of vector<int> failed");
    LOG_INFO(std::format("Sum: {}", sum_int));
    
    LOG_INFO("Testing sum with vector<double>");
    
    // create a vector of doubles
    const std::vector<double> vec_double = {1.1, 2.2, 3.3, 4.4, 5.5};
    double sum_double = sum(vec_double);
    
    // verify the result using approximate comparison for floating point
    constexpr double expected = 16.5;
    const double actual = sum_double;
    constexpr double epsilon = 0.001;
    assert(std::abs(actual - expected) < epsilon && "Sum of vector<double> failed");
    LOG_INFO(std::format("Sum: {}", sum_double));
    
    // the following line would cause a compilation error, uncomment to test
    // std::vector<std::string> vec_string = {"Hello", "World"};
    // std::string sum_string = sum(vec_string); // error: std::string doesn't satisfy numeric concept
}

// main function to run all tests
int main() {
    LOG_INFO("Starting C++ Concepts tests");
    
    // test different types with our concepts
    test_type<int>();
    test_type<double>();
    test_type<std::string>();
    
    // test the Calculator class
    test_calculator();
    
    // test the sum function
    test_sum();
    
    LOG_INFO("All tests completed successfully");
    return 0;
}
