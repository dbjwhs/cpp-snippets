// MIT License
// Copyright (c) 2025 dbjwhs

#include "../headers/multiton.hpp"
#include "../headers/database_connection.hpp"
#include "../headers/cache_manager.hpp"
#include "../../../../headers/project_utils.hpp"
#include <cassert>
#include <thread>
#include <vector>
#include <string>
#include <chrono>
#include <format>
#include <algorithm>

using namespace multiton_pattern;

// test function declarations
void test_basic_functionality();
void test_thread_safety();
void test_error_conditions();
void test_factory_functions();
void test_database_connection_multiton();
void test_cache_manager_multiton();
void test_edge_cases();
void test_performance();

int main() {
    LOG_INFO_PRINT("=== Multiton Pattern Test Suite ===");
    
    try {
        test_basic_functionality();
        test_thread_safety();
        test_error_conditions();
        test_factory_functions();
        test_database_connection_multiton();
        test_cache_manager_multiton();
        test_edge_cases();
        test_performance();
        
        LOG_INFO_PRINT("=== All tests passed successfully ===");
        return 0;
        
    } catch (const std::exception& e) {
        LOG_ERROR_PRINT("Test failed with exception: {}", e.what());
        return 1;
    } catch (...) {
        LOG_ERROR_PRINT("Test failed with unknown exception");
        return 1;
    }
}

void test_basic_functionality() {
    LOG_INFO_PRINT("\n--- Testing Basic Functionality ---");
    
    using TestMultiton = Multiton<DatabaseConnection>;
    TestMultiton::clear_all_instances();
    
    // test initial state
    assert(TestMultiton::empty());
    assert(TestMultiton::size() == 0);
    assert(TestMultiton::get_all_keys().empty());
    
    // test instance creation
    const auto result1 = TestMultiton::get_instance("test1");
    assert(result1.has_value());
    const auto& instance1 = result1.value();
    assert(instance1 != nullptr);
    assert(TestMultiton::size() == 1);
    assert(TestMultiton::has_instance("test1"));
    
    // test instance reuse
    const auto result2 = TestMultiton::get_instance("test1");
    assert(result2.has_value());
    const auto& instance2 = result2.value();
    assert(instance1.get() == instance2.get());
    assert(TestMultiton::size() == 1);
    
    // test multiple instances
    const auto result3 = TestMultiton::get_instance("test2");
    assert(result3.has_value());
    const auto& instance3 = result3.value();
    assert(instance1.get() != instance3.get());
    assert(TestMultiton::size() == 2);
    
    // test key listing
    auto keys = TestMultiton::get_all_keys();
    assert(keys.size() == 2);
    std::ranges::sort(keys);
    assert(keys[0] == "test1");
    assert(keys[1] == "test2");
    
    // test instance removal
    const auto remove_result = TestMultiton::remove_instance("test1");
    assert(remove_result.has_value());
    (void)remove_result; // suppress unused variable warning
    assert(!TestMultiton::has_instance("test1"));
    assert(TestMultiton::size() == 1);
    
    // test clear all
    TestMultiton::clear_all_instances();
    assert(TestMultiton::empty());
    assert(TestMultiton::size() == 0);
    
    LOG_INFO_PRINT("Basic functionality tests passed");
}

void test_thread_safety() {
    LOG_INFO_PRINT("\n--- Testing Thread Safety ---");
    
    using ThreadTestMultiton = Multiton<DatabaseConnection>;
    ThreadTestMultiton::clear_all_instances();
    
    constexpr int num_threads = 10;
    constexpr int operations_per_thread = 50;
    std::vector<std::thread> threads;
    std::atomic<int> success_count{0};
    std::atomic<int> error_count{0};
    
    // launch threads that create and access instances concurrently
    threads.reserve(num_threads);
    for (int thread_id = 0; thread_id < num_threads; ++thread_id) {
            threads.emplace_back([thread_id, &success_count, &error_count]() {
                for (int op = 0; op < operations_per_thread; ++op) {
                    try {
                        // create instances with overlapping keys to test contention
                        const std::string key = std::format("thread_test_{}", thread_id % 3);

                        if (auto result = ThreadTestMultiton::get_instance(key); result.has_value()) {
                            success_count.fetch_add(1);

                            // perform operations on the instance
                            const auto& instance = result.value();
                            instance->execute_query(std::format("SELECT * FROM test_{}", op));

                            // test other concurrent operations
                            ThreadTestMultiton::has_instance(key);
                            ThreadTestMultiton::get_instance_count();
                        } else {
                            error_count.fetch_add(1);
                        }
                    } catch (...) {
                        error_count.fetch_add(1);
                    }
                }
            });
        }
    
    // wait for all threads
    for (auto& thread : threads) {
        thread.join();
    }
    
    // verify results
    constexpr int expected_operations = num_threads * operations_per_thread;
    assert(success_count.load() == expected_operations);
    assert(error_count.load() == 0);
    assert(ThreadTestMultiton::size() == 3); // only 3 unique keys used
    
    LOG_INFO_PRINT("Thread safety tests passed: {} operations across {} threads", 
                  expected_operations, num_threads);
}

void test_error_conditions() {
    LOG_INFO_PRINT("\n--- Testing Error Conditions ---");
    
    using ErrorTestMultiton = Multiton<DatabaseConnection>;
    ErrorTestMultiton::clear_all_instances();
    
    // test invalid keys
    {
        Logger::StderrSuppressionGuard stderr_guard;
        
        // empty key
        auto result1 = ErrorTestMultiton::get_instance("");
        assert(!result1.has_value());
        assert(result1.error() == MultitonError::INVALID_KEY);
        
        // key with null character
        std::string null_key = "test";
        null_key.push_back('\0');
        auto result2 = ErrorTestMultiton::get_instance(null_key);
        assert(!result2.has_value());
        assert(result2.error() == MultitonError::INVALID_KEY);
    }
    
    // test removing non-existent instance
    auto remove_result = ErrorTestMultiton::remove_instance("nonexistent");
    assert(!remove_result.has_value());
    assert(remove_result.error() == MultitonError::INSTANCE_NOT_FOUND);
    (void)remove_result; // suppress unused variable warning
    
    // test has_instance with invalid key
    assert(!ErrorTestMultiton::has_instance(""));
    
    LOG_INFO_PRINT("Error condition tests passed");
}

void test_factory_functions() {
    LOG_INFO_PRINT("\n--- Testing Factory Functions ---");
    
    using FactoryTestMultiton = Multiton<DatabaseConnection>;
    FactoryTestMultiton::clear_all_instances();
    
    // set custom factory
    FactoryTestMultiton::set_factory([](const std::string& key)
        -> std::expected<std::shared_ptr<DatabaseConnection>, MultitonError> {
        if (key.starts_with("fail_")) {
            return std::unexpected(MultitonError::CREATION_FAILED);
        }
        if (key.starts_with("invalid_")) {
            return std::unexpected(MultitonError::INVALID_KEY);
        }
        
        // create custom database connection based on key
        std::string connection_string = std::format("custom://server/{}", key);
        return std::make_shared<DatabaseConnection>(connection_string, key);
    });
    
    // test successful factory creation
    const auto success_result = FactoryTestMultiton::get_instance("success_test");
    assert(success_result.has_value());
    const auto& instance = success_result.value();
    assert(instance->get_connection_string() == "custom://server/success_test");
    assert(instance->get_database_name() == "success_test");
    
    // test factory failure scenarios
    {
        Logger::StderrSuppressionGuard stderr_guard;
        
        auto fail_result = FactoryTestMultiton::get_instance("fail_test");
        assert(!fail_result.has_value());
        assert(fail_result.error() == MultitonError::CREATION_FAILED);
        
        auto invalid_result = FactoryTestMultiton::get_instance("invalid_test");
        assert(!invalid_result.has_value());
        assert(invalid_result.error() == MultitonError::INVALID_KEY);
    }
    
    // clear factory and test default behavior
    FactoryTestMultiton::clear_factory();
    const auto default_result = FactoryTestMultiton::get_instance("default_test");
    assert(default_result.has_value());
    
    LOG_INFO_PRINT("Factory function tests passed");
}

void test_database_connection_multiton() {
    LOG_INFO_PRINT("\n--- Testing Database Connection Multiton ---");
    
    using DbMultiton = Multiton<DatabaseConnection>;
    DbMultiton::clear_all_instances();
    
    // create multiple database connections
    const std::vector<std::string> db_names = {"primary", "secondary", "backup"};
    std::vector<std::shared_ptr<DatabaseConnection>> connections;
    
    for (const auto& name : db_names) {
        auto result = DbMultiton::get_instance(name);
        assert(result.has_value());
        connections.push_back(result.value());
    }
    
    // test database operations
    for (std::size_t ndx = 0; ndx < connections.size(); ++ndx) {
        const auto& db = connections[ndx];
        const auto& name = db_names[ndx];
        
        // perform operations
        db->execute_query(std::format("CREATE TABLE test_{} (id INTEGER)", name));
        db->execute_query(std::format("INSERT INTO test_{} VALUES (1)", name));
        db->execute_query(std::format("SELECT * FROM test_{}", name));
        
        assert(db->is_connected());
        assert(db->get_query_count() >= 3);
    }
    
    // verify instance reuse
    const auto primary_again_result = DbMultiton::get_instance("primary");
    assert(primary_again_result.has_value());
    assert(primary_again_result.value().get() == connections[0].get());
    
    LOG_INFO_PRINT("Database connection multiton tests passed");
}

void test_cache_manager_multiton() {
    LOG_INFO_PRINT("\n--- Testing Cache Manager Multiton ---");
    
    using CacheMultiton = Multiton<CacheManager<std::string>>;
    CacheMultiton::clear_all_instances();
    
    // create different cache managers
    const auto session_cache_result = CacheMultiton::get_instance("sessions");
    const auto user_cache_result = CacheMultiton::get_instance("users");
    assert(session_cache_result.has_value() && user_cache_result.has_value());
    
    const auto& session_cache = session_cache_result.value();
    const auto& user_cache = user_cache_result.value();
    
    // verify different instances
    assert(session_cache.get() != user_cache.get());
    
    // test cache operations
    session_cache->put("session_1", "user_data_1");
    session_cache->put("session_2", "user_data_2");
    
    user_cache->put("user_1", "profile_data_1");
    user_cache->put("user_2", "profile_data_2");
    
    // test cache isolation
    assert(session_cache->contains("session_1"));
    assert(!session_cache->contains("user_1"));
    assert(user_cache->contains("user_1"));
    assert(!user_cache->contains("session_1"));
    
    // test cache retrieval
    const auto session_data = session_cache->get("session_1");
    assert(session_data.has_value());
    assert(session_data.value() == "user_data_1");

    const auto user_data = user_cache->get("user_1");
    assert(user_data.has_value());
    assert(user_data.value() == "profile_data_1");
    
    // test cache statistics
    const auto session_stats = session_cache->get_stats();
    const auto user_stats = user_cache->get_stats();
    
    assert(session_stats.m_cache_name == "sessions");
    assert(user_stats.m_cache_name == "users");
    assert(session_stats.m_size == 2);
    assert(user_stats.m_size == 2);
    
    LOG_INFO_PRINT("Cache manager multiton tests passed");
}

void test_edge_cases() {
    LOG_INFO_PRINT("\n--- Testing Edge Cases ---");
    
    using EdgeTestMultiton = Multiton<DatabaseConnection>;
    EdgeTestMultiton::clear_all_instances();
    
    // test very long key
    const std::string long_key(500, 'x');
    const auto long_result = EdgeTestMultiton::get_instance(long_key);
    assert(long_result.has_value());
    assert(EdgeTestMultiton::has_instance(long_key));
    
    // test special characters in keys
    const std::vector<std::string> special_keys = {
        "key with spaces",
        "key-with-dashes",
        "key_with_underscores",
        "key.with.dots",
        "key123numbers",
        "MixedCaseKey"
    };
    
    for (const auto& key : special_keys) {
        auto result = EdgeTestMultiton::get_instance(key);
        assert(result.has_value());
        assert(EdgeTestMultiton::has_instance(key));
    }
    
    // test unicode characters (if supported)
    const std::string unicode_key = "key_测试_🔑";
    const auto unicode_result = EdgeTestMultiton::get_instance(unicode_key);
    assert(unicode_result.has_value());
    assert(EdgeTestMultiton::has_instance(unicode_key));
    
    // test for_each_instance with many instances
    std::atomic<int> iteration_count{0};
    EdgeTestMultiton::for_each_instance([&iteration_count](const std::string& key, const std::shared_ptr<DatabaseConnection>& instance) {
        assert(!key.empty());
        assert(instance != nullptr);
        iteration_count.fetch_add(1);
        (void)key; // suppress unused parameter warning
        (void)instance; // suppress unused parameter warning
    });
    
    const std::size_t expected_count = special_keys.size() + 2; // +2 for long_key and unicode_key
    assert(iteration_count.load() == static_cast<int>(expected_count));
    assert(EdgeTestMultiton::size() == expected_count);
    (void)expected_count; // suppress unused variable warning for release builds
    
    LOG_INFO_PRINT("Edge case tests passed");
}

void test_performance() {
    LOG_INFO_PRINT("\n--- Testing Performance ---");
    
    using PerfTestMultiton = Multiton<DatabaseConnection>;
    PerfTestMultiton::clear_all_instances();
    
    constexpr int num_instances = 100;
    constexpr int num_lookups = 1000;
    
    // measure creation time
    const auto start_time = std::chrono::high_resolution_clock::now();
    
    for (int ndx = 0; ndx < num_instances; ++ndx) {
        const std::string key = std::format("perf_{}", ndx);
        auto result = PerfTestMultiton::get_instance(key);
        assert(result.has_value());
    }

    const auto creation_time = std::chrono::high_resolution_clock::now();
    const auto creation_duration = std::chrono::duration_cast<std::chrono::microseconds>(creation_time - start_time);
    
    // measure lookup time
    for (int ndx = 0; ndx < num_lookups; ++ndx) {
        const std::string key = std::format("perf_{}", ndx % num_instances);
        auto result = PerfTestMultiton::get_instance(key);
        assert(result.has_value());
    }

    const auto lookup_time = std::chrono::high_resolution_clock::now();
    const auto lookup_duration = std::chrono::duration_cast<std::chrono::microseconds>(lookup_time - creation_time);
    
    // verify instance count
    assert(PerfTestMultiton::size() == num_instances);
    
    LOG_INFO_PRINT("Performance test completed:");
    LOG_INFO_PRINT("  Created {} instances in {} μs ({:.2f} μs/instance)",
                  num_instances, creation_duration.count(),
                  static_cast<double>(creation_duration.count()) / num_instances);
    LOG_INFO_PRINT("  Performed {} lookups in {} μs ({:.2f} μs/lookup)",
                  num_lookups, lookup_duration.count(),
                  static_cast<double>(lookup_duration.count()) / num_lookups);
    
    LOG_INFO_PRINT("Performance tests passed");
}