// MIT License
// Copyright (c) 2025 dbjwhs

#include <iostream>
#include <string>
#include <algorithm>

class StringDemoSimple {
private:
    std::string text;

public:
    // constructor with utf-8 example
    StringDemoSimple() : text("Hello! こんにちは 你好 안녕하세요") {}

    // basic string operations
    void basicOperations() {
        std::cout << "Length: " << text.length() << std::endl;
        std::cout << "Size: " << text.size() << std::endl;
        std::cout << "Capacity: " << text.capacity() << std::endl;
        std::cout << "Empty? " << (text.empty() ? "Yes" : "No") << std::endl;
    }

    // substring and append operations
    void substringDemo() {
        // get first word
        size_t firstSpace = text.find(' ');
        std::string firstWord = text.substr(0, firstSpace);
        std::cout << "First word: " << firstWord << std::endl;

        // append operation
        text.append(" 🌟"); // unicode emoji
        std::cout << "After append: " << text << std::endl;
    }

    // modification operations
    void modificationDemo() {
        // insert
        text.insert(0, "Start: ");
        std::cout << "After insert: " << text << std::endl;

        // replace
        text.replace(0, 6, "Begin:");
        std::cout << "After replace: " << text << std::endl;

        // erase
        text.erase(0, 6);
        std::cout << "After erase: " << text << std::endl;
    }

    // search operations
    void searchDemo() {
        // find
        size_t pos = text.find("こんにちは");
        if (pos != std::string::npos) {
            std::cout << "Found 'こんにちは' at position: " << pos << std::endl;
        }

        // find last
        pos = text.find_last_of("🌟");
        if (pos != std::string::npos) {
            std::cout << "Last emoji position: " << pos << std::endl;
        }
    }

    // comparison operations
    void comparisonDemo() {
        std::string other = "Hello! こんにちは 你好 안녕하세요";
        std::cout << "Compare result: " << text.compare(other) << std::endl;
        std::cout << "Starts with 'Hello'? " << (text.starts_with("Hello") ? "Yes" : "No") << std::endl;
        std::cout << "Ends with '🌟'? " << (text.ends_with("🌟") ? "Yes" : "No") << std::endl;
    }

    // case conversion (note: only affects ascii characters)
    void caseDemo() {
        std::string ascii = "Hello, World!";
        std::transform(ascii.begin(), ascii.end(), ascii.begin(), ::toupper);
        std::cout << "Uppercase: " << ascii << std::endl;

        std::transform(ascii.begin(), ascii.end(), ascii.begin(), ::tolower);
        std::cout << "Lowercase: " << ascii << std::endl;
    }

    // iterator
    void iteratorDemo() {
        std::cout << "Characters: ";
        for (char c : text) {
            std::cout << c << ' ';
        }
        std::cout << std::endl;

        // reverse iteration
        std::cout << "Reverse: ";
        for (auto it = text.rbegin(); it != text.rend(); ++it) {
            std::cout << *it;
        }
        std::cout << std::endl;
    }

    // get raw data
    void rawDataDemo() {
        const char* cstr = text.c_str();
        std::cout << "C-style string: " << cstr << std::endl;

        // get raw bytes
        const unsigned char* raw = reinterpret_cast<const unsigned char*>(text.data());
        std::cout << "First few bytes: ";
        for (size_t i = 0; i < 10; ++i) {
            std::cout << std::hex << static_cast<int>(raw[i]) << " ";
        }
        std::cout << std::dec << std::endl;
    }
};

// example usage
int main() {
    StringDemoSimple demo;

    std::cout << "=== Basic Operations ===" << std::endl;
    demo.basicOperations();

    std::cout << "\n=== Substring Demo ===" << std::endl;
    demo.substringDemo();

    std::cout << "\n=== Modification Demo ===" << std::endl;
    demo.modificationDemo();

    std::cout << "\n=== Search Demo ===" << std::endl;
    demo.searchDemo();

    std::cout << "\n=== Comparison Demo ===" << std::endl;
    demo.comparisonDemo();

    std::cout << "\n=== Case Demo ===" << std::endl;
    demo.caseDemo();

    std::cout << "\n=== Iterator Demo ===" << std::endl;
    demo.iteratorDemo();

    std::cout << "\n=== Raw Data Demo ===" << std::endl;
    demo.rawDataDemo();

    return 0;
}
