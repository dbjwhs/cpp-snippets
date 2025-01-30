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

#### Key features:
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

### RAII (Resource Acquisition Is Initialization) read write locks

Let me explain the `ReadLock` and `WriteLock` RAII wrapper classes in detail.

These classes are examples of the RAII (Resource Acquisition Is Initialization) pattern where the constructor acquires a resource and the destructor releases it automatically. Here's a detailed breakdown:

```cpp
// Read Lock RAII Wrapper
class ReadLock {
    ReadersWriters& m_rw;    // reference to the ReadersWriters instance
public:
    // constructor - acquires the read lock
    ReadLock(ReadersWriters& rw) : m_rw(rw) { 
        m_rw.startRead(); 
    }
    
    // destructor - releases the read lock
    ~ReadLock() { 
        m_rw.endRead(); 
    }
    
    // delete copy operations to prevent multiple releases
    ReadLock(const ReadLock&) = delete;
    ReadLock& operator=(const ReadLock&) = delete;
};

// Write Lock RAII Wrapper
class WriteLock {
    ReadersWriters& m_rw;    // reference to the ReadersWriters instance
public:
    // constructor - acquires the write lock
    WriteLock(ReadersWriters& rw) : m_rw(rw) { 
        m_rw.startWrite(); 
    }
    
    // destructor - releases the write lock
    ~WriteLock() { 
        m_rw.endWrite(); 
    }
    
    // delete copy operations to prevent multiple releases
    WriteLock(const WriteLock&) = delete;
    WriteLock& operator=(const WriteLock&) = delete;
};
```

#### Key aspects of these classes:

1. **Scope-Based Resource Management**:
```cpp
void readResource() {
    ReadLock readLock(*this);  // lock acquired here
    // do work...
}  // lock automatically released here when readLock goes out of scope
```

2. **Exception Safety**:
```cpp
void readResource() {
    ReadLock readLock(*this);
    throw std::runtime_error("Something went wrong");  // lock still released
}
```

3. **Usage in the Reader-Writers Class**:
```cpp
void ReadersWriters::readResource() {
    ReadLock readLock(*this);  // creates temporary RAII object
    m_logger.print("Reading...");
    // lock is automatically released when readLock is destroyed
}
```

4. **Prevention of Common Errors**:
```cpp
ReadLock lock1(rw);
ReadLock lock2(lock1);  // Error: copy constructor is deleted
lock1 = lock2;          // Error: assignment operator is deleted
```

Benefits of this approach:

1. **Automatic Resource Management**:
   - No need to manually call `endRead()` or `endWrite()`
   - Resources are released in the correct order
   - No resource leaks even if exceptions occur

2. **Exception Safety**:
```cpp
void readResource() {
    ReadLock readLock(*this);
    // If any code here throws an exception:
    // - readLock's destructor will still be called
    // - The read lock will be properly released
    doSomethingThatMightThrow();
}
```

3. **Cleaner Interface**:
```cpp
// Without RAII
void oldWay() {
    startRead();
    try {
        // do work
        endRead();
    } catch (...) {
        endRead();
        throw;
    }
}

// With RAII
void newWay() {
    ReadLock lock(*this);
    // do work
}  // automatically released
```

4. **Local Class Definition**:
   In our implementation, we defined these classes locally within the methods:
```cpp
void readResource() {
    class ReadLock { /*...*/ } readLock(*this);
    // use the lock
}
```
This limits the scope of the lock classes to only where they're needed, preventing misuse elsewhere in the code.

#### Common Pitfalls to Avoid:

1. **Don't Store RAII Objects in Pointers**:
```cpp
// Bad - defeats the purpose of RAII
ReadLock* lock = new ReadLock(rw);  // Don't do this
delete lock;  // Manual cleanup required

// Good - automatic cleanup
ReadLock lock(rw);
```

2. **Don't Create Temporary Objects Without Names**:
```cpp
// Bad - lock is released immediately
ReadLock(rw);  // temporary object
doWork();      // no lock held here

// Good - lock held for entire scope
ReadLock lock(rw);
doWork();
```

3. **Don't Allow Copying**:
   That's why we delete copy operations to prevent multiple releases of the same lock:
```cpp
ReadLock(const ReadLock&) = delete;
ReadLock& operator=(const ReadLock&) = delete;
```

This RAII pattern is widely used in C++ for resource management, from file handles to memory allocation, and is considered one of the most important idioms in modern C++ programming.

## Usage Example of ReadersWriters class (simplified)

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
