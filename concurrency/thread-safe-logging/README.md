# Thread-Safe Logger

A modern C++ thread-safe logging implementation that supports multiple log levels, file and console output, and concurrent access from multiple threads.

## Features

- Thread-safe logging operations using mutex
- Three log levels: INFO, NORMAL, and CRITICAL
- CRITICAL logs redirected to stderr
- UTC timestamp for every log event
- Concurrent file and console output
- Flexible path configuration
- Variadic template support for versatile logging
- Modern C++17 features

## Requirements

- C++17 or later
- Support for filesystem library
- pthread support (for threading)

## Build Instructions

```bash
g++ -std=c++17 logger.cpp -pthread -o logger
```

## Usage

### Basic Usage

```cpp
// Create logger with default path
Logger logger;

// Or with custom path
Logger logger("path/to/logfile.log");

// Log messages
logger.log(LogLevel::INFO, "Simple message");
logger.log(LogLevel::NORMAL, "Process completed with status: ", true);
logger.log(LogLevel::CRITICAL, "Error code: ", 42);
```

### Log Levels

- **INFO**: General information messages (outputs to stdout and file)
- **NORMAL**: Standard operational messages (outputs to stdout and file)
- **CRITICAL**: Error and critical condition messages (outputs to stderr and file)

### Template-Based Design

The Logger class uses variadic templates for its log method, which provides several key advantages:

1. **Flexible Argument Types and Counts**
   ```cpp
   logger.log(LogLevel::INFO, "Simple message");  // one string
   logger.log(LogLevel::INFO, "User ", 42, " logged in");  // string + int + string
   logger.log(LogLevel::INFO, "Temperature: ", 23.5, "°C");  // string + double + string
   logger.log(LogLevel::INFO, "Status: ", true);  // string + boolean
   ```

2. **Advantages Over Alternative Approaches**
    - Type safety at compile time
    - No runtime overhead for type conversion
    - Clean, intuitive syntax
    - Supports any type that can be sent to a stream (<<)

3. **Comparison with Non-Template Approach**
   Without templates, you would need multiple overloads:
   ```cpp
   void log(LogLevel level, const std::string& msg);
   void log(LogLevel level, const std::string& msg, int value);
   void log(LogLevel level, const std::string& msg, double value);
   void log(LogLevel level, const std::string& msg, bool value);
   // ... and many more combinations
   ```

## Thread Safety

The logger uses std::mutex to ensure thread-safe operations:
- File access is protected
- Console output is synchronized
- UTC timestamp generation is thread-safe

## Error Handling

The logger throws std::runtime_error in the following cases:
- Invalid file path provided
- Unable to open log file
- File system permission issues

Example error handling:
```cpp
try {
    Logger logger("/invalid/path/log.txt");
} catch (const std::runtime_error& e) {
    std::cerr << "Logger initialization failed: " << e.what() << std::endl;
}
```

## Random Number Generation

The test suite uses modern C++11 random number facilities:
- std::random_device for entropy source
- std::mt19937 for high-quality random number generation
- std::uniform_int_distribution for proper distribution of delay values

```cpp
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<> delay_dist(1, 100);
```

## Best Practices

1. **Resource Management**
    - Use RAII for file handling
    - Proper mutex locking/unlocking
    - Exception-safe design

2. **Naming Conventions**
    - Member variables prefixed with m_
    - Clear, descriptive function names
    - C++ style comments

3. **Performance Considerations**
    - Minimal copying of log messages
    - Efficient thread synchronization
    - Proper file buffering

## Testing

The included test program demonstrates:
- Multiple concurrent threads (10-30) logging simultaneously
- All log levels
- Path validation
- Error handling
- Various data type logging

## License
This implementation is provided under the MIT License. See the LICENSE file for details.

## Contributing
Contributions are welcome! Please feel free to submit pull requests with improvements or bug fixes.
