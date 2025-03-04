#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <chrono>
#include <functional>

// Another programming challenge from JG that I took too far, commiting here, but the large number
// is *too* large for reality

// Here is the problem statement:
//
// Given a string (of arbitrary length) containing digits from 2 - 9 inclusive, write a program to return all the possible letter combinations that the number could represent. The results can be in any order. The mapping of numbers to letters is as follows:
//
// 2: 'abc'
// 3: 'def'
// 4: 'ghi'
// 5: 'jkl'
// 6: 'mno'
// 7: 'pqrs'
// 8: 'tuv'
// 9: 'wxyz'
//
// Good luck!
//
// JG

class Solution {
private:
    // Map of a digit to corresponding letters
    const std::unordered_map<char, std::string> digitMap = {
        {'2', "abc"},
        {'3', "def"},
        {'4', "ghi"},
        {'5', "jkl"},
        {'6', "mno"},
        {'7', "pqrs"},
        {'8', "tuv"},
        {'9', "wxyz"}
        // Note: digits 0, 1 and non-digit characters are not mapped
    };

public:
    // Method to count total possible combinations without generating them
    [[nodiscard]] size_t countCombinations(const std::string& digits) const {
        size_t count = 1; // Start with 1 for empty string

        for (char digit : digits) {
            if (digitMap.contains(digit)) {
                count *= digitMap.at(digit).size();
            }
        }

        return count;
    }

    // Method to generate combinations up to a maximum count
    [[nodiscard]] std::vector<std::string> letterCombinationsLimited(const std::string& digits, const size_t maxCombinations = 1000000) const {
        // Handle empty input
        if (digits.empty()) {
            return {};
        }

        // Filter to keep only valid digits
        std::string validDigits;
        for (char digit : digits) {
            if (digitMap.contains(digit)) {
                validDigits.push_back(digit);
            }
        }

        // Check if total combinations would exceed the limit
        if (const size_t totalPossible = countCombinations(validDigits); totalPossible > maxCombinations) {
            std::cout << "Warning: Input would generate " << totalPossible
                     << " combinations. Limiting to " << maxCombinations << ".\n";
        }

        // Initialize a result with an empty string to start the combinations
        std::vector<std::string> result = {""};

        // For each digit in the valid input
        for (char digit : validDigits) {
            const std::string& letters = digitMap.at(digit);

            // Create a new result vector for this iteration
            std::vector<std::string> newResult;

            // Check if adding this digit would exceed the max combinations
            if (const size_t expectedSize = result.size() * letters.size(); expectedSize > maxCombinations) {
                // Limit the number of combinations to generate
                newResult.reserve(maxCombinations);
                const size_t combinationsToKeep = maxCombinations / letters.size();

                // Only use a subset of existing combinations
                for (size_t i = 0; i < combinationsToKeep && i < result.size(); ++i) {
                    for (const char letter : letters) {
                        newResult.push_back(result[i] + letter);
                        if (newResult.size() >= maxCombinations) {
                            break;
                        }
                    }
                    if (newResult.size() >= maxCombinations) {
                        break;
                    }
                }
            } else {
                // We can generate all combinations for this digit
                newResult.reserve(expectedSize);
                for (const std::string& combination : result) {
                    for (const char letter : letters) {
                        newResult.push_back(combination + letter);
                    }
                }
            }

            // Update result with the new combinations
            result = std::move(newResult);

            // If we've already reached the maximum, no need to continue
            if (result.size() >= maxCombinations) {
                result.resize(maxCombinations);
                break;
            }
        }

        return result;
    }

    // Iterative streaming version that processes combinations without storing them all
    void processLetterCombinations(const std::string& digits,
                                   const std::function<bool(const std::string&)>& processor,
                                   const size_t maxToProcess = std::numeric_limits<size_t>::max()) const {
        if (digits.empty()) {
            return;
        }

        // Filter valid digits
        std::string validDigits;
        for (char digit : digits) {
            if (digitMap.contains(digit)) {
                validDigits.push_back(digit);
            }
        }

        if (validDigits.empty()) {
            return;
        }

        // Initialize state for iterative approach
        std::vector<size_t> indices(validDigits.size(), 0);
        std::string currentCombination(validDigits.size(), ' ');
        size_t processedCount = 0;

        // Get the first combination
        for (size_t i = 0; i < validDigits.size(); ++i) {
            const std::string& letters = digitMap.at(validDigits[i]);
            currentCombination[i] = letters[0];
        }

        // Process combinations iteratively
        while (true) {
            // Process the current combination
            if (!processor(currentCombination) || ++processedCount >= maxToProcess) {
                break;
            }

            // Generate the next combination
            int digit = validDigits.size() - 1;
            while (digit >= 0) {
                const std::string& letters = digitMap.at(validDigits[digit]);
                if (++indices[digit] < letters.size()) {
                    // Move to the next letter for this digit
                    currentCombination[digit] = letters[indices[digit]];
                    break;
                } else {
                    // Reset this digit and carry over to the previous digit
                    indices[digit] = 0;
                    currentCombination[digit] = letters[0];
                    --digit;
                }
            }

            // If we've wrapped around, we're done
            if (digit < 0) {
                break;
            }
        }
    }
        // Original method (keep for compatibility but use with caution for large inputs)
        [[nodiscard]] std::vector<std::string> letterCombinations(const std::string& digits) const {

        // Use the limited version with a default limit
        return letterCombinationsLimited(digits, 1000000);
    }
};

// Test function that runs the solution with different inputs
void runTests() {
    Solution solution;

    // Test case 1: Single digit
    {
        std::string digits = "2";
        std::vector<std::string> expected = {"a", "b", "c"};
        std::vector<std::string> result = solution.letterCombinations(digits);
        std::cout << "Test 1: " << (result == expected ? "PASS" : "FAIL") << std::endl;
    }

    // Test case 2: Two digits
    {
        std::string digits = "23";
        std::vector<std::string> expected = {"ad", "ae", "af", "bd", "be", "bf", "cd", "ce", "cf"};
        std::vector<std::string> result = solution.letterCombinations(digits);
        std::cout << "Test 2: " << (result == expected ? "PASS" : "FAIL") << std::endl;
    }

    // Test case 3: Empty input
    {
        std::string digits{};
        std::vector<std::string> expected = {};
        std::vector<std::string> result = solution.letterCombinations(digits);
        std::cout << "Test 3: " << (result == expected ? "PASS" : "FAIL") << std::endl;
    }

    // Test case 4: Three digits
    {
        std::string digits = "234";
        auto result = solution.letterCombinations(digits);
        std::cout << "Test 4: Size = " << result.size() << " (should be 3*3*3=27)" << std::endl;
    }

    // Performance test with a medium-sized input
    {
        std::string digits = "23456789";
        auto start = std::chrono::high_resolution_clock::now();
        auto result = solution.letterCombinationsLimited(digits);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> duration = end - start;

        std::cout << "Performance test: " << digits << std::endl;
        std::cout << "  - Combinations generated: " << result.size() << std::endl;
        std::cout << "  - Time taken: " << duration.count() << " ms" << std::endl;
    }

    // Test with extremely large input - using count only
    {
        std::string digits = "12-92929-2288765-43534-272";

        // ### this is unsolvable currently, how to efficiently handle
        // huge numbers

        // First, count the total possible combinations
        auto start = std::chrono::high_resolution_clock::now();
        size_t totalCombinations = solution.countCombinations(digits);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> duration = end - start;

        std::cout << "Large test count: " << digits << std::endl;
        std::cout << "  - Total possible combinations: " << totalCombinations << std::endl;
        std::cout << "  - Count time: " << duration.count() << " ms" << std::endl;

        // Generate a limited number of combinations
        start = std::chrono::high_resolution_clock::now();
        auto result = solution.letterCombinationsLimited(digits, 1000);
        end = std::chrono::high_resolution_clock::now();
        duration = end - start;

        std::cout << "  - Limited combinations generated: " << result.size() << std::endl;
        std::cout << "  - Limited generation time: " << duration.count() << " ms" << std::endl;
    }

    // Test with streaming API - count combinations without storing them
    {
        std::string digits = "12-92929-2288765-43534-272";

        std::cout << "Streaming test: " << digits << std::endl;

        // Count combinations via streaming API
        size_t count = 0;
        auto start = std::chrono::high_resolution_clock::now();

        // Only process first 1000 combinations
        constexpr size_t maxToProcess = 1000000000;
        solution.processLetterCombinations(digits, [&count](const std::string& combo) {
            count++;
            if (count % 10000000 == 0) {
                std::cout << "  - Last generated for: " << count << " | " << combo << std::endl;
            }
            return true; // Continue processing
        }, maxToProcess);

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> duration = end - start;

        std::cout << "  - Combinations processed: " << count << std::endl;
        std::cout << "  - Processing time: " << duration.count() << " ms" << std::endl;
    }
}

#define NO_INPUT

int main() {
#ifdef NO_INPUT

    runTests();

#else
    // Interactive mode with large input handling
    std::string digits;
    std::cout << "\nEnter digits (2-9, can include other characters): ";
    std::cin >> digits;

    Solution solution;

    // First count the total combinations
    size_t totalCombinations = solution.countCombinations(digits);
    std::cout << "This input would generate " << totalCombinations << " combinations total." << std::endl;

    // Ask user how many combinations they want to see
    size_t limit;
    std::cout << "How many combinations do you want to generate? (recommended: < 1,000,000): ";
    std::cin >> limit;

    if (limit > 10000000) {
        std::cout << "Warning: Generating over 10M combinations may use significant memory." << std::endl;
        std::cout << "Continue? (y/n): ";
        char confirm;
        std::cin >> confirm;
        if (confirm != 'y' && confirm != 'Y') {
            return 0;
        }
    }

    auto start = std::chrono::high_resolution_clock::now();
    std::vector<std::string> combinations = solution.letterCombinationsLimited(digits, limit);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = end - start;

    std::cout << "Generated " << combinations.size() << " combinations in "
              << duration.count() << " ms" << std::endl;

    // Print combinations (limited to first 100 if there are too many)
    int count = 0;
    const int maxToPrint = 100;
    for (const auto& combo : combinations) {
        std::cout << combo << " ";
        if (++count >= maxToPrint && combinations.size() > maxToPrint) {
            std::cout << "... (and " << (combinations.size() - maxToPrint) << " more)";
            break;
        }
    }
    std::cout << std::endl;
#endif
    return 0;
}