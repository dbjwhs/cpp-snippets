// MIT License
// Copyright (c) 2025 dbjwhs

#include "flat_map.hpp"
#include <cassert>
#include <chrono>
#include <random>
#include <map>
#include <ranges>

using namespace performance_containers;

//
// comprehensive test suite for flat map implementation validation
//
namespace test_suite {

//
// test basic flat map functionality including insertion, lookup, and iteration
//
void test_basic_operations() {
    LOG_INFO_PRINT("=== testing basic flat map operations ===");

    // create a flat map with initial capacity for performance
    FlatMap<int, std::string> flat_map{10};

    // verify initial empty state
    assert(flat_map.empty());
    assert(flat_map.size() == 0);
    LOG_INFO_PRINT("✓ empty flat map initialization successful");

    // test basic insertions with various data types
    auto result1 = flat_map.insert(3, "three");
    assert(result1.has_value());

    auto result2 = flat_map.insert(1, "one");
    assert(result2.has_value());

    auto result3 = flat_map.insert(4, "four");
    assert(result3.has_value());

    auto result4 = flat_map.insert(2, "two");
    assert(result4.has_value());

    // verify size after insertions
    assert(flat_map.size() == 4);
    assert(!flat_map.empty());
    LOG_INFO_PRINT("✓ basic insertions successful, size: {}", flat_map.size());

    // verify elements are automatically sorted by key
    std::vector<int> expected_keys{1, 2, 3, 4};
    std::vector<int> actual_keys{};
    actual_keys.reserve(flat_map.size());

    for (const auto &key: flat_map | std::views::keys) {
        actual_keys.emplace_back(key);
    }

    assert(actual_keys == expected_keys);
    LOG_INFO_PRINT("✓ automatic sorting verification successful");

    // test key lookup functionality
    auto find_result = flat_map.find(3);
    assert(find_result.has_value());
    assert(find_result.value()->second == "three");
    LOG_INFO_PRINT("✓ key lookup successful");

    // test non-existent key lookup
    auto not_found = flat_map.find(99);
    assert(!not_found.has_value());
    assert(not_found.error() == FlatMapError::KeyNotFound);
    LOG_INFO_PRINT("✓ non-existent key handling successful");

    // test contains functionality
    assert(flat_map.contains(2));
    assert(!flat_map.contains(99));
    LOG_INFO_PRINT("✓ contains functionality successful");

    // test at() method with bounds checking
    auto at_result = flat_map.at(2);
    assert(at_result.has_value());
    assert(at_result.value() == "two");

    auto at_error = flat_map.at(99);
    assert(!at_error.has_value());
    LOG_INFO_PRINT("✓ bounds-checked access successful");

    LOG_INFO_PRINT("=== basic operations test completed successfully ===");
}

//
// test update operations and duplicate key handling
//
void test_update_operations() {
    LOG_INFO_PRINT("=== testing update operations ===");

    FlatMap<int, std::string> flat_map{};

    // insert initial value
    const auto insert_result = flat_map.insert(5, "five");
    assert(insert_result.has_value());
    assert(flat_map.size() == 1);

    // update existing key - should not increase size
    const auto update_result = flat_map.insert(5, "FIVE");
    assert(update_result.has_value());
    assert(flat_map.size() == 1);

    // verify value was updated
    const auto find_result = flat_map.find(5);
    assert(find_result.has_value());
    assert(find_result.value()->second == "FIVE");
    LOG_INFO_PRINT("✓ key update successful");

    // test move semantics for value updates
    std::string move_value{"moved_value"};
    const auto move_result = flat_map.insert(5, std::move(move_value));
    assert(move_result.has_value());

    const auto verify_move = flat_map.find(5);
    assert(verify_move.has_value());
    assert(verify_move.value()->second == "moved_value");
    LOG_INFO_PRINT("✓ move semantics successful");

    // test emplace functionality
    const auto emplace_result = flat_map.emplace(10, "emplaced_value");
    assert(emplace_result.has_value());
    assert(flat_map.size() == 2);
    LOG_INFO_PRINT("✓ emplace operation successful");

    LOG_INFO_PRINT("=== update operations test completed successfully ===");
}

//
// test removal operations and container state management
//
void test_removal_operations() {
    LOG_INFO_PRINT("=== testing removal operations ===");

    FlatMap<int, std::string> flat_map{};

    // populate with test data
    const std::vector<std::pair<int, std::string>> test_data{
        {1, "one"}, {2, "two"}, {3, "three"}, {4, "four"}, {5, "five"}
    };

    for (const auto& [key, value] : test_data) {
        auto result = flat_map.insert(key, value);
        assert(result.has_value());
    }

    const auto initial_size = flat_map.size();
    LOG_INFO_PRINT("populated flat map with {} elements", initial_size);

    // test successful removal
    const bool removed = flat_map.erase(3);
    assert(removed);
    assert(flat_map.size() == initial_size - 1);
    assert(!flat_map.contains(3));
    LOG_INFO_PRINT("✓ successful removal verified");

    // test removal of a non-existent key
    const bool not_removed = flat_map.erase(99);
    assert(!not_removed);
    assert(flat_map.size() == initial_size - 1);
    LOG_INFO_PRINT("✓ non-existent key removal handled correctly");

    // test clear operation
    flat_map.clear();
    assert(flat_map.empty());
    assert(flat_map.size() == 0);
    LOG_INFO_PRINT("✓ clear operation successful");

    LOG_INFO_PRINT("=== removal operations test completed successfully ===");
}

//
// test range construction for optimal performance scenarios
//
void test_range_construction() {
    LOG_INFO_PRINT("=== testing range construction ===");

    // create sorted test data for optimal construction
    std::vector<std::pair<int, std::string>> sorted_data{};
    sorted_data.reserve(100);

    for (int ndx = 0; ndx < 100; ++ndx) {
        sorted_data.emplace_back(ndx * 2, std::format("value_{}", ndx * 2));
    }

    LOG_INFO_PRINT("created {} sorted pairs for construction test", sorted_data.size());

    // construct a flat map from sorted range - should be very efficient
    FlatMap<int, std::string> flat_map{sorted_data.begin(), sorted_data.end(), true};

    assert(flat_map.size() == sorted_data.size());
    LOG_INFO_PRINT("✓ range construction successful, size: {}", flat_map.size());

    // verify all elements were inserted correctly
    for (int ndx = 0; ndx < 100; ++ndx) {
        const int key = ndx * 2;
        const auto find_result = flat_map.find(key);
        assert(find_result.has_value());
        assert(find_result.value()->second == std::format("value_{}", key));
    }

    LOG_INFO_PRINT("✓ range construction verification successful");

    // test unsorted range construction
    std::vector<std::pair<int, std::string>> unsorted_data{
        {30, "thirty"}, {10, "ten"}, {20, "twenty"}, {40, "forty"}
    };

    FlatMap<int, std::string> unsorted_map{unsorted_data.begin(), unsorted_data.end(), false};
    assert(unsorted_map.size() == 4);

    // verify sorting occurred during construction
    std::vector<int> keys{};
    keys.reserve(unsorted_map.size());

    for (const auto &key: unsorted_map | std::views::keys) {
        keys.emplace_back(key);
    }

    const std::vector<int> expected{10, 20, 30, 40};
    assert(keys == expected);
    LOG_INFO_PRINT("✓ unsorted range construction with automatic sorting successful");

    LOG_INFO_PRINT("=== range construction test completed successfully ===");
}

//
// performance comparison between flat_map and std::map
//
void test_performance_comparison() {
    LOG_INFO_PRINT("=== performance comparison test ===");

    constexpr int num_elements = 10000;
    constexpr int num_lookups = 1000;

    // generate test data with realistic distribution
    std::vector<std::pair<int, std::string>> test_data{};
    test_data.reserve(num_elements);

    std::random_device rd{};
    std::mt19937 gen{rd()};
    std::uniform_int_distribution<int> dist{1, 100000};

    for (int ndx = 0; ndx < num_elements; ++ndx) {
        const int key = dist(gen);
        test_data.emplace_back(key, std::format("value_{}", key));
    }

    LOG_INFO_PRINT("generated {} test elements for performance comparison", test_data.size());

    // benchmark flat_map construction
    auto start_time = std::chrono::high_resolution_clock::now();

    FlatMap<int, std::string> flat_map{test_data.begin(), test_data.end(), false};

    auto end_time = std::chrono::high_resolution_clock::now();
    const auto flat_construction_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

    LOG_INFO_PRINT("flat_map construction time: {}μs", flat_construction_time.count());

    // benchmark std::map construction for comparison
    start_time = std::chrono::high_resolution_clock::now();

    std::map<int, std::string> std_map{};
    for (const auto& [key, value] : test_data) {
        std_map[key] = value;
    }

    end_time = std::chrono::high_resolution_clock::now();
    const auto std_construction_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

    LOG_INFO_PRINT("std::map construction time: {}μs", std_construction_time.count());

    // generate lookup keys for performance testing
    std::vector<int> lookup_keys{};
    lookup_keys.reserve(num_lookups);

    std::uniform_int_distribution<int> lookup_dist{1, 100000};
    for (int ndx = 0; ndx < num_lookups; ++ndx) {
        lookup_keys.emplace_back(lookup_dist(gen));
    }

    // benchmark flat_map lookups
    start_time = std::chrono::high_resolution_clock::now();

    int flat_found = 0;
    for (const int key : lookup_keys) {
        if (flat_map.contains(key)) {
            ++flat_found;
        }
    }

    end_time = std::chrono::high_resolution_clock::now();
    const auto flat_lookup_time = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);

    LOG_INFO_PRINT("flat_map lookup time: {}ns for {} searches", flat_lookup_time.count(), num_lookups);

    // benchmark std::map lookups
    start_time = std::chrono::high_resolution_clock::now();

    int std_found = 0;
    for (const int key : lookup_keys) {
        if (std_map.contains(key)) {
            ++std_found;
        }
    }

    end_time = std::chrono::high_resolution_clock::now();
    const auto std_lookup_time = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);

    LOG_INFO_PRINT("std::map lookup time: {}ns for {} searches", std_lookup_time.count(), num_lookups);

    // calculate and report performance ratios
    if (flat_lookup_time.count() > 0 && std_lookup_time.count() > 0) {
        const double lookup_ratio = static_cast<double>(std_lookup_time.count()) / flat_lookup_time.count();
        LOG_INFO_PRINT("lookup performance ratio: {:.2f}x (flat_map advantage)", lookup_ratio);
    }

    LOG_INFO_PRINT("found counts - flat_map: {}, std::map: {} (should be equal)", flat_found, std_found);

    LOG_INFO_PRINT("=== performance comparison completed ===");
}

//
// test edge cases and error conditions
//
void test_edge_cases() {
    LOG_INFO_PRINT("=== testing edge cases ===");

    FlatMap<int, std::string> flat_map{};

    // test operations on empty container
    assert(flat_map.empty());
    assert(!flat_map.contains(42));
    assert(!flat_map.erase(42));

    auto empty_find = flat_map.find(42);
    assert(!empty_find.has_value());
    assert(empty_find.error() == FlatMapError::KeyNotFound);
    LOG_INFO_PRINT("✓ empty container operations handled correctly");

    // test with negative keys
    auto negative_result = flat_map.insert(-1, "negative");
    assert(negative_result.has_value());

    auto zero_result = flat_map.insert(0, "zero");
    assert(zero_result.has_value());

    auto positive_result = flat_map.insert(1, "positive");
    assert(positive_result.has_value());

    // verify ordering with negative numbers
    std::vector<int> expected_order{-1, 0, 1};
    std::vector<int> actual_order{};
    actual_order.reserve(flat_map.size());

    for (const auto &key: flat_map | std::views::keys) {
        actual_order.emplace_back(key);
    }

    assert(actual_order == expected_order);
    LOG_INFO_PRINT("✓ negative key ordering successful");

    // test with large strings
    const std::string large_value(10000, 'X');
    auto large_result = flat_map.insert(999, large_value);
    assert(large_result.has_value());

    auto large_find = flat_map.find(999);
    assert(large_find.has_value());
    assert(large_find.value()->second.size() == 10000);
    LOG_INFO_PRINT("✓ large value handling successful");

    // test boundary values
    auto int_min_result = flat_map.insert(INT_MIN, "minimum");
    assert(int_min_result.has_value());

    auto int_max_result = flat_map.insert(INT_MAX, "maximum");
    assert(int_max_result.has_value());

    // verify boundary lookups work correctly
    auto min_find = flat_map.find(INT_MIN);
    assert(min_find.has_value());
    assert(min_find.value()->second == "minimum");

    auto max_find = flat_map.find(INT_MAX);
    assert(max_find.has_value());
    assert(max_find.value()->second == "maximum");
    LOG_INFO_PRINT("✓ boundary value handling successful");

    // test duplicate key updates don't change size
    const auto size_before_update = flat_map.size();
    auto update_result = flat_map.insert(999, "UPDATED");
    assert(update_result.has_value());
    assert(flat_map.size() == size_before_update);

    auto verify_update = flat_map.find(999);
    assert(verify_update.has_value());
    assert(verify_update.value()->second == "UPDATED");
    LOG_INFO_PRINT("✓ duplicate key update handling successful");

    // test at() method error handling
    auto at_success = flat_map.at(999);
    assert(at_success.has_value());
    assert(at_success.value() == "UPDATED");

    auto at_failure = flat_map.at(12345);
    assert(!at_failure.has_value());
    assert(at_failure.error() == FlatMapError::KeyNotFound);
    LOG_INFO_PRINT("✓ at() method error handling successful");

    // test const version of at()
    const auto& const_flat_map = flat_map;
    auto const_at_success = const_flat_map.at(999);
    assert(const_at_success.has_value());
    assert(const_at_success.value() == "UPDATED");

    auto const_at_failure = const_flat_map.at(54321);
    assert(!const_at_failure.has_value());
    assert(const_at_failure.error() == FlatMapError::KeyNotFound);
    LOG_INFO_PRINT("✓ const at() method handling successful");

    // test memory reservation and capacity
    flat_map.clear();
    assert(flat_map.empty());
    assert(flat_map.size() == 0);

    flat_map.reserve(1000);
    assert(flat_map.capacity() >= 1000);
    assert(flat_map.empty()); // reserve shouldn't add elements
    LOG_INFO_PRINT("✓ memory reservation successful");

    // test very large capacity reservation
    FlatMap<int, std::string> large_capacity_map{};
    large_capacity_map.reserve(100000);
    assert(large_capacity_map.capacity() >= 100000);
    assert(large_capacity_map.empty());
    LOG_INFO_PRINT("✓ large capacity reservation successful");

    // test string keys with special characters
    FlatMap<std::string, int> string_key_map{};

    const std::vector<std::pair<std::string, int>> special_strings{
        {"", 0},                    // empty string
        {" ", 1},                   // space
        {"\t", 2},                  // tab
        {"\n", 3},                  // newline
        {"café", 4},                // unicode
        {"hello world", 5},         // spaces
        {"file.txt", 6},            // periods
        {"path/to/file", 7},        // slashes
        {"key-with-dashes", 8},     // dashes
        {"UPPERCASE", 9},           // uppercase
        {"lowercase", 10},          // lowercase
        {"123456", 11},             // numeric string
        {"special!@#$%", 12}        // special characters
    };

    for (const auto& [key, value] : special_strings) {
        auto result = string_key_map.insert(key, value);
        assert(result.has_value());
    }

    // verify all special strings were inserted and can be found
    for (const auto& [key, expected_value] : special_strings) {
        auto find_result = string_key_map.find(key);
        assert(find_result.has_value());
        assert(find_result.value()->second == expected_value);
    }

    LOG_INFO_PRINT("✓ special string key handling successful");

    // test iterator edge cases
    FlatMap<int, std::string> iterator_test{};

    // empty container iterators
    assert(iterator_test.begin() == iterator_test.end());
    assert(iterator_test.cbegin() == iterator_test.cend());

    // single element container
    auto single_insert = iterator_test.insert(42, "single");
    assert(single_insert.has_value());

    auto iter = iterator_test.begin();
    assert(iter != iterator_test.end());
    assert(iter->first == 42);
    assert(iter->second == "single");

    ++iter;
    assert(iter == iterator_test.end());
    LOG_INFO_PRINT("✓ iterator edge case handling successful");

    LOG_INFO_PRINT("=== edge cases test completed successfully ===");
}

//
// comprehensive iteration testing including various loop patterns
//
void test_iteration_patterns() {
    LOG_INFO_PRINT("=== testing iteration patterns ===");

    FlatMap<int, std::string> flat_map{};

    // populate with test data
    const std::vector<std::pair<int, std::string>> test_data{
        {10, "ten"}, {20, "twenty"}, {30, "thirty"}, {40, "forty"}, {50, "fifty"}
    };

    for (const auto& [key, value] : test_data) {
        auto result = flat_map.insert(key, value);
        assert(result.has_value());
    }

    LOG_INFO_PRINT("populated flat map for iteration testing");

    // test range-based for loop (preferred pattern)
    std::vector<int> range_keys{};
    range_keys.reserve(flat_map.size());

    for (const auto& [key, value] : flat_map) {
        range_keys.emplace_back(key);
        LOG_INFO_PRINT("range-based iteration: key={}, value={}", key, value);
    }

    assert(range_keys.size() == test_data.size());
    LOG_INFO_PRINT("✓ range-based iteration successful");

    // test traditional iterator loop
    std::vector<int> iterator_keys{};
    iterator_keys.reserve(flat_map.size());

    for (auto iter = flat_map.begin(); iter != flat_map.end(); ++iter) {
        iterator_keys.emplace_back(iter->first);
        LOG_INFO_PRINT("iterator-based iteration: key={}, value={}", iter->first, iter->second);
    }

    assert(iterator_keys == range_keys);
    LOG_INFO_PRINT("✓ iterator-based iteration successful");

    // test const iteration
    const auto& const_flat_map = flat_map;
    std::vector<int> const_keys{};
    const_keys.reserve(const_flat_map.size());

    for (const auto &key: const_flat_map | std::views::keys) {
        const_keys.emplace_back(key);
    }

    assert(const_keys == range_keys);
    LOG_INFO_PRINT("✓ const iteration successful");

    LOG_INFO_PRINT("=== iteration patterns test completed successfully ===");
}

} // namespace test_suite

//
// demonstration of real-world usage patterns and best practices
//
namespace usage_examples {

//
// example: configuration cache for web application
// demonstrates read-heavy workload where flat_map excels
//
void demonstrate_config_cache() {
    LOG_INFO_PRINT("=== configuration cache example ===");

    // create configuration cache with expected capacity
    FlatMap<std::string, std::string> config_cache{50};

    // load configuration settings (typically from file or database)
    const std::vector<std::pair<std::string, std::string>> config_data{
        {"database.host", "localhost"},
        {"database.port", "5432"},
        {"cache.size", "1000"},
        {"log.level", "INFO"},
        {"api.timeout", "30"},
        {"security.enabled", "true"}
    };

    // populate cache with configuration data
    for (const auto& [key, value] : config_data) {
        if (auto result = config_cache.insert(key, value); result.has_value()) {
            LOG_INFO_PRINT("loaded config: {} = {}", key, value);
        } else {
            LOG_ERROR_PRINT("failed to load config: {}", key);
        }
    }

    // simulate frequent configuration lookups (read-heavy pattern)
    const std::vector<std::string> lookup_keys{
        "database.host", "api.timeout", "log.level", "cache.size"
    };

    for (const auto& key : lookup_keys) {
        if (auto config_value = config_cache.at(key); config_value.has_value()) {
            LOG_INFO_PRINT("config lookup successful: {} = {}", key, config_value.value());
        } else {
            LOG_WARNING_PRINT("config key not found: {}", key);
        }
    }

    // demonstrate configuration update
    if (const auto update_result = config_cache.insert("log.level", "DEBUG"); update_result.has_value()) {
        LOG_INFO_PRINT("configuration updated: log.level = DEBUG");
    }

    LOG_INFO_PRINT("=== configuration cache example completed ===");
}

//
// example: entity-component mapping in game engine
// shows how flat_map can optimize game loop performance
//
void demonstrate_entity_component_system() {
    LOG_INFO_PRINT("=== entity-component system example ===");

    // entity id to component data mapping
    FlatMap<int, std::string> position_components{1000};
    FlatMap<int, std::string> render_components{1000};

    // simulate entity creation during game initialization
    constexpr int num_entities = 100;

    for (int entity_id = 0; entity_id < num_entities; ++entity_id) {
        // create a position component
        const std::string position_data = std::format("x:{}, y:{}, z:{}",
                                                     entity_id * 10,
                                                     entity_id * 5,
                                                     0);
        auto pos_result = position_components.insert(entity_id, position_data);
        assert(pos_result.has_value());

        // create render component for visible entities
        if (entity_id % 2 == 0) {
            const std::string render_data = std::format("model:entity_{}, texture:default", entity_id);
            auto render_result = render_components.insert(entity_id, render_data);
            assert(render_result.has_value());
        }
    }

    LOG_INFO_PRINT("created {} entities with components", num_entities);

    // simulate game loop - frequent component lookups
    std::vector<int> active_entities{};
    active_entities.reserve(10);

    // find entities that have both position and render components
    for (int entity_id = 0; entity_id < num_entities; ++entity_id) {
        if (position_components.contains(entity_id) && render_components.contains(entity_id)) {
            active_entities.emplace_back(entity_id);
        }
    }

    LOG_INFO_PRINT("found {} entities with both position and render components", active_entities.size());

    // process active entities (typical game loop operation)
    for (const int entity_id : active_entities) {
        auto position = position_components.at(entity_id);

        if (auto render = render_components.at(entity_id); position.has_value() && render.has_value()) {
            LOG_INFO_PRINT("processing entity {}: pos={}, render={}",
                          entity_id, position.value(), render.value());
        }
    }

    LOG_INFO_PRINT("=== entity-component system example completed ===");
}

//
// example: symbol table for compiler/interpreter
// demonstrates efficient symbol resolution in language processing
//
void demonstrate_symbol_table() {
    LOG_INFO_PRINT("=== symbol table example ===");

    // symbol name to type/value mapping
    FlatMap<std::string, std::string> symbol_table{200};

    // simulate variable declarations in source code
    const std::vector<std::pair<std::string, std::string>> declarations{
        {"main", "function"},
        {"counter", "int"},
        {"message", "string"},
        {"is_valid", "bool"},
        {"calculate", "function"},
        {"data_array", "array<int>"},
        {"user_name", "string"},
        {"max_value", "const int"}
    };

    // add symbols to table during parsing phase
    for (const auto& [symbol, type] : declarations) {
        if (auto result = symbol_table.insert(symbol, type); result.has_value()) {
            LOG_INFO_PRINT("declared symbol: {} : {}", symbol, type);
        } else {
            LOG_ERROR_PRINT("failed to declare symbol: {}", symbol);
        }
    }

    // simulate symbol resolution during compilation/interpretation
    const std::vector<std::string> symbol_references{
        "main", "counter", "unknown_var", "message", "calculate"
    };

    for (const auto& symbol : symbol_references) {
        if (auto symbol_info = symbol_table.find(symbol); symbol_info.has_value()) {
            LOG_INFO_PRINT("symbol resolved: {} -> {}", symbol, symbol_info.value()->second);
        } else {
            // use stderr suppression guard for intentional test error
            // this demonstrates proper error handling without polluting stderr output
            if (symbol == "unknown_var") {
                Logger::StderrSuppressionGuard guard{};
                LOG_ERROR_PRINT("undefined symbol: {} (intentional test case)", symbol);
            } else {
                LOG_ERROR_PRINT("undefined symbol: {}", symbol);
            }
        }
    }

    // demonstrate scoped symbol management
    LOG_INFO_PRINT("entering new scope - adding local variables");

    const auto local_result1 = symbol_table.insert("local_var", "int");
    const auto local_result2 = symbol_table.insert("temp", "double");

    assert(local_result1.has_value() && local_result2.has_value());

    LOG_INFO_PRINT("symbol table size: {}", symbol_table.size());

    LOG_INFO_PRINT("=== symbol table example completed ===");
}

} // namespace usage_examples

//
// main function orchestrating all tests and demonstrations
//
int main() {
    LOG_INFO_PRINT("starting comprehensive flat map testing and demonstration");

    try {
        // run comprehensive test suite
        test_suite::test_basic_operations();
        test_suite::test_update_operations();
        test_suite::test_removal_operations();
        test_suite::test_range_construction();
        test_suite::test_performance_comparison();
        test_suite::test_edge_cases();
        test_suite::test_iteration_patterns();

        LOG_INFO_PRINT("all test suite functions completed successfully");

        // demonstrate real-world usage patterns
        usage_examples::demonstrate_config_cache();
        usage_examples::demonstrate_entity_component_system();
        usage_examples::demonstrate_symbol_table();

        LOG_INFO_PRINT("all usage examples completed successfully");

        // final validation with debug information
        FlatMap<int, std::string> final_test{};
        const auto result1 = final_test.insert(1, "first");
        const auto result2 = final_test.insert(2, "second");
        const auto result3 = final_test.insert(3, "third");

        // verify all insertions succeeded
        assert(result1.has_value());
        assert(result2.has_value());
        assert(result3.has_value());

        LOG_INFO_PRINT("final flat map debug information:");
        final_test.debug_info();

        LOG_INFO_PRINT("=== ALL TESTS AND DEMONSTRATIONS COMPLETED SUCCESSFULLY ===");

    } catch (const std::exception& e) {
        LOG_ERROR_PRINT("exception caught during testing: {}", e.what());
        return 1;
    } catch (...) {
        LOG_ERROR_PRINT("unknown exception caught during testing");
        return 1;
    }
    return 0;
}
