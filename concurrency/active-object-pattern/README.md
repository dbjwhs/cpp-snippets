# Active Object Pattern Implementation

## Problem Statement
The Active Object Pattern solves several critical concurrency challenges in software design:
1. Thread safety in object-oriented systems
2. Asynchronous method execution
3. Decoupling method invocation from method execution
4. Managing method execution priorities
5. Handling timeouts and cancellations in asynchronous operations

## Solution Overview
This implementation provides a thread-safe way to execute operations asynchronously with support for:
- Priority-based execution
- Operation timeouts
- Cancellation
- Error handling
- Method queuing

## Key Components

### MethodResult Template Class
```cpp
template<typename T>
class MethodResult {
    // Implementation details...
};
```

#### Future/Promise Objects
std::future and std::promise provide a mechanism for asynchronous operations to return values. They work together as a producer-consumer pair:

1. std::promise
    - Acts as the producer of a value
    - Owned by the thread performing the computation
    - Can set a value or exception
    - Each promise is associated with a future
   ```cpp
   std::promise<int> promise;
   promise.set_value(42);  // Sets the result
   ```

2. std::future
    - Acts as the consumer of a value
    - Owned by the thread waiting for the result
    - Provides methods to check status and retrieve value
    - Can be used to implement timeouts
   ```cpp
   std::future<int> future = promise.get_future();
   int result = future.get();  // Waits for and retrieves the result
   ```

3. How They Work Together
   ```cpp
   void producer(std::promise<int> promise) {
       // Do some work...
       promise.set_value(42);  // Result is ready
   }
   
   void consumer(std::future<int> future) {
       int value = future.get();  // Waits until value is set
   }
   ```

4. Benefits in Active Object Pattern
    - Asynchronous result handling
    - Thread-safe data transfer
    - Exception propagation
    - Timeout support
    - One-time value transfer guarantee

5. Common Operations
    - future.get(): Retrieves result (waits if necessary)
    - future.wait(): Waits for result without retrieving
    - future.wait_for(): Waits with timeout
    - promise.set_value(): Sets the result
    - promise.set_exception(): Sets an error

The MethodResult class uses templates to provide type-safe future/promise functionality. Why use templates?
1. Type Safety: Compile-time type checking prevents runtime errors
2. Flexibility: Can handle different return types (int, double, custom objects)
3. Code Reuse: Avoid writing separate classes for different types
4. Performance: No type casting or runtime type checking needed

Example usages:
```cpp
MethodResult<int> numericResult;      // For integer operations
MethodResult<std::string> textResult; // For string operations
MethodResult<CustomObject> objResult; // For custom object operations
```

Without templates, we would need either:
- Multiple classes (MethodResultInt, MethodResultString, etc.)
- Type-unsafe void pointers that could cause runtime errors

### MethodRequest Template Class
Base class for all method requests with features:
- Priority management
- Cancellation support
- Exception handling
- Template-based type safety

### Calculator Class
Main active object implementation showcasing:
- Priority queue for method requests
- Thread-safe request processing
- Asynchronous execution
- Graceful shutdown handling

## Detailed Implementation Explanation

### Thread Safety Mechanism
1. Mutex Protection
```cpp
std::mutex m_mutex;
std::condition_variable m_condition;
```
- Guards access to shared queue
- Ensures thread-safe method execution
- Manages thread synchronization

### Priority Queue Implementation
```cpp
std::priority_queue<
    std::shared_ptr<MethodRequest<int>>,
    std::vector<std::shared_ptr<MethodRequest<int>>>,
    RequestCompare<int>
> m_activation_queue;
```
- Ensures higher priority requests execute first
- Thread-safe priority management
- Smart pointer memory management

### Method Processing Queue
The processMethodQueue() method processes the activation queue until a shutdown is requested (!m_is_running becomes true).
The condition `!m_is_running && m_activation_queue.empty()` means:
1. We received a shutdown request (!m_is_running is true)
2. AND we've processed all remaining items in the queue (m_activation_queue.empty() is true)

This ensures:
- Don't exit if there's still work to do during shutdown
- Process all pending operations before shutting down
- Exit cleanly only when both shutdown is requested and all work is completed

### Error Handling
Comprehensive error handling includes:
- Integer overflow detection
- Timeout management
- Cancellation handling
- Exception propagation

### Timeout Implementation
```cpp
T get(const std::chrono::milliseconds& timeout) {
    if (m_future.wait_for(timeout) == std::future_status::timeout) {
        throw std::runtime_error("operation timed out");
    }
    return m_future.get();
}
```
- Configurable timeout duration
- Clean timeout exception handling
- Non-blocking operation support

### Cancellation Support
```cpp
void cancel() {
    m_cancelled = true;
    try {
        m_promise.set_exception(std::make_exception_ptr(
            std::runtime_error("operation cancelled")));
    } catch (...) {
        // promise might already be satisfied
    }
}
```
- Immediate operation cancellation
- Clean exception handling
- Thread-safe cancellation

## Usage Examples

### Basic Usage
```cpp
Calculator calculator;
auto result = calculator.add(5, 3);
std::cout << "Result: " << result->get() << '\n';
```

### Priority-based Execution
```cpp
auto highPriority = calculator.add(1, 2, Priority::HIGH);
auto lowPriority = calculator.add(3, 4, Priority::LOW);
```

### Timeout Handling
```cpp
try {
    result->get(std::chrono::milliseconds(100));
} catch (const std::runtime_error& e) {
    // Handle timeout
}
```

### Cancellation
```cpp
auto operation = calculator.multiply(5, 5);
operation->cancel();
```

## Performance Considerations
1. Queue Management
    - Priority queue operations are O(log n)
    - Memory usage scales with queue size

2. Thread Synchronization
    - Mutex contention with many operations
    - Condition variable for efficient waiting

3. Memory Management
    - Smart pointers for automatic cleanup
    - Shared pointer overhead considerations

## Testing
The implementation includes comprehensive tests for:
1. Basic operations
2. Priority handling
3. Timeout functionality
4. Cancellation behavior
5. Error handling
6. Mixed operations
7. Stress testing

## Thread Safety Guarantees
- Thread-safe method execution
- Safe concurrent access to shared resources
- Protected queue operations
- Safe shutdown handling

## License
This project is licensed under the MIT License - see the LICENSE file for details.

## Contributing
Contributions are welcome! Please feel free to submit pull requests with improvements or bug fixes.
