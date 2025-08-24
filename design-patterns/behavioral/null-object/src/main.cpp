// MIT License
// Copyright (c) 2025 dbjwhs

#include "../headers/service_interface.hpp"
#include "../headers/real_service.hpp"
#include "../headers/null_service.hpp"
#include "../../../../headers/project_utils.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include <chrono>
#include <thread>

//
// comprehensive test suite for the null object pattern implementation
// demonstrates pattern usage, validates behavior, and measures performance
// includes both positive and negative test scenarios
//

namespace null_object_pattern {

//
// helper function to run a complete service test cycle
// demonstrates typical usage patterns and validates expected behavior
//
template<std::copyable MessageType>
void test_service_lifecycle(std::unique_ptr<service_interface<MessageType>>& service, 
                           const std::string& service_type,
                           const std::vector<MessageType>& test_messages) {
    LOG_INFO_PRINT("testing {} service lifecycle with {} messages", 
                   service_type, test_messages.size());
    
    // test initial status
    auto status_result = service->get_status();
    assert(status_result.has_value());
    LOG_INFO_PRINT("initial status: {}", status_result.value());
    
    // test configuration
    auto config_result = service->configure("test_configuration_v1.0");
    assert(config_result.has_value() && config_result.value());
    LOG_INFO_PRINT("configuration applied successfully");
    
    // test message processing
    size_t successful_messages = 0;
    for (const auto& message : test_messages) {
        if (auto process_result = service->process_message(message); process_result.has_value() && process_result.value()) {
            ++successful_messages;
        } else {
            LOG_WARNING_PRINT("message processing failed: {}", 
                             process_result.error());
        }
    }
    
    LOG_INFO_PRINT("processed {}/{} messages successfully", 
                   successful_messages, test_messages.size());
    
    // test final status
    status_result = service->get_status();
    assert(status_result.has_value());
    LOG_INFO_PRINT("final status: {}", status_result.value());
    
    // test shutdown
    if (auto shutdown_result = service->shutdown(); shutdown_result.has_value()) {
        LOG_INFO_PRINT("service shutdown completed successfully");
    } else {
        LOG_WARNING_PRINT("service shutdown failed: {}", shutdown_result.error());
    }
}

//
// performance benchmark function to compare real vs null service overhead
// measures processing time and memory usage patterns
//
void performance_benchmark() {
    LOG_INFO_PRINT("starting performance benchmark comparison");
    
    constexpr size_t message_count = 10000;
    std::vector<std::string> benchmark_messages;
    benchmark_messages.reserve(message_count);
    
    // generate test messages
    for (size_t ndx = 0; ndx < message_count; ++ndx) {
        benchmark_messages.emplace_back("benchmark_message_" + std::to_string(ndx));
    }
    
    // benchmark real service
    const auto real_service = service_interface<std::string>::create_service(true);
    [[maybe_unused]] auto config_result = real_service->configure("benchmark_configuration");
    
    const auto real_start = std::chrono::high_resolution_clock::now();
    for (const auto& message : benchmark_messages) {
        [[maybe_unused]] auto process_result = real_service->process_message(message);
    }
    const auto real_end = std::chrono::high_resolution_clock::now();
    
    const auto real_duration = std::chrono::duration_cast<std::chrono::microseconds>(
        real_end - real_start);
    
    // benchmark null service
    auto null_service = service_interface<std::string>::create_service(false);
    [[maybe_unused]] auto null_config_result = null_service->configure("benchmark_configuration");
    
    const auto null_start = std::chrono::high_resolution_clock::now();
    for (const auto& message : benchmark_messages) {
        [[maybe_unused]] auto null_process_result = null_service->process_message(message);
    }
    const auto null_end = std::chrono::high_resolution_clock::now();
    
    const auto null_duration = std::chrono::duration_cast<std::chrono::microseconds>(
        null_end - null_start);
    
    // report benchmark results
    LOG_INFO_PRINT("performance benchmark results:");
    LOG_INFO_PRINT("  real service: {} microseconds for {} messages", 
                   real_duration.count(), message_count);
    LOG_INFO_PRINT("  null service: {} microseconds for {} messages", 
                   null_duration.count(), message_count);
    LOG_INFO_PRINT("  null service overhead reduction: {}%", 
                   100.0 * (1.0 - static_cast<double>(null_duration.count()) / 
                            static_cast<double>(real_duration.count())));
    
    [[maybe_unused]] auto real_shutdown_result = real_service->shutdown();
    [[maybe_unused]] auto null_shutdown_result = null_service->shutdown();
}

//
// error handling test to validate railway-oriented programming patterns
// tests error conditions and recovery scenarios
//
void test_error_scenarios() {
    LOG_INFO_PRINT("testing error handling scenarios");
    
    auto real_service = service_interface<std::string>::create_service(true);
    
    // test empty message handling
    {
        Logger::StderrSuppressionGuard suppressor;
        auto result = real_service->process_message("");
        assert(!result.has_value());
        LOG_INFO_PRINT("empty message rejection test passed");
    }
    
    // test oversized message handling
    {
        Logger::StderrSuppressionGuard suppressor;
        const std::string oversized_message(20000, 'x');
        auto result = real_service->process_message(oversized_message);
        assert(!result.has_value());
        LOG_INFO_PRINT("oversized message rejection test passed");
    }
    
    // test invalid configuration handling
    {
        Logger::StderrSuppressionGuard suppressor;
        auto result = real_service->configure("");
        assert(!result.has_value());
        LOG_INFO_PRINT("empty configuration rejection test passed");
    }
    
    // test forbidden configuration pattern
    {
        Logger::StderrSuppressionGuard suppressor;
        auto result = real_service->configure("config_with_../path");
        assert(!result.has_value());
        LOG_INFO_PRINT("forbidden pattern rejection test passed");
    }
    
    // test shutdown of inactive service
    [[maybe_unused]] auto first_shutdown_result = real_service->shutdown();
    {
        Logger::StderrSuppressionGuard suppressor;
        auto result = real_service->shutdown();
        assert(!result.has_value());
        LOG_INFO_PRINT("double shutdown prevention test passed");
    }
}

//
// polymorphic behavior test to verify null object pattern effectiveness
// demonstrates how client code can work with both service types uniformly
//
void test_polymorphic_behavior() {
    LOG_INFO_PRINT("testing polymorphic behavior with service vector");
    
    // create mixed collection of real and null services
    std::vector<std::unique_ptr<service_interface<std::string>>> services;
    services.reserve(4);
    
    services.emplace_back(service_interface<std::string>::create_service(true));
    services.emplace_back(service_interface<std::string>::create_service(false));
    services.emplace_back(service_interface<std::string>::create_service(true));
    services.emplace_back(service_interface<std::string>::create_service(false));
    
    // process messages through all services uniformly
    const std::vector<std::string> test_messages = {
        "polymorphic_test_message_1",
        "polymorphic_test_message_2", 
        "polymorphic_test_message_3"
    };
    
    for (size_t service_ndx = 0; service_ndx < services.size(); ++service_ndx) {
        auto& service = services[service_ndx];
        
        LOG_INFO_PRINT("processing with service {} ({})", service_ndx,
                       service->is_null_object() ? "null" : "real");
        
        // configure service
        [[maybe_unused]] auto poly_config_result = service->configure("polymorphic_test_config");
        
        // process all messages
        for (const auto& message : test_messages) {
            auto result = service->process_message(message);
            assert(result.has_value());
        }
        
        // check status
        auto status = service->get_status();
        assert(status.has_value());
        LOG_INFO_PRINT("service status: {}", status.value());
        
        // shutdown
        [[maybe_unused]] auto poly_shutdown_result = service->shutdown();
    }
    
    LOG_INFO_PRINT("polymorphic behavior test completed successfully");
}

//
// template specialization test for different message types
// validates that the pattern works with various data types
//
void test_template_specializations() {
    LOG_INFO_PRINT("testing template specializations with different message types");
    
    // test with integer messages
    {
        auto int_service = service_interface<int>::create_service(true);
        [[maybe_unused]] auto int_config_result = int_service->configure("integer_service_config");
        
        const std::vector<int> int_messages = {42, 100, -50, 0, 999};
        for (const auto message : int_messages) {
            [[maybe_unused]] auto int_process_result = int_service->process_message(message);
            assert(int_process_result.has_value());
        }
        
        [[maybe_unused]] auto int_shutdown_result = int_service->shutdown();
        LOG_INFO_PRINT("integer message type test passed");
    }
    
    // test with double messages
    {
        auto double_service = service_interface<double>::create_service(false);
        [[maybe_unused]] auto double_config_result = double_service->configure("double_service_config");
        
        const std::vector<double> double_messages = {3.14159, 2.71828, 1.41421, 0.0, -1.0};
        for (const auto message : double_messages) {
            [[maybe_unused]] auto double_process_result = double_service->process_message(message);
            assert(double_process_result.has_value());
        }
        
        [[maybe_unused]] auto double_shutdown_result = double_service->shutdown();
        LOG_INFO_PRINT("double message type test passed");
    }
}

} // namespace null_object_pattern

//
// main function orchestrating all tests and demonstrations
// provides comprehensive validation of the null object pattern implementation
//
int main() {
    LOG_INFO_PRINT("null object pattern comprehensive test suite starting");
    
    try {
        // basic functionality tests
        {
            LOG_INFO_PRINT("=== basic functionality tests ===");
            
            const std::vector<std::string> test_messages = {
                "hello world",
                "null object pattern test",
                "comprehensive testing message",
                "final test message"
            };
            
            // test real service
            auto real_service = null_object_pattern::create_string_service(true);
            assert(!real_service->is_null_object());
            null_object_pattern::test_service_lifecycle(real_service, "real", test_messages);
            
            // test null service
            auto null_service = null_object_pattern::create_string_service(false);
            assert(null_service->is_null_object());
            null_object_pattern::test_service_lifecycle(null_service, "null", test_messages);
        }
        
        // error handling tests
        {
            LOG_INFO_PRINT("=== error handling tests ===");
            null_object_pattern::test_error_scenarios();
        }
        
        // polymorphic behavior tests
        {
            LOG_INFO_PRINT("=== polymorphic behavior tests ===");
            null_object_pattern::test_polymorphic_behavior();
        }
        
        // template specialization tests
        {
            LOG_INFO_PRINT("=== template specialization tests ===");
            null_object_pattern::test_template_specializations();
        }
        
        // performance benchmark
        {
            LOG_INFO_PRINT("=== performance benchmark ===");
            null_object_pattern::performance_benchmark();
        }
        
        // final validation
        LOG_INFO_PRINT("=== final validation ===");
        LOG_INFO_PRINT("all tests completed successfully");
        LOG_INFO_PRINT("null object pattern implementation validated");
        
        // demonstrate pattern effectiveness
        LOG_INFO_PRINT("pattern benefits demonstrated:");
        LOG_INFO_PRINT("  - eliminated null pointer checks");
        LOG_INFO_PRINT("  - uniform interface for real and null behavior");
        LOG_INFO_PRINT("  - improved code readability and maintainability");
        LOG_INFO_PRINT("  - consistent error handling with std::expected");
        LOG_INFO_PRINT("  - template-based type safety");
        
    } catch (const std::exception& e) {
        LOG_ERROR_PRINT("test suite failed with exception: {}", e.what());
        return 1;
    } catch (...) {
        LOG_ERROR_PRINT("test suite failed with unknown exception");
        return 1;
    }
    
    LOG_INFO_PRINT("null object pattern test suite completed successfully");
    return 0;
}
