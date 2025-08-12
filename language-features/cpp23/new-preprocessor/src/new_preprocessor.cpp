// MIT License
// Copyright (c) 2025 dbjwhs

#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include <format>
#include "../../../../headers/project_utils.hpp"

// Define some test macros for demonstration
#define CPP_VERSION 23
#define COMPILER_GCC 1
//#define COMPILER_CLANG 1  // Commented for testing different paths
//#define COMPILER_MSVC 1   // Commented for testing different paths

// =================================================================================
// C++23 Preprocessor Directives: #elifdef, #elifndef, and #warning
// =================================================================================
// History: The #elifdef and #elifndef directives were introduced in C++23 standard
// (ISO/IEC 14882:2024). They originated from the C23 standard proposal (WG14) and
// were subsequently adopted into C++ via proposal P2334R1. These directives serve
// as shortcuts for the commonly used patterns "#elif defined(MACRO)" and
// "#elif !defined(MACRO)" respectively.
//
// The #warning directive was also standardized in C++23 through proposal P2437R1,
// although many compilers supported it as an extension before standardization.
// It generates a compiler warning without halting compilation, unlike #error.
//
// These directives simplify conditional compilation chains, making preprocessor
// logic more readable and reducing verbosity in preprocessor conditional blocks.

// Simple function to test compiler detection
std::string detect_compiler() {
    #ifdef COMPILER_GCC
        return "GCC";
    #elifdef COMPILER_CLANG  // C++23 feature: shorthand for #elif defined(COMPILER_CLANG)
        return "Clang";
    #elifdef COMPILER_MSVC   // C++23 feature: shorthand for #elif defined(COMPILER_MSVC)
        return "MSVC";
    #else
        return "Unknown";
    #endif
}

// Function to test feature availability based on a language version
std::string check_feature_support(const std::string& feature_name) {
    // Using #warning to indicate a potential issue
    #warning "Feature detection is simplified for demonstration purposes"

    #ifdef CPP_VERSION
        #if CPP_VERSION >= 23
            return feature_name + " is supported in C++23";
        #elifdef EXPERIMENTAL_FEATURES  // C++23 feature: shorthand for #elif defined(EXPERIMENTAL_FEATURES)
            return feature_name + " is supported as experimental";
        #else
            return feature_name + " is not supported";
        #endif
    #else
        #warning "CPP_VERSION not defined, assuming C++23 features are unavailable"
        return "C++ version not defined";
    #endif
}

// Function to test CPU architecture detection using #elifndef
std::string detect_architecture() {
    #ifdef x86_64
        return "x86_64";
    #elifdef ARM64
        return "ARM64";
    #elifndef x86_32  // C++23 feature: shorthand for #elif !defined(x86_32)
        // This block executes if x86_32 is NOT defined (and previous conditions were false)
        return "Architecture is not x86_32";
    #else
        return "Unknown architecture";
    #endif
}

// Test nested conditionals with the new directives
std::vector<std::string> get_supported_features() {
    std::vector<std::string> features{};
    features.reserve(5);  // Reserve space as per requirement

    #ifdef CPP_VERSION
        #if CPP_VERSION >= 23
            features.emplace_back("elifdef");
            features.emplace_back("elifndef");
            features.emplace_back("warning");

            #ifdef COMPILER_GCC
                features.emplace_back("GCC extensions");
            #elifndef COMPILER_MSVC  // C++23 feature: shorthand for #elif !defined(COMPILER_MSVC)
                features.emplace_back("Non-MSVC extensions");
            #endif
        #else
            #warning "Using C++23 features with older C++ standard may lead to portability issues"
            features.emplace_back("Limited feature set");
        #endif
    #endif

    return features;
}

// Main function with tests
int main() {
    LOG_INFO_PRINT("C++23 Preprocessor Directives Example");

    // Test compiler detection
    const std::string compiler = detect_compiler();
    LOG_INFO_PRINT("Detected compiler: {}", compiler);
    assert(compiler == "GCC" && "Expected GCC compiler");

    // Test feature support check
    const std::string feature_status = check_feature_support("elifdef");
    LOG_INFO_PRINT("Feature status: {}", feature_status);
    assert(feature_status == "elifdef is supported in C++23" && "Feature should be supported");

    // Test architecture detection with #elifndef
    const std::string architecture = detect_architecture();
    LOG_INFO_PRINT("Architecture detection: {}", architecture);
    assert(architecture == "Architecture is not x86_32" && "Expected non-x86_32 architecture");

    // Test nested conditionals with the new directives
    const std::vector<std::string> features = get_supported_features();
    LOG_INFO_PRINT("Supported features:");
    for (const auto & feature : features) {
        LOG_INFO_PRINT("  - {}", feature);
    }
    assert(features.size() >= 3 && "Expected at least 3 supported features");

    // Test for specific feature presence
    bool has_elifdef = false;
    for (const auto & feature : features) {
        if (feature == "elifdef") {
            has_elifdef = true;
            break;
        }
    }
    assert(has_elifdef && "elifdef should be in supported features");

    LOG_INFO_PRINT("All tests passed successfully!");
    return 0;
}
