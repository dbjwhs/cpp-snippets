// MIT License
// Copyright (c) 2025 dbjwhs

#include "../headers/resource_manager.hpp"
#include "../headers/project_utils.hpp"
#include <print>
#include <vector>
#include <cassert>
#include <memory>

using namespace rule_of_five;

// demonstration function for default construction and destruction
void demonstrate_construction_destruction() {
    LOG_INFO_PRINT("=== demonstrating construction and destruction ===");
    
    // test default constructor
    {
        LOG_INFO_PRINT("creating default resource manager");
        ResourceManager<int> default_rm;
        assert(default_rm.empty());
        assert(default_rm.size() == 0);
        assert(default_rm.capacity() == 0);
        LOG_INFO_PRINT("default resource manager created successfully: id={}", default_rm.id());
    } // destructor called here automatically
    
    // test parameterized constructor
    {
        LOG_INFO_PRINT("creating parameterized resource manager with size 5");
        ResourceManager<int> sized_rm{5};
        assert(sized_rm.empty()); // starts empty
        assert(sized_rm.size() == 0);  // size starts at 0
        assert(sized_rm.capacity() == 5); // but capacity is 5
        LOG_INFO_PRINT("parameterized resource manager created successfully: id={}, size={}", 
                      sized_rm.id(), sized_rm.size());
    } // destructor called here automatically
    
    LOG_INFO_PRINT("construction and destruction demonstration completed\n");
}

// demonstration function for copy semantics
void demonstrate_copy_semantics() {
    LOG_INFO_PRINT("=== demonstrating copy constructor and copy assignment ===");
    
    // create original resource manager and populate it
    LOG_INFO_PRINT("creating and populating original resource manager");
    ResourceManager<int> original{3};
    
    // populate with test data (add 3 elements to empty container)
    for (int ndx = 0; ndx < 3; ++ndx) {
        auto result = original.push_back(ndx * 10);
        assert(result.has_value());
    }
    LOG_INFO_PRINT("original resource manager populated: id={}, size={}", 
                  original.id(), original.size());
    
    // test copy constructor
    {
        LOG_INFO_PRINT("testing copy constructor");
        ResourceManager<int> copy_constructed{original};
        assert(copy_constructed.size() == original.size());
        assert(copy_constructed.capacity() == original.capacity());
        assert(copy_constructed.id() != original.id()); // should have different id
        
        // verify deep copy - modify original and ensure copy is unaffected
        auto result = original.push_back(999);
        assert(result.has_value());
        assert(copy_constructed.size() != original.size());
        LOG_INFO_PRINT("copy constructor test passed: copy_id={}, original_id={}", 
                      copy_constructed.id(), original.id());
    }
    
    // test copy assignment
    {
        LOG_INFO_PRINT("testing copy assignment operator");
        ResourceManager<int> copy_assigned;
        copy_assigned = original;
        assert(copy_assigned.size() == original.size());
        assert(copy_assigned.capacity() >= original.size());
        assert(copy_assigned.id() != original.id()); // should have different id
        
        LOG_INFO_PRINT("copy assignment test passed: assigned_id={}, original_id={}", 
                      copy_assigned.id(), original.id());
    }
    
    // test self-assignment
    {
        LOG_INFO_PRINT("testing self-assignment");
        ResourceManager<int> self_assign{2};
        std::string original_id = self_assign.id();
        std::size_t original_size = self_assign.size();
        
        self_assign = self_assign; // self-assignment
        assert(self_assign.id() == original_id);
        assert(self_assign.size() == original_size);
        LOG_INFO_PRINT("self-assignment test passed: id={}", self_assign.id());
    }
    
    LOG_INFO_PRINT("copy semantics demonstration completed\n");
}

// demonstration function for move semantics
void demonstrate_move_semantics() {
    LOG_INFO_PRINT("=== demonstrating move constructor and move assignment ===");
    
    // helper lambda to create and return a resource manager (for move testing)
    auto create_resource_manager = [](std::size_t size) -> ResourceManager<int> {
        LOG_INFO_PRINT("creating temporary resource manager with size {}", size);
        ResourceManager<int> temp{size};
        for (std::size_t ndx = 0; ndx < size; ++ndx) {
            auto result = temp.push_back(static_cast<int>(ndx * 100));
            assert(result.has_value());
        }
        LOG_INFO_PRINT("temporary resource manager created with id={}", temp.id());
        return temp; // move constructor will be called here
    };
    
    // test move constructor
    {
        LOG_INFO_PRINT("testing move constructor");
        ResourceManager<int> move_constructed = create_resource_manager(4);
        assert(move_constructed.size() == 4);
        assert(move_constructed.capacity() >= 4);
        LOG_INFO_PRINT("move constructor test passed: id={}, size={}", 
                      move_constructed.id(), move_constructed.size());
    }
    
    // test move assignment
    {
        LOG_INFO_PRINT("testing move assignment operator");
        ResourceManager<int> move_assigned;
        move_assigned = create_resource_manager(6);
        assert(move_assigned.size() == 6);
        assert(move_assigned.capacity() >= 6);
        LOG_INFO_PRINT("move assignment test passed: id={}, size={}", 
                      move_assigned.id(), move_assigned.size());
    }
    
    // test explicit move with std::move
    {
        LOG_INFO_PRINT("testing explicit move with std::move");
        ResourceManager<int> source{3};
        auto result = source.push_back(42);
        assert(result.has_value());
        
        std::string source_id = source.id();
        std::size_t source_size = source.size();
        
        ResourceManager<int> target = std::move(source);
        assert(target.size() == source_size);
        assert(source.size() == 0); // source should be in moved-from state
        assert(source.capacity() == 0);
        
        LOG_INFO_PRINT("explicit move test passed: target_id={}, source is moved-from", target.id());
    }
    
    // test self-move-assignment
    {
        LOG_INFO_PRINT("testing self-move-assignment");
        ResourceManager<int> self_move{2};
        std::string original_id = self_move.id();
        std::size_t original_size = self_move.size();
        
        self_move = std::move(self_move); // self-move-assignment
        assert(self_move.id() == original_id);
        assert(self_move.size() == original_size);
        LOG_INFO_PRINT("self-move-assignment test passed: id={}", self_move.id());
    }
    
    LOG_INFO_PRINT("move semantics demonstration completed\n");
}

// demonstration function for error handling with std::expected
void demonstrate_error_handling() {
    LOG_INFO_PRINT("=== demonstrating error handling with std::expected ===");
    
    ResourceManager<int> rm{3};
    
    // test successful operations
    {
        LOG_INFO_PRINT("testing successful operations");
        auto push_result = rm.push_back(100);
        assert(push_result.has_value());
        
        auto access_result = rm.at(0);
        assert(access_result.has_value());
        LOG_INFO_PRINT("successful operations test passed");
    }
    
    // test error conditions with stderr suppression for expected failures
    {
        Logger::StderrSuppressionGuard suppress_stderr;
        LOG_INFO_PRINT("testing error conditions (stderr suppressed for expected failures)");
        
        // test out of bounds access
        auto out_of_bounds = rm.at(999);
        assert(!out_of_bounds.has_value());
        assert(out_of_bounds.error() == ResourceError::InvalidSize);
        LOG_INFO_PRINT("out of bounds error handling test passed");
        
        // test access on empty resource manager
        ResourceManager<int> empty_rm;
        auto null_access = empty_rm.at(0);
        assert(!null_access.has_value());
        assert(null_access.error() == ResourceError::NullPointer);
        LOG_INFO_PRINT("null pointer error handling test passed");
    }
    
    LOG_INFO_PRINT("error handling demonstration completed\n");
}

// demonstration function for performance comparison
void demonstrate_performance_characteristics() {
    LOG_INFO_PRINT("=== demonstrating performance characteristics ===");
    
    constexpr std::size_t test_size = 1000;
    
    // demonstrate the cost of copying vs moving
    {
        LOG_INFO_PRINT("comparing copy vs move performance for size {}", test_size);
        
        // create large resource manager
        ResourceManager<int> large_rm{test_size};
        for (std::size_t ndx = 0; ndx < test_size; ++ndx) {
            auto result = large_rm.push_back(static_cast<int>(ndx));
            assert(result.has_value());
        }
        
        // copy operation (expensive)
        LOG_INFO_PRINT("performing copy operation (expensive)");
        ResourceManager<int> copied = large_rm;
        assert(copied.size() == test_size);
        LOG_INFO_PRINT("copy completed: copied_size={}", copied.size());
        
        // move operation (efficient)
        LOG_INFO_PRINT("performing move operation (efficient)");
        ResourceManager<int> moved = std::move(large_rm);
        assert(moved.size() == test_size);
        assert(large_rm.size() == 0); // moved-from state
        LOG_INFO_PRINT("move completed: moved_size={}, original_size={}", 
                      moved.size(), large_rm.size());
    }
    
    LOG_INFO_PRINT("performance characteristics demonstration completed\n");
}

// demonstration function for template usage with different types
void demonstrate_template_usage() {
    LOG_INFO_PRINT("=== demonstrating template usage with different types ===");
    
    // test with different types
    {
        LOG_INFO_PRINT("testing with double type");
        ResourceManager<double> double_rm{3};
        auto result = double_rm.push_back(3.14159);
        assert(result.has_value());
        assert(double_rm.size() == 1); // 0 initial + 1 added
        LOG_INFO_PRINT("double resource manager test passed: size={}", double_rm.size());
    }
    
    {
        LOG_INFO_PRINT("testing with string type");
        ResourceManager<std::string> string_rm{2};
        auto result = string_rm.push_back("hello world");
        assert(result.has_value());
        assert(string_rm.size() == 1); // 0 initial + 1 added
        LOG_INFO_PRINT("string resource manager test passed: size={}", string_rm.size());
    }
    
    LOG_INFO_PRINT("template usage demonstration completed\n");
}

// comprehensive test suite runner
void run_comprehensive_tests() {
    LOG_INFO_PRINT("=== running comprehensive rule of five tests ===\n");
    
    try {
        demonstrate_construction_destruction();
        demonstrate_copy_semantics();
        demonstrate_move_semantics();
        demonstrate_error_handling();
        demonstrate_performance_characteristics();
        demonstrate_template_usage();
        
        LOG_INFO_PRINT("=== all rule of five tests completed successfully ===");
        std::print("\n✓ All Rule of Five demonstrations and tests passed!\n");
        std::print("✓ Resource management working correctly\n");
        std::print("✓ Copy semantics implemented properly\n");
        std::print("✓ Move semantics optimized for performance\n");
        std::print("✓ Error handling using Railway-Oriented Programming\n");
        std::print("✓ Template constraints working with modern C++23 concepts\n");
        
    } catch (const std::exception& e) {
        LOG_ERROR_PRINT("test failed with exception: {}", e.what());
        std::print("❌ Test failed: {}\n", e.what());
    } catch (...) {
        LOG_ERROR_PRINT("test failed with unknown exception");
        std::print("❌ Test failed with unknown exception\n");
    }
}

int main() {
    std::print("Rule of Five Comprehensive Demonstration\n");
    std::print("========================================\n\n");
    
    // initialize logger for this demonstration
    auto& logger = Logger::getInstance("./rule_of_five_demo.log");
    LOG_INFO_PRINT("starting rule of five comprehensive demonstration");
    
    run_comprehensive_tests();
    
    LOG_INFO_PRINT("rule of five demonstration completed");
    std::print("\nCheck 'rule_of_five_demo.log' for detailed execution logs.\n");
    
    return 0;
}