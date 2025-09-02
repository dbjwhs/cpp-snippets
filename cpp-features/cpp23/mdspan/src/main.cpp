// MIT License
// Copyright (c) 2025 dbjwhs

#include "../headers/mdspan_example.hpp"
#include <cassert>

#define ERR 1
#define NOERR 0

// comprehensive demonstration of C++23 std::mdspan functionality
// showcasing multidimensional array views, layout policies, and performance characteristics
auto main() -> int {
    
    LOG_INFO_PRINT("starting comprehensive C++23 std::mdspan demonstration");
    
    try {
        // demonstrate basic mdspan usage patterns
        auto result = mdspan_demo::demonstrate_basic_usage();
        if (!result) {
            LOG_ERROR_PRINT("basic usage demonstration failed");
            return ERR;
        }
        
        // demonstrate layout policies
        result = mdspan_demo::demonstrate_layout_policies();
        if (!result) {
            LOG_ERROR_PRINT("layout policies demonstration failed");
            return ERR;
        }
        
        // demonstrate subcommand operations
        result = mdspan_demo::demonstrate_submdspan_operations();
        if (!result) {
            LOG_ERROR_PRINT("submdspan operations demonstration failed");
            return ERR;
        }
        
        // demonstrate accessor policies
        result = mdspan_demo::demonstrate_accessor_policies();
        if (!result) {
            LOG_ERROR_PRINT("accessor policies demonstration failed");
            return ERR;
        }
        
        // demonstrate extents usage
        result = mdspan_demo::demonstrate_extents_usage();
        if (!result) {
            LOG_ERROR_PRINT("extents usage demonstration failed");
            return ERR;
        }
        
        // demonstrate performance characteristics
        result = mdspan_demo::demonstrate_performance_characteristics();
        if (!result) {
            LOG_ERROR_PRINT("performance characteristics demonstration failed");
            return ERR;
        }
        
        // comprehensive class-based examples
        LOG_INFO_PRINT("running comprehensive class-based mdspan examples");
        
        // test 1d example
        mdspan_demo::MdspanExample<int> example_1d{10};
        result = example_1d.run_comprehensive_tests();
        if (!result) {
            LOG_ERROR_PRINT("1d comprehensive tests failed");
            return ERR;
        }
        
        // test 2d example
        mdspan_demo::MdspanExample<double> example_2d{4, 5};
        result = example_2d.run_comprehensive_tests();
        if (!result) {
            LOG_ERROR_PRINT("2d comprehensive tests failed");
            return ERR;
        }
        
        // test 3d example
        mdspan_demo::MdspanExample<float> example_3d{3, 4, 2};
        result = example_3d.run_comprehensive_tests();
        if (!result) {
            LOG_ERROR_PRINT("3d comprehensive tests failed");
            return ERR;
        }
        
        LOG_INFO_PRINT("all mdspan demonstrations completed successfully");
        
    } catch (const std::exception& e) {
        Logger::StderrSuppressionGuard guard{};
        LOG_ERROR_PRINT("unhandled exception in mdspan demonstration: {}", e.what());
        return 1;
    } catch (...) {
        Logger::StderrSuppressionGuard guard{};
        LOG_ERROR_PRINT("unknown exception occurred in mdspan demonstration");
        return ERR;
    }
    
    LOG_INFO_PRINT("mdspan demonstration program completed successfully");
    return NOERR;
}
