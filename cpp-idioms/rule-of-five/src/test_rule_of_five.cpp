// MIT License
// Copyright (c) 2025 dbjwhs

#include "../headers/resource_manager.hpp"
#include "../headers/project_utils.hpp"
#include <cassert>
#include <print>
#include <memory>
#include <stdexcept>

using namespace rule_of_five;

// test fixture class for organizing tests
class RuleOfFiveTests {
private:
    static int m_test_count;
    static int m_passed_count;
    static int m_failed_count;

    static void log_test_result(const std::string& test_name, bool passed) {
        ++m_test_count;
        if (passed) {
            ++m_passed_count;
            LOG_INFO_PRINT("✓ test passed: {}", test_name);
            std::print("  ✓ {}\n", test_name);
        } else {
            ++m_failed_count;
            LOG_ERROR_PRINT("✗ test failed: {}", test_name);
            std::print("  ✗ {}\n", test_name);
        }
    }

public:
    // test default construction
    static void test_default_construction() {
        bool test_passed = false;
        try {
            ResourceManager<int> rm;
            test_passed = (rm.size() == 0 && rm.capacity() == 0 && rm.empty() && !rm.id().empty());
        } catch (...) {
            test_passed = false;
        }
        log_test_result("default construction", test_passed);
    }

    // test parameterized construction
    static void test_parameterized_construction() {
        bool test_passed = false;
        try {
            ResourceManager<int> rm{5};
            test_passed = (rm.size() == 0 && rm.capacity() == 5 && rm.empty() && !rm.id().empty()); // capacity 5, size starts at 0
        } catch (...) {
            test_passed = false;
        }
        log_test_result("parameterized construction", test_passed);
    }

    // test copy constructor
    static void test_copy_constructor() {
        bool test_passed = false;
        try {
            ResourceManager<int> original{3};
            auto result = original.push_back(42);
            assert(result.has_value());

            ResourceManager<int> copy{original};
            test_passed = (copy.size() == original.size() && 
                          copy.capacity() == original.capacity() &&
                          copy.id() != original.id()); // both should have same size after copying
        } catch (...) {
            test_passed = false;
        }
        log_test_result("copy constructor", test_passed);
    }

    // test copy assignment operator
    static void test_copy_assignment() {
        bool test_passed = false;
        try {
            ResourceManager<int> original{3};
            auto result = original.push_back(99);
            assert(result.has_value());

            ResourceManager<int> copy;
            copy = original;
            test_passed = (copy.size() == original.size() && 
                          copy.capacity() >= original.size() &&
                          copy.id() != original.id()); // assignment should match sizes
        } catch (...) {
            test_passed = false;
        }
        log_test_result("copy assignment operator", test_passed);
    }

    // test self-assignment
    static void test_self_assignment() {
        bool test_passed = false;
        try {
            ResourceManager<int> rm{2};
            std::string original_id = rm.id();
            std::size_t original_size = rm.size();

            rm = rm; // self-assignment
            test_passed = (rm.id() == original_id && rm.size() == original_size);
        } catch (...) {
            test_passed = false;
        }
        log_test_result("self assignment", test_passed);
    }

    // test move constructor
    static void test_move_constructor() {
        bool test_passed = false;
        try {
            auto create_temp = []() {
                ResourceManager<int> temp{4};
                auto result = temp.push_back(123);
                assert(result.has_value());
                return temp;
            };

            ResourceManager<int> moved = create_temp();
            test_passed = (moved.size() == 1 && moved.capacity() >= 4); // 0 initial + 1 added, capacity 4
        } catch (...) {
            test_passed = false;
        }
        log_test_result("move constructor", test_passed);
    }

    // test move assignment operator
    static void test_move_assignment() {
        bool test_passed = false;
        try {
            auto create_temp = []() {
                ResourceManager<int> temp{3};
                auto result = temp.push_back(456);
                assert(result.has_value());
                return temp;
            };

            ResourceManager<int> moved;
            moved = create_temp();
            test_passed = (moved.size() == 1 && moved.capacity() >= 3); // 0 initial + 1 added, capacity 3
        } catch (...) {
            test_passed = false;
        }
        log_test_result("move assignment operator", test_passed);
    }

    // test explicit move operations
    static void test_explicit_move() {
        bool test_passed = false;
        try {
            ResourceManager<int> source{2};
            auto result = source.push_back(789);
            assert(result.has_value());

            std::size_t original_size = source.size();
            ResourceManager<int> target = std::move(source);

            test_passed = (target.size() == original_size && 
                          source.size() == 0 && 
                          source.capacity() == 0);
        } catch (...) {
            test_passed = false;
        }
        log_test_result("explicit move operations", test_passed);
    }

    // test self-move-assignment
    static void test_self_move_assignment() {
        bool test_passed = false;
        try {
            ResourceManager<int> rm{3};
            std::string original_id = rm.id();
            std::size_t original_size = rm.size();

            rm = std::move(rm); // self-move-assignment
            test_passed = (rm.id() == original_id && rm.size() == original_size);
        } catch (...) {
            test_passed = false;
        }
        log_test_result("self move assignment", test_passed);
    }

    // test element access operations
    static void test_element_access() {
        bool test_passed = false;
        try {
            ResourceManager<int> rm{3};
            auto push_result = rm.push_back(100);
            assert(push_result.has_value());

            auto access_result = rm.at(0);
            test_passed = access_result.has_value();

            // test out of bounds access
            Logger::StderrSuppressionGuard suppress_stderr;
            auto out_of_bounds = rm.at(999);
            test_passed = test_passed && !out_of_bounds.has_value() && 
                         (out_of_bounds.error() == ResourceError::InvalidSize);
        } catch (...) {
            test_passed = false;
        }
        log_test_result("element access operations", test_passed);
    }

    // test error handling with railway-oriented programming
    static void test_error_handling() {
        bool test_passed = false;
        try {
            ResourceManager<int> empty_rm;
            
            Logger::StderrSuppressionGuard suppress_stderr;
            auto null_access = empty_rm.at(0);
            test_passed = !null_access.has_value() && 
                         (null_access.error() == ResourceError::NullPointer);

            // test error to string conversion
            std::string error_msg{error_to_string(ResourceError::AllocationFailure)};
            test_passed = test_passed && !error_msg.empty();
        } catch (...) {
            test_passed = false;
        }
        log_test_result("error handling with std::expected", test_passed);
    }

    // test push_back functionality
    static void test_push_back_functionality() {
        bool test_passed = false;
        try {
            ResourceManager<int> rm;
            assert(rm.empty());

            // test adding elements
            for (int ndx = 0; ndx < 5; ++ndx) {
                auto result = rm.push_back(ndx * 10);
                assert(result.has_value());
            }

            test_passed = (rm.size() == 5 && !rm.empty() && rm.capacity() >= 5); // 0 initial + 5 added
        } catch (...) {
            test_passed = false;
        }
        log_test_result("push_back functionality", test_passed);
    }

    // test template usage with different types
    static void test_template_types() {
        bool test_passed = false;
        try {
            // test with double
            ResourceManager<double> double_rm{2};
            auto double_result = double_rm.push_back(3.14159);
            assert(double_result.has_value());

            // test with string
            ResourceManager<std::string> string_rm{1};
            auto string_result = string_rm.push_back("test string");
            assert(string_result.has_value());

            test_passed = (double_rm.size() == 1 && string_rm.size() == 1); // 1 element added to each
        } catch (...) {
            test_passed = false;
        }
        log_test_result("template usage with different types", test_passed);
    }

    // test swap functionality
    static void test_swap_functionality() {
        bool test_passed = false;
        try {
            ResourceManager<int> rm1{2};
            auto result1 = rm1.push_back(10);
            assert(result1.has_value());

            ResourceManager<int> rm2{3};
            auto result2 = rm2.push_back(20);
            assert(result2.has_value());

            std::size_t rm1_original_size = rm1.size();
            std::size_t rm2_original_size = rm2.size();

            rm1.swap(rm2);

            test_passed = (rm1.size() == rm2_original_size && rm2.size() == rm1_original_size);
        } catch (...) {
            test_passed = false;
        }
        log_test_result("swap functionality", test_passed);
    }

    // run all tests
    static void run_all_tests() {
        LOG_INFO_PRINT("=== starting comprehensive rule of five unit tests ===");
        std::print("\nRunning Rule of Five Unit Tests:\n");
        std::print("================================\n");

        m_test_count = 0;
        m_passed_count = 0;
        m_failed_count = 0;

        test_default_construction();
        test_parameterized_construction();
        test_copy_constructor();
        test_copy_assignment();
        test_self_assignment();
        test_move_constructor();
        test_move_assignment();
        test_explicit_move();
        test_self_move_assignment();
        test_element_access();
        test_error_handling();
        test_push_back_functionality();
        test_template_types();
        test_swap_functionality();

        // print test summary
        std::print("\nTest Results Summary:\n");
        std::print("====================\n");
        std::print("Total tests run: {}\n", m_test_count);
        std::print("Tests passed:    {}\n", m_passed_count);
        std::print("Tests failed:    {}\n", m_failed_count);
        std::print("Success rate:    {:.1f}%\n", 
                  m_test_count > 0 ? (100.0 * m_passed_count / m_test_count) : 0.0);

        if (m_failed_count == 0) {
            std::print("\n🎉 All tests passed!\n");
            LOG_INFO_PRINT("all rule of five unit tests passed successfully");
        } else {
            std::print("\n❌ Some tests failed. Check logs for details.\n");
            LOG_ERROR_PRINT("{} out of {} tests failed", m_failed_count, m_test_count);
        }

        LOG_INFO_PRINT("completed rule of five unit tests: {}/{} passed", 
                      m_passed_count, m_test_count);
    }

    // get test results for main function
    static bool all_tests_passed() {
        return m_failed_count == 0;
    }
};

// initialize static members
int RuleOfFiveTests::m_test_count = 0;
int RuleOfFiveTests::m_passed_count = 0;
int RuleOfFiveTests::m_failed_count = 0;

int main() {
    std::print("Rule of Five - Comprehensive Unit Test Suite\n");
    std::print("============================================\n");

    // initialize logger for tests
    auto& logger = Logger::getInstance("./rule_of_five_tests.log");
    LOG_INFO_PRINT("starting rule of five comprehensive unit test suite");

    try {
        RuleOfFiveTests::run_all_tests();
        
        if (RuleOfFiveTests::all_tests_passed()) {
            LOG_INFO_PRINT("rule of five unit test suite completed successfully");
            std::print("\nCheck 'rule_of_five_tests.log' for detailed test execution logs.\n");
            return 0;
        } else {
            LOG_ERROR_PRINT("rule of five unit test suite completed with failures");
            std::print("\nSome tests failed. Check 'rule_of_five_tests.log' for detailed logs.\n");
            return 1;
        }
    } catch (const std::exception& e) {
        LOG_ERROR_PRINT("test suite failed with exception: {}", e.what());
        std::print("❌ Test suite failed with exception: {}\n", e.what());
        return 1;
    } catch (...) {
        LOG_ERROR_PRINT("test suite failed with unknown exception");
        std::print("❌ Test suite failed with unknown exception\n");
        return 1;
    }
}