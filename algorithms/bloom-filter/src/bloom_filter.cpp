// MIT License
// Copyright (c) 2025 dbjwhs

#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <limits>
#include "../../../headers/project_utils.hpp"

// bloom filter: ultra-efficient probabilistic set membership data structure
//
// core benefits:
// - extremely memory-efficient compared to traditional set data structures
// - constant-time o(k) insertion and lookup operations
// - zero false negatives (if it says an element is not in the set, it definitively is not)
// - controlled false positive rate (configurable memory-accuracy trade-off)
//
// classic use cases:
// 1. caching systems: quick pre-check before expensive database lookups
// 2. network routing: rapid ip/domain blacklist/whitelist checks
// 3. spell checkers: preliminary dictionary word existence tests
// 4. data deduplication: first-pass filtering before detailed comparison
//
// real-world enterprise example; when I was on the VADP team at VMware we used a Bloom filter
// as first pass filter before calculating expensive SHA hashes.
// - initial data chunk identification
// - bloom filter as a fast, memory-efficient first-pass filter
// - quickly eliminate non-duplicate chunks before expensive cryptographic hash comparisons
// - dramatically reduced computational overhead in large-scale storage systems
//
class BloomFilter {
private:
    // the size of the bit array, determines the memory footprint and false positive probability
    size_t m_size;

    // the number of hash functions used, affects the false positive rate and performance
    size_t m_hashCount;

    // bit array representing set membership, uses minimal memory
    std::vector<bool> m_bitArray;

    // generate hash index for an item using multiple hash functions
    // combines standard library hash with a seed to create multiple independent hash functions
    [[nodiscard]] size_t hash(const std::string& item, size_t seed) const {
        return std::hash<std::string>{}(item + std::to_string(seed)) % m_size;
    }

        // calculate optimal bit array size based on the bloom filter probabilistic model
        //
        // detailed theoretical background:
        // - wikipedia: https://en.wikipedia.org/wiki/Bloom_filter#Probability_of_false_positives
        // - original paper: Burton H. Bloom. "Space/Time Trade-offs in Hash Coding with Allowable Errors" (1970)
        //
        // mathematical derivation of the optimal bit array size:
        // m = -((n * ln(p)) / (ln(2)^2))
        // where:
        // - m: number of bits in the bloom filter
        // - n: number of expected elements
        // - p: desired false positive probability
        //
        // key considerations:
        // 1. larger m reduces false positive probability
        // 2. size is logarithmically related to false positive rate
        // 3. minimal memory overhead while maintaining low false positive probability
        //
        // computational complexity: o(1) - constant time calculation
        // space complexity: o(m) - proportional to bit array size
        //
        // references:
        // - "Probabilistic Data Structures and Algorithms for Big Data" by Ian Wrigley
        // - ACM Computing Surveys: Bloom Filters - https://dl.acm.org/doi/10.1145/1454370.1454380
    [[nodiscard]] static size_t calculateSize(size_t expectedElements, double falsePositiveRate) {
        // validate input parameters to prevent calculation errors
        if (expectedElements == 0 || falsePositiveRate <= 0 || falsePositiveRate >= 1) {
            Logger::getInstance().log(LogLevel::CRITICAL,
                std::format("invalid parameters for bloom filter, using default size. "
                            "elements: {}, false positive rate: {}",
                            expectedElements, falsePositiveRate));
            return 1024; // default fallback size
        }

        // calculate optimal bit array size using probabilistic formula
        double m = -((expectedElements * std::log(falsePositiveRate)) /
                     (std::log(2) * std::log(2)));

        // ensure calculated size is within reasonable bounds
        m = std::min(m, static_cast<double>(std::numeric_limits<size_t>::max()));
        return std::max(static_cast<size_t>(m), static_cast<size_t>(1024));
    }

    // calculate optimal number of hash functions
    // balances the trade-off between false positive rate and computational complexity
    static size_t calculateHashCount(const size_t m, const size_t n) {
        // handle edge cases to prevent division by zero or invalid calculations
        if (n == 0) {
            Logger::getInstance().log(LogLevel::CRITICAL,
                "zero expected elements, defaulting to single hash function");
            return 1;
        }

        // use optimal hash function count formula
        const double k = (m / static_cast<double>(n)) * std::log(2);
        return std::max(static_cast<size_t>(k), static_cast<size_t>(1));
    }

public:
    // constructor initializes bloom filter with expected elements and false positive rate
    // automatically calculates optimal bit array size, and hash function counts
    BloomFilter(const size_t expectedElements, double falsePositiveRate)
        : m_size(calculateSize(expectedElements, falsePositiveRate)),
          m_hashCount(calculateHashCount(m_size, expectedElements)),
          m_bitArray(m_size, false) {
        Logger::getInstance().log(LogLevel::INFO,
            std::format("bloom filter initialized: size={}, hash_functions={}",
                        m_size, m_hashCount));
    }

    // add an item to the bloom filter by setting corresponding bits
    void add(const std::string& item) {
        for (size_t i = 0; i < m_hashCount; ++i) {
            size_t index = hash(item, i);
            m_bitArray[index] = true;
        }
        Logger::getInstance().log(LogLevel::INFO,
            std::format("added item to bloom filter: {}", item));
    }

    // check if an item might be in the set
    // guarantees no false negatives, but allows potential false positives
    [[nodiscard]] bool contains(const std::string& item) const {
        for (size_t i = 0; i < m_hashCount; ++i) {
            size_t index = hash(item, i);
            if (!m_bitArray[index]) {
                return false;
            }
        }
        return true;
    }

    // generate detailed statistics about the bloom filter's current state
    void printStats() const {
        // count set bits to understand filter's current occupation
        size_t setBits = 0;
        for (bool bit : m_bitArray) {
            if (bit) ++setBits;
        }

        // log detailed bloom filter statistics
        Logger::getInstance().log(LogLevel::INFO,
            std::format("bloom filter statistics:"
                        "  bit array size: {} bits, "
                        "  hash function count: {}, "
                        "  bits set: {} ({:.2f}%)",
                        m_size, m_hashCount, setBits,
                        (setBits * 100.0 / m_size)));
    }

    // getters for internal parameters useful for testing and debugging
    [[nodiscard]] size_t getSize() const { return m_size; }
    [[nodiscard]] size_t getHashCount() const { return m_hashCount; }
};

int main() {
    try {
        // create a bloom filter for 1000 expected elements with 1% false positive rate
        BloomFilter bloom(1000, 0.01);

        // add some test elements
        bloom.add("apple");
        bloom.add("banana");
        bloom.add("cherry");

        // test membership
        Logger::getInstance().log(LogLevel::INFO,
            std::format("'apple' in bloom: {}",
                        bloom.contains("apple") ? "true" : "false"));
        Logger::getInstance().log(LogLevel::INFO,
            std::format("'grape' in bloom: {}",
                        bloom.contains("grape") ? "true" : "false"));

        // demonstrate potential false positives
        std::vector<std::string> testElements = {"dog", "cat", "elephant", "lion", "tiger"};
        size_t falsePositives = 0;

        for (const auto& element : testElements) {
            if (bloom.contains(element)) {
                Logger::getInstance().log(LogLevel::CRITICAL,
                    std::format("possible false positive: {}", element));
                ++falsePositives;
            }
        }

        // log false positive count
        Logger::getInstance().log(LogLevel::INFO,
            std::format("false positive count: {}", falsePositives));

        // print bloom filter statistics
        bloom.printStats();

    } catch (const std::exception& e) {
        Logger::getInstance().log(LogLevel::CRITICAL,
            std::format("exception occurred: {}", e.what()));
        return 1;
    }

    return 0;
}
