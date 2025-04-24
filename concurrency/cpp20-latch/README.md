# std::latch Example Implementation

The `std::latch` is a powerful synchronization primitive introduced in C++20 as part of the concurrency library. It provides a
single-use countdown synchronization mechanism that allows one or more threads to block until a specified counter reaches zero. The
concept of a latch or countdown latch has existed in concurrent programming for decades, with similar implementations in other
languages like Java's `CountDownLatch`. Its introduction to the C++ standard library offers a standardized, efficient, and
type-safe way to handle common thread coordination patterns without needing to build custom synchronization mechanisms.

## Use Cases and Problems Solved

Latches solve several common problems in concurrent programming by providing a clear, reusable synchronization pattern:

1. **Starting Gate Pattern**: Holding back a group of threads until they are all ready to begin execution simultaneously. This ensures
   that all threads start processing at approximately the same time, which can be important for benchmarking or simulation scenarios.

2. **Task Completion Synchronization**: Allowing a thread (often the main thread) to wait until a specific number of operations have
   completed before proceeding. This is useful when you need to ensure that all background work has finished before performing
   subsequent operations.

3. **Resource Initialization**: Ensuring that resources are fully initialized before any thread attempts to use them, preventing race
   conditions and potential undefined behavior.

4. **Thread Coordination**: Managing groups of threads that must wait for each other at specific points in execution, such as in
   phased computation where each phase must complete before the next begins.

Unlike more complex synchronization primitives, `std::latch` is simple, single-use, and does not reset. This makes it ideal for
one-time coordination events in the lifecycle of a program.

## Examples and Usage

### Basic Usage

The most basic usage of `std::latch` involves initializing it with a count and then having threads either count down or wait:

```cpp
std::latch completion_latch(3); // Initialize with count of 3

// In thread 1
completion_latch.count_down(); // Decrease count by 1

// In thread 2
completion_latch.count_down(); // Decrease count by 1

// In thread 3
completion_latch.count_down(); // Decrease count by 1

// In main thread
completion_latch.wait(); // Block until latch count reaches 0
```

### Methods

`std::latch` provides the following key methods:

- `count_down()`: Decrements the counter by 1
- `try_wait()`: Checks if the counter has reached zero without blocking
- `wait()`: Blocks until the counter reaches zero
- `arrive_and_wait()`: Atomically decrements the counter and waits for it to reach zero

### Implementation Considerations

When using `std::latch`, consider:

1. **Thread Safety**: All operations on `std::latch` are thread-safe by design.
2. **No Reset**: Once a latch reaches zero, it cannot be reset. For repeatable synchronization, consider using `std::barrier`.
3. **Memory Ordering**: Operations on a latch establish synchronization relationships between threads.

### Good Practices

- Use `arrive_and_wait()` when a thread needs to both signal its arrival and wait for others.
- Initialize latches with the exact count needed to avoid deadlocks.
- Consider using `try_wait()` with a timeout when appropriate to avoid indefinite blocking.

### Bad Practices

- Don't try to reuse a latch after it has reached zero.
- Avoid initializing a latch with a count that might never be reached.
- Don't use latches for complex synchronization patterns where a mutex, condition variable, or barrier might be more appropriate.

## Related Material

For more information on `std::latch` and concurrent programming patterns, refer to:

- "C++ Concurrency in Action" by Anthony Williams
- "The Art of Multiprocessor Programming" by Maurice Herlihy and Nir Shavit
- C++20 Standard Library documentation

## License

This code is provided under the MIT License. Feel free to use, modify, and distribute as needed.

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.