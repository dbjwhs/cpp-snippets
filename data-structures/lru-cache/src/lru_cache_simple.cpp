// MIT License
// Copyright (c) 2025 dbjwhs

#include <iostream>
#include <list>
#include <unordered_map>
#include <cassert>

/*
Challenge:
Design and implement a data structure for Least Recently Used(LRU) cache. (As stated above, feel free to make use of
whatever language classes your language provides. If Python had a native LRUCache class, you could just use that
(it does not!)

Problem Note:

The LRU Cache is initialized with a positive capacity
Your data structure must support two operations: get() and put()
get(key) : Finds and returns the value if the key exists in the cache. If the key is not present in the cache, get(key)
returns -1

put(key, value) : Inserts new key, if it is not present in the cache. If the cache is filled to capacity, it must remove
the least recently used entry.

[NOTE] Try implementing both operations in O(1) time complexity

Input in this problem would be a series of function calls to get() and put()
Example

cache = LRUCache(3)
cache.put(1,1)
cache.put(2,2)
cache.put(1,3)
cache.get(1)    ---> returns 3
cache.put(3,4)
cache.put(4,3)  // removes key 2
cache.get(2)    ---> returns -1

Input Format:

First-line contains N and C , the total number of queries and the cache size.
Each of the following N lines has a query of either type 1(put) or type 2(get).
The query of type 1 is of format: 1 k v, where k is key and v is value
The query of type 2 is of format: 2 k, where k is key whose value is to be fetched.
For example, the input for the above example will be:

7 3
1 1 1
1 2 2
1 1 3
2 1
1 3 4
1 4 3
2 2
*/

inline constexpr bool enable_debug = false;
inline constexpr int value_not_found = -1;

class LRUCache {
private:
    struct CacheEntry {
        int m_key;
        int m_value;
        explicit CacheEntry(const int k, const int v) : m_key(k), m_value(v) {}
    };

    size_t m_capacity;
    std::list<CacheEntry> m_cache_entry_list;
    std::unordered_map<int, std::list<CacheEntry>::iterator> m_cache_iter_map;

    void move_to_front(const std::list<CacheEntry>::iterator iter) {
        if (enable_debug) {
            std::cout << "DEBUG: Moving key " << iter->m_key << " (value: " << iter->m_value << ") to front" << std::endl;
        }

        // JG this is why I wanted to use a std::list, b/c it has splice which transfers elements by repointing nodes
        // (no copying/moving) -> 0(1)
        m_cache_entry_list.splice(m_cache_entry_list.begin(), m_cache_entry_list, iter);
    }

    void print_cache_state() const {
        if (enable_debug) {
            std::cout << "DEBUG: Cache state [";
            for (const auto& entry : m_cache_entry_list) {
                std::cout << " (" << entry.m_key << ":" << entry.m_value << ")";
            }
            std::cout << " ]" << std::endl;
        }
    }

public:
    explicit LRUCache(const int size) : m_capacity(size) {
        if (size <= 0) {
            // ### what to do here? I guess throw?
            throw std::invalid_argument("Cache capacity must greater than or equal to zero");
        }
        if (enable_debug) {
            std::cout << "DEBUG: Created LRU cache with capacity " << m_capacity << std::endl;
        }
    }

    int get(const int key) {
        if (enable_debug) {
            std::cout << "DEBUG: GET operation - key: " << key << std::endl;
        }

        // check if we found our cache interator
        const auto cache_iter = m_cache_iter_map.find(key);
        if (cache_iter == m_cache_iter_map.end()) {
            if (enable_debug) {
                std::cout << "DEBUG: Key " << key << " not found in cache" << std::endl;
                print_cache_state();
            }
            return value_not_found;
        }

        // found, move to the front and return
        move_to_front(cache_iter->second);
        if (enable_debug) {
            std::cout << "DEBUG: Found value " << cache_iter->second->m_value << " for key " << key << std::endl;
            print_cache_state();
        }
        return cache_iter->second->m_value;
    }

    void put(int key, int value) {
        if (enable_debug) {
            std::cout << "DEBUG: PUT operation - key: " << key << ", value: " << value << std::endl;
        }

        // first, check if we are updating an existing key
        auto const cache_iter = m_cache_iter_map.find(key);
        if (cache_iter != m_cache_iter_map.end()) {
            if (enable_debug) {
                std::cout << "DEBUG: Updating existing key " << key << " with new value " << value << std::endl;
            }
            cache_iter->second->m_value = value;
            move_to_front(cache_iter->second);
            print_cache_state();
            return;
        }

        // second, check our capacity if reached erase last key from list and our iter map
        if (size() == m_capacity) {
            const int last_key = m_cache_entry_list.back().m_key;
            if (enable_debug) {
                std::cout << "DEBUG: Cache full, removing LRU item with key " << last_key << std::endl;
            }
            m_cache_iter_map.erase(last_key);
            m_cache_entry_list.pop_back();
        }

        // add value to our list, and it's interator to our map, note: std::list iterators remain valid even when elements
        // are added/removed elsewhere in the list or are moved via splice
        m_cache_entry_list.emplace_front(key, value);
        m_cache_iter_map[key] = m_cache_entry_list.begin();
        if (enable_debug) {
            std::cout << "DEBUG: Added new entry - key: " << key << ", value: " << value << std::endl;
            print_cache_state();
        }
    }

    [[nodiscard]] size_t size() const { return m_cache_iter_map.size(); }
    [[nodiscard]] bool empty() const { return m_cache_iter_map.empty(); }
};

void run_tests() {
    if (enable_debug) {
        std::cout << "DEBUG: starting tests" << std::endl;
    }

    // test object construction valid
    try {
        LRUCache cache(-1);
    } catch (const std::invalid_argument& e) {
        if (enable_debug) {
            std::cout << "\nDEBUG: CTOR test passed: " << e.what() << std::endl;
        }
    }


    // basic functionality (test defined in the above description
    {
        if (enable_debug) {
            std::cout << "\nDEBUG: test 1: basic functionality" << std::endl;
        }

        /*
        cache = LRUCache(3)
        cache.put(1,1)
        cache.put(2,2)
        cache.put(1,3)
        cache.get(1)    ---> returns 3
        cache.put(3,4)
        cache.put(4,3)  // removes key 2
        cache.get(2)    ---> returns -1
        */

        LRUCache cache(3);
        assert(cache.empty());

        // cache.put(1,1)
        cache.put(1, 1);
        assert(cache.size() == 1);
        assert(cache.get(1) == 1);

        // cache.put(2,2)
        // cache.put(1,3)
        cache.put(2, 2);
        cache.put(1, 3);
        assert(cache.size() == 2);

        // cache.get(1)    ---> returns 3
        assert(cache.get(1) == 3);

        // cache.put(3,4)
        // cache.put(4,3)  // removes key 2
        // cache.get(2)    ---> returns -1
        cache.put(3, 4);
        cache.put(4, 3);
        assert(cache.get(2) == -1);
    }

    // capacity eviction
    {
        if (enable_debug) {
            std::cout << "\nDEBUG: test 2: capacity and eviction" << std::endl;
        }

        LRUCache cache(3);
        cache.put(1, 1);
        cache.put(2, 2);
        cache.put(3, 3);

        // should evict key 1
        cache.put(4, 4);
        assert(cache.get(1) == -1);

        // verify all cache entries are valid
        assert(cache.get(2) == 2);
        assert(cache.get(3) == 3);
        assert(cache.get(4) == 4);
    }

    // check ordering
    {
        if (enable_debug) {
            std::cout << "\nDEBUG: test 3: verify ordering" << std::endl;
        }

        LRUCache cache(3);
        cache.put(1, 1);
        cache.put(2, 2);
        cache.put(3, 3);

        // let's make 1 most recently used
        assert(cache.get(1) == 1);

        // should evict 2, not 1
        cache.put(4, 4);

        assert(cache.get(1) != -1);
        assert(cache.get(2) == -1);
        assert(cache.get(3) == 3);
        assert(cache.get(4) == 4);
    }

    std::cout << "DEBUG: All tests passed!" << std::endl;
}

int main() {
    run_tests();
    return 0;
}
