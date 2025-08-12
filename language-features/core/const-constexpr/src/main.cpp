// MIT License
// Copyright (c) 2025 dbjwhs

#include <iostream>
#include <cassert>
#include <string>
#include <type_traits>
#include <array>
#include <format>
#include "../../../../headers/project_utils.hpp"

#define CONST_CONSTEXPR_DEMO_VERSION "1.0.0"
#define CONST_CONSTEXPR_DEMO_AUTHOR "dbjwhs"

// const and constexpr in c++
// -------------------------------------------------------------------------
// const: introduced in c++98, const is used to declare objects or member functions that cannot be modified
// after initialization. it enforces immutability at runtime and compile-time for literals.
//
// constexpr: introduced in c++11, enhanced in c++14 and c++17, constexpr is used to enable computations
// at compile-time rather than runtime. it guarantees that the evaluation can happen during compilation,
// which can lead to performance optimizations.
//
// common usages:
// - const: immutable variables, function parameters, return types, member functions, and pointers/references
// - constexpr: compile-time constants, functions, constructors, and complex computations that can be
//   evaluated at compile-time to improve runtime performance
//
// evolution:
// - c++11: introduced constexpr with limitations (single return statement)
// - c++14: relaxed constexpr function restrictions (allowing loops, conditions, etc.)
// - c++17: added constexpr if, lambda expressions, and enhanced standard library support
// - c++20: further expanded constexpr support to include try-catch, virtual calls, and more
// -------------------------------------------------------------------------

// basic usage of const
void demonstrate_basic_const()
{
    LOG_INFO("Demonstrating basic const usage");

    // const variables - cannot be modified after initialization
    const int kImmutableValue = 42;
    // kImmutableValue = 43;  // error: assignment of read-only variable

    // const reference - cannot modify the referenced object
    int mutable_value = 100;
    const int& kRefToValue = mutable_value;
    // kRefToValue = 101;  // error: assignment of read-only reference

    // proving const reference doesn't prevent changing the original variable
    mutable_value = 101;
    assert(kRefToValue == 101);
    LOG_INFO(std::format("Original value changed to {}, const reference shows {}",
                                                         mutable_value, kRefToValue));

    // const pointers vs. pointers to const
    int value1 = 10;
    int value2 = 20;

    // pointer to const - pointer can be reassigned, but cannot modify pointee
    const int* ptr_to_const = &value1;
    // *ptr_to_const = 11;  // error: assignment of read-only location
    ptr_to_const = &value2;  // valid: pointer itself can change

    // const pointer - pointer cannot be reassigned, but can modify pointee
    int* const const_ptr = &value1;
    *const_ptr = 11;  // valid: can modify the value
    // const_ptr = &value2;  // error: assignment of read-only variable

    // const pointer to const - neither pointer nor pointee can be modified
    const int* const const_ptr_to_const = &value2;
    // *const_ptr_to_const = 21;  // error: assignment of read-only location
    // const_ptr_to_const = &value1;  // error: assignment of read-only variable

    LOG_INFO("Basic const tests passed");
}

// const with function parameters and return values
const std::string get_message(const int& value)
{
    // const parameter prevents modification of the input
    // const return type prevents modification of the returned object
    return "The value is: " + std::to_string(value);
}

// const member functions
class ConstExample
{
private:
    int m_value;

public:
    // constructor
    explicit ConstExample(int value) : m_value(value) {}

    // non-const member function - can modify member variables
    void set_value(int new_value)
    {
        m_value = new_value;
    }

    // const member function - cannot modify member variables
    int get_value() const
    {
        // m_value = 42;  // error: assignment of member in read-only object
        return m_value;
    }

    // mutable member variables can be modified even in const member functions
    class WithMutable
    {
    private:
        int m_value;
        mutable int m_access_count;

    public:
        explicit WithMutable(int value) : m_value(value), m_access_count(0) {}

        int get_value() const
        {
            m_access_count++;  // valid despite const because m_access_count is mutable
            return m_value;
        }

        int get_access_count() const
        {
            return m_access_count;
        }
    };
};

void demonstrate_const_functions()
{
    LOG_INFO("Demonstrating const with functions");

    const int kValue = 100;
    std::string message = get_message(kValue);
    LOG_INFO(std::format("Got message: {}", message));

    // const objects can only call const member functions
    const ConstExample kConstObj(42);
    int value = kConstObj.get_value();  // valid: get_value() is const
    // kConstObj.set_value(43);  // error: cannot call non-const function on const object

    // non-const objects can call both const and non-const member functions
    ConstExample mutable_obj(42);
    mutable_obj.set_value(43);
    value = mutable_obj.get_value();

    // testing mutable members in const context
    ConstExample::WithMutable with_mutable(50);
    const ConstExample::WithMutable kConstWithMutable(50);

    // access the const object multiple times
    for (int i = 0; i < 5; i++) {
        kConstWithMutable.get_value();
    }

    assert(kConstWithMutable.get_access_count() == 5);
    LOG_INFO(std::format("Access count on const object: {}",
                                                         kConstWithMutable.get_access_count()));

    LOG_INFO("Const function tests passed");
}

// basic usage of constexpr
constexpr int square(int x)
{
    return x * x;
}

void demonstrate_basic_constexpr()
{
    LOG_INFO("Demonstrating basic constexpr usage");

    // constexpr variables - evaluated at compile-time
    constexpr int kSquareOfFive = square(5);
    static_assert(kSquareOfFive == 25, "Square of 5 should be 25");

    // constexpr can be used with variables
    int runtime_value = 10;
    int sq = square(runtime_value);  // evaluated at runtime

    // constexpr for compile-time array sizing
    constexpr int kArraySize = square(4);
    int array[kArraySize];  // array of size 16, determined at compile-time
    static_assert(sizeof(array) / sizeof(int) == 16, "Array size should be 16");

    // constexpr with standard types
    constexpr double kPi = 3.14159265358979323846;
    constexpr bool kIsDebug = false;

    LOG_INFO(std::format("kSquareOfFive = {}, runtime square = {}",
                                                         kSquareOfFive, sq));
    LOG_INFO("Basic constexpr tests passed");
}

// more complex constexpr functions (c++14 and beyond)
constexpr int factorial(int n)
{
    // c++14 allows more complex constexpr functions
    if (n <= 1) {
        return 1;
    }
    return n * factorial(n - 1);
}

constexpr bool is_prime(int n, int i = 2)
{
    // recursive prime checking (c++14 style)
    return n <= 1 ? false : (i * i > n ? true : (n % i == 0 ? false : is_prime(n, i + 1)));
}

void demonstrate_advanced_constexpr()
{
    LOG_INFO("Demonstrating advanced constexpr usage");

    // constexpr functions with complex logic
    constexpr int kFact5 = factorial(5);
    static_assert(kFact5 == 120, "Factorial of 5 should be 120");

    constexpr bool kIsPrime17 = is_prime(17);
    constexpr bool kIsPrime16 = is_prime(16);
    static_assert(kIsPrime17, "17 should be prime");
    static_assert(!kIsPrime16, "16 should not be prime");

    // checking at runtime as well
    for (int i = 2; i <= 10; i++) {
        LOG_INFO(std::format("Is {} prime? {}", i, is_prime(i) ? "Yes" : "No"));
    }

    // constexpr with arrays
    constexpr std::array<int, 5> kFactorials = {
        factorial(1),
        factorial(2),
        factorial(3),
        factorial(4),
        factorial(5)
    };

    static_assert(kFactorials[4] == 120, "Factorial of 5 should be 120");

    LOG_INFO("Advanced constexpr tests passed");
}

// constexpr class (c++11 and later)
class ConstexprPoint
{
private:
    int m_x;
    int m_y;

public:
    // constexpr constructor
    constexpr ConstexprPoint(int x, int y) : m_x(x), m_y(y) {}

    // constexpr getter methods
    constexpr int get_x() const { return m_x; }
    constexpr int get_y() const { return m_y; }

    // constexpr methods (c++14 and later allow more complex constexpr methods)
    constexpr ConstexprPoint add(const ConstexprPoint& other) const
    {
        return ConstexprPoint(m_x + other.m_x, m_y + other.m_y);
    }

    constexpr double distance_from_origin() const
    {
        return std::sqrt(m_x * m_x + m_y * m_y);
    }

    // constexpr operator
    constexpr bool operator==(const ConstexprPoint& other) const
    {
        return m_x == other.m_x && m_y == other.m_y;
    }
};

void demonstrate_constexpr_classes()
{
    LOG_INFO("Demonstrating constexpr classes");

    // constexpr objects
    constexpr ConstexprPoint kP1(3, 4);
    constexpr ConstexprPoint kP2(1, 2);

    // compile-time operations on constexpr objects
    constexpr ConstexprPoint kP3 = kP1.add(kP2);
    static_assert(kP3.get_x() == 4, "P3.x should be 4");
    static_assert(kP3.get_y() == 6, "P3.y should be 6");

    // compile-time comparison
    constexpr bool kPointsEqual = (kP1 == kP2);
    static_assert(!kPointsEqual, "P1 and P2 should not be equal");

    // runtime operations using the same constexpr class
    ConstexprPoint p4(5, 6);
    ConstexprPoint p5 = p4.add(kP1);

    LOG_INFO(std::format("P5 coordinates: ({}, {})", p5.get_x(), p5.get_y()));
    LOG_INFO(std::format("Distance of P1 from origin: {}", kP1.distance_from_origin()));

    LOG_INFO("Constexpr class tests passed");
}

// constexpr if (c++17)
// use enum for type categories to allow compile-time comparison
enum class TypeCategory { Integral, FloatingPoint, String, Other };

template <typename T>
constexpr TypeCategory get_value_type_category()
{
    // constexpr if allows compile-time branching in templates
    if constexpr (std::is_integral_v<T>) {
        return TypeCategory::Integral;
    }
    else if constexpr (std::is_floating_point_v<T>) {
        return TypeCategory::FloatingPoint;
    }
    else if constexpr (std::is_same_v<T, std::string>) {
        return TypeCategory::String;
    }
    else {
        return TypeCategory::Other;
    }
}

// helper function to convert enum to string (for display purposes)
constexpr const char* type_category_to_string(TypeCategory category)
{
    switch (category) {
        case TypeCategory::Integral:
            return "integral";
        case TypeCategory::FloatingPoint:
            return "floating point";
        case TypeCategory::String:
            return "string";
        case TypeCategory::Other:
            return "other";
    }
    return "unknown"; // To satisfy compiler, never reached
}

void demonstrate_constexpr_if()
{
    LOG_INFO("Demonstrating constexpr if (C++17)");

    // during compilation, only the relevant branch is instantiated
    constexpr TypeCategory kIntCategory = get_value_type_category<int>();
    constexpr TypeCategory kDoubleCategory = get_value_type_category<double>();
    constexpr TypeCategory kStringCategory = get_value_type_category<std::string>();
    constexpr TypeCategory kBoolCategory = get_value_type_category<bool>();

    // now we can compare enums in static_assert which works correctly
    static_assert(kIntCategory == TypeCategory::Integral, "int should be integral");
    static_assert(kDoubleCategory == TypeCategory::FloatingPoint, "double should be floating point");
    static_assert(kStringCategory == TypeCategory::String, "std::string should be string");
    static_assert(kBoolCategory == TypeCategory::Integral, "bool should be integral");

    // for logging, convert to strings
    LOG_INFO(std::format("int type: {}", type_category_to_string(kIntCategory)));
    LOG_INFO(std::format("double type: {}", type_category_to_string(kDoubleCategory)));
    LOG_INFO(std::format("string type: {}", type_category_to_string(kStringCategory)));
    LOG_INFO(std::format("bool type: {}", type_category_to_string(kBoolCategory)));

    LOG_INFO("Constexpr if tests passed");
}

// putting it all together: a comprehensive example
template <typename T, size_t N>
class ConstexprArray
{
private:
    std::array<T, N> m_data;

public:
    // constexpr constructor with initializer list
    template <typename... Args>
    constexpr ConstexprArray(Args... args) : m_data{args...}
    {
        static_assert(sizeof...(args) == N, "Number of arguments must match array size");
    }

    // constexpr constructor from std::array
    constexpr ConstexprArray(const std::array<T, N>& arr) : m_data(arr) {}

    // constexpr element access
    constexpr const T& at(size_t index) const
    {
        // bounds checking at compile time when possible
        if (index >= N) {
            throw std::out_of_range("Index out of bounds");
        }
        return m_data[index];
    }

    // constexpr size
    constexpr size_t size() const { return N; }

    // constexpr algorithm: sum of elements
    constexpr T sum() const
    {
        T result = {};
        for (size_t i = 0; i < N; ++i) {
            result += m_data[i];
        }
        return result;
    }

    // constexpr algorithm: find maximum element
    constexpr T max() const
    {
        if (N == 0) {
            throw std::logic_error("Cannot find max of empty array");
        }

        T max_val = m_data[0];
        for (size_t i = 1; i < N; ++i) {
            if (m_data[i] > max_val) {
                max_val = m_data[i];
            }
        }
        return max_val;
    }

    // constexpr transformation
    template <typename Func>
    constexpr ConstexprArray<std::invoke_result_t<Func, T>, N> transform(Func func) const
    {
        // cannot use std::transform in constexpr prior to c++20
        std::array<std::invoke_result_t<Func, T>, N> result;
        for (size_t i = 0; i < N; ++i) {
            result[i] = func(m_data[i]);
        }
        return ConstexprArray<std::invoke_result_t<Func, T>, N>(result);
    }
};

void demonstrate_comprehensive_example()
{
    LOG_INFO("Demonstrating comprehensive constexpr/const example");

    // compile-time creation and operations on array
    constexpr ConstexprArray<int, 5> kArrayOne(1, 2, 3, 4, 5);
    constexpr int kSum = kArrayOne.sum();
    constexpr int kMax = kArrayOne.max();

    static_assert(kSum == 15, "Sum should be 15");
    static_assert(kMax == 5, "Max should be 5");

    // constexpr lambda (c++17)
    constexpr auto kSquareLambda = [](int x) constexpr { return x * x; };

    // transform array at compile-time using constexpr lambda
    constexpr auto kSquaredArray = kArrayOne.transform(kSquareLambda);
    constexpr int kSquaredSum = kSquaredArray.sum();

    static_assert(kSquaredSum == 55, "Sum of squared values should be 55");

    // runtime operations on the same class
    ConstexprArray<double, 4> floating_array(1.1, 2.2, 3.3, 4.4);
    double runtime_sum = floating_array.sum();

    LOG_INFO(std::format("Compile-time sum: {}", kSum));
    LOG_INFO(std::format("Compile-time max: {}", kMax));
    LOG_INFO(std::format("Compile-time sum of squares: {}", kSquaredSum));
    LOG_INFO(std::format("Runtime sum: {}", runtime_sum));

    LOG_INFO("Comprehensive example tests passed");
}

int main()
{
    LOG_INFO("Starting const/constexpr demonstration");
    LOG_INFO(std::format("Version: {}", CONST_CONSTEXPR_DEMO_VERSION));

    // run all demonstrations
    demonstrate_basic_const();
    demonstrate_const_functions();
    demonstrate_basic_constexpr();
    demonstrate_advanced_constexpr();
    demonstrate_constexpr_classes();
    demonstrate_constexpr_if();
    demonstrate_comprehensive_example();

    LOG_INFO("All demonstrations completed successfully");
    return 0;
}
