// MIT License
// Copyright (c) 2025 dbjwhs

#ifndef RESOURCE_MANAGER_HPP
#define RESOURCE_MANAGER_HPP

// The Rule of Five: A Comprehensive Guide to Modern C++ Resource Management
//
// Historical Background:
// The Rule of Five evolved from the original "Rule of Three" established in the early days of C++ by Bjarne Stroustrup.
// The Rule of Three (circa 1991) stated that if a class needs a custom destructor, copy constructor, or copy assignment
// operator, it likely needs all three. This rule emerged because C++ automatically generates these special member
// functions, but the defaults often don't handle dynamic resource management correctly.
//
// With the introduction of move semantics in C++11 (2011), the Rule of Three became insufficient. Move operations
// (move constructor and move assignment operator) were added to enable efficient transfer of resources without
// expensive deep copying. This evolution led to the "Rule of Five" around 2012-2013.
//
// The Rule of Five states: If a class needs any of the following special member functions, it likely needs all five:
// 1. Destructor (~Class)
// 2. Copy constructor (Class(const Class&))
// 3. Copy assignment operator (Class& operator=(const Class&))
// 4. Move constructor (Class(Class&&))
// 5. Move assignment operator (Class& operator=(Class&&))
//
// Modern Evolution (C++11 onwards):
// The rule has been further refined with the "Rule of Zero" concept, which suggests that well-designed classes
// should not need custom implementations of these functions at all, relying instead on smart pointers and
// RAII wrappers. However, when direct resource management is required (raw pointers, file handles, network
// connections, etc.), the Rule of Five remains essential.
//
// The Rule of Five ensures:
// - Proper resource cleanup (destructor)
// - Safe copying behavior (copy constructor/assignment)
// - Efficient resource transfer (move constructor/assignment)
// - Exception safety and strong exception guarantee
// - Prevention of resource leaks, double-deletion, and dangling pointers

#include <memory>
#include <utility>
#include <expected>
#include <string>
#include <cstddef>
#include <cassert>
#include "project_utils.hpp"

namespace rule_of_five {

// error types for railway-oriented programming pattern
enum class ResourceError {
    AllocationFailure,
    InvalidSize,
    NullPointer,
    CopyFailure,
    MoveFailure
};

// convert error enum to descriptive string
constexpr std::string_view error_to_string(ResourceError error) noexcept {
    switch (error) {
        case ResourceError::AllocationFailure:
            return "Memory allocation failed";
        case ResourceError::InvalidSize:
            return "Invalid size parameter";
        case ResourceError::NullPointer:
            return "Null pointer encountered";
        case ResourceError::CopyFailure:
            return "Copy operation failed";
        case ResourceError::MoveFailure:
            return "Move operation failed";
        default:
            return "Unknown error";
    }
}

// comprehensive demonstration class implementing the rule of five
// manages a dynamically allocated array of integers
template<std::copyable ElementType = int>
requires std::is_default_constructible_v<ElementType>
class ResourceManager {
private:
    // raw pointer to dynamically allocated memory
    ElementType* m_data{nullptr};
    
    // size of the allocated array
    std::size_t m_size{0};
    
    // capacity of the allocated array (may be larger than size for growth optimization)
    std::size_t m_capacity{0};
    
    // unique identifier for debugging and logging purposes
    std::string m_id{};

    // helper method to safely deallocate memory and reset all state
    void deallocate_memory() noexcept {
        LOG_INFO_PRINT("deallocating memory for resource manager {}", m_id);
        delete[] m_data;
        m_data = nullptr;
        m_size = 0;
        m_capacity = 0;
    }

    // helper method to safely deallocate memory without resetting size/capacity
    void deallocate_memory_only() noexcept {
        LOG_INFO_PRINT("deallocating old memory for resource manager {}", m_id);
        delete[] m_data;
        m_data = nullptr;
    }

    // helper method to allocate memory with error handling
    [[nodiscard]] std::expected<ElementType*, ResourceError> allocate_memory(std::size_t capacity) noexcept {
        if (capacity == 0) {
            return std::unexpected{ResourceError::InvalidSize};
        }

        try {
            LOG_INFO_PRINT("allocating memory for {} elements in resource manager {}", capacity, m_id);
            return new ElementType[capacity]{};
        } catch (const std::bad_alloc&) {
            LOG_ERROR_PRINT("memory allocation failed for {} elements in resource manager {}", capacity, m_id);
            return std::unexpected{ResourceError::AllocationFailure};
        }
    }

    // helper method to copy data from another resource manager
    [[nodiscard]] std::expected<void, ResourceError> copy_data_from(const ResourceManager& other) noexcept {
        if (other.m_data == nullptr) {
            return std::unexpected{ResourceError::NullPointer};
        }

        try {
            LOG_INFO_PRINT("copying data from resource manager {} to {}", other.m_id, m_id);
            for (std::size_t ndx = 0; ndx < other.m_size; ++ndx) {
                m_data[ndx] = other.m_data[ndx];
            }
            return {};
        } catch (...) {
            LOG_ERROR_PRINT("copy operation failed from resource manager {} to {}", other.m_id, m_id);
            return std::unexpected{ResourceError::CopyFailure};
        }
    }

public:
    // default constructor - creates empty resource manager
    ResourceManager() noexcept
        : m_id{utils::generate_guid()} {
        LOG_INFO_PRINT("default constructor called for resource manager {}", m_id);
    }

    // parameterized constructor - creates resource manager with specified capacity (size starts at 0)
    explicit ResourceManager(std::size_t capacity) noexcept
        : m_size{0}  // size starts at 0, capacity is the parameter
        , m_capacity{capacity}
        , m_id{utils::generate_guid()} {
        LOG_INFO_PRINT("parameterized constructor called for resource manager {} with capacity {}", m_id, capacity);
        
        if (capacity > 0) {
            auto allocation_result = allocate_memory(capacity);
            if (!allocation_result) {
                // construction failed - set to default state
                m_size = 0;
                m_capacity = 0;
                LOG_ERROR_PRINT("constructor failed for resource manager {} due to allocation failure", m_id);
                return;
            }
            m_data = *allocation_result;
        }
    }

    // 1. DESTRUCTOR
    // responsible for cleaning up dynamically allocated resources
    // called automatically when object goes out of scope
    ~ResourceManager() noexcept {
        LOG_INFO_PRINT("destructor called for resource manager {}", m_id);
        deallocate_memory();
    }

    // 2. COPY CONSTRUCTOR
    // creates a new object as a deep copy of another object
    // essential for value semantics and preventing shallow copy issues
    ResourceManager(const ResourceManager& other) noexcept
        : m_size{other.m_size}
        , m_capacity{other.m_capacity}
        , m_id{utils::generate_guid()} {
        LOG_INFO_PRINT("copy constructor called: copying from {} to {}", other.m_id, m_id);

        if (other.m_data != nullptr && other.m_capacity > 0) {
            auto allocation_result = allocate_memory(other.m_capacity);
            if (!allocation_result) {
                // copy construction failed - set to default state
                m_size = 0;
                m_capacity = 0;
                LOG_ERROR_PRINT("copy constructor failed for resource manager {} due to allocation failure", m_id);
                return;
            }
            m_data = *allocation_result;

            auto copy_result = copy_data_from(other);
            if (!copy_result) {
                // copy failed - clean up and set to default state
                deallocate_memory();
                LOG_ERROR_PRINT("copy constructor failed for resource manager {} due to copy failure", m_id);
            }
        }
    }

    // 3. COPY ASSIGNMENT OPERATOR
    // assigns the contents of one existing object to another existing object
    // must handle self-assignment and provide strong exception safety
    ResourceManager& operator=(const ResourceManager& other) noexcept {
        LOG_INFO_PRINT("copy assignment operator called: assigning from {} to {}", other.m_id, m_id);

        // check for self-assignment
        if (this == &other) {
            LOG_INFO_PRINT("self-assignment detected in resource manager {}", m_id);
            return *this;
        }

        // create temporary copy (copy-and-swap idiom for exception safety)
        ResourceManager temp{other};
        
        // if copy construction succeeded, swap with temporary
        if (temp.m_data != nullptr || other.m_size == 0) {
            swap(temp);
            LOG_INFO_PRINT("copy assignment completed successfully for resource manager {}", m_id);
        } else {
            LOG_ERROR_PRINT("copy assignment failed for resource manager {}", m_id);
        }

        return *this;
    }

    // 4. MOVE CONSTRUCTOR
    // efficiently transfers resources from a temporary object
    // enables optimization by avoiding unnecessary deep copies
    ResourceManager(ResourceManager&& other) noexcept
        : m_data{std::exchange(other.m_data, nullptr)}
        , m_size{std::exchange(other.m_size, 0)}
        , m_capacity{std::exchange(other.m_capacity, 0)}
        , m_id{utils::generate_guid()} {
        LOG_INFO_PRINT("move constructor called: moving from {} to {}", other.m_id, m_id);
        LOG_INFO_PRINT("source object {} is now in valid but unspecified state", other.m_id);
    }

    // 5. MOVE ASSIGNMENT OPERATOR
    // efficiently transfers resources between existing objects
    // provides optimal performance for assignment from temporaries
    ResourceManager& operator=(ResourceManager&& other) noexcept {
        LOG_INFO_PRINT("move assignment operator called: moving from {} to {}", other.m_id, m_id);

        // check for self-assignment
        if (this == &other) {
            LOG_INFO_PRINT("self-move-assignment detected in resource manager {}", m_id);
            return *this;
        }

        // clean up current resources
        deallocate_memory();

        // transfer resources from other object
        m_data = std::exchange(other.m_data, nullptr);
        m_size = std::exchange(other.m_size, 0);
        m_capacity = std::exchange(other.m_capacity, 0);

        LOG_INFO_PRINT("move assignment completed successfully for resource manager {}", m_id);
        LOG_INFO_PRINT("source object {} is now in valid but unspecified state", other.m_id);

        return *this;
    }

    // utility method to swap contents with another resource manager
    void swap(ResourceManager& other) noexcept {
        LOG_INFO_PRINT("swapping contents between resource managers {} and {}", m_id, other.m_id);
        std::swap(m_data, other.m_data);
        std::swap(m_size, other.m_size);
        std::swap(m_capacity, other.m_capacity);
    }

    // accessor methods for testing and demonstration
    [[nodiscard]] std::size_t size() const noexcept {
        return m_size;
    }

    [[nodiscard]] std::size_t capacity() const noexcept {
        return m_capacity;
    }

    [[nodiscard]] const std::string& id() const noexcept {
        return m_id;
    }

    [[nodiscard]] bool empty() const noexcept {
        return m_size == 0;
    }

    // safe element access with bounds checking
    [[nodiscard]] std::expected<ElementType*, ResourceError> at(std::size_t index) noexcept {
        if (m_data == nullptr) {
            return std::unexpected{ResourceError::NullPointer};
        }
        if (index >= m_size) {
            return std::unexpected{ResourceError::InvalidSize};
        }
        return &m_data[index];
    }

    // const version of safe element access
    [[nodiscard]] std::expected<const ElementType*, ResourceError> at(std::size_t index) const noexcept {
        if (m_data == nullptr) {
            return std::unexpected{ResourceError::NullPointer};
        }
        if (index >= m_size) {
            return std::unexpected{ResourceError::InvalidSize};
        }
        return &m_data[index];
    }

    // method to add element to the resource manager
    [[nodiscard]] std::expected<void, ResourceError> push_back(const ElementType& value) noexcept {
        if (m_size >= m_capacity) {
            // need to grow capacity
            std::size_t new_capacity = (m_capacity == 0) ? 1 : m_capacity * 2;
            auto new_allocation = allocate_memory(new_capacity);
            if (!new_allocation) {
                return std::unexpected{ResourceError::AllocationFailure};
            }

            ElementType* new_data = *new_allocation;

            // copy existing data
            for (std::size_t ndx = 0; ndx < m_size; ++ndx) {
                new_data[ndx] = m_data[ndx];
            }

            // clean up old memory and update pointers
            deallocate_memory_only();
            m_data = new_data;
            m_capacity = new_capacity;
        }

        m_data[m_size] = value;
        ++m_size;
        LOG_INFO_PRINT("added element to resource manager {}, new size: {}", m_id, m_size);
        return {};
    }
};

} // namespace rule_of_five

#endif // RESOURCE_MANAGER_HPP