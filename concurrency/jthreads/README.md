# Cooperative Thread Cancellation with std::jthread

The std::jthread class, introduced in C++20, represents a significant evolution in C++'s thread management capabilities.
It builds upon the foundation laid by std::thread (C++11) by addressing two critical issues: automatic resource cleanup
and cooperative cancellation. This pattern emerged from years of experience with thread management in C++ and other
languages, particularly influenced by Java's interrupt mechanism and Python's threading module. The design specifically
targets the common pitfalls of thread management: resource leaks from forgotten join() calls and the challenges of gracefully
shutting down long-running threads. The pattern was proposed by Nicolai Josuttis in 2018 (P0660R10) and became part of the
C++20 standard, marking a significant improvement in C++'s concurrency toolkit.

## Availability on OSX and Compiler Support

Despite being part of the C++20 standard, std::jthread is not currently available in Apple's implementation of the C++
standard library on OSX. This can be verified with the following test program:

```cpp
#include <iostream>

int main() {
    std::cout << "C++ Standard: " << __cplusplus << "\n";
    
    #if defined(__cpp_lib_jthread)
        std::cout << "jthread is supported (version " << __cpp_lib_jthread << ")\n";
    #else
        std::cout << "jthread is not supported\n";
    #endif
    
    return 0;
}
```

When compiled on OSX, this program outputs:
```
C++ Standard: 202002
jthread is not supported
```

This output shows that despite using C++20 (indicated by 202002), the jthread feature is not implemented in the standard
library. This is a known limitation of Apple's toolchain and requires either using alternative compilers (like GCC via Homebrew)
or implementing the functionality manually.

## Use Cases and Problem Solutions

The jthread pattern addresses several common threading scenarios and challenges:

1. Resource Management
    - Automatically joins the thread on destruction
    - Prevents resource leaks from forgotten join() calls
    - Eliminates the "terminate on destruction" problem of std::thread

2. Graceful Shutdown
    - Provides a standardized cancellation mechanism
    - Allows threads to clean up resources before terminating
    - Prevents the need for volatile flags or complex signaling mechanisms

3. Long-running Operations
    - Background processing threads
    - Worker pools
    - Event monitoring systems
    - Service threads that need clean shutdown capability

4. Exception Safety
    - Ensures proper thread cleanup even when exceptions occur
    - Maintains RAII principles in concurrent code

## Examples and Usage

### Basic Usage
```cpp
std::jthread worker([](std::stop_token stoken) {
    while (!stoken.stop_requested()) {
        // Do work
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
});
// No join() needed - happens automatically in destructor
```

### Worker Pool Pattern
```cpp
class WorkerPool {
    std::vector<std::jthread> workers;
public:
    WorkerPool(size_t count) {
        for (size_t i = 0; i < count; ++i) {
            workers.emplace_back([](std::stop_token stoken) {
                while (!stoken.stop_requested()) {
                    // Process work items
                }
            });
        }
    }
    // No explicit cleanup needed
};
```

### Good Practices
- Always check stop_token in loop conditions
- Use stop_token instead of volatile flags
- Leverage RAII for resource management
- Consider cooperative cancellation points in long operations

### Bad Practices
- Ignoring stop_requested() signals
- Using external stop flags with jthread
- Manual joining of jthreads
- Copying jthreads (they're move-only)

## Further Reading

### Books
1. "C++20 - The Complete Guide" by Nicolai Josuttis
    - Comprehensive coverage of jthread and modern C++ threading
    - Includes real-world examples and best practices

2. "Modern C++ Programming Cookbook - Second Edition" by Marius Bancila
    - Practical recipes for using jthread and other C++20 features
    - Covers migration strategies from thread to jthread

3. "C++ Concurrency in Action - Second Edition" by Anthony Williams
    - Foundational concepts for concurrent programming
    - Though published before C++20, covers the concepts that led to jthread

### Technical Papers
1. P0660R10: "Stop Token and Joining Thread" by Nicolai Josuttis
    - Original proposal for jthread
    - Detailed design rationale and use cases

2. P1287R0: "thread::join_for()" by Nicolai Josuttis
    - Related proposal for timed joining
    - Provides context for jthread design decisions

### Online Resources
- CppReference: https://en.cppreference.com/w/cpp/thread/jthread
- ISO C++ Committee Papers: https://wg21.link/p0660
- Herb Sutter's Blog: https://herbsutter.com/
    - Various articles on C++ concurrency best practices

## License

This code is provided under the MIT License. Feel free to use, modify, and distribute as needed.

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.
