// MIT License
// Copyright (c) 2025 dbjwhs

#include "../headers/real_service.hpp"
#include "../../../../headers/project_utils.hpp"
#include <algorithm>
#include <limits>
#include <ranges>

namespace null_object_pattern {

//
// constructor implementation with proper initialization
// reserves space for message history to optimize performance
// logs service creation for debugging and monitoring
//
template<std::copyable MessageType>
real_service<MessageType>::real_service() {
    // reserve space for expected message volume to reduce reallocations
    m_message_history.reserve(1000);
    
    LOG_INFO_PRINT("real service created and initialized successfully");
    LOG_INFO_PRINT("message history reserved for {} messages", m_message_history.capacity());
}

//
// destructor implementation with cleanup logging
// provides final statistics and resource cleanup confirmation
//
template<std::copyable MessageType>
real_service<MessageType>::~real_service() {
    if (m_is_active) {
        LOG_WARNING_PRINT("service destroyed while still active, performing emergency shutdown");
        // perform emergency shutdown without error checking
        m_is_active = false;
    }
    
    const auto uptime = get_uptime();
    LOG_INFO_PRINT("real service destroyed after processing {} messages", m_message_count);
    LOG_INFO_PRINT("service uptime was {} milliseconds", uptime.count());
}

//
// message processing implementation with full validation and logging
// maintains comprehensive audit trail and error handling
//
template<std::copyable MessageType>
std::expected<bool, std::string> 
real_service<MessageType>::process_message(const MessageType& message) {
    // check if service is active before processing
    if (!m_is_active) {
        const std::string error_msg = "service is not active, cannot process message";
        LOG_ERROR_PRINT("{}", error_msg);
        return std::unexpected(error_msg);
    }
    
    // validate message content before processing
    if (!validate_message(message)) {
        const std::string error_msg = "message validation failed, invalid content";
        LOG_ERROR_PRINT("{}", error_msg);
        return std::unexpected(error_msg);
    }
    
    // check message history capacity and manage if needed
    if (m_message_history.size() >= m_message_history.capacity()) {
        LOG_WARNING_PRINT("message history at capacity, clearing old entries");
        // keep only the most recent half of messages
        const size_t keep_count = m_message_history.size() / 2;
        m_message_history.erase(m_message_history.begin(), 
                               m_message_history.end() - keep_count);
    }
    
    // store message in history using emplace_back for efficiency
    m_message_history.emplace_back(message);
    ++m_message_count;
    
    // log successful processing with message details
    const std::string formatted_message = format_message_for_logging(message);
    LOG_INFO_PRINT("processed message successfully: {}", formatted_message);
    LOG_INFO_PRINT("total messages processed: {}", m_message_count);
    
    return true;
}

//
// configuration implementation with validation and state management
// ensures configuration integrity and provides audit trail
//
template<std::copyable MessageType>
std::expected<bool, std::string> 
real_service<MessageType>::configure(const std::string& config) {
    // validate configuration format and content
    if (!validate_configuration(config)) {
        const std::string error_msg = "configuration validation failed: invalid format or content";
        LOG_ERROR_PRINT("{}", error_msg);
        return std::unexpected(error_msg);
    }
    
    // store previous configuration for rollback capability
    const std::string previous_config = m_configuration;
    
    // apply new configuration
    m_configuration = config;
    m_last_config_time = std::chrono::steady_clock::now();
    
    // activate service if it was inactive
    if (!m_is_active) {
        m_is_active = true;
        LOG_INFO_PRINT("service activated during configuration");
    }
    
    LOG_INFO_PRINT("configuration updated successfully from '{}' to '{}'", 
                   previous_config, m_configuration);
    
    return true;
}

//
// status reporting implementation with comprehensive information
// provides detailed service state for monitoring and debugging
//
template<std::copyable MessageType>
std::expected<std::string, std::string> 
real_service<MessageType>::get_status() const {
    std::ostringstream status_stream;
    
    // build comprehensive status report
    status_stream << "real service status report:\n";
    status_stream << "  active: " << (m_is_active ? "yes" : "no") << "\n";
    status_stream << "  configuration: '" << m_configuration << "'\n";
    status_stream << "  messages processed: " << m_message_count << "\n";
    status_stream << "  history size: " << m_message_history.size() << "\n";
    status_stream << "  uptime: " << get_uptime().count() << " milliseconds\n";
    
    // calculate time since last configuration
    const auto config_age = std::chrono::duration_cast<std::chrono::minutes>(
        std::chrono::steady_clock::now() - m_last_config_time);
    status_stream << "  last configured: " << config_age.count() << " minutes ago";
    
    std::string status_report = status_stream.str();
    LOG_INFO_PRINT("status report generated: {} characters", status_report.length());
    
    return status_report;
}

//
// shutdown implementation with proper cleanup and finalization
// ensures graceful termination with resource cleanup
//
template<std::copyable MessageType>
std::expected<bool, std::string> 
real_service<MessageType>::shutdown() {
    if (!m_is_active) {
        const std::string warning_msg = "service already inactive, shutdown request ignored";
        LOG_WARNING_PRINT("{}", warning_msg);
        return std::unexpected(warning_msg);
    }
    
    // log final statistics before shutdown
    const auto uptime = get_uptime();
    LOG_INFO_PRINT("initiating service shutdown after {} milliseconds uptime", uptime.count());
    LOG_INFO_PRINT("final message count: {}", m_message_count);
    LOG_INFO_PRINT("message history entries: {}", m_message_history.size());
    
    // perform cleanup operations
    m_is_active = false;
    // note: we keep message history for post-shutdown analysis
    
    LOG_INFO_PRINT("service shutdown completed successfully");
    return true;
}

//
// null object identification implementation
// clearly identifies this as a real service implementation
//
template<std::copyable MessageType>
bool real_service<MessageType>::is_null_object() const {
    return false;
}

//
// message history accessor implementation
// provides read-only access to processed message history
//
template<std::copyable MessageType>
const std::vector<MessageType>& 
real_service<MessageType>::get_message_history() const {
    return m_message_history;
}

//
// message count accessor implementation
// returns current message processing statistics
//
template<std::copyable MessageType>
size_t real_service<MessageType>::get_message_count() const {
    return m_message_count;
}

//
// uptime calculation implementation
// provides precise service runtime duration
//
template<std::copyable MessageType>
std::chrono::milliseconds 
real_service<MessageType>::get_uptime() const {
    const auto current_time = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        current_time - m_creation_time);
}

//
// message validation implementation
// ensures message content meets processing requirements
//
template<std::copyable MessageType>
bool real_service<MessageType>::validate_message(const MessageType& message) const {
    // for string messages, check for empty content
    if constexpr (std::is_same_v<MessageType, std::string>) {
        if (message.empty()) {
            LOG_WARNING_PRINT("rejecting empty string message");
            return false;
        }
        
        // check for reasonable message size limits
        constexpr size_t max_message_size = 10000;
        if (message.length() > max_message_size) {
            LOG_WARNING_PRINT("rejecting oversized message: {} characters", message.length());
            return false;
        }
    }
    
    // for numeric types, check for valid ranges
    if constexpr (std::is_arithmetic_v<MessageType>) {
        if constexpr (std::is_floating_point_v<MessageType>) {
            if (!std::isfinite(message)) {
                LOG_WARNING_PRINT("rejecting non-finite numeric message");
                return false;
            }
        }
    }
    
    return true;
}

//
// configuration validation implementation
// ensures configuration parameters are valid and safe
//
template<std::copyable MessageType>
bool real_service<MessageType>::validate_configuration(const std::string& config) const {
    // reject empty configurations
    if (config.empty()) {
        LOG_WARNING_PRINT("rejecting empty configuration");
        return false;
    }
    
    // check for reasonable configuration size
    constexpr size_t max_config_size = 1000;
    if (config.length() > max_config_size) {
        LOG_WARNING_PRINT("rejecting oversized configuration: {} characters", config.length());
        return false;
    }
    
    // check for forbidden characters or patterns
    const std::vector<std::string> forbidden_patterns = {"../", "null", "void"};
    if (std::ranges::any_of(forbidden_patterns, [&config](const std::string& pattern) {
        return config.find(pattern) != std::string::npos;
    })) {
        LOG_WARNING_PRINT("rejecting configuration containing forbidden pattern");
        return false;
    }
    
    return true;
}

//
// message formatting implementation for consistent logging
// provides uniform message representation across different types
//
template<std::copyable MessageType>
std::string real_service<MessageType>::format_message_for_logging(const MessageType& message) const {
    std::ostringstream formatter;
    
    if constexpr (std::is_same_v<MessageType, std::string>) {
        // for strings, add quotes and truncate if necessary
        constexpr size_t max_log_length = 100;
        if (message.length() > max_log_length) {
            formatter << "\"" << message.substr(0, max_log_length) << "...\" (truncated)";
        } else {
            formatter << "\"" << message << "\"";
        }
    } else {
        // for other types, use stream operator
        formatter << message;
    }
    
    return formatter.str();
}

//
// explicit template instantiations for common types
// ensures template code is compiled and available in the library
//
template class real_service<std::string>;
template class real_service<int>;
template class real_service<double>;

} // namespace null_object_pattern
