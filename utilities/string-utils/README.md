# C++ String Demo Class

This repository contains a basic demonstration class for C++ string manipulation, focusing on `std::string` functionality including UTF-8 and Unicode support. As time goes on I will add more examples of functionality that I have used in the past, for for the time being this is a good starting point.

## Overview

The `StringDemo` class serves as an educational tool to showcase various string operations and manipulations in modern C++. It demonstrates both basic and advanced features of the `std::string` class, with special attention to Unicode and UTF-8 handling.

## Features

### Basic String Operations
- Length and size querying
- Capacity checking
- Empty state verification

### Text Manipulation
- Substring extraction
- String concatenation and appending
- Text insertion and replacement
- Character/substring erasure

### Unicode Support
- UTF-8 text handling
- Multi-language text examples:
    - Japanese (こんにちは)
    - Chinese (你好)
    - Korean (안녕하세요)
- Unicode emoji support (🌟)

### Search Operations
- Forward and backward text searching
- Position-based character/substring location
- Unicode character searching

### String Comparison
- Full string comparison
- Prefix checking (starts_with)
- Suffix checking (ends_with)

### Case Manipulation
- ASCII text case conversion
- Uppercase and lowercase transformation
- Note: Case conversion limitations with Unicode text

### Iterator Operations
- Forward iteration
- Reverse iteration
- Character-by-character processing

### Raw Data Access
- C-style string conversion
- Raw byte access
- Memory representation inspection

## Usage Example

```cpp
int main() {
    StringDemo demo;
    
    // Basic operations demonstration
    demo.basicOperations();
    
    // String modification example
    demo.modificationDemo();
    
    // Unicode search demonstration
    demo.searchDemo();
    
    // ... other operations
}
```

## Important Notes

### UTF-8 Handling
1. The `length()` and `size()` methods return the number of bytes in the string, not the number of characters. This is important when working with Unicode text.
2. String iteration processes individual bytes, not characters. Special handling may be needed for proper Unicode character iteration.
3. Case conversion operations are reliable only for ASCII characters.

### Memory Management
- The class uses `std::string`, which handles memory management automatically
- No manual memory management is required
- The class is exception-safe

### Performance Considerations
- `std::string` allocates memory from the heap (unless it is a very small string), and though not a big deal is a performance consideration for real world applications.
- String operations like concatenation and substring creation create new string objects
- For **performance-critical** applications, consider using string_view for non-modifying operations
- Large-scale string modifications might benefit from using a **string builder pattern**

## Requirements

- C++20 or later (for `starts_with()` and `ends_with()` support)
- UTF-8 compatible terminal for proper display of Unicode characters
- Compiler with proper Unicode support

## Building

The class can be compiled with any modern C++ compiler:

```bash
g++ -std=c++20 string_demo.cpp -o string_demo
```

## License

This code is provided under the MIT License. Feel free to use and modify it for educational purposes.

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request. For major changes, please open an issue first to discuss what you would like to change.

## Contact

If you have any questions or suggestions about this code, please open an issue in the repository.
