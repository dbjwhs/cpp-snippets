# C++20 Multithreading: A Comprehensive Guide

## Overview

This README comprehensively summarizes Anthony Williams' talk "An Introduction to Multithreading in C++20" from CppCon 2022. The document covers key C++20 multithreading facilities, practical usage patterns, and best practices for writing safe and efficient concurrent code.

**Video Link:** [An Introduction to Multithreading in C++20 - Anthony Williams - CppCon 2022](https://www.youtube.com/watch?v=A7sVFJLJM-A)

Anthony Williams is a multithreading expert, the author of "C++ Concurrency in Action," and has been instrumental in developing the C++ Standard's thread library since C++11.

I discovered Anthony's book in 2012, which became the foundation for many examples in this directory. Given its influence on this codebase, explicitly acknowledging his contributions is fitting. If you're working with C++ concurrency, particularly if you remember the days before portable threading libraries existed, I highly recommend watching the video above. If it sparks your interest, his book is worth the investment.

## Table of Contents

1. [Why Use Multithreading](#why-use-multithreading)
2. [Choosing Your Concurrency Model](#choosing-your-concurrency-model)
3. [Managing Threads with `std::jthread`](#managing-threads-with-stdjthread)
4. [Cooperative Cancellation with Stop Tokens](#cooperative-cancellation-with-stop-tokens)
5. [Synchronization Mechanisms](#synchronization-mechanisms)
   - [Latches](#latches)
   - [Barriers](#barriers)
   - [Futures](#futures)
   - [Mutexes](#mutexes)
   - [Condition Variables](#condition-variables)
   - [Semaphores](#semaphores)
   - [Atomics](#atomics)
6. [Testing Multithreaded Code](#testing-multithreaded-code)
7. [Further Resources](#further-resources)

## Why Use Multithreading

Two fundamental reasons to use multithreading:

1. **Scalability**: Taking advantage of multiple CPU cores to improve performance.
   - Amdahl's Law applies: speedup = 1 / ((1 - p) + p/n), where p is the parallel fraction and n is the number of cores.
   - Even with 90% parallelizable code, a 1000-core system only gives ~9.9x speedup.
   - For substantial gains, you need to maximize parallelizable portions (99%+ for significant scaling).

2. **Separation of Concerns**: Running independent tasks concurrently.
   - Used for long-running processes that should execute independently.
   - Examples include background spell checking, rendering, or other tasks that shouldn't block the main application flow.

## Choosing Your Concurrency Model

The hierarchy of approaches (from most to least recommended):

1. **Parallel Algorithms**: Use `std::execution::par` with standard algorithms when possible.
   - Easiest approach if your workload fits standard algorithms.
   - Combine consecutive algorithm calls (e.g., use `transform_reduce` instead of separate `transform` and `reduce`).

2. **Thread Pools**: Use for independent tasks that can be executed concurrently.
   - Note: C++20 doesn't include a standard thread pool yet.
   - Many third-party implementations available.

3. **Dedicated Threads**: Use `std::jthread` for larger sequential tasks that run concurrently.

## Managing Threads with `std::jthread`

`std::jthread` is C++20's improved thread management class:

```cpp
#include <thread>

void threadFunction(std::stop_token stop, int param) {
    while (!stop.stop_requested()) {
        // Do work
    }
}

int main() {
    std::jthread worker(threadFunction, 42);
    // No need to explicitly join - happens in destructor
}
```

Key features:
- **RAII semantics**: Automatically joins on destruction.
- **Integrated stop tokens**: Built-in cooperative cancellation.
- **Movable** (not copyable) value type.
- **Arguments are copied** into thread storage to prevent dangling references.

## Cooperative Cancellation with Stop Tokens

Stop tokens provide a cooperative means of cancellation:

```cpp
#include <thread>

void stopper(std::stop_source src) {
    // Do some work
    src.request_stop(); // Signal time to stop
}

void stoppable(std::stop_token token) {
    while (!token.stop_requested()) {
        // Do stuff until asked to stop
    }
}

int main() {
    std::stop_source source;
    std::jthread worker(stoppable, source.get_token());
    std::jthread worker2(stopper, std::move(source));
}
```

### Stop Callbacks

Stop callbacks allow integration with libraries that provide their own cancellation:

```cpp
void processWithCancellation(std::stop_token token) {
    auto fileHandle = openFile("example.dat");

    // Register callback that will be invoked when stop is requested
    std::stop_callback cb(token, [fileHandle]() {
        cancelIO(fileHandle);
    });

    // Read data - will be interrupted if token is signaled
    readData(fileHandle);
}
```

**Important**: The callback is executed on the thread that calls `request_stop()`.

## Synchronization Mechanisms

The talk recommends using the following synchronization mechanisms in order of preference:

### Latches

`std::latch` is a single-use counter for thread synchronization:

```cpp
#include <latch>
#include <vector>
#include <thread>

void process_in_parallel(int num_threads) {
    std::vector<int> data(num_threads);
    std::latch completion_latch(num_threads);
    std::vector<std::jthread> threads;

    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back([&, i](std::stop_token st) {
            data[i] = makeData(i); // Populate data
            completion_latch.count_down(); // Signal completion
            doMoreStuff(i); // Continue with other work
        });
    }

    // Wait for all threads to complete the data generation
    completion_latch.wait();

    // Safe to process data now
    processData(data);
}
```

**Strengths:**
- Extremely lightweight and efficient for one-time synchronization
- Simple API with clear semantics (count_down(), wait(), arrive_and_wait())
- No reset capability prevents accidental reuse bugs
- Lock-free implementation on most platforms
- Perfect for fork-join patterns

**Weaknesses:**
- Cannot be reused after reaching zero (single-use only)
- No way to query current count or check if latch is ready without blocking
- Cannot increase the count after construction
- Not suitable for producer-consumer patterns or repeated synchronization

**Industry-Accepted Usage:**
- **Test synchronization**: Ensuring all test threads start simultaneously
- **Initialization coordination**: Waiting for all worker threads to complete setup
- **Fork-join parallelism**: Parent thread waiting for child threads to complete specific work
- **One-time events**: Signaling that a resource is ready or an event has occurred
- **Staged shutdown**: Coordinating multi-phase application shutdown

Latches are ideal for one-time synchronization, particularly when one thread needs to wait for several others to complete a task.

#### Synchronizing Test Setup

```cpp
#include <latch>
#include <vector>
#include <thread>

void test_concurrent_data_structure() {
    DataStructure ds;
    std::latch start_signal(3); // Main + 2 test threads

    std::jthread reader([&](std::stop_token st) {
        start_signal.arrive_and_wait(); // Wait for all threads to be ready
        // Test reading operations
    });

    std::jthread writer([&](std::stop_token st) {
        start_signal.arrive_and_wait(); // Wait for all threads to be ready
        // Test writing operations
    });

    // Main thread signals it's ready too
    start_signal.arrive_and_wait();
    // Now all threads start simultaneously
}
```

### Barriers

`std::barrier` is reusable and designed for loop synchronization:

```cpp
#include <barrier>
#include <thread>
#include <vector>

void process_in_phases(int num_threads, int num_phases) {
    auto on_completion = []() noexcept {
        // This runs after each phase completes
        prepareNextPhase();
    };

    std::barrier phase_barrier(num_threads, on_completion);
    std::vector<std::jthread> threads;

    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back([&, i](std::stop_token st) {
            for (int phase = 0; phase < num_phases && !st.stop_requested(); ++phase) {
                processPhase(phase, i);
                phase_barrier.arrive_and_wait();
                // All threads have completed this phase and on_completion has run
            }
        });
    }
}
```

**Strengths:**
- Reusable for multiple synchronization points (unlike latches)
- Built-in completion callback executed after each phase
- Efficient for iterative algorithms with regular synchronization
- Can dynamically adjust expected thread count with arrive_and_drop()
- Lock-free implementation on many platforms

**Weaknesses:**
- More complex API than latches
- Overhead of resetting internal state after each use
- All threads must participate in each phase (or explicitly drop)
- Not suitable for producer-consumer or asymmetric patterns
- Completion callback runs on one of the arriving threads (unpredictable which)

**Industry-Accepted Usage:**
- **Parallel simulations**: Physics simulations, cellular automata, finite element analysis
- **Matrix operations**: Parallel matrix multiplication with phase synchronization
- **Iterative algorithms**: Parallel implementations of iterative solvers (Jacobi, Gauss-Seidel)
- **Game loops**: Synchronizing update and render phases across multiple subsystems
- **Batch processing**: Processing data in synchronized batches across worker threads
- **Machine learning**: Synchronizing gradient updates in parallel training algorithms

Barriers are ideal for algorithms with multiple phases requiring synchronization between iterations.

### Futures

Futures allow one-shot transfer of data between threads:

```cpp
#include <future>
#include <thread>

void producer_consumer() {
    // Using std::async
    std::future<int> result = std::async(std::launch::async, []() {
        // Do calculation
        return 42;
    });

    // Do other work

    // Wait and get result
    int value = result.get();

    // Using explicit promise
    std::promise<std::string> data_promise;
    std::future<std::string> data_future = data_promise.get_future();

    std::jthread producer([p = std::move(data_promise)](std::stop_token st) mutable {
        std::string data = generateData();
        p.set_value(std::move(data));
    });

    // Get data when ready
    std::string received_data = data_future.get();
}
```

For multiple receivers, use `std::shared_future`:

```cpp
std::promise<int> p;
std::shared_future<int> sf = p.get_future().share();

std::jthread t1([sf](std::stop_token st) {
    int value = sf.get(); // Gets copy of the value
});

std::jthread t2([sf](std::stop_token st) {
    int value = sf.get(); // Gets another copy of the same value
});

p.set_value(42); // Both threads receive 42
```

**Strengths:**
- Clean separation between data producer and consumer
- Type-safe data transfer with automatic memory management
- Exception propagation from producer to consumer
- Can query readiness without blocking (wait_for, wait_until)
- std::async provides easy task-based parallelism
- shared_future enables broadcasting results to multiple consumers

**Weaknesses:**
- Single-use only (can only get() once from a future)
- No built-in cancellation mechanism (except through exceptions)
- std::async has implementation-defined behavior for deferred tasks
- Overhead compared to raw synchronization primitives
- Cannot reset or reuse for multiple values
- Blocking get() can lead to deadlocks if not careful

**Industry-Accepted Usage:**
- **Task-based parallelism**: Offloading computations with std::async
- **Pipeline processing**: Passing results between pipeline stages
- **Lazy evaluation**: Deferring expensive computations until needed
- **Asynchronous I/O**: Returning results from async file/network operations
- **Request-response patterns**: Web servers, RPC systems, database queries
- **Parallel algorithms**: Divide-and-conquer algorithms with result aggregation
- **GUI applications**: Background tasks reporting results to UI thread

### Mutexes

Mutexes provide mutual exclusion for protecting shared data:

```cpp
#include <mutex>
#include <thread>

class ThreadSafeCounter {
private:
    std::mutex m;
    int value = 0;

public:
    int increment() {
        std::scoped_lock lock(m);
        return ++value;
    }

    int get() {
        std::scoped_lock lock(m);
        return value;
    }
};
```

Always use `std::scoped_lock` when locking mutexes, especially when locking multiple mutexes to avoid deadlocks:

```cpp
void transfer(Account& from, Account& to, double amount) {
    std::scoped_lock lock(from.mutex, to.mutex); // Deadlock-free locking
    from.balance -= amount;
    to.balance += amount;
}
```

**Strengths:**
- Simple and intuitive model for protecting shared data
- RAII wrappers (scoped_lock, unique_lock) prevent forgetting to unlock
- Multiple variants for different needs (mutex, recursive_mutex, timed_mutex, shared_mutex)
- std::scoped_lock prevents deadlocks when locking multiple mutexes
- shared_mutex enables multiple readers/single writer patterns
- Well-understood semantics across all platforms

**Weaknesses:**
- Can cause significant contention and serialization
- Risk of deadlocks if not used carefully (especially with multiple mutexes)
- No priority inheritance, can cause priority inversion
- Recursive mutexes can hide design problems
- Overhead of OS kernel calls on some platforms
- Can't be used in signal handlers or interrupt contexts

**Industry-Accepted Usage:**
- **Data structure protection**: Thread-safe containers, caches, registries
- **Critical sections**: Protecting invariants during multi-step operations
- **Resource management**: Controlling access to files, sockets, hardware
- **Read-write patterns**: Using shared_mutex for read-heavy workloads
- **Transaction processing**: Ensuring consistency in financial operations
- **Logging systems**: Serializing output to prevent interleaved messages
- **Configuration updates**: Protecting global settings and state

### Condition Variables

Condition variables allow threads to wait for specific conditions:

```cpp
#include <mutex>
#include <condition_variable>
#include <optional>

class DataQueue {
private:
    std::mutex m;
    std::condition_variable cv;
    std::optional<Data> data;

public:
    void setData(Data newData) {
        {
            std::scoped_lock lock(m);
            data = std::move(newData);
        } // Unlock before notification for efficiency
        cv.notify_one();
    }

    Data waitForData() {
        std::unique_lock lock(m); // Must use unique_lock, not scoped_lock
        cv.wait(lock, [this]{ return data.has_value(); });
        Data result = std::move(*data);
        data.reset();
        return result;
    }
};
```

C++20 adds `std::condition_variable_any` for integration with stop tokens:

```cpp
std::condition_variable_any cv;
std::mutex m;
std::optional<Data> data;

void waitForDataWithCancellation(std::stop_token st) {
    std::unique_lock lock(m);
    if (cv.wait(lock, st, [&]{ return data.has_value(); })) {
        // Condition was satisfied, process data
        processData(std::move(*data));
    } else {
        // Wait was interrupted by stop token
        cleanUp();
    }
}
```

**Strengths:**
- Efficient waiting without busy-spinning
- Integrates with mutexes for atomic check-and-wait
- Can wake one (notify_one) or all (notify_all) waiting threads
- Supports timed waits (wait_for, wait_until)
- C++20 condition_variable_any supports stop tokens for cancellation
- Lower CPU usage compared to polling

**Weaknesses:**
- Prone to spurious wakeups (must always use with predicate)
- Complex to use correctly (requires mutex, predicate, proper ordering)
- Risk of lost wakeups if notify happens before wait
- Can cause thundering herd problem with notify_all
- Requires unique_lock (not scoped_lock) which is less safe
- Difficult to debug when used incorrectly

**Industry-Accepted Usage:**
- **Producer-consumer queues**: Blocking queues, thread pools, work queues
- **Event notification**: Signaling state changes between threads
- **Resource availability**: Waiting for resources to become available
- **Rate limiting**: Implementing token buckets or rate limiters
- **Synchronous handoff**: Direct hand-off between producer and consumer
- **State machines**: Waiting for specific state transitions
- **Message passing**: Implementing synchronous message channels

### Semaphores

Semaphores represent a count of available resources:

```cpp
#include <semaphore>
#include <thread>
#include <vector>

void limitedConcurrentAccess(int maxConcurrent, int numTasks) {
    std::counting_semaphore<5> limitSem(maxConcurrent);
    std::vector<std::jthread> threads;

    for (int i = 0; i < numTasks; ++i) {
        threads.emplace_back([&, i](std::stop_token st) {
            limitSem.acquire(); // Block if maxConcurrent threads are already running

            // This section has at most maxConcurrent threads executing simultaneously
            processTask(i);

            limitSem.release(); // Allow another thread to enter
        });
    }
}
```

`std::binary_semaphore` is a specialization with a maximum count of 1 (equivalent to `std::counting_semaphore<1>`).

**Strengths:**
- Simple counting model for resource management
- Can be used across process boundaries (with OS support)
- Binary semaphore can replace mutex in some scenarios
- Supports try_acquire and timed acquire operations
- Lock-free implementation on many platforms
- No ownership requirement (any thread can release)

**Weaknesses:**
- No ownership tracking (can lead to incorrect usage)
- Easy to misuse (acquire/release imbalance)
- Can't detect programming errors at compile time
- Less type safety than higher-level abstractions
- No RAII wrapper in standard library
- Doesn't prevent priority inversion

**Industry-Accepted Usage:**
- **Connection pooling**: Limiting database or network connections
- **Thread pooling**: Controlling number of active worker threads
- **Rate limiting**: Implementing fixed-rate processing
- **Resource allocation**: Managing limited hardware resources (GPUs, file handles)
- **Producer-consumer**: Implementing bounded buffers
- **Parking lots**: Managing fixed number of slots/spaces
- **License management**: Controlling concurrent usage of licensed features

### Atomics

Atomics provide low-level lock-free operations for trivially copyable types:

```cpp
#include <atomic>
#include <thread>

class AtomicCounter {
private:
    std::atomic<int> count{0};

public:
    void increment() {
        ++count;
    }

    int get() {
        return count.load();
    }
};
```

**Strengths:**
- Lock-free programming for maximum performance
- No risk of deadlocks or priority inversion
- Fine-grained control over memory ordering
- Suitable for high-frequency operations
- Can work in signal handlers and interrupt contexts
- Hardware-level atomic operations

**Weaknesses:**
- Extremely difficult to use correctly
- Memory ordering semantics are complex and error-prone
- Limited to trivially copyable types
- ABA problem with pointer types
- Platform-dependent performance characteristics
- Debugging lock-free code is notoriously difficult
- Can actually be slower than mutexes in some scenarios

**Industry-Accepted Usage:**
- **High-frequency counters**: Statistics, metrics, performance monitoring
- **Lock-free data structures**: Queues, stacks, ring buffers (expert-level)
- **State machines**: Simple state transitions without complex invariants
- **Flags and signals**: Boolean flags for thread communication
- **Memory management**: Reference counting, hazard pointers (expert-level)
- **System programming**: Kernel code, device drivers, real-time systems
- **Performance-critical paths**: When profiling shows mutex contention

Atomics should be your last resort, as they're the most low-level and error-prone synchronization mechanism.

For most types, use atomics only when:
- The type is small and trivially copyable
- You need high-performance synchronization
- You've verified that atomics actually provide a performance improvement

## Testing Multithreaded Code

Testing concurrent code requires special considerations:

1. **Use latches for synchronization**:
   ```cpp
   void testConcurrentOperations() {
       std::latch startSignal(threadCount + 1);

       for (int i = 0; i < threadCount; ++i) {
           threads.emplace_back([&, i]() {
               startSignal.arrive_and_wait(); // Wait for all threads to be ready
               // Perform test operations
           });
       }

       // Start all threads simultaneously
       startSignal.arrive_and_wait();
   }
   ```

2. **Test with varying thread counts**:
   ```cpp
   TEST_CASE("Concurrent data structure is thread-safe") {
       for (int threadCount : {2, 4, 8, 16}) {
           SECTION("With " + std::to_string(threadCount) + " threads") {
               // Test with this thread count
           }
       }
   }
   ```

3. **Stress testing with repetition**:
   ```cpp
   TEST_CASE("Stress test") {
       for (int iteration = 0; iteration < 1000; ++iteration) {
           // Run the same test many times to increase chance of exposing race conditions
       }
   }
   ```

4. **Use tools like ThreadSanitizer, Helgrind, or DRD** to detect data races and deadlocks.

## Summary of Best Practices

1. Prefer the highest-level abstraction that works:
   - Parallel algorithms
   - Thread pools
   - Dedicated threads with `std::jthread`

2. Use cooperative cancellation with stop tokens.

3. Choose synchronization mechanisms in this order:
   - Latches (single-use synchronization)
   - Barriers (multi-phase synchronization)
   - Futures (one-shot data transfer)
   - Mutexes with `std::scoped_lock` (data protection)
   - Condition variables (waiting for conditions)
   - Semaphores (resource limiting)
   - Atomics (last resort, low-level)

4. Always thoroughly test multithreaded code with appropriate tools and techniques.

## Further Resources

- "C++ Concurrency in Action" by Anthony Williams
- "The Little Book of Semaphores" by Allen B. Downey
- CppCon talks on concurrency and parallelism
- C++ reference documentation for thread support library

## About the Speaker

Anthony Williams is a renowned expert in C++ concurrency, author of "C++ Concurrency in Action," and has been an active member of the BSI C++ Standards Panel since 2001. He is the author or coauthor of many C++ Standards Committee papers that led to the inclusion of the thread library in C++11 and continues to work on enhancing C++ concurrency capabilities.
