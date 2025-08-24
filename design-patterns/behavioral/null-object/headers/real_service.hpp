// MIT License
// Copyright (c) 2025 dbjwhs

#pragma once

#include "service_interface.hpp"
#include "../../../../headers/project_utils.hpp"
#include <string>
#include <memory>
#include <expected>
#include <vector>
#include <chrono>
#include <sstream>

namespace null_object_pattern {

//
// concrete implementation of service_interface that performs actual work
// this class represents the "real" object in the null object pattern
// it maintains state, processes messages, and provides full functionality
//
template<std::copyable MessageType>
class real_service : public service_interface<MessageType> {
private:
    //
    // internal state variables using member prefix convention
    // all member variables are initialized using braced initialization
    //
    
    // tracks whether the service is currently active and processing messages
    bool m_is_active{false};
    
    // stores the current configuration string for service behavior
    std::string m_configuration{"default"};
    
    // maintains a history of processed messages for debugging and analysis
    std::vector<MessageType> m_message_history{};
    
    // counts the total number of messages processed since service creation
    size_t m_message_count{0};
    
    // records the timestamp when the service was created
    std::chrono::steady_clock::time_point m_creation_time{std::chrono::steady_clock::now()};
    
    // tracks the last configuration timestamp for status reporting
    std::chrono::steady_clock::time_point m_last_config_time{std::chrono::steady_clock::now()};

public:
    //
    // constructor initializes the service in an active state
    // reserves space for message history to avoid frequent reallocations
    //
    explicit real_service();
    
    //
    // destructor ensures proper cleanup and logging of service shutdown
    //
    ~real_service() override;
    
    //
    // processes incoming messages with full validation and logging
    // maintains message history and updates statistics
    // returns success/failure status with descriptive error messages
    //
    std::expected<bool, std::string> process_message(const MessageType& message) override;
    
    //
    // applies configuration changes to modify service behavior
    // validates configuration parameters and updates internal state
    // logs configuration changes for audit trail
    //
    std::expected<bool, std::string> configure(const std::string& config) override;
    
    //
    // provides comprehensive status information including:
    // - service state (active/inactive)
    // - message processing statistics
    // - uptime calculation
    // - current configuration
    //
    [[nodiscard]] std::expected<std::string, std::string> get_status() const override;
    
    //
    // gracefully shuts down the service with proper cleanup
    // flushes any pending operations and releases resources
    // logs shutdown event with final statistics
    //
    std::expected<bool, std::string> shutdown() override;
    
    //
    // identifies this as a real service implementation
    // used by clients to distinguish between real and null objects
    //
    [[nodiscard]] bool is_null_object() const override;
    
    //
    // provides access to message history for debugging and analysis
    // returns const reference to prevent external modification
    //
    const std::vector<MessageType>& get_message_history() const;
    
    //
    // returns current message processing statistics
    // useful for performance monitoring and capacity planning
    //
    [[nodiscard]] size_t get_message_count() const;
    
    //
    // calculates service uptime in milliseconds
    // provides runtime duration for monitoring and debugging
    //
    [[nodiscard]] std::chrono::milliseconds get_uptime() const;

private:
    //
    // validates message content before processing
    // checks for empty messages, size limits, and format requirements
    //
    bool validate_message(const MessageType& message) const;
    
    //
    // validates configuration string format and content
    // ensures configuration parameters are within acceptable ranges
    //
    [[nodiscard]] bool validate_configuration(const std::string& config) const;
    
    //
    // formats message for logging and storage
    // applies consistent formatting rules across all message types
    //
    std::string format_message_for_logging(const MessageType& message) const;
};

} // namespace null_object_pattern
