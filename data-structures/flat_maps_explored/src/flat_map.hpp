// MIT License
// Copyright (c) 2025 dbjwhs

#pragma once

#include <vector>
#include <algorithm>
#include <expected>
#include <string>
#include <chrono>
#include <format>
#include <concepts>
#include <type_traits>
#include "../../../headers/project_utils.hpp"

//
// flat container pattern implementation for c++23
//
// this pattern represents a fundamental shift in how we approach associative containers in modern c++.
// traditionally, std::map and std::set use red-black trees (balanced binary search trees) which store
// elements in separate nodes scattered throughout memory. while this provides excellent o(log n)
// performance guarantees for all operations, it suffers from poor cache locality due to pointer
// chasing and memory fragmentation.
//
// the flat container pattern emerged from performance-critical domains like gaming, high-frequency
// trading, and embedded systems where cache performance often matters more than theoretical complexity.
// instead of tree structures, flat containers use sorted arrays (typically std::vector) to store
// elements in contiguous memory. this trades o(n) insertion performance for dramatically improved
// lookup and iteration performance due to superior cache locality.
//
// key innovations of this pattern:
// 1. contiguous memory layout - all elements stored sequentially for optimal cache usage
// 2. binary search algorithms - maintaining o(log n) lookup complexity with better constants
// 3. batch operations - efficient construction from sorted ranges
// 4. customizable underlying storage - ability to use different container types
// 5. memory efficiency - no pointer overhead or node allocations
//
// the pattern gained standardization momentum through boost.container's flat_map implementation
// and performance studies showing 2-5x improvements in real-world scenarios. c++23 formalizes
// this approach, recognizing that modern hardware characteristics (deep cache hierarchies,
// predictive prefetching) favor sequential memory access patterns over tree traversal.
//
// common usage patterns:
// - configuration lookups in web servers and applications
// - entity-component mappings in game engines
// - symbol tables in compilers and interpreters
// - price lookup tables in financial systems
// - any read-heavy associative container with infrequent modifications
//

namespace performance_containers {

//
// error types for a railway-oriented programming pattern
//
enum class FlatMapError {
    KeyNotFound,
    InvalidOperation,
    OutOfMemory,
    InvalidIterator
};

//
// error message mapping for debugging and logging
//
[[nodiscard]] constexpr std::string_view error_message(const FlatMapError error) noexcept {
    switch (error) {
        case FlatMapError::KeyNotFound:
            return "key not found in flat map";
        case FlatMapError::InvalidOperation:
            return "invalid operation attempted on flat map";
        case FlatMapError::OutOfMemory:
            return "insufficient memory for flat map operation";
        case FlatMapError::InvalidIterator:
            return "invalid iterator used with flat map";
        default:
            return "unknown flat map error";
    }
}

//
// concept to ensure key types are orderable for binary search operations
//
template<typename T>
concept Orderable = requires(const T& a, const T& b) {
    { a < b } -> std::convertible_to<bool>;
    { a == b } -> std::convertible_to<bool>;
};

//
// high-performance flat map implementation using sorted vector storage
//
// provides std::map-like interface with superior cache performance for lookup-heavy workloads
// trades o(n) insertion complexity for improved memory locality and reduced allocation overhead
//
template<Orderable Key, typename Value>
class FlatMap {
public:
    //
    // standard container type definitions for stl compatibility
    //
    using key_type = Key;
    using mapped_type = Value;
    using value_type = std::pair<Key, Value>;
    using size_type = std::size_t;
    using iterator = typename std::vector<value_type>::iterator;
    using const_iterator = typename std::vector<value_type>::const_iterator;

    //
    // result types for railway-oriented programming
    //
    using InsertResult = std::expected<iterator, FlatMapError>;
    using FindResult = std::expected<const_iterator, FlatMapError>;
    using AccessResult = std::expected<Value, FlatMapError>;
    using ConstAccessResult = std::expected<Value, FlatMapError>;

private:
    //
    // core storage using vector for contiguous memory layout
    // maintained in sorted order by key for efficient binary search operations
    //
    std::vector<value_type> m_data{};

    //
    // internal binary search implementation for key lookup
    // returns iterator to an element with matching key or end() if not found
    //
    [[nodiscard]] const_iterator find_internal(const Key& key) const noexcept {
        // create a temporary pair for binary search comparison
        // default-constructed value is acceptable since we only compare keys
        const auto target = std::make_pair(key, Value{});

        // std::lower_bound finds the first element not less than target
        const auto pos = std::lower_bound(
            m_data.cbegin(),
            m_data.cend(),
            target,
            [](const value_type& a, const value_type& b) -> bool {
                return a.first < b.first;
            }
        );

        // verify we found exact key match (lower_bound finds first not-less-than)
        if (pos != m_data.cend() && pos->first == key) {
            return pos;
        }

        return m_data.cend();
    }

    //
    // find optimal insertion position maintaining sorted order
    // returns iterator to insertion point for a new key-value pair
    //
    [[nodiscard]] iterator find_insertion_point(const Key& key) noexcept {
        const auto target = std::make_pair(key, Value{});

        return std::lower_bound(
            m_data.begin(),
            m_data.end(),
            target,
            [](const value_type& a, const value_type& b) -> bool {
                return a.first < b.first;
            }
        );
    }

public:
    //
    // default constructor creates an empty flat map with no initial capacity
    //
    FlatMap() = default;

    //
    // constructor with a capacity hint for performance optimization
    // reserves memory upfront to avoid reallocations during bulk insertions
    //
    explicit FlatMap(size_type initial_capacity) {
        LOG_INFO_PRINT("creating flat map with initial capacity: {}", initial_capacity);
        m_data.reserve(initial_capacity);
    }

    //
    // construct from sorted range for optimal performance
    // assumes input range is already sorted by key - no sorting overhead
    //
    template<typename InputIterator>
    FlatMap(InputIterator first, InputIterator last, bool assume_sorted = false) {
        const auto distance = std::distance(first, last);
        LOG_INFO_PRINT("constructing flat map from range of {} elements", distance);

        // reserve exact capacity to avoid reallocations
        m_data.reserve(static_cast<size_type>(distance));

        // copy elements into internal storage
        for (auto iter = first; iter != last; ++iter) {
            m_data.emplace_back(*iter);
        }

        // sort if not already sorted for correct binary search behavior
        if (!assume_sorted) {
            LOG_INFO_PRINT("sorting {} elements for flat map construction", m_data.size());
            std::sort(m_data.begin(), m_data.end(),
                [](const value_type& a, const value_type& b) -> bool {
                    return a.first < b.first;
                });
        }
    }

    //
    // insert a key-value pair maintaining sorted order
    // returns success with iterator or failure with error code
    //
    [[nodiscard]] InsertResult insert(const Key& key, const Value& value) {
        try {
            // find the correct insertion position using binary search
            auto pos = find_insertion_point(key);

            // check if key already exists (update scenario)
            if (pos != m_data.end() && pos->first == key) {
                LOG_INFO_PRINT("updating existing key: {}", key);
                pos->second = value;
                return pos;
            }

            // insert a new element, shifting later elements as needed
            // this is the expensive o(n) operation in flat containers
            LOG_INFO_PRINT("inserting new key-value pair at position: {}",
                          std::distance(m_data.begin(), pos));

            auto result = m_data.insert(pos, std::make_pair(key, value));
            return result;

        } catch (const std::bad_alloc&) {
            LOG_ERROR_PRINT("memory allocation failed during flat map insertion");
            return std::unexpected(FlatMapError::OutOfMemory);
        } catch (...) {
            LOG_ERROR_PRINT("unknown error during flat map insertion");
            return std::unexpected(FlatMapError::InvalidOperation);
        }
    }

    //
    // insert by moving value for performance optimization
    // avoids unnecessary copying of expensive-to-copy value types
    //
    [[nodiscard]] InsertResult insert(const Key& key, Value&& value) {
        try {
            auto pos = find_insertion_point(key);

            if (pos != m_data.end() && pos->first == key) {
                LOG_INFO_PRINT("updating existing key with moved value: {}", key);
                pos->second = std::move(value);
                return pos;
            }

            LOG_INFO_PRINT("inserting new key with moved value");
            auto result = m_data.insert(pos, std::make_pair(key, std::move(value)));
            return result;

        } catch (const std::bad_alloc&) {
            return std::unexpected(FlatMapError::OutOfMemory);
        } catch (...) {
            return std::unexpected(FlatMapError::InvalidOperation);
        }
    }

    //
    // emplace construction for optimal performance
    // constructs value in-place avoiding temporary object creation
    //
    template<typename... Args>
    [[nodiscard]] InsertResult emplace(const Key& key, Args&&... args) {
        try {
            auto pos = find_insertion_point(key);

            if (pos != m_data.end() && pos->first == key) {
                LOG_INFO_PRINT("emplacing into existing key: {}", key);
                pos->second = Value{std::forward<Args>(args)...};
                return pos;
            }

            LOG_INFO_PRINT("emplacing new key-value pair");
            auto result = m_data.emplace(pos, key, Value{std::forward<Args>(args)...});
            return result;

        } catch (const std::bad_alloc&) {
            return std::unexpected(FlatMapError::OutOfMemory);
        } catch (...) {
            return std::unexpected(FlatMapError::InvalidOperation);
        }
    }

    //
    // find an element by key using binary search
    // returns iterator to found element or error if not found
    //
    [[nodiscard]] FindResult find(const Key& key) const noexcept {
        const auto result = find_internal(key);

        if (result != m_data.cend()) {
            LOG_INFO_PRINT("key found in flat map: {}", key);
            return result;
        }

        LOG_WARNING_PRINT("key not found in flat map: {}", key);
        return std::unexpected(FlatMapError::KeyNotFound);
    }

    //
    // access value by key with bounds checking
    // returns copy of value or error if key not found
    //
    [[nodiscard]] AccessResult at(const Key& key) {
        auto iter = find_internal(key);

        if (iter != m_data.cend()) {
            return iter->second;
        }

        return std::unexpected(FlatMapError::KeyNotFound);
    }

    //
    // const version of at() for read-only access
    //
    [[nodiscard]] ConstAccessResult at(const Key& key) const {
        const auto iter = find_internal(key);

        if (iter != m_data.cend()) {
            return iter->second;
        }

        return std::unexpected(FlatMapError::KeyNotFound);
    }

    //
    // check if key exists in a map
    //
    [[nodiscard]] bool contains(const Key& key) const noexcept {
        return find_internal(key) != m_data.cend();
    }

    //
    // remove an element by key
    // returns true if an element was removed, false if not found
    //
    bool erase(const Key& key) {
        auto iter = find_internal(key);

        if (iter != m_data.cend()) {
            LOG_INFO_PRINT("erasing key from flat map: {}", key);
            m_data.erase(iter);
            return true;
        }

        LOG_WARNING_PRINT("attempted to erase non-existent key: {}", key);
        return false;
    }

    //
    // reserve capacity for performance optimization
    // prevents reallocations during bulk operations
    //
    void reserve(size_type capacity) {
        LOG_INFO_PRINT("reserving capacity for flat map: {}", capacity);
        m_data.reserve(capacity);
    }

    //
    // container size and state queries
    //
    [[nodiscard]] size_type size() const noexcept {
        return m_data.size();
    }

    [[nodiscard]] bool empty() const noexcept {
        return m_data.empty();
    }

    [[nodiscard]] size_type capacity() const noexcept {
        return m_data.capacity();
    }

    //
    // clear all elements
    //
    void clear() noexcept {
        LOG_INFO_PRINT("clearing flat map with {} elements", m_data.size());
        m_data.clear();
    }

    //
    // iterator access for stl compatibility
    //
    [[nodiscard]] iterator begin() noexcept {
        return m_data.begin();
    }

    [[nodiscard]] iterator end() noexcept {
        return m_data.end();
    }

    [[nodiscard]] const_iterator begin() const noexcept {
        return m_data.cbegin();
    }

    [[nodiscard]] const_iterator end() const noexcept {
        return m_data.cend();
    }

    [[nodiscard]] const_iterator cbegin() const noexcept {
        return m_data.cbegin();
    }

    [[nodiscard]] const_iterator cend() const noexcept {
        return m_data.cend();
    }

    //
    // debug utility to display memory layout and performance characteristics
    //
    void debug_info() const {
        LOG_INFO_PRINT("=== flat map debug information ===");
        LOG_INFO_PRINT("size: {}", size());
        LOG_INFO_PRINT("capacity: {}", capacity());
        LOG_INFO_PRINT("memory usage: {} bytes", capacity() * sizeof(value_type));

        if (!empty()) {
            LOG_INFO_PRINT("first element address: {:p}", static_cast<const void*>(&m_data[0]));

            if (size() > 1) {
                const void* first = &m_data[0];
                const void* second = &m_data[1];
                const auto element_spacing = static_cast<const char*>(second) - static_cast<const char*>(first);
                LOG_INFO_PRINT("element spacing: {} bytes", element_spacing);
            }

            // show the first few elements for debugging
            const auto display_count = std::min(size_type{5}, size());
            for (size_type ndx = 0; ndx < display_count; ++ndx) {
                LOG_INFO_PRINT("element[{}]: key={}, value={}", ndx, m_data[ndx].first, m_data[ndx].second);
            }

            if (size() > 5) {
                LOG_INFO_PRINT("... and {} more elements", size() - 5);
            }
        }

        LOG_INFO_PRINT("=== end debug information ===");
    }
};

} // namespace performance_containers
