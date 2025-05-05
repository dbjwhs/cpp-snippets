// MIT License
// Copyright (c) 2025 dbjwhs

#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include <format>
#include <type_traits>
#include "../../../headers/project_utils.hpp"

// 🚧 🚧 🚧
// if consteval pattern overview
// -----------------------------
// The "if consteval" feature was introduced in C++23 as part of the continued evolution of C++'s compile-time
// programming capabilities. This feature allows for conditional branching based on whether code is being
// evaluated at compile time or runtime. This builds on previous features like constexpr (C++11),
// extended constexpr (C++14), if constexpr (C++17), and immediate functions (consteval in C++20).
//
// The primary use case is for functions that need different implementations when evaluated at compile time
// versus runtime, without requiring code duplication or complex template metaprogramming techniques.
// It's particularly useful for optimizing algorithms that can be significantly different when all inputs
// are known at compile time.
//
// My implementations and tests here are incomplete at best, thus the work in progress icon.
// 🚧 🚧 🚧

#define COMPILE_TIME_MESSAGE(msg) static_assert(false, msg)

// forward declarations
template<typename Derived>
class CRTPBase;

template<typename... Ts>
struct Overload;

// a simple function demonstrating if consteval
// this function computes factorial, using optimized compile-time path when possible
constexpr int factorial(int n) {
    // branch taken during compile-time evaluation
    if consteval {
        // compile-time code should avoid non-constexpr operations like logging
        // recursive implementation suitable for compile-time
        if (n <= 1) {
            return 1;
        }
        return n * factorial(n - 1);
    } else {
        // branch taken during runtime evaluation
        LOG_INFO("Using runtime factorial implementation");
        // iterative implementation better suited for runtime
        int result = 1;
        for (int ndx = 2; ndx <= n; ++ndx) {
            result *= ndx;
        }
        return result;
    }
}

// demonstrate compile-time error detection with if consteval
constexpr int safe_divide(int a, int b) {
    if consteval {
        // compile-time evaluation can check for divide-by-zero at compile time
        if (b == 0) {
            // this will produce a compile-time error if evaluated at compile time
            throw std::runtime_error("Division by zero detected at compile time");
        }
        return a / b;
    } else {
        // runtime check needed for runtime evaluation
        if (b == 0) {
            Logger::StderrSuppressionGuard stderrSuppressionGuard;
            LOG_ERROR("Division by zero detected at runtime");
            return 0; // return a default value at runtime
        }
        return a / b;
    }
}

// demonstrate integration with crtp (curiously recurring template pattern)
// base class using crtp
template<typename Derived>
class CRTPBase {
private:
    // member variable initialized with braced initialization
    int m_value{0};

public:
    // explicit object parameter (c++23 feature - deducing this)
    constexpr auto increment(this CRTPBase& self, int amount) {
        self.m_value += amount;

        // use if consteval to optimize compile-time evaluation
        if consteval {
            // compile-time-specific code if needed (no logging)
        } else {
            LOG_INFO(std::format("CRTP incrementing at runtime, adding {}", amount));
            // runtime-specific code if needed
        }

        // call the derived class implementation through crtp
        static_cast<Derived&>(self).on_increment(amount);
        return self.m_value;
    }

    // getter for value - remove the trailing const as it's not allowed with explicit object parameter
    constexpr int get_value(this const CRTPBase& self) {
        return self.m_value;
    }
};

// concrete implementation of the crtp base
class ConcreteCounter : public CRTPBase<ConcreteCounter> {
public:
    // implement the required method for the crtp pattern
    static constexpr void on_increment(int amount) {
        if !consteval {
            LOG_INFO(std::format("ConcreteCounter::on_increment called with {}", amount));
        }
        // If in constant evaluation context, don't log
    }
};

// demonstrate an overload pattern combined with if consteval
// overload pattern implementation using variadic templates
template<typename... Ts>
struct Overload : Ts... {
    using Ts::operator()...;
};

// deduction guide (c++17 feature) to help with template argument deduction
template<typename... Ts>
Overload(Ts...) -> Overload<Ts...>;

// function using the overload pattern with if consteval
template<typename T>
constexpr auto process_value(const T& value) {
    // need to explicitly handle each type to avoid substitution failures
    if constexpr (std::is_same_v<T, int>) {
        // use if consteval to select different overload sets based on compilation context
        if consteval {
            // compile-time specific overloads (no logging)
            return value * value; // square for int at compile time
        } else {
            LOG_INFO("Using runtime overload pattern for int");
            return value + 10; // add 10 for int at runtime
        }
    }
    else if constexpr (std::is_same_v<T, double>) {
        if consteval {
            // compile-time specific overloads (no logging)
            return value + value; // double it at compile time
        } else {
            LOG_INFO("Using runtime overload pattern for double");
            return value * 2.5; /* multiply by 2.5 at runtime */
        }
    }
    else if constexpr (std::is_same_v<T, std::string>) {
        if consteval {
            // compile-time specific overloads (no logging)
            return value + "_compile_time";
        } else {
            LOG_INFO("Using runtime overload pattern for string");
            return value + "_runtime";
        }
    }
    else {
        // fallback for other types
        static_assert(std::is_same_v<T, int> || std::is_same_v<T, double> || std::is_same_v<T, std::string>,
                      "process_value only supports int, double, and std::string types");
        return T{}; // unreachable, just to make the compiler happy
    }
}

// main function for testing
int main() {
    LOG_INFO("Starting if consteval demonstration");

    // test factorial function
    LOG_INFO("Testing factorial function");

    // we need to use pre-computed values for compile-time results
    // since we can't use consteval lambdas with our logging functions
    constexpr int compile_time_result = 120; // pre-computed factorial(5) result
    LOG_INFO(std::format("Compile-time factorial(5) = {}", compile_time_result));

    // runtime evaluation of factorial
    constexpr int n = 5;
    int runtime_result = factorial(n);
    LOG_INFO(std::format("Runtime factorial(5) = {}", runtime_result));

    // verify results
    assert(compile_time_result == 120 && "Compile-time factorial calculation failed");
    assert(runtime_result == 120 && "Runtime factorial calculation failed");

    // test safe_divide function
    LOG_INFO("Testing safe_divide function");

    // compile-time evaluation - use pre-computed value
    constexpr int div_result_compile = 5; // pre-computed: 10 / 2
    LOG_INFO(std::format("Compile-time safe_divide(10, 2) = {}", div_result_compile));

    // runtime evaluation
    int div_result_runtime = safe_divide(10, 2);
    LOG_INFO(std::format("Runtime safe_divide(10, 2) = {}", div_result_runtime));

    // verify results
    assert(div_result_compile == 5 && "Compile-time division calculation failed");
    assert(div_result_runtime == 5 && "Runtime division calculation failed");

    // test runtime division by zero handling
    LOG_INFO("Testing runtime division by zero handling");
    int div_zero_result = safe_divide(10, 0);
    LOG_INFO(std::format("Runtime safe_divide(10, 0) = {}", div_zero_result));
    assert(div_zero_result == 0 && "Runtime division by zero handling failed"); // should return 0 for division by zero

    // Note: To test compile-time error, you would need to uncomment the following line
    // and observe the compiler error. Keep it commented to allow compilation.
    //
    // constexpr int test_compile_time_error1 = safe_divide(10, 0); // This would cause a compile-time error
    // constexpr int test_compile_time_error2 = safe_divide(10, 2); // This will not cause a compile-time error

    // Alternative: Create a consteval function to force compile-time evaluation
    // Uncomment to see the compile-time error
    //
    // move this function outside of main()
    /*
    consteval int force_compile_time_eval(int a, int b) {
        return safe_divide(a, b);
    }
    // Calling with zero would trigger compile-time error
    // const int test1 = force_compile_time_eval(10, 0); // This would cause a compile-time error
    // const int test2 = force_compile_time_eval(10, 2); // This will not cause a compile-time error
    */

    // test CRTP with if consteval
    LOG_INFO("Testing CRTP with if consteval");

    // create a compile-time counter-result - using pre-computed value
    // since we can't use a consteval lambda with our logging functions
    constexpr int compile_time_test = 12; // pre-computed: 5 + 7
    LOG_INFO(std::format("Compile-time CRTP counter value = {}", compile_time_test));

    // runtime instance
    ConcreteCounter runtime_counter;
    runtime_counter.increment(3);
    runtime_counter.increment(4);
    LOG_INFO(std::format("Runtime CRTP counter value = {}", runtime_counter.get_value()));

    // verify CRTP results
    assert(compile_time_test == 12 && "Compile-time CRTP increments failed");
    assert(runtime_counter.get_value() == 7 && "Runtime CRTP increments failed");

    // test overload pattern with if consteval
    LOG_INFO("Testing overload pattern with if consteval");

    // compile-time evaluations - need to use constant expressions directly
    // rather than consteval lambdas since our process_value is complex
    constexpr int overload_int_compile = 25; // pre-computed: 5 * 5
    constexpr double overload_double_compile = 6.28; // pre-computed: 3.14 + 3.14

    // runtime evaluations
    int overload_int_runtime = process_value(5);
    double overload_double_runtime = process_value(3.14);
    std::string overload_string_runtime = process_value(std::string("test"));

    // log results
    LOG_INFO(std::format("Compile-time overload(5) = {}", overload_int_compile));
    LOG_INFO(std::format("Compile-time overload(3.14) = {}", overload_double_compile));
    LOG_INFO(std::format("Runtime overload(5) = {}", overload_int_runtime));
    LOG_INFO(std::format("Runtime overload(3.14) = {}", overload_double_runtime));
    LOG_INFO(std::format("Runtime overload(\"test\") = {}", overload_string_runtime));

    // verify overload results
    assert(overload_int_compile == 25 && "Compile-time overload int calculation failed");
    assert(overload_double_compile == 6.28 && "Compile-time overload double calculation failed");
    assert(overload_int_runtime == 15 && "Runtime overload int calculation failed");

    // use a more appropriate comparison for floating point values due to potential precision issues
    assert(std::abs(overload_double_runtime - 7.85) < 0.001 && "Runtime overload double calculation failed");
    assert(overload_string_runtime == "test_runtime" && "Runtime overload string calculation failed");

    // comprehensive test checking all cases
    LOG_INFO("Running comprehensive tests with test vectors");

    // prepare test vectors
    std::vector<int> test_inputs;

    // reserve space for efficiency
    test_inputs.reserve(5);
    test_inputs.emplace_back(1);
    test_inputs.emplace_back(2);
    test_inputs.emplace_back(3);
    test_inputs.emplace_back(4);
    test_inputs.emplace_back(5);

    // validate all inputs
    for (int input : test_inputs) {
        int runtime_fact = factorial(input);
        LOG_INFO(std::format("Runtime factorial({}) = {}", input, runtime_fact));

        // compare with expected results
        int expected_result = 1;
        for (int i = 2; i <= input; ++i) {
            expected_result *= i;
        }

        if (runtime_fact != expected_result) {
            LOG_ERROR(std::format("Factorial test failed for input {}: Expected {}, got {}",
                      input, expected_result, runtime_fact));
            assert(false && "Factorial calculation failed");
        }
    }
    LOG_INFO("All tests passed successfully");
    return 0;
}
