// MIT License
// Copyright (c) 2025 dbjwhs

// define the primary module interface
export module math_module;

// standard library imports
// Note: Clang modules may require different import syntax for std libs
#include <stdexcept>
#include <string>

// export a namespace to organize our module contents
export namespace math {
    // export a constant that will be available to importers
    export constexpr double PI = 3.14159265359;

    // export a class that provides basic math operations
    export class Calculator {
    public:
        // default constructor
        Calculator() = default;

        // destructor
        ~Calculator() = default;

        // add two integers and return the result
        int add(int a, int b) const {
            return a + b;
        }

        // subtract b from a and return the result
        int subtract(int a, int b) const {
            return a - b;
        }

        // multiply two integers and return the result
        int multiply(int a, int b) const {
            return a * b;
        }

        // divide a by b and return the result
        // throws an exception if b is zero
        int divide(int a, int b) const {
            if (b == 0) {
                throw std::invalid_argument("Division by zero is not allowed");
            }
            return a / b;
        }

    private:
        // private helper method that's not exported and not accessible outside
        int m_privateHelper(int x) const {
            return x * x;
        }

        // private member variable following naming convention
        int m_lastResult = 0;
    };
}

// Note: Defining the advanced partition in the same file for Clang compatibility
export module math_module:advanced;

// export functions in the advanced partition
export namespace math {
    // calculate x raised to the power of y
    export int power(int x, int y) {
        int result = 1;
        for (int ndx = 0; ndx < y; ndx++) {
            result *= x;
        }
        return result;
    }

    // calculate the factorial of n
    export int factorial(int n) {
        if (n <= 1) {
            return 1;
        }
        return n * factorial(n - 1);
    }
}
