// MIT License
// Copyright (c) 2025 dbjwhs

#include "../headers/decltype_examples.hpp"
#include "../../../../headers/project_utils.hpp"

#include <iostream>

// comprehensive demonstration of c++ decltype functionality
// this program showcases all aspects of decltype from basic usage to advanced metaprogramming
int main() {
    LOG_INFO_PRINT("starting comprehensive decltype demonstration");
    
    // suppress stderr during testing to keep output clean
    Logger::StderrSuppressionGuard suppressGuard{};
    
    // run comprehensive test suite using railway-oriented programming
    auto testResult = decltype_examples::DeclTypeTestRunner::runAllTests();
    
    if (testResult.has_value()) {
        LOG_INFO_PRINT("all decltype demonstrations completed successfully");
        LOG_INFO_PRINT("✓ decltype deep dive demonstration completed successfully");
        LOG_INFO_PRINT("✓ all test categories passed:");
        LOG_INFO_PRINT("  - basic decltype functionality");
        LOG_INFO_PRINT("  - advanced container and iterator usage");
        LOG_INFO_PRINT("  - function objects and lambda expressions");
        LOG_INFO_PRINT("  - expression templates and operator overloading");
        LOG_INFO_PRINT("  - utility functions and type deduction");
        LOG_INFO_PRINT("✓ comprehensive type safety and concept constraints verified");
        
        return 0;
    } else {
        LOG_ERROR_PRINT("decltype demonstration failed with error code: {}", 
                        static_cast<int>(testResult.error()));
        
        LOG_ERROR_PRINT("✗ decltype demonstration failed");
        LOG_ERROR_PRINT("check logs for detailed error information");
        
        return 1;
    }
}
