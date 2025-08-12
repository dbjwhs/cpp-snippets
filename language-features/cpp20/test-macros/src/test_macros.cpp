// MIT License
// Copyright (c) 2025 dbjwhs

#include <iostream>
#include <string>
#include <cassert>
#include <type_traits>
#include <concepts>
#include <ranges>
#include <format>
#include "../../../../headers/project_utils.hpp"

// feature detection macros history and usage:
// c++20 introduced feature-testing macros defined in <version> header which allow developers to 
// detect at compile time whether the compiler supports specific c++20 features.
// these macros were standardized by iso/iec technical specification p0941r2 to address the issue of 
// detecting language and library features separately from compiler versions or platform-specific macros.
// they enable portable code that can adapt to different compilers and compiler versions without relying on
// vendor-specific detection mechanisms. each macro corresponds to a specific feature and is defined
// when that feature is available. the value of the macro indicates the version of the standard in which
// the feature was introduced (e.g., 201811 for c++20 features approved in november 2018).
// common usages include:
// 1. conditionally enabling modern code paths when available
// 2. providing fallback implementations for older compilers
// 3. ensuring that required features are available before compilation proceeds
// 4. building cross-platform libraries that work across different compiler versions

#include <version>

// main class to demonstrate feature detection macros
class FeatureTester {
private:
    // class instance counter
    static int m_instanceCount;
    
    // storage for feature test results
    bool m_hasConceptsSupport;
    bool m_hasRangesSupport;
    bool m_hasCoroutinesSupport;
    bool m_hasFormatSupport;
    bool m_hasSpanSupport;
    bool m_hasModulesSupport;
    bool m_hasThreeWayComparisonSupport;

public:
    // constructor that initializes feature detection results
    FeatureTester() {
        // increment instance counter
        m_instanceCount++;
        
        // log construction of the tester object
        LOG_INFO("constructing feature tester object");
        
        // check for concept support
        #ifdef __cpp_concepts
        m_hasConceptsSupport = true;
        LOG_INFO(std::format("concepts supported: {}", __cpp_concepts));
        #else
        m_hasConceptsSupport = false;
        LOG_INFO("concepts not supported");
        #endif

        // check for range support
        #ifdef __cpp_lib_ranges
        m_hasRangesSupport = true;
        LOG_INFO(std::format("ranges supported: {}", __cpp_lib_ranges));
        #else
        m_hasRangesSupport = false;
        LOG_INFO("ranges not supported");
        #endif

        // check for coroutines support
        #ifdef __cpp_coroutines
        m_hasCoroutinesSupport = true;
        LOG_INFO(std::format("coroutines supported: {}", __cpp_coroutines));
        #else
        m_hasCoroutinesSupport = false;
        LOG_INFO("coroutines not supported");
        #endif

        // check for std::format support
        #ifdef __cpp_lib_format
        m_hasFormatSupport = true;
        LOG_INFO(std::format("std::format supported: {}", __cpp_lib_format));
        #else
        m_hasFormatSupport = false;
        LOG_INFO("std::format not supported");
        #endif

        // check for std::span support
        #ifdef __cpp_lib_span
        m_hasSpanSupport = true;
        LOG_INFO(std::format("std::span supported: {}", __cpp_lib_span));
        #else
        m_hasSpanSupport = false;
        LOG_INFO("std::span not supported");
        #endif

        // check for module support
        #ifdef __cpp_modules
        m_hasModulesSupport = true;
        LOG_INFO(std::format("modules supported: {}", __cpp_modules));
        #else
        m_hasModulesSupport = false;
        LOG_INFO("modules not supported");
        #endif

        // check for three-way comparison support (spaceship operator)
        #ifdef __cpp_impl_three_way_comparison
        m_hasThreeWayComparisonSupport = true;
        LOG_INFO(std::format("three-way comparison supported: {}", __cpp_impl_three_way_comparison));
        #else
        m_hasThreeWayComparisonSupport = false;
        LOG_INFO("three-way comparison not supported");
        #endif
    }

    // destructor
    ~FeatureTester() {
        // log destruction of the tester object
        LOG_INFO("destroying feature tester object");
        
        // decrement instance counter
        m_instanceCount--;
    }

    // method to demonstrate conditional compilation based on feature availability
    static void demonstrateFeatures() {
        LOG_INFO("demonstrating detected c++20 features");

        // demonstrate concepts if available
        demonstrateConcepts();
        
        // demonstrate ranges if available
        demonstrateRanges();
        
        // demonstrate three-way comparison if available
        demonstrateThreeWayComparison();
        
        // demonstrate format if available
        demonstrateFormat();
    }

    // method to get instance count
    static int getInstanceCount() {
        return m_instanceCount;
    }

    // method to test if a specific feature is supported
    [[nodiscard]] bool isFeatureSupported(const std::string& featureName) const {
        if (featureName == "concepts") return m_hasConceptsSupport;
        if (featureName == "ranges") return m_hasRangesSupport;
        if (featureName == "coroutines") return m_hasCoroutinesSupport;
        if (featureName == "format") return m_hasFormatSupport;
        if (featureName == "span") return m_hasSpanSupport;
        if (featureName == "modules") return m_hasModulesSupport;
        if (featureName == "three-way-comparison") return m_hasThreeWayComparisonSupport;
        
        // unknown feature
        LOG_INFO(std::format("unknown feature requested: {}", featureName));
        return false;
    }

private:
    // helper method to demonstrate concepts feature
    static void demonstrateConcepts() {
        #ifdef __cpp_concepts
        LOG_INFO("demonstrating concepts feature");
        
        // define a simple concept for numeric types
        auto testConcept = []<typename T>() {
            if constexpr (std::integral<T>) {
                LOG_INFO("type is integral");
                return true;
            } else {
                LOG_INFO("type is not integral");
                return false;
            }
        };
        
        // test the concept with different types
        const bool intResult = testConcept.template operator()<int>();
        const bool doubleResult = testConcept.template operator()<double>();
        
        // verify results with assertions
        assert(intResult == true);
        assert(doubleResult == false);
        
        LOG_INFO("concepts demonstration completed");
        #else
        LOG_INFO("skipping concepts demonstration (not supported)");
        #endif
    }

    // helper method to demonstrate ranges feature
    static void demonstrateRanges() {
        #ifdef __cpp_lib_ranges
        LOG_INFO("demonstrating ranges feature");
        
        // create a simple vector for demonstration
        std::vector<int> numbers = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        
        // use ranges to filter and transform
        auto evenSquares = numbers | 
                          std::views::filter([](const int n) {
                              return n % 2 == 0;
                          }) |
                          std::views::transform([](const int n) {
                              return n * n;
                          });
        
        // output the results
        LOG_INFO("even squares from ranges:");
        for (int ndx = 0; auto val : evenSquares) {
            LOG_INFO(std::format("  [{}]: {}", ndx++, val));
            
            // verify results with assertions (4, 16, 36, 64, 100)
            switch(ndx-1) {
                case 0: assert(val == 4); break;
                case 1: assert(val == 16); break;
                case 2: assert(val == 36); break;
                case 3: assert(val == 64); break;
                case 4: assert(val == 100); break;
                default: assert(false); // should not reach here
            }
        }
        
        LOG_INFO("ranges demonstration completed");
        #else
        LOG_INFO("skipping ranges demonstration (not supported)");
        #endif
    }

    // helper method to demonstrate three-way comparison operator
    static void demonstrateThreeWayComparison() {
        #ifdef __cpp_impl_three_way_comparison
        LOG_INFO("demonstrating three-way comparison feature");
        
        // define a class that uses the three-way comparison operator
        struct Point {
            int x, y;
            
            // automatic three-way comparison (note; default to tell the compiler to generate the implementation automatically)
            auto operator<=>(const Point&) const = default;
        };
        
        // create some test points
        constexpr Point p1{1, 2};
        constexpr Point p2{1, 2};
        constexpr Point p3{3, 4};
        
        // perform comparisons
        bool equal = (p1 == p2);
        bool notEqual = (p1 != p3);
        bool less = (p1 < p3);
        
        // log and verify results
        LOG_INFO(std::format("p1 == p2: {}", equal));
        LOG_INFO(std::format("p1 != p3: {}", notEqual));
        LOG_INFO(std::format("p1 < p3: {}", less));
        
        // verify with assertions
        assert(equal);
        assert(notEqual);
        assert(less);
        
        LOG_INFO("three-way comparison demonstration completed");
        #else
        LOG_INFO("skipping three-way comparison demonstration (not supported)");
        #endif
    }

    // helper method to demonstrate std::format feature
    static void demonstrateFormat() {
        #ifdef __cpp_lib_format
        LOG_INFO("demonstrating std::format feature");
        
        // demonstrate different format specifiers
        std::string formatted1 = std::format("Integer: {:d}", 42);
        std::string formatted2 = std::format("Hex: {:x}", 255);
        std::string formatted3 = std::format("Float with precision: {:.2f}", 3.14159);
        std::string formatted4 = std::format("Multiple values: {}, {}, {}", "hello", 123, true);
        
        // log the formatted strings
        LOG_INFO(std::format("Format result 1: {}", formatted1));
        LOG_INFO(std::format("Format result 2: {}", formatted2));
        LOG_INFO(std::format("Format result 3: {}", formatted3));
        LOG_INFO(std::format("Format result 4: {}", formatted4));
        
        // verify with assertions
        assert(formatted1 == "Integer: 42");
        assert(formatted2 == "Hex: ff");
        assert(formatted3 == "Float with precision: 3.14");
        assert(formatted4 == "Multiple values: hello, 123, true");
        
        LOG_INFO("std::format demonstration completed");
        #else
        LOG_INFO("skipping std::format demonstration (not supported)");
        #endif
    }
};

// initialize static member
int FeatureTester::m_instanceCount = 0;

// function to run comprehensive tests
void runTests() {
    LOG_INFO("starting feature detection tests");
    
    // create a feature tester instance
    FeatureTester tester;
    
    // verify instance count
    assert(FeatureTester::getInstanceCount() == 1);
    
    // test feature support reporting
    const std::vector<std::string> features = {
        "concepts", "ranges", "coroutines", "format", "span", "modules", "three-way-comparison"
    };
    
    // test each feature
    for (const auto & feature : features) {
        const bool supported = tester.isFeatureSupported(feature);
        LOG_INFO(std::format("test report: {} is {}", 
                                 feature, supported ? "supported" : "not supported"));
                                 
        // we can't assert on support as it depends on the compiler
    }
    
    // demonstrate the features
    FeatureTester::demonstrateFeatures();
    
    // test unknown feature
    const bool unknownSupported = tester.isFeatureSupported("unknown-feature");
    assert(unknownSupported == false);
    
    // create a second instance to test the counter
    {
        FeatureTester tester2;
        assert(FeatureTester::getInstanceCount() == 2);
    }
    
    // verify the second instance was cleaned up
    assert(FeatureTester::getInstanceCount() == 1);
    
    LOG_INFO("all tests completed successfully");
}

// main entry point
int main() {
    LOG_INFO("starting c++20 feature detection demo");
    
    // output compiler information
    #ifdef __cplusplus
    LOG_INFO(std::format("c++ standard version: {}", __cplusplus));
    #else
    LOG_INFO("c++ standard version: unknown");
    #endif
    
    #ifdef __GNUC__
    LOG_INFO(std::format("gcc version: {}.{}.{}", 
                             __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__));
    #endif
    
    #ifdef _MSC_VER
    LOG_INFO(std::format("msvc version: {}", _MSC_VER));
    #endif
    
    #ifdef __clang__
    LOG_INFO(std::format("clang version: {}.{}.{}", 
                             __clang_major__, __clang_minor__, __clang_patchlevel__));
    #endif
    
    // run comprehensive tests
    runTests();
    
    // summary of feature detection macros
    LOG_INFO("summary of c++20 feature detection macros:");
    
    #ifdef __cpp_concepts
    LOG_INFO(std::format("__cpp_concepts: {}", __cpp_concepts));
    #endif
    
    #ifdef __cpp_lib_concepts
    LOG_INFO(std::format("__cpp_lib_concepts: {}", __cpp_lib_concepts));
    #endif
    
    #ifdef __cpp_lib_ranges
    LOG_INFO(std::format("__cpp_lib_ranges: {}", __cpp_lib_ranges));
    #endif
    
    #ifdef __cpp_coroutines
    LOG_INFO(std::format("__cpp_coroutines: {}", __cpp_coroutines));
    #endif
    
    #ifdef __cpp_lib_coroutine
    LOG_INFO(std::format("__cpp_lib_coroutine: {}", __cpp_lib_coroutine));
    #endif
    
    #ifdef __cpp_lib_format
    LOG_INFO(std::format("__cpp_lib_format: {}", __cpp_lib_format));
    #endif
    
    #ifdef __cpp_lib_span
    LOG_INFO(std::format("__cpp_lib_span: {}", __cpp_lib_span));
    #endif
    
    #ifdef __cpp_modules
    LOG_INFO(std::format("__cpp_modules: {}", __cpp_modules));
    #endif
    
    #ifdef __cpp_impl_three_way_comparison
    LOG_INFO(std::format("__cpp_impl_three_way_comparison: {}", __cpp_impl_three_way_comparison));
    #endif
    
    #ifdef __cpp_lib_three_way_comparison
    LOG_INFO(std::format("__cpp_lib_three_way_comparison: {}", __cpp_lib_three_way_comparison));
    #endif
    
    LOG_INFO("program completed successfully");
    
    return 0;
}
