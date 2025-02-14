// MIT License
// Copyright (c) 2025 dbjwhs

#include <iostream>
#include <list>
#include <vector>
#include <cassert>
#include "../../../headers/project_utils.hpp"

// hash function interface class
// ----------------------------
// provides a standardized way to implement different hashing algorithms through
// the strategy pattern, enabling runtime selection of hash functions.
//
// key design aspects:
// - pure virtual interface enforcing hash function contract
// - templated to maintain type safety for different key types
// - includes naming capability for logging and debugging
//
// required implementations:
// 1. operator(): core hashing logic
//    - takes key and bucket count
//    - returns size_t hash value within bucket range
//    - const for thread safety
// 2. name(): returns string identifier of hash implementation
//    - useful for logging and performance comparison
//
// usage:
// - inherit from this base class to implement new hash functions
// - pass implementations to hash table constructor
// - allows switching hash functions without modifying table logic
//
// note I show three implementations:
// - std::hash wrapper (default c++ hash)
// - fnv-1a (good for strings and small keys)
// - murmur3 (excellent general-purpose hash)
template<typename KeyType>
class HashFunction {
public:
    virtual size_t operator()(const KeyType& key, size_t bucket_count) const = 0;
    [[nodiscard]] virtual std::string name() const = 0;
    virtual ~HashFunction() = default;
};

// standard hash implementation
template<typename KeyType>
class StdHash final : public HashFunction<KeyType> {
public:
    // I want to call out a subtle piece of the code below, the uniform initialization
    // with curly braces {}
    //
    // std::hash<KeyType>{} is uniform initialization syntax (introduced in c++11)
    // it creates a temporary instance of the std::hash class for KeyType
    //
    // can be written in older styles as:
    // pre-c++11:  return std::hash<KeyType>()(key) % bucket_count;
    // or:         std::hash<KeyType> hasher; return hasher(key) % bucket_count;
    //
    // -------------------------------------------------
    //
    // historical problems before c++11:
    // - c++ had multiple initialization syntaxes:
    //   parentheses ():    std::string str("hello");
    //   equals =:          std::string str = "hello";
    //   old constructors:  std::string str("hello", 5);
    //   array style:       char arr[] = {'a', 'b'};
    // - these different methods caused confusion and inconsistency
    // - some contexts only allowed certain initialization forms
    // - implicit narrowing conversions could cause silent bugs
    //
    // the {} solution (introduced in c++11):
    // - provides one consistent syntax for all initialization scenarios
    // - std::string str{"hello"};     // objects
    // - std::vector<int> v{1,2,3};    // containers
    // - int arr[]{1,2,3};             // arrays
    // - return widget{};              // temporary objects
    // - point p{1.0, 2.0};            // aggregates
    //
    // key benefits:
    // - prevents narrowing conversions (compile error if data might be lost)
    // - works in all contexts (class members, arrays, return values)
    // - uniform syntax reduces cognitive load
    // - more explicit about initialization intent
    //
    // common modern usage patterns:
    // 1. temporary objects:
    //    auto result = compute_thing({});  // default construct
    //    return my_type{};                 // return temporary
    //
    // 2. class member initialization:
    //    class foo {
    //        std::vector<int> vec_{};      // default initialize member
    //        int count_{0};                // zero initialize
    //    };
    //
    // 3. structured bindings (c++17):
    //    auto [x, y] = point{1, 2};
    //
    // 4. aggregate initialization:
    //    struct point { int x, y; };
    //    point p{1, 2};                    // direct member init
    //
    // prevalence in modern code:
    // - highly common in post-2011 codebases
    // - google style guide recommends {} for object initialization
    // - boost, qt, and other major libraries use extensively
    // - particularly common in:
    //   * factory functions returning temporaries
    //   * class member initialization
    //   * container initialization
    //   * any context requiring explicit initialization
    //
    // gotchas/considerations:
    // - std::initializer_list constructor precedence can cause surprises
    // - some developers prefer () for single-argument construction
    // - {} can be more verbose with auto: auto x{1} vs auto x = 1
    // - some older codebases maintain () for consistency
    size_t operator()(const KeyType& key, size_t bucket_count) const override {
        return std::hash<KeyType>{}(key) % bucket_count;
    }
    [[nodiscard]] std::string name() const override { return "std::hash"; }
};

// fnv-1a hash implementation
template<typename KeyType>
class FNV1aHash final : public HashFunction<KeyType> {
public:

    // fnv-1a hash constants:
    // fnv_prime = 16777619 (32 bit) or 1099511628211 (64 bit)
    // - this special prime number was chosen because it enables good dispersion
    //   and follows the form: 2^24 + 2^8 + 0x93 = 16777619
    // fnv_offset = 2166136261 (32 bit) or 14695981039346656037 (64 bit)
    // - also known as the fnv_offset_basis
    // - non-zero starting value that helps with avalanche behavior for small input changes
    // - helps avoid common hash attacks by providing a non-zero initial state
    //
    // these values are core to the fnv-1a algorithm which works by:
    // 1. starting with fnv_offset as initial hash value
    // 2. for each byte: hash = (hash ^ byte) * fnv_prime
    // this simple combination produces surprisingly good hash distribution and performance
    size_t operator()(const KeyType& key, const size_t bucket_count) const override {
        // fnv_offset = 2166136261 (32 bit) or 14695981039346656037 (64 bit)
        // also known as the fnv_offset_basis
        // non-zero starting value that helps with avalanche behavior for small input changes
        // helps avoid common hash attacks by providing a non-zero initial state
        constexpr uint64_t fnv_offset = 14695981039346656037ULL;

        uint64_t hash = fnv_offset;
        const auto* bytes = reinterpret_cast<const unsigned char*>(&key);

        for(size_t ndx = 0; ndx < sizeof(KeyType); ++ndx) {
            // fnv_prime = 16777619 (32 bit) or 1099511628211 (64 bit)
            // this special prime number was chosen because it enables good dispersion
            // and follows the form: 2^24 + 2^8 + 0x93 = 16777619
            constexpr uint64_t fnv_prime = 1099511628211ULL;

            hash ^= bytes[ndx];
            hash *= fnv_prime;
        }
        return hash % bucket_count;
    }
    [[nodiscard]] std::string name() const override { return "FNV1a"; }
};

// murmur3 hash implementation
template<typename KeyType>
class Murmur3Hash final : public HashFunction<KeyType> {
public:

    // murmur3 magic constants:
    // these specially chosen prime numbers (c1=0xcc9e2d51, c2=0x1b873593) were selected by
    // Austin Appleby through empirical testing to achieve key hash function properties:
    // - strong avalanche effect (changing 1 input bit changes ~50% of output bits)
    // - good bit distribution (hash outputs are uniformly distributed)
    // - low collision rates (different inputs rarely produce the same output)
    // - high performance (fast to compute)
    // the constants are used in sequence for bit mixing operations:
    // 1. k1 *= c1 (first mixing step)
    // 2. bit rotation
    // 3. k1 *= c2 (second mixing step)
    // this combination of operations with these specific constants produces optimal
    // hash distribution characteristics while maintaining high performance
    size_t operator()(const KeyType& key, const size_t bucket_count) const override {
        const auto* bytes = reinterpret_cast<const unsigned char*>(&key);
        uint32_t h1 = 0;

        for (size_t ndx = 0; ndx < sizeof(KeyType) - sizeof(KeyType) % 4; ndx += 4) {
            // murmur3 magic constants
            constexpr uint32_t c2 = 0x1b873593;
            constexpr uint32_t c1 = 0xcc9e2d51;

            uint32_t k1 = *reinterpret_cast<const uint32_t*>(bytes + ndx);
            k1 *= c1;
            k1 = (k1 << 15) | (k1 >> 17);
            k1 *= c2;
            h1 ^= k1;
            h1 = (h1 << 13) | (h1 >> 19);
            h1 = h1 * 5 + 0xe6546b64;
        }
        return h1 % bucket_count;
    }
    [[nodiscard]] std::string name() const override {
        return "Murmur3";
    }
};

template<typename KeyType, typename ValueType>
class HashTable {
private:
    // internal structure to store key-value pairs
    struct HashNode {
        KeyType m_key;
        ValueType m_value;
        HashNode(const KeyType& k, ValueType  v) : m_key(k), m_value(std::move(v)) {}
    };

    // member variables prefixed with m_
    std::vector<std::list<HashNode>> m_buckets;     // vector of linked lists for chaining
    size_t m_size;                                  // current number of elements
    size_t m_bucketCount;                           // total number of buckets
    float m_loadFactorThreshold;                    // threshold for rehashing
    std::shared_ptr<HashFunction<KeyType>> m_hashFunc;       // hash function strategy

    // hash function to map key to bucket index
    [[nodiscard]] size_t hash(const KeyType& key) const {
        return (*m_hashFunc)(key, m_bucketCount);
    }

    // private method to handle rehashing when a load factor exceeds a threshold
    void rehash() {
        size_t newBucketCount = m_bucketCount * 2;

        Logger::getInstance().log(LogLevel::INFO,
            std::format("rehashing: old bucket count={}, new bucket count={}",
                       m_bucketCount, newBucketCount));

        // create new buckets
        std::vector<std::list<HashNode>> newBuckets(newBucketCount);

        // update bucket count before rehashing
        m_bucketCount = newBucketCount;

        // rehash all existing elements
        for (const auto& bucket : m_buckets) {
            for (const auto& node : bucket) {
                size_t newIndex = hash(node.m_key);
                newBuckets[newIndex].push_back(node);
            }
        }
        m_buckets = std::move(newBuckets);
    }

public:
    // constructor initializes hash table with specified hash function
    explicit HashTable(std::shared_ptr<HashFunction<KeyType>> hashFunc,
             size_t initialBucketCount = 16,
             const float loadFactorThreshold = 0.75)
        : m_buckets(initialBucketCount),
          m_size(0),
          m_bucketCount(initialBucketCount),
          m_loadFactorThreshold(loadFactorThreshold),
          m_hashFunc(std::move(hashFunc)) {
        Logger::getInstance().log(LogLevel::INFO,
            std::format("hash table created with {} buckets using {}",
                       initialBucketCount, m_hashFunc->name()));
    }

    // insert operation - o(1) average case, o(n) worst case
    void insert(const KeyType& key, const ValueType& value) {
        // check if rehashing is needed
        float currentLoadFactor = static_cast<float>(m_size + 1) / m_bucketCount;
        if (currentLoadFactor > m_loadFactorThreshold) {
            rehash();
        }

        size_t index = hash(key);

        // check if key already exists
        for (auto& node : m_buckets[index]) {
            if (node.m_key == key) {
                node.m_value = value;  // update existing value
                Logger::getInstance().log(LogLevel::INFO, std::format("updated value for key={}", key));
                return;
            }
        }

        // insert a new key-value pair
        m_buckets[index].push_back(HashNode(key, value));
        m_size++;

        Logger::getInstance().log(LogLevel::INFO,
            std::format("inserted key={}, bucket={}", key, index));
    }

    // search operation - o(1) average case, o(n) worst case
    bool search(const KeyType& key, ValueType& value) const {
        size_t index = hash(key);

        for (const auto& node : m_buckets[index]) {
            if (node.m_key == key) {
                value = node.m_value;
                Logger::getInstance().log(LogLevel::INFO,
                    std::format("found key={} in bucket={}", key, index));
                return true;
            }
        }

        Logger::getInstance().log(LogLevel::INFO,
            std::format("key={} not found", key));
        return false;
    }

    // delete operation - o(1) average case, o(n) worst case
    bool remove(const KeyType& key) {
        size_t index = hash(key);

        auto& bucket = m_buckets[index];
        for (auto it = bucket.begin(); it != bucket.end(); ++it) {
            if (it->m_key == key) {
                bucket.erase(it);
                m_size--;
                Logger::getInstance().log(LogLevel::INFO,
                    std::format("removed key={} from bucket={}", key, index));
                return true;
            }
        }

        Logger::getInstance().log(LogLevel::INFO,
            std::format("key={} not found for removal", key));
        return false;
    }

    // utility methods
    [[nodiscard]] size_t size() const { return m_size; }
    [[nodiscard]] bool empty() const { return m_size == 0; }
    [[nodiscard]] float loadFactor() const {
        return static_cast<float>(m_size) / m_bucketCount;
    }
};

void runHashTableTests(const std::shared_ptr<HashFunction<int>>& hashFunc) {
    Logger::getInstance().log(LogLevel::INFO,
        std::format("\n=== Testing with {} ===\n", hashFunc->name()));

    // create hash table with initial bucket count of 4 for testing
    HashTable<int, std::string> hashTable(hashFunc, 4, 0.75);

    // test 1: basic insertion and search
    Logger::getInstance().log(LogLevel::INFO, "--- test 1: basic operations ---");
    hashTable.insert(1, "one");
    hashTable.insert(2, "two");
    hashTable.insert(3, "three");

    std::string value;
    assert(hashTable.search(1, value) && value == "one");
    assert(hashTable.search(2, value) && value == "two");
    assert(hashTable.search(3, value) && value == "three");
    assert(!hashTable.search(4, value));

    // test 2: collision handling and chaining
    Logger::getInstance().log(LogLevel::INFO, "--- test 2: collision handling ---");
    hashTable.insert(5, "five");
    hashTable.insert(9, "nine");  // may collide depending on hash function

    assert(hashTable.search(5, value) && value == "five");
    assert(hashTable.search(9, value) && value == "nine");

    // test 3: deletion
    Logger::getInstance().log(LogLevel::INFO, "--- test 3: deletion ---");
    assert(hashTable.remove(2));
    assert(!hashTable.search(2, value));
    assert(!hashTable.remove(10));  // non-existent key

    // test 4: update existing key
    Logger::getInstance().log(LogLevel::INFO, "--- test 4: update existing key ---");
    hashTable.insert(1, "ONE");
    assert(hashTable.search(1, value) && value == "ONE");

    // test 5: rehashing
    Logger::getInstance().log(LogLevel::INFO, "--- test 5: rehashing ---");
    for (int i = 10; i < 20; ++i) {
        hashTable.insert(i, std::to_string(i));
    }

    // verify all elements are still accessible after rehashing
    for (int i = 10; i < 20; ++i) {
        assert(hashTable.search(i, value) && value == std::to_string(i));
    }

    Logger::getInstance().log(LogLevel::INFO,
        std::format("final load factor: {}", hashTable.loadFactor()));

    Logger::getInstance().log(LogLevel::INFO,
        std::format("=== Completed {} tests successfully! ===\n", hashFunc->name()));
}

int main() {
    // create vector of hash functions to test
    const std::vector<std::shared_ptr<HashFunction<int>>> hashFuncs = {
        std::make_shared<StdHash<int>>(),
        std::make_shared<FNV1aHash<int>>(),
        std::make_shared<Murmur3Hash<int>>()
    };

    // run all tests for each hash function
    for (const auto& hashFunc : hashFuncs) {
        runHashTableTests(hashFunc);
    }
    return 0;
}
