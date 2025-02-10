// MIT License
// Copyright (c) 2025 dbjwhs

#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <cassert>
#include "../../../headers/project_utils.hpp"

class QuickSort {
private:
    std::vector<int> m_array;

    // partition the array and return the pivot index
    std::size_t partition(const std::size_t low, const std::size_t high) {
        const int m_pivot = m_array[high];
        std::size_t m_i = low - 1;

        for (std::size_t j = low; j < high; j++) {
            if (m_array[j] <= m_pivot) {
                m_i++;
                std::swap(m_array[m_i], m_array[j]);
            }
        }
        std::swap(m_array[m_i + 1], m_array[high]);
        return m_i + 1;
    }

    // recursive quicksort implementation with depth control
    //
    // depth control is critical in recursive functions for several reasons:
    // 1. stack overflow prevention:
    //    - each recursive call adds a frame to the call stack (local vars, params, return address)
    //    - stack size is limited (typical: windows=1mb, linux=8mb)
    //    - without depth control, deep recursion can crash the program
    //
    // 2. performance:
    //    - each recursive call has overhead (stack frame creation/deletion)
    //    - very deep recursion can cause cache misses
    //    - excessive depth can cause memory paging
    //
    // 3. graceful degradation:
    //    - allows fallback to alternative methods (e.g., std::sort) when too deep
    //    - handles pathological cases gracefully
    //    - maintains stability with unexpected inputs
    //
    // 4. security:
    //    - prevents stack overflow attacks
    //    - prevents denial of service via resource exhaustion
    //    - protects against malicious inputs designed to trigger excessive recursion
    //
    void quickSortRecursive(std::size_t low, std::size_t high, std::size_t depth = 0) { // NOLINT(misc-no-recursion)

        // adjust max_depth per your needs
        while (low < high) {
            if (constexpr std::size_t max_depth = 10000; depth >= max_depth) {
                // fall back to std::sort for very deep recursions
                std::sort(m_array.begin() + static_cast<std::ptrdiff_t>(low),
                         m_array.begin() + static_cast<std::ptrdiff_t>(high) + 1);
                return;
            }

            // optimize tail recursion by handling smaller partition first
            if (std::size_t m_pivot_index = partition(low, high); m_pivot_index - low < high - m_pivot_index) {
                if (m_pivot_index > 0) {  // check to prevent underflow
                    quickSortRecursive(low, m_pivot_index - 1, depth + 1);
                }
                low = m_pivot_index + 1; // tail recursion optimization
            } else {
                quickSortRecursive(m_pivot_index + 1, high, depth + 1);
                if (m_pivot_index > 0) {  // check to prevent underflow
                    high = m_pivot_index - 1; // tail recursion optimization
                } else {
                    break;
                }
            }
            depth++;
        }
    }

public:
    // constructor
    explicit QuickSort(const std::vector<int>& arr) : m_array(arr) {}

    // public sort interface
    void sort() {
        if (m_array.size() <= 1) return;
        quickSortRecursive(0, m_array.size() - 1);
    }

    // getter for sorted array
    [[nodiscard]] std::vector<int> getSortedArray() const {
        return m_array;
    }
};

// test helper function to check if an array is sorted
bool isSorted(const std::vector<int>& arr) {
    return std::ranges::is_sorted(arr);
}

// test helper function to generate random vector
std::vector<int> generateRandomVector(const int size) {
    RandomGenerator random(-10000, 10000);
    std::vector<int> random_vec(size);

    for (int ndx = 0; ndx < size; ndx++) {
        random_vec[ndx] = random.getNumber();
    }
    return random_vec;
}

int main() {
    Logger& logger = Logger::getInstance();

    // test case 1: empty array
    logger.log(LogLevel::INFO, "test 1: empty array");
    std::vector<int> empty_array;
    QuickSort qs1(empty_array);
    qs1.sort();
    assert(qs1.getSortedArray().empty());

    // test case 2: single element
    logger.log(LogLevel::INFO, "test 2: single element array");
    std::vector<int> single_element = {42};
    QuickSort qs2(single_element);
    qs2.sort();
    assert(qs2.getSortedArray() == single_element);

    // test case 3: already sorted array
    logger.log(LogLevel::INFO, "test 3: already sorted array");
    std::vector<int> sorted_array = {1, 2, 3, 4, 5};
    QuickSort qs3(sorted_array);
    qs3.sort();
    assert(isSorted(qs3.getSortedArray()));

    // test case 4: reverse sorted array
    logger.log(LogLevel::INFO, "test 4: reverse sorted array");
    std::vector<int> reverse_sorted = {5, 4, 3, 2, 1};
    QuickSort qs4(reverse_sorted);
    qs4.sort();
    assert(isSorted(qs4.getSortedArray()));

    // test case 5: array with duplicates
    logger.log(LogLevel::INFO, "test 5: array with duplicates");
    std::vector<int> duplicates = {3, 1, 4, 1, 5, 9, 2, 6, 5, 3};
    QuickSort qs5(duplicates);
    qs5.sort();
    assert(isSorted(qs5.getSortedArray()));

    // test case 6: random large array
    logger.log(LogLevel::INFO, "test 6: random large array");
    std::vector<int> large_random = generateRandomVector(1000);
    QuickSort qs6(large_random);
    qs6.sort();
    assert(isSorted(qs6.getSortedArray()));

    // test case 7: array with negative numbers
    logger.log(LogLevel::INFO, "test 7: array with negative numbers");
    std::vector<int> negative_numbers = {-5, 3, -2, 7, -1, 0, 4};
    QuickSort qs7(negative_numbers);
    qs7.sort();
    assert(isSorted(qs7.getSortedArray()));

    logger.log(LogLevel::INFO, "all tests passed successfully!");
    return 0;
}
