// MIT License
// Copyright (c) 2025 dbjwhs

#include <vector>
#include <string>
#include <algorithm>
#include <cassert>
#include <stdexcept>
#include <format>
#include "../../../headers/project_utils.hpp"

// negative space programming is a coding philosophy that emerged from multiple disciplines:
// - visual arts (negative space concept, early 1900s)
// - defensive programming (1970s)
// - design by contract (bertrand meyer, 1986)
//
// key principles:
// 1. define behavior by restrictions rather than permissions
// 2. explicitly handle edge cases and failures
// 3. focus on what cannot happen rather than what can
// 4. establish clear boundaries through constraints

// safestring class demonstrates negative space programming principles
// by explicitly defining what cannot happen rather than what can
class SafeString {
private:
    std::string m_value;
    size_t m_maxLength;
    std::vector<char> m_forbiddenChars;

public:
    // constructor defines what's not allowed rather than what is
    explicit SafeString(const size_t maxLength = 100) : m_maxLength(maxLength) {
        // explicitly forbid control characters
        for (char bad_char = 0; bad_char < 32; ++bad_char) {
            m_forbiddenChars.push_back(bad_char);
        }
        m_forbiddenChars.push_back(127); // del character

        // ### others?
    }

    // set string value with negative space validation
    void setValue(const std::string& value) {
        // check what's not allowed first
        if (value.empty()) {
            // make this info since we have negative tests, but should be error/critical
            Logger::getInstance().log(LogLevel::INFO, std::format("Empty string not allowed"));
            throw std::invalid_argument("empty string not allowed");
        }

        if (value.length() > m_maxLength) {
            // make this info since we have negative tests, but should be error/critical
            Logger::getInstance().log(LogLevel::INFO,
                std::format("String exceeds maximum length of {}", m_maxLength));
            throw std::invalid_argument("string length exceeds maximum");
        }

        // check for forbidden characters
        for (const char c : value) {
            if (std::find(m_forbiddenChars.begin(), m_forbiddenChars.end(), c) != m_forbiddenChars.end()) {
                // make this info since we have negative tests, but should be error/critical
                Logger::getInstance().log(LogLevel::INFO,
                    std::format("String contains forbidden character: {}", static_cast<int>(c)));
                throw std::invalid_argument("string contains forbidden character");
            }
        }

        // if we haven't thrown by now, the string is valid
        m_value = value;
        Logger::getInstance().log(LogLevel::INFO, 
            std::format("Successfully set string value: {}", m_value));
    }

    // add a new forbidden character
    void addForbiddenChar(const char bad_char) {
        // check if character is already forbidden
        if (std::find(m_forbiddenChars.begin(), m_forbiddenChars.end(), bad_char) != m_forbiddenChars.end()) {
            Logger::getInstance().log(LogLevel::ERROR,
                std::format("Character {} is already forbidden", static_cast<int>(bad_char)));
            return;
        }

        m_forbiddenChars.push_back(bad_char);
        Logger::getInstance().log(LogLevel::INFO, 
            std::format("Added forbidden character: {}", static_cast<int>(bad_char)));

        // if the current value contains the newly forbidden character, clear it
        if (m_value.find(bad_char) != std::string::npos) {
            m_value.clear();
            Logger::getInstance().log(LogLevel::INFO, 
                "Cleared current value due to new forbidden character");
        }
    }

    // getter for the string value
    [[nodiscard]] std::string getValue() const {
        return m_value;
    }
};

int main() {
    Logger::getInstance().log(LogLevel::INFO, "Starting SafeString tests");

    // test case 1: basic valid string
    try {
        SafeString ss;
        ss.setValue("Hello, World!");
        assert(ss.getValue() == "Hello, World!");
        Logger::getInstance().log(LogLevel::INFO, "Test case 1 passed: Basic valid string");
    } catch (const std::exception& e) {
        Logger::getInstance().log(LogLevel::ERROR, 
            std::format("Test case 1 failed: {}", e.what()));
        return 1;
    }

    // test case 2: empty string (should fail)
    try {
        SafeString ss;
        ss.setValue("");
        Logger::getInstance().log(LogLevel::ERROR, "Test case 2 failed: Empty string was accepted");
        return 1;
    } catch (const std::invalid_argument&) {
        Logger::getInstance().log(LogLevel::INFO, "Test case 2 passed: Empty string rejected");
    }

    // test case 3: string with control character (should fail)
    try {
        SafeString ss;
        ss.setValue("Hello\nWorld");
        Logger::getInstance().log(LogLevel::ERROR, 
            "Test case 3 failed: Control character was accepted");
        return 1;
    } catch (const std::invalid_argument&) {
        Logger::getInstance().log(LogLevel::INFO, 
            "Test case 3 passed: Control character rejected");
    }

    // test case 4: string exceeding maximum length
    try {
        SafeString ss(5);
        ss.setValue("Too long string");
        Logger::getInstance().log(LogLevel::ERROR, 
            "Test case 4 failed: Oversized string was accepted");
        return 1;
    } catch (const std::invalid_argument&) {
        Logger::getInstance().log(LogLevel::INFO, "Test case 4 passed: Oversized string rejected");
    }

    // test case 5: adding new forbidden character
    try {
        SafeString ss;
        ss.setValue("Test!");
        ss.addForbiddenChar('!');
        assert(ss.getValue().empty());  // value should be cleared
        Logger::getInstance().log(LogLevel::INFO, 
            "Test case 5 passed: String cleared after adding forbidden character");
    } catch (const std::exception& e) {
        Logger::getInstance().log(LogLevel::ERROR, 
            std::format("Test case 5 failed: {}", e.what()));
        return 1;
    }

    Logger::getInstance().log(LogLevel::INFO, "All tests completed successfully");
    return 0;
}
