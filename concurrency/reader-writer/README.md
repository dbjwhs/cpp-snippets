# C++ Readers-Writers Implementation

This project implements a thread-safe Readers-Writers synchronization mechanism in C++. The implementation features RAII (Resource Acquisition Is Initialization) pattern, thread-safe logging using a singleton pattern, and prevention of writer starvation.

## Overview

The Readers-Writers problem is a classic synchronization challenge where multiple threads need to access a shared resource, with some threads reading the resource and others writing to it. The key constraints are:
- Multiple readers can access the resource simultaneously
- Only one writer can access the resource at a time
- When a writer is writing, no readers can access the resource
- All console output must be thread-safe

## Features

- Thread-safe implementation using modern C++ constructs
- RAII pattern for automatic resource management
- Writer preference to prevent writer starvation
- Centralized thread-safe logging through singleton pattern
- Exception-safe design
- Comprehensive usage example

## Key Components

### Logger (Singleton)

A singleton class that provides thread-safe logging capabilities:
```cpp
class Logger {
private:
    static std::mutex m_mutex;
    std::ostream& m_stream;
    
    // private constructor for singleton
    explicit Logger() : m_stream(std::cout) {}
    
    // delete copy and move operations
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger(Logger&&) = delete;
    Logger& operator=(Logger&&) = delete;

public:
    static Logger& getInstance() {
        static Logger instance;
        return instance;
    }

    template<typename... Args>
    void print(Args&&... args);
};
```

Key features:
- Single instance ensures all logging goes through one mutex
- Thread-safe output to std::cout
- Cannot be copied or moved
- Variadic template support for flexible logging

### ReadersWriters Class

The main class implementing the synchronization mechanism:

#### Member Variables
- `m_mutex`: Protects access to shared state
- `m_readCV`: Condition variable for reader synchronization
- `m_writeCV`: Condition variable for writer synchronization
- `m_activeReaders`: Count of currently active readers
- `m_waitingReaders`: Count of readers waiting to acquire access
- `m_isWriting`: Flag indicating if a writer is currently active
- `m_waitingWriters`: Count of writers waiting to acquire access
- `m_sharedResource`: The protected resource

#### RAII Lock Classes

The implementation uses two RAII wrapper classes for automatic resource management:

##### ReadLock
```cpp
class ReadLock {
    ReadersWriters& m_rw;
public:
    ReadLock(ReadersWriters& rw) : m_rw(rw) { m_rw.startRead(); }
    ~ReadLock() { m_rw.endRead(); }
    // Copy operations deleted
};
```

##### WriteLock
```cpp
class WriteLock {
    ReadersWriters& m_rw;
public:
    WriteLock(ReadersWriters& rw) : m_rw(rw) { m_rw.startWrite(); }
    ~WriteLock() { m_rw.endWrite(); }
    // Copy operations deleted
};
```

Key benefits of the RAII approach:
- Automatic resource cleanup
- Exception safety
- Prevention of resource leaks
- Cleaner, more maintainable code

## Usage Example (simplified)

```cpp
int main() {
    ReadersWriters rw;
    std::vector<std::thread> threads;

    // Create reader threads
    for (int i = 0; i < 5; ++i) {
        threads.emplace_back([&rw, i]() {
            Logger::getInstance().print("Started reader thread ", i);
            rw.readResource();
            Logger::getInstance().print("Finished reader thread ", i);
        });
    }

    // Create writer threads
    for (int i = 0; i < 2; ++i) {
        threads.emplace_back([&rw, i]() {
            Logger::getInstance().print("Started writer thread ", i);
            rw.writeResource(i);
            Logger::getInstance().print("Finished writer thread ", i);
        });
    }

    // Join all threads
    for (auto& thread : threads) {
        thread.join();
    }
}
```

## Thread Safety Guarantees

- Safe concurrent access to the shared resource
- No reader starvation
- Prevention of writer starvation through writer preference
- Thread-safe logging through singleton logger
- Exception safety through RAII design
- All console output is synchronized through a single mutex

## Implementation Details

### Logging Operations
1. All logging is done through the Logger singleton
2. Single mutex protects all console output
3. Logging is thread-safe across all threads
4. Variadic template support for flexible message formatting

### Reader Operations
1. Readers request access through `startRead()`
2. Multiple readers can access simultaneously if no writer is active
3. Readers are blocked if there's an active or waiting writer
4. Access is automatically released when the `ReadLock` goes out of scope

### Writer Operations
1. Writers request access through `startWrite()`
2. Only one writer can access at a time
3. Writers have preference over readers to prevent writer starvation
4. Access is automatically released when the `WriteLock` goes out of scope

## Best Practices Demonstrated

1. **RAII Pattern**
   - Automatic resource management
   - Exception-safe design
   - Scope-based locking

2. **Thread Safety**
   - Proper mutex usage
   - Condition variable synchronization
   - Singleton pattern for logging

3. **Modern C++ Features**
   - Smart pointers
   - RAII wrappers
   - Template variadic arguments
   - Lambda expressions

## Requirements

- C++17 or later
- Support for std::thread
- Support for std::mutex and std::condition_variable

## Build Instructions

This is a header-only implementation that can be built with any modern C++ compiler:

```bash
g++ -std=c++17 main.cpp -pthread
```

## Notes

- The implementation gives preference to writers to prevent writer starvation
- All member variables follow the m_ prefix convention
- Comprehensive logging helps track the flow of operations
- Thread-safe logging is guaranteed through singleton pattern
- The code is designed to be both efficient and maintainable

## Design Decisions

### Why Singleton for Logging?
While there are other approaches to handle thread-safe logging (such as dependency injection or using established logging libraries), the singleton pattern was chosen for this implementation because:
- It ensures a single point of synchronization for all console output
- Provides a simple, globally accessible interface
- Guarantees that only one mutex is used for all logging operations
- Easy to understand and implement

## License
This implementation is provided under the MIT License. See the LICENSE file for details.

## Contributing
Contributions are welcome! Please feel free to submit pull requests with improvements or bug fixes.
