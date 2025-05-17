# C++23 std::latch Example

## Overview

The `std::latch` class is a synchronization primitive introduced in C++20 as part of the standard library's concurrency
toolset. It functions as a downward counter that can be used to synchronize threads. A latch is initialized with a count
and threads can block on the latch until its count reaches zero. Once the latch's count is decremented to zero, all
waiting threads are released, and the latch cannot be reset or reused - making it a single-use synchronization barrier.

The concept of latches has existed in concurrent programming for decades, appearing in various threading libraries and
frameworks before being standardized in C++20. They represent one of the fundamental synchronization primitives alongside
mutexes, condition variables, and semaphores. Unlike more complex synchronization mechanisms, latches are designed for
simplicity and performance in specific use cases.

## Use Cases and Benefits

Latches excel at several common synchronization scenarios:

1. **Thread coordination** - Wait for a group of threads to complete their initialization or tasks before proceeding with
   further execution.

2. **Simultaneous thread start** - Ensure that multiple threads begin their execution at approximately the same time,
   useful for benchmarking or when timing-critical operations need to start together.

3. **Fork-join parallelism** - Implement patterns where a main thread "forks" work to multiple worker threads and then
   "joins" by waiting for all workers to complete.

4. **Event counting** - Wait for a specific number of events to occur before proceeding.

5. **One-time initialization barriers** - Ensure that initialization code runs exactly once before any dependent code.

The benefits of using `std::latch` over other synchronization primitives include:

- **Simplicity** - Latches have a straightforward API with just a few methods, making them easy to understand and use
  correctly.
- **Performance** - They are typically implemented efficiently with atomic operations rather than requiring heavyweight
  mutex operations.
- **Safety** - The single-use nature of latches prevents certain classes of concurrency bugs that can occur with reusable
  barriers.

## Example Usage

Here's a simple example of using `std::latch`:

```cpp
#include <iostream>
#include <latch>
#include <thread>
#include <vector>

void worker(std::latch& start_signal, std::latch& completion_signal, int id) {
    // Wait for the start signal
    start_signal.arrive_and_wait();
    
    // Do some work
    std::cout << "Thread " << id << " is working\n";
    
    // Signal completion
    completion_signal.count_down();
}

int main() {
    const int thread_count = 4;
    std::latch start_signal(thread_count + 1);  // +1 for the main thread
    std::latch completion_signal(thread_count);
    
    std::vector<std::thread> threads;
    
    // Create worker threads
    for (int i = 0; i < thread_count; ++i) {
        threads.emplace_back(worker, std::ref(start_signal), std::ref(completion_signal), i);
    }
    
    // Allow time for threads to initialize
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // Start all threads simultaneously
    std::cout << "Starting all threads\n";
    start_signal.arrive_and_wait();
    
    // Wait for all threads to complete
    completion_signal.wait();
    std::cout << "All threads have completed\n";
    
    // Join all threads
    for (auto& t : threads) {
        t.join();
    }
    
    return 0;
}
```

## Best Practices

- Use latches for one-off synchronization scenarios; for repeating barriers, use `std::barrier` instead.
- Always ensure that the latch count is decremented the expected number of times to avoid deadlocks.
- Consider using the convenience methods like `arrive_and_wait()` which both decrements the counter and waits for it to reach zero.
- Remember that after a latch reaches zero, attempting to wait on it again is safe and will not block.
- For exception safety, consider using RAII wrappers around latch count-downs in complex code.

## Common Pitfalls

- Setting an incorrect initial count can lead to threads either never waking up or waking up too soon.
- Forgetting to call `count_down()` in all expected places can cause deadlocks.
- Trying to reuse a latch after it has been released will not work as intended.
- Using a latch when a more complex synchronization primitive is needed can lead to convoluted code.

## Further Reading

- "C++ Concurrency in Action" by Anthony Williams - Contains thorough explanations of synchronization primitives
- "Effective Modern C++" by Scott Meyers - General best practices for modern C++
- [C++ Reference for std::latch](https://en.cppreference.com/w/cpp/thread/latch)
- "The Art of Multiprocessor Programming" by Maurice Herlihy and Nir Shavit - For deeper understanding of concurrency concepts

## License

This code is provided under the MIT License. Feel free to use, modify, and distribute as needed.

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.