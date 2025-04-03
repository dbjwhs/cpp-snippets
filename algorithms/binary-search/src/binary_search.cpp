// MIT License
// Copyright (c) 2025 dbjwhs

#include <utility>
#include <vector>
#include <string>
#include <format>
#include <cassert>
#include <functional>
#include <random>
#include <chrono>
#include <algorithm>
#include "../../../headers/project_utils.hpp"

template<typename ElementType>
class BinarySearch {
public:
    // constructor initializes the data and optional comparator
    explicit BinarySearch(const std::vector<ElementType>& data,
                         std::function<bool(const ElementType&, const ElementType&)> comp = std::less<ElementType>())
        : m_data(data)
        , m_comparator(std::move(comp))
        , m_last_search_passes(0) {
        // verify the data is sorted according to the comparator
        for (size_t ndx = 1; ndx < m_data.size(); ++ndx) {
            // in a sorted sequence, no element should be less than its predecessor
            assert(!m_comparator(m_data[ndx], m_data[ndx-1]) ||
                   (!m_comparator(m_data[ndx-1], m_data[ndx])) && "input data must be sorted!");
        }
    }

    // search returns index of found item or -1 if not found
    int search(const ElementType& target) const {
        if (m_data.empty()) {
            LOG_INFO(std::format("binary search called on empty container"));
            return -1;
        }

        m_last_search_passes = 0;
        int left = 0;
        int right = static_cast<int>(m_data.size()) - 1;

        while (left <= right) {
            m_last_search_passes++;
            // use midpoint calculation that prevents integer overflow
            int mid = left + (right - left) / 2;

            // log the current search window
            LOG_INFO(std::format("searching window [{}, {}], checking index {}",
                           left, right, mid));

            // found exact match
            if (!m_comparator(m_data[mid], target) && !m_comparator(target, m_data[mid])) {
                LOG_INFO(std::format("found target at index {} in {} passes", mid, m_last_search_passes));
                return mid;
            }

            // adjust a search window based on comparison
            if (m_comparator(m_data[mid], target)) {
                left = mid + 1;  // target is in right half
            } else {
                right = mid - 1;  // target is in left half
            }
        }

        LOG_INFO(std::format("target not found after {} passes", m_last_search_passes));
        return -1;
    }

    // getter for last search pass count
    size_t get_last_search_passes() const { return m_last_search_passes; }

private:
    // store the data in sorted order
    std::vector<ElementType> m_data;
    // comparator function for comparing elements
    std::function<bool(const ElementType&, const ElementType&)> m_comparator;
    // track the number of passes in last search
    mutable size_t m_last_search_passes;
};

void simple_tests() {
    // test suite 1: string comparisons with default comparator
    {
        const std::vector<std::string> words = {
            "apple", "banana", "cherry", "date", "elderberry"
        };

        const BinarySearch<std::string> bs(words);

        // test successful searches
        assert(bs.search("apple") == 0);  // safe since we know it's first and unique
        const int cherry_idx = bs.search("cherry");
        assert(cherry_idx != -1 && words[cherry_idx] == "cherry");
        const int elder_idx = bs.search("elderberry");
        assert(elder_idx != -1 && words[elder_idx] == "elderberry");

        // test unsuccessful searches
        assert(bs.search("apricot") == -1);
        assert(bs.search("zebra") == -1);
        assert(bs.search("") == -1);

        LOG_INFO("completed string comparison test suite with default comparator");
    }

    // test suite 2: string comparisons with custom case-insensitive comparator
    {
        auto case_insensitive_less = [](const std::string& a, const std::string& b) {
            return std::ranges::lexicographical_compare(a, b, [](const char c1, const char c2) {
                    return std::tolower(c1) < std::tolower(c2);
                }
            );
        };

        const std::vector<std::string> words = {
            "Alpha", "beta", "Charlie", "delta", "Echo"
        };

        const BinarySearch<std::string> bs(words, case_insensitive_less);

        // test case-insensitive searches
        const int alpha_idx = bs.search("alpha");
        assert(alpha_idx != -1 && case_insensitive_less("Alpha", words[alpha_idx]) == false
               && case_insensitive_less(words[alpha_idx], "Alpha") == false);

        const int beta_idx = bs.search("BETA");
        assert(beta_idx != -1 && case_insensitive_less("beta", words[beta_idx]) == false
               && case_insensitive_less(words[beta_idx], "beta") == false);

        const int charlie_idx = bs.search("cHaRlIe");
        assert(charlie_idx != -1 && case_insensitive_less("Charlie", words[charlie_idx]) == false
               && case_insensitive_less(words[charlie_idx], "Charlie") == false);

        LOG_INFO("completed string comparison test suite with case-insensitive comparator");
    }

    // test suite 3: edge cases
    {
        // empty container
        const BinarySearch<std::string> empty_bs({});
        assert(empty_bs.search("anything") == -1);

        // single element
        const BinarySearch<std::string> single_bs({"solo"});
        assert(single_bs.search("solo") == 0);
        assert(single_bs.search("other") == -1);

        // duplicate elements
        const std::vector<std::string> with_dupes = {
            "one", "one", "three", "two", "two"
        };  // must be sorted for binary search to work
        BinarySearch<std::string> dupe_bs(with_dupes);

        // search for "two" and verify we found one of its instances
        const int two_idx = dupe_bs.search("two");
        assert(two_idx != -1 && with_dupes[two_idx] == "two");

        // verify "one" and "three" are found correctly
        const int one_idx = dupe_bs.search("one");
        assert(one_idx != -1 && with_dupes[one_idx] == "one");
        const int three_idx = dupe_bs.search("three");
        assert(three_idx != -1 && with_dupes[three_idx] == "three");

        LOG_INFO("completed edge cases test suite");
    }
}

void deeper_tests() {
    // random number generator for creating test data
    std::random_device rd;
    std::mt19937 gen(rd());

    // test different sizes of data
    std::vector<size_t> test_sizes = {1000, 10000, 100000, 1000000};

    for (size_t size : test_sizes) {
        // generate sorted vector of random integers
        std::vector<int> data;
        data.reserve(size);
        std::uniform_int_distribution<> dis(1, static_cast<int>(size * 2));

        for (size_t ndx = 0; ndx < size; ++ndx) {
            data.push_back(dis(gen));
        }
        std::ranges::sort(data);

        BinarySearch<int> bs(data);

        LOG_INFO(std::format("testing with {} elements", size));

        // test cases for timing:
        // 1. best case (middle element)
        // 2. worst case (first or last element)
        // 3. random existing elements
        // 4. non-existing elements

        // middle element (the best case)
        {
            auto start = std::chrono::high_resolution_clock::now();
            const int mid_idx = static_cast<int>(size / 2);
            int mid_value = data[mid_idx];
            const int result = bs.search(mid_value);
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);

            LOG_INFO(std::format("best case (middle element) search took {} ns and {} passes",
                           duration.count(), bs.get_last_search_passes()));
            assert(result != -1 && data[result] == mid_value);
        }

        // worst case (first element)
        {
            auto start = std::chrono::high_resolution_clock::now();
            int first_value = data[0];
            const int result = bs.search(first_value);
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);

            LOG_INFO(std::format("worst case (first element) search took {} ns and {} passes",
                           duration.count(), bs.get_last_search_passes()));
            assert(result != -1 && data[result] == first_value);
        }

        // random existing elements (10 searches)
        {
            std::uniform_int_distribution<> index_dis(0, static_cast<int>(size - 1));
            double total_time = 0;
            size_t total_passes = 0;

            for (int ndx = 0; ndx < 10; ++ndx) {
                const int target_idx = index_dis(gen);
                int target_value = data[target_idx];
                auto start = std::chrono::high_resolution_clock::now();
                const int result = bs.search(target_value);
                auto end = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);

                total_time += duration.count();
                total_passes += bs.get_last_search_passes();
                // verify that we found the correct value, not necessarily at the same index
                // due to possible duplicates
                assert(result != -1 && data[result] == target_value);
            }

            LOG_INFO(std::format("random existing elements: avg search time {} ns, avg passes {}",
                           total_time / 10, total_passes / 10));
        }

        // non-existing elements (10 searches)
        {
            double total_time = 0;
            size_t total_passes = 0;

            for (int ndx = 0; ndx < 10; ++ndx) {
                int target = -1 * dis(gen);  // negative numbers won't exist in our data
                auto start = std::chrono::high_resolution_clock::now();
                const int result = bs.search(target);
                auto end = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);

                total_time += duration.count();
                total_passes += bs.get_last_search_passes();
                assert(result == -1);
            }

            LOG_INFO(std::format("non-existing elements: avg search time {} ns, avg passes {}",
                           total_time / 10, total_passes / 10));
        }
    }
}

int main() {
    LOG_INFO("starting simple tests...");
    simple_tests();
    LOG_INFO("simple tests completed successfully");

    LOG_INFO("starting deeper tests...");
    deeper_tests();
    LOG_INFO("deeper tests completed successfully");

    return 0;
}
