# Thread Pool with Priority and Load Balancing

## Overview

The Thread Pool pattern is a software design pattern that emerged in the mid-1990s as a solution to the performance overhead
associated with thread creation and destruction in concurrent applications. It was first formalized in the paper "Half-Sync/Half-Async:
An Architectural Pattern for Efficient and Well-Structured Concurrent I/O" by Douglas Schmidt in 1995. The pattern gained
widespread adoption in server applications, where efficiently handling numerous concurrent client requests became critical for
performance. With the introduction of standard threading capabilities in C++11 and synchronization primitives like semaphores in
C++20, implementing thread pools in standard C++ became significantly more straightforward and efficient.

## Use Cases and Problem Solutions

Thread pools effectively address several common problems in concurrent programming. They amortize the cost of thread
creation/destruction by reusing threads for multiple tasks, reducing system overhead. They provide controlled concurrency by
limiting the number of active threads, preventing system resource exhaustion. Modern thread pool implementations often
incorporate task prioritization to ensure critical tasks are executed first, and load balancing to distribute work evenly
across available threads. Thread pools are particularly valuable in applications with unpredictable or bursty workloads, such
as web servers, database systems, game engines, real-time data processing systems, and GUI applications that need to maintain
responsiveness while performing background operations.

## Implementation Details

This implementation provides a modern C++ thread pool with several advanced features:

1. **Priority-based Scheduling**: Tasks can be assigned different priority levels (LOW, MEDIUM, HIGH), ensuring that high-priority tasks are executed before lower-priority ones.

2. **Load Balancing**: The implementation uses a semaphore-based approach to limit the number of concurrent tasks, preventing thread starvation and ensuring balanced resource utilization.

3. **C++17/C++20 Compatibility**: The code can use the standard `std::counting_semaphore` when compiled with C++20 or later, but also provides a compatible implementation for C++17.

4. **Thread Safety**: All operations on the thread pool are thread-safe, allowing tasks to be enqueued from multiple threads simultaneously.

5. **Pause/Resume Functionality**: The thread pool can be paused and resumed, which is useful for situations where task execution needs to be temporarily halted.

### Internal Architecture

#### Thread Pool Setup and Initialization
The thread pool is initialized with a specified number of worker threads (defaulting to the hardware concurrency if not specified). During construction:

1. A semaphore is initialized with a count equal to the number of threads, which will regulate concurrent task execution.
2. Worker threads are created and stored in a vector, each running the `worker_thread` method.
3. Atomic counters are set up to track active tasks and busy threads.
4. Control flags (`m_stop` and `m_paused`) are initialized to manage the thread pool's lifecycle.

#### Task Priority Implementation
Task prioritization is implemented using a standard library priority queue with a custom comparator:

1. Tasks are wrapped in a `Task` class that stores both the callable function and its assigned priority.
2. The priority queue uses a custom `TaskComparator` that orders tasks based on their priority level.
3. The comparator ensures that higher priority tasks (HIGH = 2) are placed at the top of the queue.
4. When a worker thread requests a task, it always gets the highest priority task available.
5. Within the same priority level, tasks are executed in FIFO (First In, First Out) order.

#### Worker Thread Execution Flow
Each worker thread follows a specific execution flow:

1. The thread enters a loop waiting for tasks or a shutdown signal.
2. When notified of available work, it checks if the pool is stopping or paused.
3. If tasks are available and the pool is running, it pops the highest priority task from the queue.
4. Before executing the task, it acquires a slot from the semaphore to maintain load balance.
5. The thread marks itself as busy, executes the task, and then marks itself as no longer busy.
6. After task completion, it releases the semaphore slot and decrements the active task counter.
7. If no tasks are available and task stealing is unsuccessful, the thread yields the CPU.
8. Exception handling ensures that resources are properly released even if a task throws an exception.

#### Load Balancing Mechanism
Load balancing is achieved through a counting semaphore:

1. The semaphore is initialized with a count equal to the number of threads in the pool.
2. Before executing a task, a thread must acquire a slot from the semaphore.
3. If all slots are in use, the thread will block until a slot becomes available.
4. After task execution, the thread releases its slot back to the semaphore.
5. This mechanism prevents all threads from executing long-running tasks simultaneously, ensuring that short tasks can be processed promptly.
6. The implementation includes a placeholder for task stealing (though not fully implemented), which would allow idle threads to take tasks from busy threads' queues.

#### Pause/Resume Implementation
The pause/resume mechanism is implemented using an atomic flag and condition variable:

1. When `pause()` is called, the `m_paused` atomic flag is set to true.
2. Worker threads check this flag before attempting to retrieve tasks from the queue.
3. If the flag is set, threads will wait on the condition variable without processing new tasks.
4. Currently executing tasks will run to completion, but no new tasks will start.
5. When `resume()` is called, the flag is set to false, and all threads are notified via the condition variable.
6. Upon notification, threads will check the flag again and resume processing tasks if the pool is no longer paused.

#### Task Enqueuing Process
When a task is submitted to the pool:

1. The callable function and its arguments are captured in a `std::packaged_task`.
2. A future is obtained from the packaged task to allow the caller to wait for the result.
3. The packaged task is wrapped in a function that will be called when the task executes.
4. A Task object is created with the specified priority and the wrapper function.
5. Under mutex protection, the Task is pushed onto the priority queue and the active task counter is incremented.
6. One waiting thread is notified that a new task is available.
7. The future is returned to the caller to allow synchronization with task completion.

## Usage Example

```cpp
#include "thread_pool.hpp"
#include <iostream>
#include <future>

// Create a thread pool with the default number of threads
ThreadPool pool;

// Enqueue a task with high priority
auto result = pool.enqueue(
    ThreadPool::Priority::HIGH,
    [](int value) { return value * 2; },
    21
);

// Wait for and get the result
std::cout << "Result: " << result.get() << std::endl;  // Output: Result: 42
```

## Best Practices

When using this thread pool implementation, consider the following best practices:

- Choose an appropriate number of threads for your specific hardware and workload. Too many threads can lead to excessive context switching.
- Use task priorities judiciously. Not every task needs HIGH priority; overuse diminishes the effectiveness of prioritization.
- For long-running tasks, consider implementing a way to check if the pool is shutting down to allow for clean cancellation.
- Avoid creating nested tasks that could lead to deadlocks, especially when waiting for results within another task.
- Consider the granularity of tasks: too fine-grained tasks might not amortize the overhead of task scheduling.

## Potential Pitfalls

- **Deadlocks**: Can occur if tasks wait on each other's results in a circular dependency.
- **Priority Inversion**: Lower-priority tasks holding resources needed by higher-priority tasks.
- **Thread Exhaustion**: All threads becoming blocked on long-running tasks, preventing short tasks from executing.
- **Resource Contention**: High contention on the task queue mutex in high-throughput scenarios.

## Further Reading

For a deeper understanding of thread pools and concurrent programming patterns, consider these resources:

- "C++ Concurrency in Action" by Anthony Williams
- "Pattern-Oriented Software Architecture Volume 2: Patterns for Concurrent and Networked Objects" by Schmidt et al.
- "Effective Modern C++" by Scott Meyers
- "The Art of Multiprocessor Programming" by Maurice Herlihy and Nir Shavit

## License

This code is provided under the MIT License. Feel free to use, modify, and distribute as needed.

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.