// MIT License
// Copyright (c) 2025 dbjwhs

#pragma once

#include <string>
#include <memory>
#include <expected>

//
// service interface design pattern: null object pattern
//
// the null object pattern was introduced by bobby woolf in the pattern languages of program design 3 (1997),
// though the concept existed earlier in smalltalk environments. this behavioral pattern provides a way to
// avoid null pointer checks by using polymorphism to handle the "do nothing" case. instead of using null
// references, the pattern provides an object with neutral (null) behavior that implements the same interface
// as the real object. this eliminates the need for conditional checks and makes code more readable and
// maintainable.
//
// the pattern became widely recognized after martin fowler's refactoring book (1999) and joshua bloch's
// effective java series. it's particularly useful in c++ where null pointer dereferencing can cause
// undefined behavior and crashes. the pattern works by defining a common interface, implementing both
// a real object and a null object that performs no operations, and using polymorphism to treat them
// uniformly.
//
// common usage scenarios include:
// - logging systems where you might want to disable logging without conditional checks
// - database connections where you need a "disconnected" state
// - ui components that might not be initialized
// - command objects that might represent "no operation"
// - visitor patterns where some nodes might not need processing
// - state machines where some states perform no actions
//

namespace null_object_pattern {

// forward declarations
template<std::copyable MessageType>
class service_interface;

template<std::copyable MessageType>
class real_service;

template<std::copyable MessageType>
class null_service;

//
// abstract interface that defines the contract for both real and null implementations
// this interface establishes the common behavior that all concrete implementations must provide
//
template<std::copyable MessageType>
class service_interface {
public:
    // virtual destructor ensures proper cleanup of derived classes
    virtual ~service_interface() = default;
    
    //
    // core service operation that processes messages
    // returns expected<bool, std::string> following railway-oriented programming
    // true indicates successful processing, false indicates graceful failure
    //
    virtual std::expected<bool, std::string> process_message(const MessageType& message) = 0;
    
    //
    // configuration operation that applies settings to the service
    // allows runtime configuration of service behavior
    //
    virtual std::expected<bool, std::string> configure(const std::string& config) = 0;
    
    //
    // status query operation that returns current service state
    // provides introspection into service health and configuration
    //
    [[nodiscard]] virtual std::expected<std::string, std::string> get_status() const = 0;
    
    //
    // cleanup operation that releases resources and prepares for shutdown
    // ensures proper resource management and graceful termination
    //
    virtual std::expected<bool, std::string> shutdown() = 0;
    
    //
    // factory method that creates appropriate service instance based on configuration
    // returns real service for normal operation, null service for disabled/testing scenarios
    //
    static std::unique_ptr<service_interface<MessageType>> create_service(bool enable_real_service);
    
    //
    // utility method to check if this is a null object implementation
    // allows clients to differentiate between real and null behavior when needed
    //
    [[nodiscard]] virtual bool is_null_object() const = 0;

    //
    // disable copy operations to prevent object slicing
    // services should be managed through smart pointers
    //
    service_interface(const service_interface&) = delete;
    service_interface& operator=(const service_interface&) = delete;

protected:
    //
    // protected constructor prevents direct instantiation
    // forces use of factory method or derived class constructors
    //
    service_interface() = default;
    
    //
    // enable move operations for efficient resource transfer
    // allows transfer of service ownership when needed
    //
    service_interface(service_interface&&) = default;
    service_interface& operator=(service_interface&&) = default;
};

//
// alias template for easier usage with common string message type
// provides convenient shorthand for the most common use case
//
using string_service = service_interface<std::string>;

//
// factory function for creating string-based services
// convenience function that wraps the template factory method
//
std::unique_ptr<string_service> create_string_service(bool enable_real_service);

} // namespace null_object_pattern