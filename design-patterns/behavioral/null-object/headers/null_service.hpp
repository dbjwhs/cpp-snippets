// MIT License
// Copyright (c) 2025 dbjwhs

#pragma once

#include "service_interface.hpp"
#include <string>
#include <memory>
#include <expected>

namespace null_object_pattern {

//
// null object implementation of service_interface that performs no operations
// this class represents the "null" object in the null object pattern
// it provides neutral behavior that satisfies the interface contract without doing actual work
// eliminates the need for null pointer checks and conditional logic in client code
//
template<std::copyable MessageType>
class null_service : public service_interface<MessageType> {
public:
    //
    // default constructor creates a null service instance
    // no initialization required as null objects maintain no state
    //
    null_service() = default;
    
    //
    // default destructor with no cleanup required
    // null objects have no resources to release
    //
    ~null_service() override = default;
    
    //
    // accepts any message but performs no processing
    // always returns successful result to maintain interface contract
    // provides neutral behavior that doesn't affect program flow
    //
    std::expected<bool, std::string> process_message(const MessageType& message) override;
    
    //
    // accepts any configuration but makes no changes
    // always returns successful result to indicate acceptance
    // maintains consistent behavior regardless of configuration content
    //
    std::expected<bool, std::string> configure(const std::string& config) override;
    
    //
    // returns standard status indicating null object state
    // provides consistent status message for identification
    // useful for debugging and system monitoring
    //
    [[nodiscard]] std::expected<std::string, std::string> get_status() const override;
    
    //
    // performs no shutdown operations but returns success
    // maintains interface contract while doing nothing
    // allows clean integration with resource management patterns
    //
    std::expected<bool, std::string> shutdown() override;
    
    //
    // identifies this as a null object implementation
    // allows clients to detect null object behavior when needed
    // enables conditional logic for special handling if required
    //
    [[nodiscard]] bool is_null_object() const override;
    
    //
    // copy operations are allowed for null objects
    //  have no state to corrupt through copying
    // enables flexible usage patterns with value semantics
    //
    null_service(const null_service&) = default;
    null_service& operator=(const null_service&) = default;
    
    //
    // move operations are efficient for stateless objects
    // provides consistent interface with real service implementations
    // enables polymorphic usage with move semantics
    //
    null_service(null_service&&) = default;
    null_service& operator=(null_service&&) = default;
};

} // namespace null_object_pattern