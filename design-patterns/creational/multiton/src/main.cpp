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

// type aliases for cleaner code
using DatabaseMultiton = Multiton<DatabaseConnection>;
using CacheMultiton = Multiton<CacheManager<std::string>>;

// demonstration functions
void demonstrate_basic_multiton_usage();
void demonstrate_database_connections();
void demonstrate_cache_managers();
void demonstrate_factory_functions();
void demonstrate_thread_safety();
void demonstrate_error_handling();
void demonstrate_performance_characteristics();
void run_comprehensive_tests();

int main() {
    LOG_INFO_PRINT("=== Multiton Pattern Demonstration ===");
    
    try {
        demonstrate_basic_multiton_usage();
        demonstrate_database_connections();
        demonstrate_cache_managers();
        demonstrate_factory_functions();
        demonstrate_thread_safety();
        demonstrate_error_handling();
        demonstrate_performance_characteristics();
        run_comprehensive_tests();
        
        LOG_INFO_PRINT("=== All demonstrations completed successfully ===");
        
    } catch (const std::exception& e) {
        LOG_ERROR_PRINT("Exception caught in main: {}", e.what());
        return 1;
    } catch (...) {
        LOG_ERROR_PRINT("Unknown exception caught in main");
        return 1;
    }
    
    return 0;
}

void demonstrate_basic_multiton_usage() {
    LOG_INFO_PRINT("\n--- Basic Multiton Usage ---");
    
    // clear any existing instances
    DatabaseMultiton::clear_all_instances();
    
    // verify initial state
    assert(DatabaseMultiton::empty());
    assert(DatabaseMultiton::get_instance_count() == 0);
    LOG_INFO_PRINT("Initial state verified: registry is empty");
    
    // create first instance
    const auto db1_result = DatabaseMultiton::get_instance("primary");
    assert(db1_result.has_value());
    const auto& db1 = db1_result.value();
    assert(db1 != nullptr);
    assert(DatabaseMultiton::get_instance_count() == 1);
    LOG_INFO_PRINT("First instance created successfully");
    
    // get same instance again - should be identical
    const auto db1_again_result = DatabaseMultiton::get_instance("primary");
    assert(db1_again_result.has_value());
    const auto& db1_again = db1_again_result.value();
    assert(db1.get() == db1_again.get());
    assert(DatabaseMultiton::get_instance_count() == 1);
    LOG_INFO_PRINT("Same instance returned for identical key");
    
    // create second instance with different key
    const auto db2_result = DatabaseMultiton::get_instance("secondary");
    assert(db2_result.has_value());
    const auto& db2 = db2_result.value();
    assert(db2 != nullptr);
    assert(db1.get() != db2.get());
    assert(DatabaseMultiton::get_instance_count() == 2);
    LOG_INFO_PRINT("Second instance created with different key");
    
    // verify key existence
    assert(DatabaseMultiton::has_instance("primary"));
    assert(DatabaseMultiton::has_instance("secondary"));
    assert(!DatabaseMultiton::has_instance("nonexistent"));
    LOG_INFO_PRINT("Key existence verification passed");
    
    // get all keys
    auto keys = DatabaseMultiton::get_all_keys();
    assert(keys.size() == 2);
    std::ranges::sort(keys);
    assert(keys[0] == "primary");
    assert(keys[1] == "secondary");
    LOG_INFO_PRINT("All keys retrieved successfully: [{}]", 
                  std::format("{}, {}", keys[0], keys[1]));
    
    // remove one instance
    const auto remove_result = DatabaseMultiton::remove_instance("secondary");
    assert(remove_result.has_value());
    (void)remove_result; // suppress unused variable warning
    assert(!DatabaseMultiton::has_instance("secondary"));
    assert(DatabaseMultiton::get_instance_count() == 1);
    LOG_INFO_PRINT("Instance removed successfully");
    
    LOG_INFO_PRINT("Basic multiton usage demonstration completed");
}

void demonstrate_database_connections() {
    LOG_INFO_PRINT("\n--- Database Connection Management ---");
    
    DatabaseMultiton::clear_all_instances();
    
    // set up custom factory for database connections
    DatabaseMultiton::set_factory([](const std::string& key) -> std::expected<std::shared_ptr<DatabaseConnection>, MultitonError> {
        std::string connection_string;
        std::string database_name;
        
        if (key == "production") {
            connection_string = "postgresql://prod-server:5432";
            database_name = "production_db";
        } else if (key == "staging") {
            connection_string = "postgresql://staging-server:5432";
            database_name = "staging_db";
        } else if (key == "development") {
            connection_string = "postgresql://localhost:5432";
            database_name = "development_db";
        } else {
            LOG_ERROR_PRINT("Unknown database configuration: {}", key);
            return std::unexpected(MultitonError::CREATION_FAILED);
        }
        
        try {
            return std::make_shared<DatabaseConnection>(connection_string, database_name);
        } catch (...) {
            return std::unexpected(MultitonError::CREATION_FAILED);
        }
    });
    
    // create database connections for different environments

    for (const std::vector<std::string> environments = {"production", "staging", "development"}; const auto& env : environments) {
        auto db_result = DatabaseMultiton::get_instance(env);
        assert(db_result.has_value());
        const auto& db = db_result.value();
        
        // perform some operations
        db->execute_query(std::format("SELECT * FROM users WHERE environment = '{}'", env));
        db->execute_query(std::format("UPDATE settings SET environment = '{}'", env));
        
        LOG_INFO_PRINT("Database operations completed for environment: {}", env);
        std::print("  Status: {}\n", db->get_status());
    }
    
    // demonstrate connection reuse
    const auto prod_db1_result = DatabaseMultiton::get_instance("production");
    const auto prod_db2_result = DatabaseMultiton::get_instance("production");
    assert(prod_db1_result.has_value() && prod_db2_result.has_value());
    assert(prod_db1_result.value().get() == prod_db2_result.value().get());
    LOG_INFO_PRINT("Connection reuse verified for production database");
    
    // iterate over all database connections
    DatabaseMultiton::for_each_instance([](const std::string& key, const std::shared_ptr<DatabaseConnection>& db) {
        db->execute_query(std::format("ANALYZE TABLE performance_metrics_{}", key));
        LOG_INFO_PRINT("Performance analysis completed for database: {}", key);
    });
    
    DatabaseMultiton::clear_factory();
    LOG_INFO_PRINT("Database connection demonstration completed");
}

void demonstrate_cache_managers() {
    LOG_INFO_PRINT("\n--- Cache Manager Configuration ---");
    
    CacheMultiton::clear_all_instances();
    
    // set up custom factory for cache managers
    CacheMultiton::set_factory([](const std::string& key) -> std::expected<std::shared_ptr<CacheManager<std::string>>, MultitonError> {
        std::chrono::milliseconds ttl{300000}; // 5 minutes default
        std::size_t max_size{1000};
        
        if (key == "session_cache") {
            ttl = std::chrono::milliseconds{1800000}; // 30 minutes
            max_size = 10000;
        } else if (key == "api_cache") {
            ttl = std::chrono::milliseconds{600000}; // 10 minutes
            max_size = 5000;
        } else if (key == "temp_cache") {
            ttl = std::chrono::milliseconds{60000}; // 1 minute
            max_size = 500;
        }
        
        try {
            return std::make_shared<CacheManager<std::string>>(key, ttl, max_size);
        } catch (...) {
            return std::unexpected(MultitonError::CREATION_FAILED);
        }
    });
    
    // create different cache types

    for (std::vector<std::string> cache_types = {"session_cache", "api_cache", "temp_cache"}; const auto& cache_type : cache_types) {
        auto cache_result = CacheMultiton::get_instance(cache_type);
        assert(cache_result.has_value());
        const auto& cache = cache_result.value();
        
        // populate cache with test data
        for (int ndx = 0; ndx < 10; ++ndx) {
            const std::string key = std::format("{}_{}", cache_type, ndx);
            const std::string value = std::format("cached_value_{}_{}", cache_type, ndx);
            cache->put(key, value);
        }
        
        // test cache retrieval
        for (int ndx = 0; ndx < 5; ++ndx) {
            const std::string key = std::format("{}_{}", cache_type, ndx);
            auto cached_value = cache->get(key);
            assert(cached_value.has_value());
            LOG_INFO_PRINT("Retrieved from {}: {} = {}", cache_type, key, cached_value.value());
        }
        
        // display cache statistics
        auto [m_size, m_hit_count, m_miss_count, m_hit_ratio, m_cache_name] = cache->get_stats();
        LOG_INFO_PRINT("Cache stats for {}: Size={}, Hits={}, Misses={}, Hit Ratio={:.2f}",
                      m_cache_name, m_size, m_hit_count,
                      m_miss_count, m_hit_ratio);
    }
    
    // test cache interaction
    auto session_cache_result = CacheMultiton::get_instance("session_cache");
    auto api_cache_result = CacheMultiton::get_instance("api_cache");
    assert(session_cache_result.has_value() && api_cache_result.has_value());
    
    const auto& session_cache = session_cache_result.value();
    const auto& api_cache = api_cache_result.value();
    
    // verify different cache instances
    assert(session_cache.get() != api_cache.get());
    assert(session_cache->get_cache_name() != api_cache->get_cache_name());
    LOG_INFO_PRINT("Cache isolation verified");
    
    CacheMultiton::clear_factory();
    LOG_INFO_PRINT("Cache manager demonstration completed");
}

void demonstrate_factory_functions() {
    LOG_INFO_PRINT("\n--- Factory Function Demonstration ---");
    
    DatabaseMultiton::clear_all_instances();
    
    // demonstrate factory function with validation
    DatabaseMultiton::set_factory([](const std::string& key) -> std::expected<std::shared_ptr<DatabaseConnection>, MultitonError> {
        // validate key format
        if (key.length() < 3) {
            LOG_ERROR_PRINT("Database key too short: {}", key);
            return std::unexpected(MultitonError::INVALID_KEY);
        }
        
        if (key.find("test") == 0) {
            // create test database connection
            return std::make_shared<DatabaseConnection>("sqlite://memory", key);
        } else if (key.find("prod") == 0) {
            // create production database connection
            return std::make_shared<DatabaseConnection>("postgresql://prod:5432", key);
        } else {
            LOG_ERROR_PRINT("Unknown database type for key: {}", key);
            return std::unexpected(MultitonError::CREATION_FAILED);
        }
    });
    
    // test successful factory creation
    const auto test_db_result = DatabaseMultiton::get_instance("test_main");
    assert(test_db_result.has_value());
    LOG_INFO_PRINT("Test database created successfully via factory");

    const auto prod_db_result = DatabaseMultiton::get_instance("prod_primary");
    assert(prod_db_result.has_value());
    LOG_INFO_PRINT("Production database created successfully via factory");
    
    // test factory validation failure
    auto invalid_db_result = DatabaseMultiton::get_instance("ab"); // too short
    assert(!invalid_db_result.has_value());
    assert(invalid_db_result.error() == MultitonError::INVALID_KEY);
    LOG_INFO_PRINT("Factory validation correctly rejected short key");
    
    // test factory creation failure
    auto unknown_db_result = DatabaseMultiton::get_instance("unknown_type");
    assert(!unknown_db_result.has_value());
    assert(unknown_db_result.error() == MultitonError::CREATION_FAILED);
    LOG_INFO_PRINT("Factory correctly rejected unknown database type");
    
    DatabaseMultiton::clear_factory();
    LOG_INFO_PRINT("Factory function demonstration completed");
}

void demonstrate_thread_safety() {
    LOG_INFO_PRINT("\n--- Thread Safety Demonstration ---");
    
    DatabaseMultiton::clear_all_instances();
    
    constexpr int num_threads = 8;
    constexpr int operations_per_thread = 100;
    std::vector<std::thread> threads;
    std::atomic<int> success_count{0};
    std::atomic<int> failure_count{0};
    
    // launch multiple threads performing concurrent operations
    threads.reserve(num_threads);
    for (int thread_id = 0; thread_id < num_threads; ++thread_id) {
            threads.emplace_back([thread_id, &success_count, &failure_count]() {
                for (int op_id = 0; op_id < operations_per_thread; ++op_id) {
                    try {
                        // use thread_id to create different instances, but with some overlap
                        const std::string key = std::format("db_{}", thread_id % 3);

                        if (auto db_result = DatabaseMultiton::get_instance(key); db_result.has_value()) {
                            const auto& db = db_result.value();
                            db->execute_query(std::format("SELECT count(*) FROM table_{}_{}", thread_id, op_id));
                            success_count.fetch_add(1);
                        } else {
                            failure_count.fetch_add(1);
                        }

                        // occasionally test other operations
                        if (op_id % 10 == 0) {
                            DatabaseMultiton::has_instance(key);
                            DatabaseMultiton::get_instance_count();
                        }

                    } catch (...) {
                        failure_count.fetch_add(1);
                    }
                }
            });
        }
    
    // wait for all threads to complete
    for (auto& thread : threads) {
        thread.join();
    }

    constexpr int expected_operations = num_threads * operations_per_thread;
    
    assert(success_count.load() + failure_count.load() == expected_operations);
    assert(failure_count.load() == 0); // no failures expected with default factory
    assert(DatabaseMultiton::get_instance_count() == 3); // only 3 unique keys used
    
    LOG_INFO_PRINT("Thread safety test completed: {} threads, {} operations, {} successes, {} failures",
                  num_threads, expected_operations, success_count.load(), failure_count.load());
    
    // verify all instances are still accessible
    for (int ndx = 0; ndx < 3; ++ndx) {
        const std::string key = std::format("db_{}", ndx);
        assert(DatabaseMultiton::has_instance(key));
    }
    
    LOG_INFO_PRINT("Thread safety demonstration completed successfully");
}

void demonstrate_error_handling() {
    LOG_INFO_PRINT("\n--- Error Handling Demonstration ---");
    
    DatabaseMultiton::clear_all_instances();
    
    // test invalid key validation
    {
        Logger::StderrSuppressionGuard stderr_guard; // suppress expected error output
        
        auto result = DatabaseMultiton::get_instance("");
        assert(!result.has_value());
        assert(result.error() == MultitonError::INVALID_KEY);
        LOG_INFO_PRINT("Empty key correctly rejected");
    }
    
    // test key with null character
    {
        Logger::StderrSuppressionGuard stderr_guard;
        
        std::string invalid_key = "test";
        invalid_key.push_back('\0');
        invalid_key += "suffix";
        
        auto result = DatabaseMultiton::get_instance(invalid_key);
        assert(!result.has_value());
        assert(result.error() == MultitonError::INVALID_KEY);
        LOG_INFO_PRINT("Key with null character correctly rejected");
    }
    
    // test removal of non-existent instance
    {
        auto remove_result = DatabaseMultiton::remove_instance("nonexistent_key");
        assert(!remove_result.has_value());
        assert(remove_result.error() == MultitonError::INSTANCE_NOT_FOUND);
        (void)remove_result; // suppress unused variable warning
        LOG_INFO_PRINT("Removal of non-existent instance correctly failed");
    }
    
    // test factory function error propagation
    DatabaseMultiton::set_factory([](const std::string& key) -> std::expected<std::shared_ptr<DatabaseConnection>, MultitonError> {
        if (key == "fail_creation") {
            return std::unexpected(MultitonError::CREATION_FAILED);
        }
        if (key == "fail_init") {
            return std::unexpected(MultitonError::INITIALIZATION_FAILED);
        }
        return std::make_shared<DatabaseConnection>();
    });
    
    {
        Logger::StderrSuppressionGuard stderr_guard;
        
        auto fail_create_result = DatabaseMultiton::get_instance("fail_creation");
        assert(!fail_create_result.has_value());
        assert(fail_create_result.error() == MultitonError::CREATION_FAILED);
        LOG_INFO_PRINT("Factory creation failure correctly propagated");
        
        auto fail_init_result = DatabaseMultiton::get_instance("fail_init");
        assert(!fail_init_result.has_value());
        assert(fail_init_result.error() == MultitonError::INITIALIZATION_FAILED);
        LOG_INFO_PRINT("Factory initialization failure correctly propagated");
    }
    
    // test successful creation after failures
    const auto success_result = DatabaseMultiton::get_instance("success");
    assert(success_result.has_value());
    LOG_INFO_PRINT("Successful creation after previous failures");
    
    DatabaseMultiton::clear_factory();
    LOG_INFO_PRINT("Error handling demonstration completed");
}

void demonstrate_performance_characteristics() {
    LOG_INFO_PRINT("\n--- Performance Characteristics ---");
    
    DatabaseMultiton::clear_all_instances();
    
    constexpr int num_instances = 1000;
    constexpr int num_lookups = 10000;
    
    // measure instance creation time
    const auto creation_start = std::chrono::high_resolution_clock::now();
    
    for (int ndx = 0; ndx < num_instances; ++ndx) {
        const std::string key = std::format("perf_test_{}", ndx);
        auto result = DatabaseMultiton::get_instance(key);
        assert(result.has_value());
    }

    const auto creation_end = std::chrono::high_resolution_clock::now();
    const auto creation_duration = std::chrono::duration_cast<std::chrono::microseconds>(creation_end - creation_start);
    
    assert(DatabaseMultiton::get_instance_count() == num_instances);
    LOG_INFO_PRINT("Created {} instances in {} microseconds ({:.2f} μs per instance)",
                  num_instances, creation_duration.count(), 
                  static_cast<double>(creation_duration.count()) / num_instances);
    
    // measure lookup performance
    const auto lookup_start = std::chrono::high_resolution_clock::now();
    
    for (int ndx = 0; ndx < num_lookups; ++ndx) {
        const std::string key = std::format("perf_test_{}", ndx % num_instances);
        auto result = DatabaseMultiton::get_instance(key);
        assert(result.has_value());
    }

    const auto lookup_end = std::chrono::high_resolution_clock::now();
    const auto lookup_duration = std::chrono::duration_cast<std::chrono::microseconds>(lookup_end - lookup_start);
    
    LOG_INFO_PRINT("Performed {} lookups in {} microseconds ({:.2f} μs per lookup)",
                  num_lookups, lookup_duration.count(),
                  static_cast<double>(lookup_duration.count()) / num_lookups);
    
    // measure memory efficiency by checking that instances are shared
    const auto instance1_result = DatabaseMultiton::get_instance("perf_test_0");
    const auto instance2_result = DatabaseMultiton::get_instance("perf_test_0");
    assert(instance1_result.has_value() && instance2_result.has_value());
    assert(instance1_result.value().get() == instance2_result.value().get());
    LOG_INFO_PRINT("Memory efficiency verified: instances are properly shared");
    
    LOG_INFO_PRINT("Performance characteristics demonstration completed");
}

void run_comprehensive_tests() {
    LOG_INFO_PRINT("\n--- Comprehensive Test Suite ---");
    
    // test edge cases and boundary conditions
    DatabaseMultiton::clear_all_instances();
    
    // test very long key names
    const std::string long_key(1000, 'a');
    const auto long_key_result = DatabaseMultiton::get_instance(long_key);
    assert(long_key_result.has_value());
    assert(DatabaseMultiton::has_instance(long_key));
    LOG_INFO_PRINT("Very long key (1000 characters) handled successfully");
    
    // test special characters in keys
    const std::vector<std::string> special_keys = {
        "key with spaces",
        "key-with-dashes",
        "key_with_underscores",
        "key.with.dots",
        "key@with@symbols",
        "key123with456numbers",
        "MixedCaseKey",
        "UPPERCASEKEY",
        "key/with/slashes"
    };
    
    for (const auto& key : special_keys) {
        auto result = DatabaseMultiton::get_instance(key);
        assert(result.has_value());
        assert(DatabaseMultiton::has_instance(key));
    }
    LOG_INFO_PRINT("Special character keys handled successfully");
    
    // test cleanup and state consistency
    assert(DatabaseMultiton::get_instance_count() > 0);
    
    DatabaseMultiton::clear_all_instances();
    assert(DatabaseMultiton::empty());
    assert(DatabaseMultiton::get_instance_count() == 0);
    assert(DatabaseMultiton::get_all_keys().empty());
    LOG_INFO_PRINT("Complete cleanup verified");
    
    // test state after cleanup
    const auto new_instance_result = DatabaseMultiton::get_instance("post_cleanup");
    assert(new_instance_result.has_value());
    assert(DatabaseMultiton::get_instance_count() == 1);
    LOG_INFO_PRINT("State consistency after cleanup verified");
    
    // test for_each_instance functionality
    std::atomic<int> iteration_count{0};
    DatabaseMultiton::for_each_instance([&iteration_count](const std::string& key, const std::shared_ptr<DatabaseConnection>& instance) {
        assert(!key.empty());
        assert(instance != nullptr);
        iteration_count.fetch_add(1);
        (void)key; // suppress unused parameter warning
        (void)instance; // suppress unused parameter warning
    });
    assert(iteration_count.load() == 1);
    LOG_INFO_PRINT("for_each_instance functionality verified");
    
    LOG_INFO_PRINT("All comprehensive tests passed successfully");
}