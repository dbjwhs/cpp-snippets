// MIT License
// Copyright (c) 2025 dbjwhs

#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include <algorithm>
#include "../../headers/project_utils.hpp"

std::string paginate(int currentPage, int totalPages, int maxPages) {
    // ensure parameters are valid
    currentPage = std::max(1, std::min(currentPage, totalPages));
    totalPages = std::max(1, totalPages);
    maxPages = std::max(3, maxPages); // minimum 3 to ensure we can show at least first, current, last
    
    // for very few pages, just show all pages
    if (totalPages <= maxPages) {
        std::string result;
        for (int i = 1; i <= totalPages; i++) {
            if (i == currentPage) {
                result += "[" + std::to_string(i) + "]";
            } else {
                result += std::to_string(i);
            }
            
            if (i < totalPages) {
                result += " ";
            }
        }
        return result;
    }
    
    // determine how many pages to show around the current page
    // we need to distribute maxPages - 2 pages (excluding first and last)
    int remainingPages = maxPages - 2;
    std::vector<int> pagesToShow;
    
    // first, determine if we need one or two ellipses
    bool needLeftEllipsis = (currentPage > remainingPages / 2 + 2);
    bool needRightEllipsis = (currentPage < totalPages - remainingPages / 2 - 1);
    
    // add first page to the list
    pagesToShow.push_back(1);
    
    // determine start and end page numbers to show in the middle section
    int startPage, endPage;
    
    if (needLeftEllipsis && needRightEllipsis) {
        // need both ellipses, current page is in the middle
        // distribute remaining slots evenly on both sides of current page
        int sidePages = (remainingPages - 1) / 2; // -1 for current page
        startPage = std::max(2, currentPage - sidePages);
        endPage = std::min(totalPages - 1, currentPage + sidePages);
        
        // adjust if we have an even number of pages to show
        if (remainingPages % 2 == 0) {
            // we need to add one more page - decide which side
            if (currentPage - startPage < endPage - currentPage) {
                // more room on the right side
                endPage++;
            } else {
                // more room on the left side or equal
                startPage--;
            }
        }
    } else if (needLeftEllipsis) {
        // only need left ellipsis, current page is near the end
        startPage = totalPages - remainingPages;
        endPage = totalPages - 1;
    } else if (needRightEllipsis) {
        // only need right ellipsis, current page is near the beginning
        startPage = 2;
        endPage = remainingPages + 1;
    } else {
        // should never get here with our constraints
        startPage = 2;
        endPage = totalPages - 1;
    }
    
    // add left ellipsis if needed
    if (needLeftEllipsis) {
        pagesToShow.push_back(-1); // -1 represents ellipsis
    }
    
    // add middle pages
    for (int i = startPage; i <= endPage; i++) {
        pagesToShow.push_back(i);
    }
    
    // add right ellipsis if needed
    if (needRightEllipsis) {
        pagesToShow.push_back(-1); // -1 represents ellipsis
    }
    
    // add last page if not already included
    if (totalPages > 1) {
        pagesToShow.push_back(totalPages);
    }
    
    // convert to string
    std::string result;
    for (size_t i = 0; i < pagesToShow.size(); i++) {
        if (pagesToShow[i] == -1) {
            // ellipsis
            result += "...";
        } else if (pagesToShow[i] == currentPage) {
            // current page with brackets
            result += "[" + std::to_string(pagesToShow[i]) + "]";
        } else {
            // regular page
            result += std::to_string(pagesToShow[i]);
        }
        
        if (i < pagesToShow.size() - 1) {
            result += " ";
        }
    }
    
    return result;
}

// test function to verify the pagination functionality against the provided examples
void testPaginationWithExamples() {
    LOG_INFO("Testing pagination with provided examples");
    
    // test case 1: all pages fit
    {
        std::string expected = "[1] 2 3 4 5 6 7 8 9 10 11";
        std::string result = paginate(1, 11, 11);
        LOG_INFO(std::format("Example 1: Expected: '{}', Got: '{}'", expected, result));
        assert(result == expected);
    }
    
    // test case 2: first page, right ellipsis
    {
        std::string expected = "[1] 2 3 4 5 6 7 8 9 10 ... 30";
        std::string result = paginate(1, 30, 11);
        LOG_INFO(std::format("Example 2: Expected: '{}', Got: '{}'", expected, result));
        assert(result == expected);
    }
    
    // test case 3: all pages fit, smaller dataset
    {
        std::string expected = "1 2 3 4 5 [6] 7 8 9 10";
        std::string result = paginate(6, 10, 11);
        LOG_INFO(std::format("Example 3: Expected: '{}', Got: '{}'", expected, result));
        assert(result == expected);
    }
    
    // test case 4: middle pages, right ellipsis
    {
        std::string expected = "1 2 3 4 5 [6] 7 8 9 10 ... 30";
        std::string result = paginate(6, 30, 11);
        LOG_INFO(std::format("Example 4: Expected: '{}', Got: '{}'", expected, result));
        assert(result == expected);
    }
    
    // test case 5: both ellipses
    {
        std::string expected = "1 ... 3 4 5 6 [7] 8 9 10 11 ... 30";
        std::string result = paginate(7, 30, 11);
        LOG_INFO(std::format("Example 5: Expected: '{}', Got: '{}'", expected, result));
        assert(result == expected);
    }
    
    // test case 6: both ellipses, later in a list
    {
        std::string expected = "1 ... 20 21 22 23 [24] 25 26 27 28 ... 30";
        std::string result = paginate(24, 30, 11);
        LOG_INFO(std::format("Example 6: Expected: '{}', Got: '{}'", expected, result));
        assert(result == expected);
    }
    
    // test case 7: left ellipsis, near end
    {
        std::string expected = "1 ... 21 22 23 24 25 26 [27] 28 29 30";
        std::string result = paginate(27, 30, 11);
        LOG_INFO(std::format("Example 7: Expected: '{}', Got: '{}'", expected, result));
        assert(result == expected);
    }
    
    // test case 8: left ellipsis, last page
    {
        std::string expected = "1 ... 21 22 23 24 25 26 27 28 29 [30]";
        std::string result = paginate(30, 30, 11);
        LOG_INFO(std::format("Example 8: Expected: '{}', Got: '{}'", expected, result));
        assert(result == expected);
    }
    
    // test case 9: even number of max pages
    {
        std::string expected = "[1] 2 3 4 5 6 7 8 9 ... 30";
        std::string result = paginate(1, 30, 10);
        LOG_INFO(std::format("Example 9: Expected: '{}', Got: '{}'", expected, result));
        assert(result == expected);
    }
    
    // test case 10: even number of max pages, last page
    {
        std::string expected = "1 ... 22 23 24 25 26 27 28 29 [30]";
        std::string result = paginate(30, 30, 10);
        LOG_INFO(std::format("Example 10: Expected: '{}', Got: '{}'", expected, result));
        assert(result == expected);
    }
    
    LOG_INFO("All examples passed!");
}

// additional test function with edge cases and variations
void testAdditionalCases() {
    LOG_INFO("Testing additional edge cases");
    
    // test case: very small total pages
    {
        std::string expected = "[1] 2 3";
        std::string result = paginate(1, 3, 11);
        LOG_INFO(std::format("Small dataset: Expected: '{}', Got: '{}'", expected, result));
        assert(result == expected);
    }
    
    // test case: single page
    {
        std::string expected = "[1]";
        std::string result = paginate(1, 1, 5);
        LOG_INFO(std::format("Single page: Expected: '{}', Got: '{}'", expected, result));
        assert(result == expected);
    }
    
    // test case: current page out of bounds (too high)
    {
        std::string expected = "1 ... 21 22 23 24 25 26 27 28 29 [30]";
        std::string result = paginate(35, 30, 11); // Should correct to page 30
        LOG_INFO(std::format("Current page too high: Expected: '{}', Got: '{}'", expected, result));
        assert(result == expected);
    }
    
    // test case: current page out of bounds (too low)
    {
        std::string expected = "[1] 2 3 4 5 6 7 8 9 10 ... 30";
        std::string result = paginate(0, 30, 11); // Should correct to page 1
        LOG_INFO(std::format("Current page too low: Expected: '{}', Got: '{}'", expected, result));
        assert(result == expected);
    }
    
    // test case: very small max pages
    {
        std::string expected = "1 ... [5] ... 30";
        std::string result = paginate(5, 30, 3);
        LOG_INFO(std::format("Small max pages: '{}'", result));
        assert(result == expected);
    }
    
    // test case: large number of pages
    {
        std::string expected = "1 ... 46 47 48 49 [50] 51 52 53 54 ... 1000";
        std::string result = paginate(50, 1000, 11);
        LOG_INFO(std::format("Large dataset: '{}'", result));
        assert(result == expected);
    }
    
    LOG_INFO("All additional tests completed!");
}

int main() {
    testPaginationWithExamples();
    testAdditionalCases();
    return 0;
}
