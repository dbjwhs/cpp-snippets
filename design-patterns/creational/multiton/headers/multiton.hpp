// MIT License
// Copyright (c) 2025 dbjwhs

#ifndef MULTITON_HPP
#define MULTITON_HPP

// The Multiton Design Pattern: A Comprehensive Historical Analysis
//
// The Multiton pattern, first documented by Erich Gamma and colleagues in the early 1990s as an extension
// to the Singleton pattern, represents a sophisticated approach to controlled instance management. Unlike
// the Singleton which ensures exactly one instance of a class, the Multiton pattern manages a finite set
// of named instances, each identified by a unique key. This pattern emerged from the need to have multiple
// instances of a class while still maintaining strict control over their creation and access.
//
// Historical Development:
// The pattern was formally introduced in the "Design Patterns: Elements of Reusable Object-Oriented Software"
// book (1994) as a variation of the Singleton pattern. However, its practical applications became more
// apparent with the rise of multithreaded applications and distributed systems in the late 1990s and
// early 2000s. The pattern gained significant traction in enterprise applications where multiple database
// connections, configuration managers, or service endpoints needed to be managed efficiently.
//
// Core Mechanics:
// The Multiton pattern operates through a registry mechanism that maps unique keys to specific instances.
// When a client requests an instance with a particular key, the pattern either returns an existing instance
// from the registry or creates a new one if it doesn't exist. This approach provides controlled instantiation
// while allowing multiple instances to coexist, each serving different purposes or contexts.
//
// Modern C++23 Implementation Features:
// This implementation leverages cutting-edge C++23 features including concept-constrained template parameters,
// std::expected for Railway-Oriented Programming, comprehensive RAII resource management, and advanced
// template specialization techniques. The design emphasizes thread safety, performance optimization through
// move semantics, and robust error handling without exceptions.
//
// Common Use Cases:
// - Database connection pools with different configurations
// - Cache managers for different data types or regions
// - Logger instances for different modules or subsystems
// - Configuration managers for different environments (dev, staging, prod)
// - Service locators managing multiple service endpoints
// - Thread pools optimized for different workload types
// - Resource managers handling different resource categories

#include <memory>
#include <unordered_map>
#include <mutex>
#include <string>
#include <concepts>
#include <expected>
#include <functional>
#include <type_traits>
#include "../../../../headers/project_utils.hpp"

namespace multiton_pattern {

// error types for railway-oriented programming
enum class MultitonError {
    INVALID_KEY,
    CREATION_FAILED,
    INSTANCE_NOT_FOUND,
    INITIALIZATION_FAILED,
    THREAD_SAFETY_VIOLATION
};

// helper function to convert error to string
constexpr std::string_view error_to_string(const MultitonError error) noexcept {
    switch (error) {
        case MultitonError::INVALID_KEY:
            return "Invalid key provided";
        case MultitonError::CREATION_FAILED:
            return "Failed to create instance";
        case MultitonError::INSTANCE_NOT_FOUND:
            return "Instance not found";
        case MultitonError::INITIALIZATION_FAILED:
            return "Failed to initialize instance";
        case MultitonError::THREAD_SAFETY_VIOLATION:
            return "Thread safety violation detected";
        default:
            return "Unknown error";
    }
}

// concept for types that can be managed by multiton with modern c++23 template parameter naming
template<typename InstanceType>
requires std::destructible<InstanceType>
class Multiton {
private:
    // type alias for the registry mapping keys to instances
    using InstanceRegistry = std::unordered_map<std::string, std::shared_ptr<InstanceType>>;
    
    // type alias for factory function
    using FactoryFunction = std::function<std::expected<std::shared_ptr<InstanceType>, MultitonError>(const std::string&)>;

    // static registry to store all instances by key
    inline static InstanceRegistry m_instance_registry{};
    
    // mutex for thread-safe access to the registry
    inline static std::mutex m_registry_mutex{};
    
    // optional factory function for custom instance creation
    inline static FactoryFunction m_factory_function{};

    // private constructor to prevent direct instantiation
    Multiton() = default;

    // validate key before operations
    static std::expected<void, MultitonError> validate_key(const std::string& key) noexcept {
        if (key.empty()) {
            LOG_ERROR_PRINT("Validation failed: empty key provided");
            return std::unexpected(MultitonError::INVALID_KEY);
        }
        
        // additional validation: check for special characters that might cause issues
        if (key.find('\0') != std::string::npos) {
            LOG_ERROR_PRINT("Validation failed: key contains null character");
            return std::unexpected(MultitonError::INVALID_KEY);
        }
        
        return {};
    }

    // internal method to create instance using factory or default constructor
    static std::expected<std::shared_ptr<InstanceType>, MultitonError> create_instance(const std::string& key) {
        LOG_INFO_PRINT("Creating new instance for key: {}", key);
        
        try {
            std::shared_ptr<InstanceType> instance;
            
            if (m_factory_function) {
                // use custom factory function if available
                auto factory_result = m_factory_function(key);
                if (!factory_result) {
                    LOG_ERROR_PRINT("Factory function failed for key: {}", key);
                    return std::unexpected(factory_result.error());
                }
                instance = std::move(factory_result.value());
            } else {
                // default construction
                if constexpr (std::is_default_constructible_v<InstanceType>) {
                    instance = std::make_shared<InstanceType>();
                } else {
                    LOG_ERROR_PRINT("Type {} is not default constructible and no factory provided", typeid(InstanceType).name());
                    return std::unexpected(MultitonError::CREATION_FAILED);
                }
            }
            
            if (!instance) {
                LOG_ERROR_PRINT("Failed to create instance for key: {}", key);
                return std::unexpected(MultitonError::CREATION_FAILED);
            }
            
            LOG_INFO_PRINT("Successfully created instance for key: {}", key);
            return instance;
            
        } catch (...) {
            LOG_ERROR_PRINT("Exception occurred while creating instance for key: {}", key);
            return std::unexpected(MultitonError::CREATION_FAILED);
        }
    }

public:
    // delete copy and move operations to maintain singleton behavior per key
    Multiton(const Multiton&) = delete;
    Multiton& operator=(const Multiton&) = delete;
    Multiton(Multiton&&) = delete;
    Multiton& operator=(Multiton&&) = delete;

    // destructor
    ~Multiton() = default;

    // set a custom factory function for instance creation
    static void set_factory(FactoryFunction factory) noexcept {
        std::lock_guard<std::mutex> lock(m_registry_mutex);
        m_factory_function = std::move(factory);
        LOG_INFO_PRINT("Custom factory function has been set");
    }

    // clear the custom factory function
    static void clear_factory() noexcept {
        std::lock_guard<std::mutex> lock(m_registry_mutex);
        m_factory_function = {};
        LOG_INFO_PRINT("Custom factory function has been cleared");
    }

    // get instance by key, creating it if it doesn't exist
    static std::expected<std::shared_ptr<InstanceType>, MultitonError> get_instance(const std::string& key) {
        // validate key first
        if (auto validation_result = validate_key(key); !validation_result) {
            return std::unexpected(validation_result.error());
        }

        std::lock_guard<std::mutex> lock(m_registry_mutex);
        
        // check if instance already exists
        if (auto it = m_instance_registry.find(key); it != m_instance_registry.end()) {
            LOG_INFO_PRINT("Returning existing instance for key: {}", key);
            return it->second;
        }

        // create new instance
        auto creation_result = create_instance(key);
        if (!creation_result) {
            return std::unexpected(creation_result.error());
        }

        // store in registry
        m_instance_registry[key] = creation_result.value();
        LOG_INFO_PRINT("Instance stored in registry for key: {}", key);
        
        return creation_result.value();
    }

    // check if an instance exists for the given key
    static bool has_instance(const std::string& key) noexcept {
        if (const auto validation_result = validate_key(key); !validation_result) {
            return false;
        }

        std::lock_guard<std::mutex> lock(m_registry_mutex);
        return m_instance_registry.contains(key);
    }

    // remove instance by key
    static std::expected<void, MultitonError> remove_instance(const std::string& key) {
        // validate key first
        if (auto validation_result = validate_key(key); !validation_result) {
            return std::unexpected(validation_result.error());
        }

        std::lock_guard<std::mutex> lock(m_registry_mutex);
        
        if (auto it = m_instance_registry.find(key); it != m_instance_registry.end()) {
            LOG_INFO_PRINT("Removing instance for key: {}", key);
            m_instance_registry.erase(it);
            return {};
        }
        
        LOG_WARNING_PRINT("Attempted to remove non-existent instance for key: {}", key);
        return std::unexpected(MultitonError::INSTANCE_NOT_FOUND);
    }

    // clear all instances
    static void clear_all_instances() noexcept {
        std::lock_guard<std::mutex> lock(m_registry_mutex);
        const std::size_t count = m_instance_registry.size();
        m_instance_registry.clear();
        LOG_INFO_PRINT("Cleared {} instances from registry", count);
    }

    // get count of managed instances
    static std::size_t get_instance_count() noexcept {
        std::lock_guard<std::mutex> lock(m_registry_mutex);
        return m_instance_registry.size();
    }

    // get all keys currently in the registry
    static std::vector<std::string> get_all_keys() {
        std::lock_guard<std::mutex> lock(m_registry_mutex);
        std::vector<std::string> keys{};
        keys.reserve(m_instance_registry.size());
        
        for (const auto& [key, instance] : m_instance_registry) {
            keys.emplace_back(key);
        }
        
        return keys;
    }

    // thread-safe iteration over all instances
    template<typename CallableType>
    requires std::invocable<CallableType, const std::string&, std::shared_ptr<InstanceType>>
    static void for_each_instance(CallableType&& callable) {
        std::lock_guard<std::mutex> lock(m_registry_mutex);
        
        for (const auto& [key, instance] : m_instance_registry) {
            callable(key, instance);
        }
    }

    // get registry size safely
    static std::size_t size() noexcept {
        return get_instance_count();
    }

    // check if registry is empty
    static bool empty() noexcept {
        return get_instance_count() == 0;
    }
};

// template deduction guide for modern c++ usage
template<typename InstanceType>
Multiton(InstanceType) -> Multiton<InstanceType>;

} // namespace multiton_pattern

#endif // MULTITON_HPP