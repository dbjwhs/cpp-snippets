# C++ Regular Expression (Regex) Library

Regular expressions (regex) are powerful pattern matching tools that have been an integral part of text processing since the
1950s. The concept was first formalized by mathematician Stephen Cole Kleene as part of his work on regular language theory.
They were later implemented in early text editors like QED and ed, and became widely popular through tools like grep in Unix.
C++'s implementation, introduced in the C++11 standard, provides a standardized way to perform pattern matching operations that
previously required external libraries. The std::regex library follows the ECMAScript syntax by default, though it supports
other grammar variants. This implementation offers a compromise between the performance of specialized libraries and the
convenience of having regex capabilities built into the standard library.

Regular expressions address numerous common programming challenges related to text processing and validation. They excel at tasks
such as data extraction, format validation, search and replace operations, and tokenization. By using regex, developers can
replace dozens of lines of manual string parsing code with concise pattern definitions. This is particularly valuable for
validating inputs like email addresses, phone numbers, dates, and URLs. It's also essential for text processing tasks like
extracting specific data from structured text (logs, CSV data, etc.), cleaning text by removing unwanted characters, and
transforming text from one format to another. The C++ implementation specifically helps standardize code that previously might
have relied on platform-specific or third-party libraries, improving portability and reducing external dependencies.

## Key Features

- Standard C++ library implementation (C++11 and newer)
- ECMAScript syntax by default, with support for other grammar variants
- Comprehensive pattern matching capabilities
- Capturing groups for extracting matched portions
- Search, match, and replace operations
- Support for different regex flags (case sensitivity, etc.)

## Basic Usage

```cpp
#include <iostream>
#include <string>
#include <regex>

int main() {
    std::string text = "Email me at example@email.com";
    std::regex email_pattern(R"(\b[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Za-z]{2,}\b)");
    
    std::smatch matches;
    if(std::regex_search(text, matches, email_pattern)) {
        std::cout << "Found email: " << matches[0] << std::endl;
    }
    
    return 0;
}
```

## Examples

### Example 1: Simple Pattern Matching

```cpp
std::string text = "The quick brown fox jumps over the lazy dog";
std::regex pattern("fox");

bool contains_fox = std::regex_search(text, pattern); // Returns true
```

### Example 2: Using Capture Groups to Extract Information

```cpp
std::string date = "Date: 2023-11-15";
std::regex date_pattern(R"(Date: (\d{4})-(\d{2})-(\d{2}))");

std::smatch matches;
if (std::regex_search(date, matches, date_pattern)) {
    std::string year = matches[1]; // "2023"
    std::string month = matches[2]; // "11"
    std::string day = matches[3]; // "15"
}
```

### Example 3: Text Replacement

```cpp
std::string text = "Contact us at secret@company.com";
std::regex email_pattern(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})");

std::string redacted = std::regex_replace(text, email_pattern, "[EMAIL REDACTED]");
// Result: "Contact us at [EMAIL REDACTED]"
```

### Example 4: Finding All Matches

```cpp
std::string text = "Numbers: 42, 17, 99";
std::regex number_pattern(R"(\d+)");

std::sregex_iterator it(text.begin(), text.end(), number_pattern);
std::sregex_iterator end;

std::vector<int> numbers;
while (it != end) {
    numbers.push_back(std::stoi((*it)[0]));
    ++it;
}
// numbers = {42, 17, 99}
```

## Best Practices

1. **Use Raw String Literals**: Use the `R"(...)"` syntax to avoid excessive escaping of backslashes.
2. **Precompile Patterns**: Compile regex patterns once and reuse them to avoid repeated compilation.
3. **Consider Performance**: Regular expressions can be computationally expensive for complex patterns or large inputs.
4. **Test Edge Cases**: Always test your regex patterns with a variety of inputs, including edge cases.
5. **Handle Errors**: Use try-catch blocks to handle potential std::regex_error exceptions.

## Common Pitfalls

1. **Greedy vs. Non-Greedy Matching**: By default, quantifiers are greedy. Use `*?`, `+?`, etc. for non-greedy matching.
2. **Capturing Everything**: Avoid capturing more than needed with `(?:...)` non-capturing groups.
3. **Overlooking Escape Sequences**: Remember to escape special characters like `.`, `*`, `+`, etc.
4. **Complexity Limitations**: Very complex patterns might hit implementation limits.
5. **Unicode Support**: The C++ regex library has limited Unicode support compared to some specialized libraries.

## Advanced Features

- Lookahead and lookbehind assertions
- Conditional expressions
- Back-references
- Unicode character properties (limited support)

## Reference Books

1. **Mastering Regular Expressions** by Jeffrey Friedl
2. **Regular Expressions Cookbook** by Jan Goyvaerts and Steven Levithan
3. **C++17 Standard Library Quick Reference** by Peter Van Weert and Marc Gregoire
4. **Effective Modern C++** by Scott Meyers (includes some regex best practices)
5. **The C++ Programming Language (4th Edition)** by Bjarne Stroustrup

## Alternative Libraries

If you need more advanced features or better performance, consider:
- Boost.Regex
- RE2 (Google's regular expression library)
- PCRE (Perl Compatible Regular Expressions)

## License
This code is provided under the MIT License. Feel free to use, modify, and distribute as needed.

## Contributing
Contributions are welcome! Please feel free to submit a Pull Request.
