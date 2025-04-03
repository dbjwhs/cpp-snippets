// MIT License
// Copyright (c) 2025 dbjwhs

#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <cmath>
#include <format>
#include <algorithm>
#include <ranges>
#include "../../../headers/project_utils.hpp"

// hyperloglog: a probabilistic data structure for estimating the cardinality (number of unique elements)
// in a multiset with remarkable space efficiency
//
// key characteristics:
// - provides approximate count of unique elements
// - uses o(log(log(n))) memory space
// - trade-off between memory usage and accuracy
// - error rate typically around 2%
//
// how it works:
// 1. hash input elements
// 2. count leading zeros in hash representation
// 3. use statistical properties to estimate unique count
//
// references:
// - wikipedia: https://en.wikipedia.org/wiki/hyperloglog
// - original paper: "hyperloglog: the analysis of a near-optimal cardinality estimation algorithm"
//   by flajolet et al. (2007)
// - google's implementation: https://github.com/google/hyperloglog
//
// typical use cases:
// - counting unique visitors on websites
// - analyzing large datasets
// - distributed systems for cardinality estimation
//
// advantages over naive approaches:
// - much lower memory footprint compared to hash set
// - constant memory usage regardless of input size
// - fast estimation with minimal computational overhead
//
class SimpleHyperLogLog {
private:
    std::vector<int> m_registers;
    int m_registerCount;
    std::hash<std::string> m_hasher;

    // estimate number of unique elements
    [[nodiscard]] double estimate() const {
        double harmonicMean = 0.0;
        for (const int register_val : m_registers) {
            harmonicMean += std::pow(2.0, -register_val);
        }
        harmonicMean = 1.0 / harmonicMean;

        // rough estimation formula
        return m_registers.size() * m_registers.size() * harmonicMean;
    }

    // get position of first set bit
    [[nodiscard]] static int countLeadingZeros(size_t hash) {
        int zeros = 0;
        while ((hash & (1ULL << (sizeof(size_t) * 8 - 1))) == 0) {
            zeros++;
            hash <<= 1;
        }
        return zeros;
    }

    DECLARE_NON_COPYABLE(SimpleHyperLogLog);
    DECLARE_NON_MOVEABLE(SimpleHyperLogLog);

public:
    explicit SimpleHyperLogLog(const int registers_count = 16) :
        m_registers(registers_count, 0), m_registerCount(registers_count) {}

    void add(const std::string& element) {
        // hash the element
        const size_t hash = m_hasher(element);

        // use the first few bits to choose register
        const int registerIndex = hash % m_registerCount;

        // count leading zeros in the rest of the hash
        const int leadingZeros = countLeadingZeros(hash);

        // update register if new value is larger
        m_registers[registerIndex] = std::max(
            m_registers[registerIndex],
            leadingZeros
        );
    }

    // estimate unique elements
    [[nodiscard]] int uniqueCount() const {
        return static_cast<int>(std::round(estimate()));
    }
};

class DuplicateStringGenerator {
private:
    std::random_device m_rd;
    std::mt19937 m_gen;

    // predefined word lists to create more meaningful duplicates
    const std::vector<std::string> m_prefixes = {
        "super", "mega", "ultra", "hyper", "extra",
        "cool", "awesome", "great", "epic", "wild"
    };

    const std::vector<std::string> m_bases = {
        "cat", "dog", "bird", "fish", "horse",
        "tiger", "lion", "eagle", "wolf", "shark"
    };

    const std::vector<std::string> m_suffixes = {
        "master", "pro", "elite", "supreme", "king",
        "hero", "legend", "genius", "star", "warrior"
    };

public:
    DuplicateStringGenerator() : m_gen(m_rd()) {}

    // generate a vector with a specified number of strings and duplicate frequency
    std::vector<std::string> generate(
        const size_t m_total_elements,
        const double m_duplicate_ratio = 0.7,  // 70% chance of picking an existing string
        const size_t m_unique_base_count = 500 // number of unique base strings to generate
    ) {
        std::vector<std::string> elements;
        elements.reserve(m_total_elements);

        //### left is here need it to understand what this is doing
        std::uniform_int_distribution<> prefix_dist(0, m_prefixes.size() - 1);
        std::uniform_int_distribution<> base_dist(0, m_bases.size() - 1);
        std::uniform_int_distribution<> suffix_dist(0, m_suffixes.size() - 1);
        std::uniform_real_distribution<> duplicate_dist(0.0, 1.0);

        // generate a base set of unique strings
        std::unordered_map<std::string, size_t> string_counts;
        for (size_t i = 0; i < m_unique_base_count; ++i) {
            std::string unique_str = generateUniqueString(
                m_prefixes[prefix_dist(m_gen)],
                m_bases[base_dist(m_gen)],
                m_suffixes[suffix_dist(m_gen)]
            );
            string_counts[unique_str] = 0;
        }

        // fill the vector
        for (size_t ndx = 0; ndx < m_total_elements; ++ndx) {
            // decide whether to duplicate an existing string
            if (!string_counts.empty() && duplicate_dist(m_gen) < m_duplicate_ratio) {
                // pick a random existing string
                const auto it = std::next(string_counts.begin(),
                    std::uniform_int_distribution<>(0, string_counts.size() - 1)(m_gen));
                elements.push_back(it->first);
                it->second++;
            } else {
                // generate a new string
                std::string new_str = generateUniqueString(
                    m_prefixes[prefix_dist(m_gen)],
                    m_bases[base_dist(m_gen)],
                    m_suffixes[suffix_dist(m_gen)]
                );

                // if this is a truly new string, add it to the map
                if (!string_counts.contains(new_str)) {
                    string_counts[new_str] = 1;
                }
                elements.push_back(new_str);
            }
        }

        // print some statistics
        LOG_INFO(std::format("Total unique strings: {}", string_counts.size()));
        LOG_INFO(std::format("Duplicate string statistics:"));

        // find and print most duplicated strings
        std::vector<std::pair<std::string, size_t>> sorted_counts;
        for (const auto& pair : string_counts) {
            if (pair.second > 1) {
                sorted_counts.emplace_back(pair);
            }
        }

        std::ranges::partial_sort(sorted_counts, sorted_counts.begin() + std::min(static_cast<size_t>(10), sorted_counts.size())
                                  , [](const auto& a, const auto& b) {
                                      return a.second > b.second;
                                  }
        );

        LOG_INFO(std::format("Top 10 most duplicated strings:"));
        for (size_t ndx = 0; ndx < std::min(static_cast<size_t>(10), sorted_counts.size()); ++ndx) {
            Logger::getInstance().log(LogLevel::INFO, std::format("\t{}: {} times"
                , sorted_counts[ndx].first, sorted_counts[ndx].second));
        }

        return elements;
    }

private:
    // generate a unique string by combining prefix, base, and suffix
    static std::string generateUniqueString(
        const std::string& prefix,
        const std::string& base,
        const std::string& suffix
    ) {
        return prefix + "-" + base + "-" + suffix;
    }
};

void testHyperLogLog() {
    DuplicateStringGenerator generator;
    auto elements = generator.generate(1'000'000);

    // demonstrate hyperloglog's unique counting
    SimpleHyperLogLog hll;
    for (const auto& elem : elements) {
        hll.add(elem);
    }

    // get actual unique elements
    const std::unordered_set<std::string> uniqueSet(elements.begin(), elements.end());

    // compare actual unique count with hyperloglog estimate
    LOG_INFO(std::format("Actual unique elements: {}", uniqueSet.size()));
    LOG_INFO(std::format("HyperLogLog estimate: {}", hll.uniqueCount()));
}

int main() {
    testHyperLogLog();
    return 0;
}
