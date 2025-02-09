# Modern C++ Thread Pool Implementation

## Overview
This repository contains a modern C++17 implementation of a thread pool pattern. The thread pool manages a collection of worker threads that can execute tasks asynchronously, providing an efficient way to parallelize work across multiple threads.

## Features
- Modern C++17 implementation
- Thread-safe task queue
- Asynchronous task execution
- Future-based result handling
- Exception-safe design
- Automatic thread management
- Clean shutdown mechanism

## Requirements
- C++17 or later
- Standard Template Library (STL)
- Compatible C++ compiler (e.g., GCC, Clang, MSVC)

## Implementation Details

### Class Structure
The `ThreadPool` class consists of the following key components:

```cpp
class ThreadPool {
private:
    std::vector<std::thread> m_workers;
    std::queue<std::function<void()>> m_tasks;
    std::mutex m_queueMutex;
    std::condition_variable m_condition;
    bool m_stop;
    // ...
};
```

### Key Components

1. **Worker Threads**
    - Stored in `std::vector<std::thread>`
    - Created during thread pool initialization
    - Continuously wait for and execute tasks
    - Automatically join on destruction

2. **Task Queue**
    - Uses `std::queue<std::function<void()>>`
    - Thread-safe implementation
    - Stores pending tasks
    - FIFO (First In, First Out) processing

3. **Synchronization Mechanisms**
    - Mutex for thread-safe queue access
    - Condition variable for thread notification
    - Stop flag for clean shutdown

### Understanding the Thread Pool Template

#### Template Function Signature
```cpp
template<class TQueue, class... Args>
auto enqueue(TQueue&& tqueue, Args&&... args)
    -> std::future<typename std::invoke_result<TQueue, Args...>::type>
```

### Breaking Down the Components

#### Template Parameters
- `TQueue`: Represents the type of the callable object (function, lambda, functor)
- `Args...`: A parameter pack that can accept any number of additional argument types
- The `...` syntax indicates this is a variadic template, allowing the function to accept any number of arguments

#### Function Parameters
- `TQueue&& tqueue`: A universal reference (forwarding reference) to the callable object
- `Args&&... args`: Universal references to all the arguments needed by the callable object
- The `&&` allows for perfect forwarding, preserving the value category (lvalue/rvalue) of the original arguments
- For a great video explaning `lvalue` vs `rvalues` checkout the following [lvalues and rvalues in C++](https://www.youtube.com/watch?v=fbYknr-HPYE)

#### Return Type
- Uses trailing return type syntax (`->`) *for a deeeeep dive on this see the following (I have not made it all the way through this yet) -> [Embracing Trailing Return Types...](https://www.youtube.com/watch?v=Tnl7FnwJ2Uw)*
- `std::future<...>`: Represents a future value that will be computed asynchronously
- `std::invoke_result<TQueue, Args...>::type`: Determines the return type of calling `TQueue` with `Args`

## Key Implementation Details

### Type Deduction
```cpp
using return_type = typename std::invoke_result<TQueue, Args...>::type;
```
- Determines the type that will be returned when calling the function
- `std::invoke_result` is a type trait that deduces the return type of calling a callable with given arguments
- Example: If `TQueue` is `int(int, int)`, and `Args` are `int, int`, then `return_type` will be `int`

### Task Creation
```cpp
auto task = std::make_shared<std::packaged_task<return_type()>>(
    std::bind(std::forward<TQueue>(tqueue), std::forward<Args>(args)...)
);
```
- Creates a `shared_ptr` to manage the task's lifetime
- Uses `std::bind` to package the function and its arguments together
- `std::forward` preserves the value category of arguments (perfect forwarding)
- The task is wrapped in a `packaged_task` to enable asynchronous execution

### Future Retrieval
```cpp
std::future<return_type> result = task->get_future();
```
- Gets a `future` object that will eventually contain the task's result
- Allows asynchronous access to the result once the task completes

## Usage Example
```cpp
ThreadPool pool(4);  // Create pool with 4 threads

// Example 1: Enqueue a lambda
auto future1 = pool.enqueue([](int x) { return x * x; }, 42);
int result1 = future1.get();  // Returns 1764

// Example 2: Enqueue a function
double multiply(double x, double y) { return x * y; }
auto future2 = pool.enqueue(multiply, 3.14, 2.0);
double result2 = future2.get();  // Returns 6.28
```

## Thread Safety
- The template function ensures thread-safe task submission through mutex locking
- Tasks are executed asynchronously by the thread pool's worker threads
- Results can be safely retrieved through the returned `future` object

## Error Handling
- Throws `std::runtime_error` if attempting to enqueue tasks after the thread pool has been stopped
- The `future` object will propagate any exceptions thrown during task execution

This template design provides a flexible and type-safe way to submit any callable object with any number of arguments to the thread pool while maintaining thread safety and proper resource management.

### Usage Example

```cpp
// Create a thread pool with 4 threads
ThreadPool pool(4);

// Create a future to store the result
auto future = pool.enqueue([](int x) { return x * x; }, 12);

// Get the result
int result = future.get();  // Will be 144
```

## Advanced Features

### Task Enqueuing
The `enqueue` method offers:
- Template parameter deduction
- Perfect forwarding of arguments
- Future-based result handling
- Exception safety

```cpp
template<class TQueue, class... Args>
auto enqueue(TQueue&& tqueue, Args&&... args)
    -> std::future<typename std::invoke_result<TQueue, Args...>::type>;
```

### Exception Handling
The implementation includes:
- Exception-safe task execution
- Error propagation through futures
- Safe thread pool shutdown
- Runtime error detection for stopped pools

## Thread Safety
The implementation ensures thread safety through:
- Mutex-protected task queue access
- Atomic stop flag operations
- Synchronized thread initialization and shutdown
- Safe task enqueuing and execution

## Performance Considerations

1. **Task Granularity**
    - Best for compute-intensive tasks
    - Avoid very short tasks due to overhead
    - Consider task batching for small operations

2. **Thread Count**
    - Default to hardware thread count
    - Adjust based on workload characteristics
    - Consider system resources and context

## Best Practices

1. **Resource Management**
    - Create thread pool with appropriate thread count
    - Ensure proper shutdown
    - Handle exceptions appropriately

2. **Task Design**
    - Make tasks independent
    - Avoid shared state when possible
    - Use appropriate synchronization when needed

3. **Error Handling**
    - Check futures for exceptions
    - Handle pool shutdown gracefully
    - Implement appropriate error recovery

## Example Use Cases

1. **Parallel Processing**
```cpp
std::vector<std::future<int>> results;
for(int i = 0; i < 100; i++) {
    results.emplace_back(pool.enqueue([i] {
        return expensive_computation(i);
    }));
}
```

2. **Batch Processing**
```cpp
void process_batch(const std::vector<Data>& batch) {
    ThreadPool pool(4);
    for(const auto& data : batch) {
        pool.enqueue(&process_item, data);
    }
}
```

## Limitations
- No task prioritization
- No thread count adjustment after creation
- No task cancellation mechanism
- Limited to function-based tasks

## License
This implementation is provided under the MIT License. See the LICENSE file for details.

## Contributing
Contributions are welcome! Please feel free to submit pull requests with improvements or bug fixes.

## References
- C++17 Standard Library
- Modern C++ Design Patterns
- Thread Pool Pattern Documentation
