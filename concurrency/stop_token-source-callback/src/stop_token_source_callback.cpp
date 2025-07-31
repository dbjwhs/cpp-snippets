// MIT License
// Copyright (c) 2025 dbjwhs

#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
#include <cassert>
#include <format>
#include "../../../headers/project_utils.hpp"

// On macOS, std::stop_token is experimental but available with -fexperimental-library flag
// for now steal/use other examples, once supported officially, I will revisit this project
// and update
#include <stop_token>

// Simple function to test stop_token functionality
void work_function(std::stop_token stop_token) {
    LOG_INFO("Thread started");
    
    int counter = 0;
    
    // This callback will be invoked when stop is requested
    std::stop_callback callback(stop_token, []() {
        LOG_INFO("Stop callback invoked");
    });
    
    while (!stop_token.stop_requested() && counter < 5) {
        LOG_INFO(std::format("Working... iteration {}", counter));
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        counter++;
        
        // Explicit cancellation check for demonstration
        if (stop_token.stop_requested()) {
            LOG_INFO("Stop requested, exiting loop");
            break;
        }
    }
    
    LOG_INFO("Thread finished");
}

int main() {
    LOG_INFO("Testing stop_token functionality on macOS");
    
    // Basic demonstration of stop_token
    {
        LOG_INFO("\nTest 1: Basic stop_token functionality");
        
        std::stop_source source;
        std::stop_token token = source.get_token();
        
        bool callback_executed = false;
        std::stop_callback callback(token, [&callback_executed]() {
            callback_executed = true;
            LOG_INFO("Callback executed");
        });
        
        LOG_INFO("Requesting stop");
        source.request_stop();
        
        assert(source.stop_requested());
        assert(token.stop_requested());
        assert(callback_executed);
        
        LOG_INFO("Test 1 passed");
    }
    
    // Using jthread for automatic handling of stop_token
    {
        LOG_INFO("\nTest 2: Using jthread with stop_token");
        
        std::jthread thread(work_function);
        
        // Let the thread run for a bit
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
        
        LOG_INFO("Requesting thread to stop");
        thread.request_stop();
        
        // Wait for thread to finish (jthread automatically joins in destructor)
        LOG_INFO("Test 2 completed");
    }
    
    // Using stop_token with a regular thread
    {
        LOG_INFO("\nTest 3: Using stop_token with regular thread");
        
        std::stop_source source;
        std::thread thread(work_function, source.get_token());
        
        // Let the thread run for a bit
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
        
        LOG_INFO("Requesting thread to stop");
        source.request_stop();
        
        // Wait for the thread to finish
        thread.join();
        
        LOG_INFO("Test 3 completed\n");
    }
    
    // Test stop_callback that's registered after stop is already requested
    {
        LOG_INFO("Test 4: Callback with already stopped token");
        
        std::stop_source source;
        source.request_stop();

        const std::stop_token token = source.get_token();
        assert(token.stop_requested());
        
        bool callback_executed = false;
        std::stop_callback callback(token, [&callback_executed]() {
            callback_executed = true;
            LOG_INFO("Callback executed immediately");
        });
        
        assert(callback_executed);
        
        LOG_INFO("Test 4 passed");
    }
    
    LOG_INFO("\nAll tests completed successfully");
    return 0;
}