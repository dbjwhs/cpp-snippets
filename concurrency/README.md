# C++20 Multithreading: A Comprehensive Guide

## Overview

This README provides a comprehensive summary of Anthony Williams' talk "An Introduction to Multithreading in C++20" from CppCon 2022. The document covers key C++20 multithreading facilities, practical usage patterns, and best practices for writing safe and efficient concurrent code.

**Video Link:** [An Introduction to Multithreading in C++20 - Anthony Williams - CppCon 2022](https://www.youtube.com/watch?v=A7sVFJLJM-A)

Anthony Williams is a multithreading expert, the author of "C++ Concurrency in Action," and has been instrumental in the development of the C++ Standard's thread library since C++11.

I personally discovered Anthony's book in 2012 and it led to seeding many of the examples in this directory. It feels appropriate that he is not only called out in and around my example code. If you are interested in C++ concurrency, especially if you lived as I did in a time where there was no OS agnostic threading libraries, then take the time to at least watch the above video, if your interest is peeked then buy his book.

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