# Directory Scanner

A platform-agnostic C++ directory scanner that uses an iterative approach with a queue to traverse directories efficiently. This implementation uses modern C++ features and provides detailed file information including permissions, timestamps, and ownership details on POSIX systems.

## Features

- Platform agnostic using `std::filesystem`
- Iterative scanning using a queue-based approach
- Detailed file information including:
    - File size
    - Permissions (POSIX-style rwxrwxrwx)
    - Last modification time
    - Owner and group information (POSIX systems only)
    - Symbolic link status
- Pattern matching support (glob patterns like "*.cpp")
- Custom filtering capabilities
- Exception-safe operations

Here's a comparison of iterative versus recursive directory scanning approaches:

### Advantages of Iterative (Queue-Based) Scanning

1. **Prevents Stack Overflow**
  - Recursive approaches add a new stack frame for each directory level
  - Deep directory structures can exceed the stack size limit (typically a few MB)
  - Stack overflow causes program crashes that can't be caught with try-catch
  - Iterative approach uses heap memory via queue, which has much larger limits

2. **Memory Efficiency**
  - Memory usage grows and shrinks based on actual needs
  - Only stores directory paths needed for pending processing
  - Memory is automatically freed as directories are processed
  - More predictable memory usage patterns

3. **Better Control**
  - Easier to pause/resume scanning
  - Can implement progress reporting more easily
  - Simple to add rate limiting or throttling
  - Can cancel operation at any queue step

4. **Performance**
  - No overhead from function call stack management
  - Better cache utilization due to localized memory access
  - More efficient for very wide directory trees
  - Lower context switching overhead

5. **Error Handling**
  - More robust error recovery
  - Can skip problematic directories without affecting the rest of the scan
  - Easier to implement retry logic
  - Better transaction control

While recursive implementations might appear more elegant or intuitive, the iterative approach is generally more robust and production-ready for filesystem operations.

## Implementation Details

The scanner uses a queue-based approach to traverse directories:
- Starts with the root directory in a queue
- For each directory in the queue:
    1. Processes all immediate files and subdirectories
    2. Adds any found subdirectories to the queue
    3. Continues until the queue is empty
       This approach provides efficient memory usage and avoids stack limitations that can occur with recursive implementations.

## Requirements

- C++17 or later
- CMake 3.10 or later
- A C++ compiler with filesystem support

## Building

```bash
mkdir build
cd build
cmake ..
make
```

## Usage

### Basic Usage

```cpp
#include "dir_scanner.hpp"

int main() {
    try {
        // Create a scanner for the current directory
        DirectoryScanner scanner(".");

        // Get all files and directories
        for (const auto& info : scanner.scan_detailed()) {
            std::cout << "Path: " << info.m_path << "\n";
            std::cout << "Size: " << info.m_file_size << " bytes\n";
            std::cout << "Permissions: " << info.get_permission_string() << "\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}
```

### Pattern Matching

```cpp
// Find all C++ source files
auto cpp_files = scanner.scan_by_pattern_detailed("*.cpp");

// Find all header files
auto header_files = scanner.scan_by_pattern_detailed("*.h*");

// Find all text files
auto text_files = scanner.scan_by_pattern_detailed("*.txt");
```

### Custom Filtering

```cpp
// Find large files (> 1MB)
auto large_files = scanner.scan_with_filter([](const FileInfo& info) {
    return !info.m_is_directory && info.m_file_size > 1024 * 1024;
});

// Find empty directories
auto empty_dirs = scanner.scan_with_filter([](const FileInfo& info) {
    return info.m_is_directory && /* check if empty */;
});
```

## API Reference

### Class: DirectoryScanner

#### Constructor
```cpp
explicit DirectoryScanner(const fs::path& root_path);
```
Creates a scanner for the specified directory path.

#### Methods
- `scan_detailed()`: Returns detailed information about all files and directories
- `scan_files_detailed()`: Returns information about files only
- `scan_directories_detailed()`: Returns information about directories only
- `scan_by_pattern_detailed(const std::string& pattern)`: Returns files matching the pattern
- `scan_with_filter(const std::function<bool(const FileInfo&)>& filter)`: Returns items matching the filter

### Struct: FileInfo

Contains detailed information about a file or directory:

- `m_path`: Full path to the item
- `m_file_size`: Size in bytes (0 for directories)
- `m_permissions`: File permissions
- `m_last_write_time`: Last modification time
- `m_is_directory`: Whether the item is a directory
- `m_is_symlink`: Whether the item is a symbolic link
- `m_owner`: Owner name (POSIX only)
- `m_group`: Group name (POSIX only)

## Error Handling

The scanner uses exception handling for error cases:
- Throws `std::runtime_error` if the root path doesn't exist or isn't a directory
- Skips inaccessible directories during scanning
- Uses `std::error_code` for non-throwing filesystem operations

## License

MIT License - See LICENSE file for details.

## Contributing

Contributions are welcome! Please feel free to submit pull requests with improvements or bug fixes.

