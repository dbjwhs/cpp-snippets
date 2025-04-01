// MIT License
// Copyright (c) 2025 dbjwhs

#include <iostream>
#include <vector>
#include <ranges>
#include <algorithm>
#include <numeric>
#include <cassert>
#include <string>
#include <format>
#include "../../../headers/project_utils.hpp"

// c++ ranges history and overview
// ranges were introduced in c++20 as part of the standard library to provide a more functional programming approach to
// working with collections of data. they were inspired by eric niebler's range-v3 library and the boost range library
// before that. ranges extend the standard library algorithms by allowing operations to be composed via the pipe operator (|)
// making code more readable and expressive. ranges also introduce laziness, which means operations are only performed when
// needed, improving performance for large data sets or complex operation chains. common usage includes filtering, transforming,
// and performing operations on collections without explicit loops, iterator management, or temporary containers.

class RangesExample {
private:
    // member variable to store our collection
    std::vector<int> m_data;

public:
    // constructor taking a vector of integers
    explicit RangesExample(std::vector<int> data) : m_data(std::move(data)) {
        Logger::getInstance().log(LogLevel::INFO, "ranges example created with {} elements", m_data.size());
    }

    // method to demonstrate basic ranges views
    void demonstrateBasicViews() {
        // filter view - selects elements that satisfy a predicate
        auto even_numbers = m_data | std::views::filter([](int n) {
            return n % 2 == 0;
        });
        
        Logger::getInstance().log(LogLevel::INFO, "demonstrating filter view for even numbers");
        for (int n : even_numbers) {
            Logger::getInstance().log(LogLevel::INFO, std::format("even number: {}", n));
        }
        
        // testing the filter view with assertions
        std::vector<int> even_result;
        for (int n : even_numbers) {
            even_result.push_back(n);
        }
        
        // verify all numbers in result are even
        for (int n : even_result) {
            assert(n % 2 == 0 && "all numbers should be even");
        }
        
        // transform view - applies a function to each element
        auto squared_numbers = m_data | std::views::transform([](int n) {
            return n * n;
        });
        
        Logger::getInstance().log(LogLevel::INFO, "demonstrating transform view for squaring numbers");
        for (int n : squared_numbers) {
            Logger::getInstance().log(LogLevel::INFO, std::format("squared number: {}", n));
        }
        
        // testing the transform view with assertions
        std::vector<int> squared_result;
        for (int n : squared_numbers) {
            squared_result.push_back(n);
        }
        
        // verify each number is the square of the original
        for (size_t ndx = 0; ndx < m_data.size(); ++ndx) {
            assert(squared_result[ndx] == m_data[ndx] * m_data[ndx] && "number should be squared");
        }
    }
    
    // method to demonstrate range composition
    void demonstrateComposition() {
        // composing multiple views: filter even numbers, then square them
        auto even_squared = m_data 
            | std::views::filter([](int n) {
                return n % 2 == 0;
            })
            | std::views::transform([](int n) {
                return n * n;
            });
        
        Logger::getInstance().log(LogLevel::INFO, "demonstrating composition: even numbers squared");
        for (int n : even_squared) {
            Logger::getInstance().log(LogLevel::INFO, std::format("even number squared: {}", n));
        }
        
        // testing the composition with assertions
        std::vector<int> result;
        for (int n : even_squared) {
            result.push_back(n);
        }
        
        // verify result manually
        std::vector<int> manual_result;
        for (const int n : m_data) {
            if (n % 2 == 0) {
                manual_result.push_back(n * n);
            }
        }
        
        assert(result.size() == manual_result.size() && "should have same number of elements");
        for (size_t ndx = 0; ndx < result.size(); ++ndx) {
            assert(result[ndx] == manual_result[ndx] && "elements should match");
        }
    }
    
    // method to demonstrate take and drop views
    void demonstrateTakeAndDrop() {
        // take view - selects the first n elements
        const auto first_three = m_data | std::views::take(3);
        
        Logger::getInstance().log(LogLevel::INFO, "demonstrating take view for first three elements");
        for (int n : first_three) {
            Logger::getInstance().log(LogLevel::INFO, std::format("element: {}", n));
        }
        
        // testing take view
        std::vector<int> take_result;
        for (int n : first_three) {
            take_result.push_back(n);
        }
        
        assert(take_result.size() <= 3 && "should have at most 3 elements");
        for (size_t i = 0; i < take_result.size(); ++i) {
            assert(take_result[i] == m_data[i] && "elements should match original");
        }
        
        // drop view - skips the first n elements
        auto after_two = m_data | std::views::drop(2);
        
        Logger::getInstance().log(LogLevel::INFO, "demonstrating drop view to skip first two elements");
        for (int n : after_two) {
            Logger::getInstance().log(LogLevel::INFO, std::format("element: {}", n));
        }
        
        // testing drop view
        std::vector<int> drop_result;
        for (int n : after_two) {
            drop_result.push_back(n);
        }
        
        assert(drop_result.size() == m_data.size() - 2 || m_data.size() < 2 && "should have size-2 elements");
        for (size_t i = 0; i < drop_result.size(); ++i) {
            assert(drop_result[i] == m_data[i + 2] && "elements should match original with offset");
        }
    }
    
    // method to demonstrate more advanced range operations
    void demonstrateAdvancedOperations() {
        // reverse view - reverses the order of elements
        auto reversed = m_data | std::views::reverse;
        
        Logger::getInstance().log(LogLevel::INFO, "demonstrating reverse view");
        for (int n : reversed) {
            Logger::getInstance().log(LogLevel::INFO, std::format("reversed element: {}", n));
        }
        
        // testing reverse view
        std::vector<int> reverse_result;
        for (int n : reversed) {
            reverse_result.push_back(n);
        }
        
        assert(reverse_result.size() == m_data.size() && "should have same number of elements");
        for (size_t i = 0; i < reverse_result.size(); ++i) {
            assert(reverse_result[i] == m_data[m_data.size() - 1 - i] && "elements should be reversed");
        }
        
        // join view - flattens a range of ranges (requires c++23 - uncomment if supported)
        /*
        std::vector<std::vector<int>> nested = {{1, 2}, {3, 4, 5}, {6}};
        auto flattened = nested | std::views::join;
        
        Logger::getInstance().log(LogLevel::INFO, "demonstrating join view to flatten nested vectors");
        for (int n : flattened) {
            Logger::getInstance().log(LogLevel::INFO, std::format("flattened element: {}", n));
        }
        */
        
        // elements view (c++20) - creates a range from the ith elements of a range of tuples
        std::vector<std::pair<int, std::string>> pairs = {
            {1, "one"}, {2, "two"}, {3, "three"}
        };
        
        auto numbers = pairs | std::views::elements<0>;  // extract first elements
        
        Logger::getInstance().log(LogLevel::INFO, "demonstrating elements view to extract first elements of pairs");
        for (int n : numbers) {
            Logger::getInstance().log(LogLevel::INFO, std::format("number: {}", n));
        }
        
        // testing elements view
        std::vector<int> elements_result;
        for (int n : numbers) {
            elements_result.push_back(n);
        }
        
        assert(elements_result.size() == pairs.size() && "should have same number of elements");
        for (size_t i = 0; i < elements_result.size(); ++i) {
            assert(elements_result[i] == pairs[i].first && "should match first element of pair");
        }
    }
    
    // method to demonstrate range algorithms
    void demonstrateAlgorithms() {
        // ranges::find algorithm
        auto it = std::ranges::find(m_data, 5);
        
        Logger::getInstance().log(LogLevel::INFO, "demonstrating ranges::find to locate element 5");
        if (it != m_data.end()) {
            Logger::getInstance().log(LogLevel::INFO, std::format("found element 5 at position {}", 
                std::distance(m_data.begin(), it)));
        } else {
            Logger::getInstance().log(LogLevel::INFO, "element 5 not found");
        }
        
        // ranges::sort algorithm
        std::vector<int> data_copy = m_data;
        std::ranges::sort(data_copy);
        
        Logger::getInstance().log(LogLevel::INFO, "demonstrating ranges::sort");
        for (int n : data_copy) {
            Logger::getInstance().log(LogLevel::INFO, std::format("sorted element: {}", n));
        }
        
        // testing sort
        for (size_t i = 1; i < data_copy.size(); ++i) {
            assert(data_copy[i-1] <= data_copy[i] && "elements should be in ascending order");
        }
        
        // ranges::count_if algorithm
        int even_count = std::ranges::count_if(m_data, [](int n) { return n % 2 == 0; });
        
        Logger::getInstance().log(LogLevel::INFO, 
            std::format("demonstrating ranges::count_if to count even numbers: {}", even_count));
        
        // test count_if
        int manual_count = 0;
        for (int n : m_data) {
            if (n % 2 == 0) {
                manual_count++;
            }
        }
        assert(even_count == manual_count && "count of even numbers should match");
    }
};

int main() {
    Logger::getInstance().log(LogLevel::INFO, "starting c++ ranges example program");
    
    // create a sample vector for demonstration
    std::vector<int> data = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    
    // create an instance of our example class
    RangesExample example(data);
    
    // demonstrate different range features
    example.demonstrateBasicViews();
    example.demonstrateComposition();
    example.demonstrateTakeAndDrop();
    example.demonstrateAdvancedOperations();
    example.demonstrateAlgorithms();
    
    Logger::getInstance().log(LogLevel::INFO, "all tests passed successfully");
    return 0;
}