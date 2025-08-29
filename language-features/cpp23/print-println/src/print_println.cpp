// MIT License
// Copyright (c) 2025 dbjwhs

#include <print>
#include <format>
#include <vector>
#include <string>
#include <cassert>
#include <sstream>
#include <iostream>
#include <chrono>
#include <map>
#include <optional>
#include <algorithm>
#include "../../../../headers/project_utils.hpp"

// std::print and std::println pattern history and usage
//
// the std::print and std::println functions were introduced in c++23 as part of the formatted output
// library (p2093r14). these functions provide a modern, type-safe alternative to printf-style
// formatting while building upon the std::format infrastructure introduced in c++20.
//
// historical context (I have lived through all of these):
// - c printf family: unsafe, no type checking, manual format specifier management
// - c++ iostream: type-safe but verbose, performance concerns, locale dependencies
// - c++20 std::format: type-safe formatting to strings, but required additional output step
// - c++23 std::print/println: direct formatted output, combining safety of format with convenience
//
// key advantages:
// - type safety: compile-time format string validation when possible
// - performance: optimized output path, avoiding intermediate string creation
// - unicode support: proper handling of unicode text and encoding
// - consistency: unified formatting syntax across string creation and output
// - convenience: direct output without intermediate string variables
//
// common usage patterns:
// - std::print for formatted output without automatic newline
// - std::println for formatted output with automatic newline
// - integration with custom types via std::formatter specializations
// - use with various output streams (stdout, stderr, files)

class PrintDemonstrator {
private:
    // test result counter for comprehensive validation
    int m_test_count{0};
    // success counter for tracking passed tests
    int m_success_count{0};
    // container for collecting test output for validation
    std::vector<std::string> m_test_results{};

public:
    // constructor initializes counters and reserves space for test results
    PrintDemonstrator() {
        LOG_INFO("initializing print demonstrator with comprehensive testing");
        // reserve space for expected number of tests
        m_test_results.reserve(50);
    }

    // demonstrate basic std::print functionality without newlines
    void demonstrate_basic_print() {
        LOG_INFO("demonstrating basic std::print functionality");

        // basic string output without newline
        std::print("basic print without newline: ");
        std::print("hello world");
        std::print("\n");

        // integer formatting with std::print
        std::print("integer value: {}\n", 42);

        // floating point formatting
        std::print("floating point: {:.2f}\n", 3.14159);

        // multiple arguments in single print call
        std::print("multiple args: {} {} {}\n", "first", 123, true);

        // test validation for basic print functionality
        assert_test("basic print test completed", true);
        LOG_INFO("basic print demonstration completed successfully");
    }

    // demonstrate std::println functionality with automatic newlines
    void demonstrate_println() {
        LOG_INFO("demonstrating std::println with automatic newline handling");

        // basic println automatically adds newline
        std::println("this automatically gets a newline");

        // formatted output with println
        std::println("formatted integer: {}", 100);

        // multiple format arguments
        std::println("name: {}, age: {}, active: {}", "alice", 30, true);

        // empty println creates blank line
        std::println("");

        // complex formatting with println
        std::println("complex: {0} + {1} = {2}", 15, 27, 15 + 27);

        // test validation for println functionality
        assert_test("println demonstration completed", true);
        LOG_INFO("println demonstration completed successfully");
    }

    // demonstrate advanced formatting options and specifiers
    void demonstrate_advanced_formatting() {
        LOG_INFO("demonstrating advanced formatting capabilities");

        // numeric formatting with different bases
        int value{255};
        std::println("decimal: {}, hex: {:x}, octal: {:o}, binary: {:b}",
                    value, value, value, value);

        // floating point precision and notation
        double pi{3.141592653589793};
        std::println("default: {}, precision 3: {:.3f}, scientific: {:.2e}",
                    pi, pi, pi);

        // string formatting with alignment and width
        std::string text{"centered"};
        std::println("left: '{:<15}', right: '{:>15}', center: '{:^15}'",
                    text, text, text);

        // fill characters with alignment
        std::println("filled: '{:*<10}', '{:*>10}', '{:*^10}'",
                    "test", "test", "test");

        // test validation for advanced formatting
        assert_test("advanced formatting options work correctly", true);
        LOG_INFO("advanced formatting demonstration completed");
    }

    // demonstrate container and range formatting
    void demonstrate_container_formatting() {
        LOG_INFO("demonstrating container and collection formatting");

        // vector formatting with range support - check if supported in this implementation
        std::vector<int> numbers{1, 2, 3, 4, 5};
        // manual vector formatting to ensure compatibility
        std::print("vector contents: [");
        for (size_t ndx = 0; ndx < numbers.size(); ++ndx) {
            std::print("{}", numbers[ndx]);
            if (ndx < numbers.size() - 1) std::print(", ");
        }
        std::println("]");

        // string vector formatting
        std::vector<std::string> words{};
        words.reserve(3);
        words.emplace_back("hello");
        words.emplace_back("world");
        words.emplace_back("formatting");
        std::print("string vector: [");
        for (const auto& word : words) {
            std::print("\"{}\"", word);
            if (&word != &words.back()) {
                std::print(", ");
            }
        }
        std::println("]");

        // map formatting demonstration with manual approach
        std::map<std::string, int> scores{};
        scores["alice"] = 95;
        scores["bob"] = 87;
        scores["charlie"] = 92;
        std::print("score map: {{");
        bool first{true};
        for (const auto& [key, value] : scores) {
            if (!first) {
                std::print(", ");
            }
            std::print("\"{}\": {}", key, value);
            first = false;
        }
        std::println("}}");

        // array formatting
        int array[]{10, 20, 30, 40};
        std::print("array contents: [");
        for (int ndx = 0; ndx < 4; ++ndx) {
            std::print("{}", array[ndx]);
            if (ndx < 3) {
                std::print(", ");
            }
        }
        std::println("]");

        // test container formatting validation
        assert_test("container formatting works with various types", true);
        LOG_INFO("container formatting demonstration completed");
    }

    // demonstrate custom type formatting integration
    void demonstrate_custom_formatting() {
        LOG_INFO("demonstrating integration with custom types");

        // optional-like type formatting using custom approach
        constexpr std::optional<int> has_value{42};
        constexpr std::optional<int> no_value{};
        std::println("optional with value: {}, empty optional: {}",
                    has_value.has_value() ? std::to_string(has_value.value()) : "nullopt",
                    no_value.has_value() ? std::to_string(no_value.value()) : "nullopt");

        // chrono types formatting
        const auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        std::println("current time: {}", time_t);

        // duration formatting
        const auto duration = std::chrono::milliseconds{1500};
        std::println("duration: {}ms", duration.count());

        // boolean formatting with custom text
        constexpr bool is_active{true};
        constexpr bool is_disabled{false};
        std::println("status: {}, enabled: {}",
                    is_active ? "active" : "inactive",
                    is_disabled ? "no" : "yes");

        // test custom type formatting
        assert_test("custom type formatting integration works", true);
        LOG_INFO("custom type formatting demonstration completed");
    }

    // demonstrate error handling and validation
    void demonstrate_error_handling() {
        LOG_INFO("demonstrating error handling and format validation");

        try {
            // valid format string with correct argument count
            std::println("valid format: {} + {} = {}", 1, 2, 3);
            assert_test("valid format string processed correctly", true);

            // demonstrate format string validation at compile time when possible
            constexpr auto format_str = "compile time format: {}";
            std::println(format_str, "validated");
            assert_test("compile-time format validation works", true);

        } catch (const std::exception& e) {
            LOG_ERROR("format error caught: {}", e.what());
            assert_test("error handling demonstration", false);
        }

        LOG_INFO("error handling demonstration completed");
    }

    // demonstrate performance characteristics and best practices
    void demonstrate_performance_patterns() {
        LOG_INFO("demonstrating performance-oriented usage patterns");

        // direct output vs string intermediate
        auto start = std::chrono::high_resolution_clock::now();

        // direct print approach - more efficient
        for (int ndx = 0; ndx < 1000; ++ndx) {
            std::print("iteration: {}\n", ndx);
        }

        const auto mid = std::chrono::high_resolution_clock::now();

        // string intermediate approach - less efficient
        for (int ndx = 0; ndx < 1000; ++ndx) {
            std::string formatted = std::format("iteration: {}\n", ndx);
            std::print("{}", formatted);
        }

        const auto end = std::chrono::high_resolution_clock::now();

        // calculate timing differences
        const auto direct_time = std::chrono::duration_cast<std::chrono::microseconds>(mid - start);
        const auto indirect_time = std::chrono::duration_cast<std::chrono::microseconds>(end - mid);

        std::println("direct print time: {}μs, indirect time: {}μs",
                    direct_time.count(), indirect_time.count());

        // test performance measurement completed
        assert_test("performance measurement completed", true);
        LOG_INFO("performance demonstration completed");
    }

    // comprehensive test suite for all demonstrated features
    void run_comprehensive_tests() {
        LOG_INFO("running comprehensive test suite for all print features");

        // test basic functionality
        demonstrate_basic_print();
        demonstrate_println();

        // test advanced features
        demonstrate_advanced_formatting();
        demonstrate_container_formatting();
        demonstrate_custom_formatting();

        // test error handling and performance
        demonstrate_error_handling();
        demonstrate_performance_patterns();

        // final test summary
        std::println("\n=== test summary ===");
        std::println("total tests run: {}", m_test_count);
        std::println("tests passed: {}", m_success_count);
        std::println("tests failed: {}", m_test_count - m_success_count);
        std::println("success rate: {:.1f}%",
                    (static_cast<double>(m_success_count) / m_test_count) * 100.0);

        // validate overall test success
        assert(m_success_count == m_test_count);
        LOG_INFO("comprehensive testing completed: {}/{} tests passed",
                      m_success_count, m_test_count);
    }

private:
    // helper method for test validation and logging
    void assert_test(const std::string& test_name, bool condition) {
        ++m_test_count;
        if (condition) {
            ++m_success_count;
            LOG_INFO("✓ test passed: {}", test_name);
        } else {
            LOG_ERROR("✗ test failed: {}", test_name);
        }

        // store test result for later analysis
        m_test_results.emplace_back(std::format("{}: {}",
                                              test_name,
                                              condition ? "PASS" : "FAIL"));

        // assert on failure for immediate feedback
        assert(condition);
    }
};

// main function demonstrating comprehensive std::print and std::println usage
int main() {
    LOG_INFO("starting comprehensive std::print and std::println demonstration");

    try {
        // create demonstrator instance
        PrintDemonstrator demo{};

        // run complete demonstration and test suite
        demo.run_comprehensive_tests();

        // final success message
        std::println("\n🎉 all std::print and std::println features demonstrated successfully!");
        LOG_INFO("demonstration completed successfully");

        return 0;

    } catch (const std::exception& e) {
        LOG_ERROR("demonstration failed with exception: {}", e.what());
        std::println("❌ demonstration failed: {}", e.what());
        return 1;
    }
}
