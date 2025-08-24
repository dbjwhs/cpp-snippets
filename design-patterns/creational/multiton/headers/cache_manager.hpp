// MIT License
// Copyright (c) 2025 dbjwhs

#ifndef CACHE_MANAGER_HPP
#define CACHE_MANAGER_HPP

#include <unordered_map>
#include <string>
#include <chrono>
#include <mutex>
#include <memory>
#include <concepts>
#include <optional>
#include <utility>
#include "../../../../headers/project_utils.hpp"

namespace multiton_pattern {

// cache entry template with modern c++23 template parameter naming
template<std::copyable ValueType>
struct CacheEntry {
    ValueType m_value{};
    std::chrono::steady_clock::time_point m_creation_time{};
    std::chrono::milliseconds m_ttl{};
    std::size_t m_access_count{0};

    CacheEntry() = default;

    explicit CacheEntry(ValueType value, const std::chrono::milliseconds ttl = std::chrono::milliseconds{300000})
        : m_value{std::move(value)}
        , m_creation_time{std::chrono::steady_clock::now()}
        , m_ttl{ttl}
        , m_access_count{1} {}

    // check if entry has expired
    [[nodiscard]] bool is_expired() const noexcept {
        const auto now = std::chrono::steady_clock::now();
        return (now - m_creation_time) > m_ttl;
    }

    // update access count and return value
    const ValueType& access() noexcept {
        ++m_access_count;
        return m_value;
    }

    // get age in milliseconds
    [[nodiscard]] std::chrono::milliseconds get_age() const noexcept {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - m_creation_time
        );
    }
};

// cache manager class template demonstrating multiton pattern usage
template<std::copyable ValueType>
class CacheManager {
private:
    using CacheStorage = std::unordered_map<std::string, CacheEntry<ValueType>>;
    
    CacheStorage m_cache_storage{};
    mutable std::mutex m_cache_mutex{};
    std::string m_cache_name{};
    std::string m_instance_guid{};
    std::chrono::milliseconds m_default_ttl{};
    std::size_t m_max_size{};
    std::atomic<std::size_t> m_hit_count{0};
    std::atomic<std::size_t> m_miss_count{0};

    // evict expired entries
    void evict_expired_entries() {
        std::size_t evicted_count{0};
        for (auto it = m_cache_storage.begin(); it != m_cache_storage.end();) {
            if (it->second.is_expired()) {
                it = m_cache_storage.erase(it);
                ++evicted_count;
            } else {
                ++it;
            }
        }
        
        if (evicted_count > 0) {
            LOG_INFO_PRINT("Evicted {} expired entries from cache: {} [{}]", evicted_count, m_cache_name, m_instance_guid);
        }
    }

    // evict least recently used entries if cache is full
    void evict_lru_if_needed() {
        if (m_cache_storage.size() >= m_max_size) {
            // find entry with the oldest creation time
            auto oldest_it = std::min_element(m_cache_storage.begin(), m_cache_storage.end(),
                [](const auto& a, const auto& b) {
                    return a.second.m_creation_time < b.second.m_creation_time;
                });
            
            if (oldest_it != m_cache_storage.end()) {
                LOG_INFO_PRINT("Evicting LRU entry from cache: {} (key: {}) [{}]", m_cache_name, oldest_it->first, m_instance_guid);
                m_cache_storage.erase(oldest_it);
            }
        }
    }

public:
    // constructor with configuration
    explicit CacheManager(std::string  cache_name = "default",
                         const std::chrono::milliseconds default_ttl = std::chrono::milliseconds{300000},
                         const std::size_t max_size = 1000)
        : m_cache_name{std::move(cache_name)}
        , m_instance_guid{utils::generate_guid()}
        , m_default_ttl{default_ttl}
        , m_max_size{max_size} {
        LOG_INFO_PRINT("CacheManager created: {} (TTL: {}ms, Max Size: {}) [{}]", 
                      m_cache_name, m_default_ttl.count(), m_max_size, m_instance_guid);
    }

    // destructor
    ~CacheManager() {
        clear();
        LOG_INFO_PRINT("CacheManager destroyed: {} [{}]", m_cache_name, m_instance_guid);
    }

    // delete copy operations
    CacheManager(const CacheManager&) = delete;
    CacheManager& operator=(const CacheManager&) = delete;

    // allow move operations
    CacheManager(CacheManager&& other) noexcept
        : m_cache_storage{std::move(other.m_cache_storage)}
        , m_cache_name{std::move(other.m_cache_name)}
        , m_instance_guid{std::move(other.m_instance_guid)}
        , m_default_ttl{other.m_default_ttl}
        , m_max_size{other.m_max_size}
        , m_hit_count{other.m_hit_count.load()}
        , m_miss_count{other.m_miss_count.load()} {
        other.m_hit_count = 0;
        other.m_miss_count = 0;
        LOG_INFO_PRINT("CacheManager moved: {} [{}]", m_cache_name, m_instance_guid);
    }

    CacheManager& operator=(CacheManager&& other) noexcept {
        if (this != &other) {
            std::lock_guard<std::mutex> lock(m_cache_mutex);
            std::lock_guard<std::mutex> other_lock(other.m_cache_mutex);
            
            m_cache_storage = std::move(other.m_cache_storage);
            m_cache_name = std::move(other.m_cache_name);
            m_instance_guid = std::move(other.m_instance_guid);
            m_default_ttl = other.m_default_ttl;
            m_max_size = other.m_max_size;
            m_hit_count = other.m_hit_count.load();
            m_miss_count = other.m_miss_count.load();
            
            other.m_hit_count = 0;
            other.m_miss_count = 0;
            LOG_INFO_PRINT("CacheManager move-assigned: {} [{}]", m_cache_name, m_instance_guid);
        }
        return *this;
    }

    // store value in cache
    void put(const std::string& key, ValueType value,
             const std::optional<std::chrono::milliseconds> custom_ttl = std::nullopt) {
        std::lock_guard<std::mutex> lock(m_cache_mutex);
        
        evict_expired_entries();
        evict_lru_if_needed();
        
        const auto ttl = custom_ttl.value_or(m_default_ttl);
        m_cache_storage[key] = CacheEntry<ValueType>{std::move(value), ttl};
        
        LOG_INFO_PRINT("Stored value in cache: {} (key: {}, TTL: {}ms) [{}]", 
                      m_cache_name, key, ttl.count(), m_instance_guid);
    }

    // retrieve value from cache
    std::optional<ValueType> get(const std::string& key) {
        std::lock_guard<std::mutex> lock(m_cache_mutex);
        
        evict_expired_entries();
        
        if (auto it = m_cache_storage.find(key); it != m_cache_storage.end()) {
            if (!it->second.is_expired()) {
                m_hit_count.fetch_add(1);
                LOG_INFO_PRINT("Cache hit for key: {} in cache: {} [{}]", key, m_cache_name, m_instance_guid);
                return it->second.access();
            } else {
                // remove expired entry
                m_cache_storage.erase(it);
            }
        }
        
        m_miss_count.fetch_add(1);
        LOG_INFO_PRINT("Cache miss for key: {} in cache: {} [{}]", key, m_cache_name, m_instance_guid);
        return std::nullopt;
    }

    // check if key exists and is not expired
    bool contains(const std::string& key) {
        std::lock_guard<std::mutex> lock(m_cache_mutex);
        
        evict_expired_entries();
        
        if (auto it = m_cache_storage.find(key); it != m_cache_storage.end()) {
            return !it->second.is_expired();
        }
        return false;
    }

    // remove specific key
    bool remove(const std::string& key) {
        std::lock_guard<std::mutex> lock(m_cache_mutex);
        
        if (auto it = m_cache_storage.find(key); it != m_cache_storage.end()) {
            m_cache_storage.erase(it);
            LOG_INFO_PRINT("Removed key: {} from cache: {} [{}]", key, m_cache_name, m_instance_guid);
            return true;
        }
        return false;
    }

    // clear all entries
    void clear() {
        std::lock_guard<std::mutex> lock(m_cache_mutex);
        const std::size_t count = m_cache_storage.size();
        m_cache_storage.clear();
        LOG_INFO_PRINT("Cleared {} entries from cache: {} [{}]", count, m_cache_name, m_instance_guid);
    }

    // get cache statistics
    struct CacheStats {
        std::size_t m_size{};
        std::size_t m_hit_count{};
        std::size_t m_miss_count{};
        double m_hit_ratio{};
        std::string m_cache_name{};
    };

    CacheStats get_stats() const {
        std::lock_guard<std::mutex> lock(m_cache_mutex);
        
        const std::size_t hits = m_hit_count.load();
        const std::size_t misses = m_miss_count.load();
        const std::size_t total_requests = hits + misses;
        const double hit_ratio = total_requests > 0 ? static_cast<double>(hits) / total_requests : 0.0;
        
        return CacheStats{
            .m_size = m_cache_storage.size(),
            .m_hit_count = hits,
            .m_miss_count = misses,
            .m_hit_ratio = hit_ratio,
            .m_cache_name = m_cache_name
        };
    }

    // get cache name
    const std::string& get_cache_name() const noexcept {
        return m_cache_name;
    }

    // get current size
    std::size_t size() const {
        std::lock_guard<std::mutex> lock(m_cache_mutex);
        return m_cache_storage.size();
    }

    // check if cache is empty
    bool empty() const {
        std::lock_guard<std::mutex> lock(m_cache_mutex);
        return m_cache_storage.empty();
    }

    // manual cleanup of expired entries
    std::size_t cleanup_expired() {
        std::lock_guard<std::mutex> lock(m_cache_mutex);
        const std::size_t original_size = m_cache_storage.size();
        evict_expired_entries();
        return original_size - m_cache_storage.size();
    }

    // get the instance guid
    const std::string& get_instance_guid() const noexcept {
        return m_instance_guid;
    }
};

} // namespace multiton_pattern

#endif // CACHE_MANAGER_HPP
