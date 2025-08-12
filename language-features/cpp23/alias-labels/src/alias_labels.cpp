// MIT License
// Copyright (c) 2025 dbjwhs

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <cassert>
#include "../../../../headers/project_utils.hpp"

// ======================================================================================
// C++23 Feature: Labels at end of compound statements and alias declarations in init-statements
// ======================================================================================
// History:
// This feature was proposed in P1063 (Labels at the End of Compound Statements) by Bjarne
// Stroustrup and adopted in C++23. The ability to place labels at the end of compound statements
// improves the readability of complex control flow structures, particularly in state machines and
// code that utilizes goto statements. It also allows for more natural placement of labels at logical
// points of completion rather than only at the beginning of statements.
//
// The alias declarations in init-statements feature extends the existing init-statement functionality
// introduced in C++17 (which allowed declarations in if and switch statements) to include using declarations.
// This enables more localized type alias definitions, improving encapsulation and reducing naming conflicts.
//
// Both features enhance code locality and maintainability while reducing scope pollution.
// ======================================================================================
//
// ======================================================================================
// The Philosophy of Code Locality and The Dangers of Scope Pollution
// ======================================================================================
// Code Locality:
// The principle of code locality is a foundational concept in software engineering that advocates
// for keeping related code elements physically close to each other within the source code. This
// proximity creates a cognitive cohesion that significantly enhances comprehension, maintainability,
// and debugging efficiency. When declarations, definitions, and their usages are placed in close
// proximity, developers can more easily trace the flow of data and control, reducing the mental
// overhead required to understand the code's behavior. Good locality enables a programmer to
// understand a section of code without having to jump to distant parts of the codebase, which
// becomes particularly valuable as systems grow in complexity. The C++23 features demonstrated
// in this file directly support improved code locality by allowing type aliases and control
// flow markers to be defined precisely where they are needed and used.
//
// Scope Pollution:
// Scope pollution refers to the unnecessary broadening of identifier visibility beyond their
// logical usage area, creating a cluttered namespace that leads to several serious problems:
//
// 1. Name Collisions: When identifiers exist in broader scopes than necessary, the likelihood
//    of unintentional name collisions increases dramatically, potentially leading to subtle bugs
//    that are challenging to diagnose. These collisions might manifest as shadowed variables,
//    accidental reuse of already-defined names, or naming conflicts in headers.
//
// 2. Reduced Code Clarity: Overly broad scopes obscure the relationships between declarations and
//    their usages, making it more difficult to trace data flow and understand variable lifecycles.
//    When a reader must search through large scopes to find where a variable is defined,
//    comprehension becomes fragmented and error-prone.
//
// 3. Increased Cognitive Load: Each identifier in a given scope is an element that must be held in
//    a programmer's working memory. Polluted scopes significantly increase this cognitive burden,
//    making code more difficult to understand, maintain, and debug correctly.
//
// 4. Lifetime Management Issues: Broader scopes typically imply longer lifetimes for objects, which
//    can lead to inefficient resource utilization, dangling references, and difficult-to-track memory
//    management problems, particularly in complex or long-running applications.
//
// 5. Reduced Modularity: Scope pollution inherently violates encapsulation principles by exposing
//    implementation details more broadly than necessary, creating unwanted dependencies between
//    logically separate components and making code more difficult to refactor or extend.
//
// 6. Testing Challenges: Excessive scope breadth makes unit testing more complex because isolating
//    components becomes difficult when they reference identifiers from broader scopes, leading to
//    tests that are more fragile and less focused.
//
// C++23's alias declarations in init-statements directly combat scope pollution by allowing
// developers to limit the scope of type aliases precisely to where they are needed, rather than
// polluting broader scopes. Similarly, end-of-compound-statement labels improve locality by
// allowing control flow markers to be placed exactly where they make logical sense in the code
// structure.
// ======================================================================================

class LabelDemo {
private:
    // member variable storing test data
    std::map<std::string, std::vector<int>> m_testData{};

    // member variable tracking whether initialization is complete
    bool m_initialized{false};

public:
    LabelDemo() {
        // initialize the test data map with sample values
        m_testData["first"] = {1, 2, 3, 4, 5};
        m_testData["second"] = {10, 20, 30, 40, 50};
        m_testData["third"] = {100, 200, 300, 400, 500};
        m_initialized = true;
    }

    // demonstrates the use of labels at the end of compound statements
    [[nodiscard]] bool demonstrateEndLabels(const std::string& targetKey, int targetValue) const {
        LOG_INFO_PRINT("Testing labels at the end of compound statements");

        // using a label to mark the end of a try-block for cleaner flow control
        try {
            // check if the key exists before proceeding
            if (!m_testData.contains(targetKey)) {
                LOG_WARNING_PRINT("Key '{}' not found in test data", targetKey);
                goto cleanup;
            }

            // search for the target value in the appropriate vector
            const auto& values = m_testData.at(targetKey);

            // reserve space for results to avoid reallocation
            std::vector<int> matchedIndices;
            matchedIndices.reserve(values.size());

            for (int ndx = 0; ndx < static_cast<int>(values.size()); ++ndx) {
                if (values[ndx] == targetValue) {
                    LOG_INFO_PRINT("Found match at index {} with value {}", ndx, targetValue);
                    matchedIndices.emplace_back(ndx);
                }
            }

            // check if we found any matches
            if (matchedIndices.empty()) {
                LOG_WARNING_PRINT("Value {} not found in key {}", targetValue, targetKey);
                return false;
            }

            // output all matched indices
            LOG_INFO_PRINT("Found {} matches for value {}", matchedIndices.size(), targetValue);
            for (const auto& idx : matchedIndices) {
                LOG_INFO_PRINT("Match at index: {}", idx);
            }

            return true;
        } catch (const std::exception& e) {
            LOG_ERROR_PRINT("Exception caught: {}", e.what());
        } cleanup: // C++23 label at the end of the try block

        // any cleanup code would go here, guaranteed to be executed
        LOG_INFO_PRINT("Performing cleanup operations");
        return false;
    }

    // demonstrates the use of alias declarations in init-statements
    void demonstrateAliasInInit() {
        LOG_INFO_PRINT("Testing alias declarations in init-statements");

        // C++23 feature: alias declaration in if init-statement
        // this enhances code locality by defining the alias exactly where it's necessary
        // pre-C++23, this alias would have to be defined at a higher scope level,
        // potentially causing namespace pollution
        if (using ValueContainer = std::vector<int>; m_initialized) {
            LOG_INFO_PRINT("Using alias declaration in if init-statement");

            // the ValueContainer alias is only visible within this if block,
            // reducing scope pollution and improving code locality
            for (const auto& [key, values] : m_testData) {
                // explicit use of the local alias to demonstrate its availability
                ValueContainer filteredValues;
                filteredValues.reserve(values.size());

                // using range-based for loop per coding standards
                for (const auto& value : values) {
                    if (value % 2 == 0) {  // filter even numbers
                        filteredValues.emplace_back(value);
                    }
                }

                LOG_INFO_PRINT("Key: {} has {} even values", key, filteredValues.size());

                // testing with assertions
                if (key == "first") {
                    // the first vector has 2 even numbers: 2, 4
                    assert(filteredValues.size() == 2 && "Expected 2 even values in 'first'");
                } else if (key == "second") {
                    // the second vector has 5 even numbers: 10, 20, 30, 40, 50
                    assert(filteredValues.size() == 5 && "Expected 5 even values in 'second'");
                }
            }
        } else {
            LOG_ERROR_PRINT("Data not initialized");
        }

        // C++23 feature: alias declaration in for loop init-statement
        // this provides better context by keeping the alias definition close to its usage
        for (using KeyType = std::string; const auto& [key, values] : m_testData) {
            LOG_INFO_PRINT("Using alias declaration in for loop init-statement");

            // explicit use of KeyType to demonstrate alias is in scope
            KeyType keyUppercase = key;
            // convert to uppercase for demonstration
            for (auto& c : keyUppercase) {
                c = std::toupper(static_cast<unsigned char>(c));
            }

            LOG_INFO_PRINT("Processing key: {} (uppercase: {})", key, keyUppercase);

            // calculate sum of values
            int sum = 0;
            for (const auto& value : values) {
                sum += value;
            }

            LOG_INFO_PRINT("Sum of values for key {}: {}", key, sum);

            // verify calculations with assertions
            if (key == "first") {
                // sum of [1,2,3,4,5] = 15
                assert(sum == 15 && "Expected sum of 15 for 'first'");
            } else if (key == "second") {
                // sum of [10,20,30,40,50] = 150
                assert(sum == 150 && "Expected sum of 150 for 'second'");
            } else if (key == "third") {
                // sum of [100,200,300,400,500] = 1500
                assert(sum == 1500 && "Expected sum of 1500 for 'third'");
            }
        }
    }

    // demonstrates both features combined in a practical example
    void combinedDemo() {
        LOG_INFO_PRINT("Demonstrating both features combined");

        // using a switch statement with alias in an init-statement and labels at compound statement ends
        for (const auto& [key, values] : m_testData) {
            // alias declaration in a switch init-statement enhancing code locality
            switch (using Stats = struct { int min; int max; int sum; }; key[0]) {
                case 'f': {
                    LOG_INFO_PRINT("Processing 'first' key");

                    // initialize stats using the locally defined Stats alias
                    Stats stats{values[0], values[0], 0};

                    // calculate statistics
                    for (const auto& value : values) {
                        stats.min = std::min(stats.min, value);
                        stats.max = std::max(stats.max, value);
                        stats.sum += value;
                    }

                    LOG_INFO_PRINT("First key stats - Min: {}, Max: {}, Sum: {}",
                                        stats.min, stats.max, stats.sum);

                    // verify calculations
                    assert(stats.min == 1 && "Expected min of 1");
                    assert(stats.max == 5 && "Expected max of 5");
                    assert(stats.sum == 15 && "Expected sum of 15");

                    break;
                } case_f_end: // C++23 label at end of case block

                case 's': {
                    LOG_INFO_PRINT("Processing 'second' key");

                    // initialize stats using the locally defined Stats alias
                    Stats stats{values[0], values[0], 0};

                    // calculate statistics
                    for (const auto& value : values) {
                        stats.min = std::min(stats.min, value);
                        stats.max = std::max(stats.max, value);
                        stats.sum += value;
                    }

                    LOG_INFO_PRINT("Second key stats - Min: {}, Max: {}, Sum: {}",
                                        stats.min, stats.max, stats.sum);

                    // verify calculations
                    assert(stats.min == 10 && "Expected min of 10");
                    assert(stats.max == 50 && "Expected max of 50");
                    assert(stats.sum == 150 && "Expected sum of 150");

                    break;
                } case_s_end: // C++23 label at end of case block

                default: {
                    LOG_INFO_PRINT("Processing '{}' key", key);

                    // initialize stats using the locally defined Stats alias
                    Stats stats{values[0], values[0], 0};

                    // calculate statistics
                    for (const auto& value : values) {
                        stats.min = std::min(stats.min, value);
                        stats.max = std::max(stats.max, value);
                        stats.sum += value;
                    }

                    LOG_INFO_PRINT("Key stats - Min: {}, Max: {}, Sum: {}",
                                        stats.min, stats.max, stats.sum);

                    // For demonstration purposes, if we wanted to reprocess as if it were the 'first' case
                    // we could use: goto case_f_end;

                    break;
                } default_end: // C++23 label at the end of default block
            }
        }
    }
};

int main() {
    LOG_INFO_PRINT("Starting C++23 Features Demo");

    // create the demo object
    LabelDemo demo;

    // test the end labels feature
    LOG_INFO_PRINT("=== Testing Labels at End of Compound Statements ===");
    bool foundTarget = demo.demonstrateEndLabels("first", 3);
    assert(foundTarget && "Should find value 3 in 'first' key");

    foundTarget = demo.demonstrateEndLabels("second", 25);
    assert(!foundTarget && "Should not find value 25 in 'second' key");

    // test the alias in init statements feature
    LOG_INFO_PRINT("=== Testing Alias Declarations in Init-Statements ===");
    demo.demonstrateAliasInInit();

    // test both features combined
    LOG_INFO_PRINT("=== Testing Combined Features ===");
    demo.combinedDemo();

    LOG_INFO_PRINT("All tests completed successfully");
    return 0;
}
