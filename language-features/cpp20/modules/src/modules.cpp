// MIT License
// Copyright (c) 2025 dbjwhs

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// !!! COMPILER SUPPORT NOTICE                                                              !!!
// !!! This file was designed to use C++20 Modules, but current compiler support is limited !!!
// !!! As of April 2025, full module support requires:                                      !!!
// !!!   - GCC 14+ with -fmodules-ts                                                        !!!
// !!!   - Clang 16+ with -fmodules -fbuiltin-module-map                                    !!!
// !!!   - MSVC from VS 2022 17.4+ with /experimental:module                                !!!
// !!!   - CMake 3.28+ for proper build integration                                         !!!
// !!! This code is checked in as-is for future implementation when compiler support        !!!
// !!! becomes more widely available and standardized. I have wrestled with it enough and   !!!
// !!! could hack around some of the issues but I will wait...                              !!!
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


// Regular includes for standard library
#include <iostream>
#include <cassert>
#include <string>
#include <vector>
#include <format>
#include "../../../headers/project_utils.hpp"

// c++20 modules history and overview
// modules were introduced in c++20 as a solution to the long-standing issues with the preprocessor-based
// include system. the traditional #include mechanism simply performs textual inclusion, which leads to
// long compilation times, header inclusion order issues, and symbol leakage. modules provide encapsulation,
// faster compilation, and explicit interface control. they were first proposed in 2004, but took many
// years to standardize due to the complex interactions with existing c++ features. modules support
// explicit exports, better control over symbol visibility, and significantly improved compilation time
// by avoiding redundant parsing and allowing better parallelization of the build process.
// common usages include:
// - library development with clear api boundaries
// - large-scale projects with many components
// - performance-critical applications where build time matters
// - code that needs to control symbol visibility precisely

// import the math module
import math_module;

// import module partition
import math_module.advanced;  // Note: Using dot notation for Clang compatibility

int main() {
    // log the start of the program
    LOG_INFO("Starting C++20 Modules example program");

    // create an instance of our calculator class from the module
    math::Calculator calculator;

    // test basic arithmetic operations from the main module interface
    // addition
    int result = calculator.add(5, 3);
    LOG_INFO(std::format("Addition result: {}", result));
    assert(result == 8 && "Addition test failed");

    // subtraction
    result = calculator.subtract(10, 4);
    LOG_INFO(std::format("Subtraction result: {}", result));
    assert(result == 6 && "Subtraction test failed");

    // multiplication
    result = calculator.multiply(3, 7);
    LOG_INFO(std::format("Multiplication result: {}", result));
    assert(result == 21 && "Multiplication test failed");

    // division
    result = calculator.divide(20, 5);
    LOG_INFO(std::format("Division result: {}", result));
    assert(result == 4 && "Division test failed");

    // test error handling for division by zero
    try {
        result = calculator.divide(10, 0);
        // this should not execute
        assert(false && "Division by zero did not throw exception");
    } catch (const std::invalid_argument& e) {
        LOG_INFO(std::format("Caught expected exception: {}", e.what()));
        // test passed
    }

    // test functions from the advanced partition
    // power function
    result = math::power(2, 3);
    LOG_INFO(std::format("Power result: {}", result));
    assert(result == 8 && "Power test failed");

    // factorial function
    result = math::factorial(5);
    LOG_INFO(std::format("Factorial result: {}", result));
    assert(result == 120 && "Factorial test failed");

    // test a vector of operations to ensure consistent results
    std::vector<int> numbers = {1, 2, 3, 4, 5};
    int sum = 0;

    // use ndx for loop variable as requested
    for (int ndx = 0; ndx < numbers.size(); ndx++) {
        sum = calculator.add(sum, numbers[ndx]);
    }

    LOG_INFO(std::format("Sum of vector: {}", sum));
    assert(sum == 15 && "Vector sum test failed");

    // test the module-level constant
    LOG_INFO(std::format("Math module PI value: {}", math::PI));
    // compare with precision to account for floating point differences
    assert(std::abs(math::PI - 3.14159265359) < 0.0000001 && "PI constant test failed");

    // test that private implementation details are properly hidden
    // the following line would not compile if uncommented:
    // calculator.m_privateHelper(5);

    LOG_INFO("All tests passed successfully");

    return 0;
}
