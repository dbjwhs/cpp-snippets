# Barrier Pattern Implementation in C++

## Overview
The Barrier Pattern is a synchronization mechanism used in concurrent programming to ensure that multiple threads wait for each other to reach a specific point before any of them can proceed further. It's like establishing a checkpoint where all threads must arrive before the race can continue.

## Real-World Use Cases

### 1. Parallel Matrix Computation
When computing large matrices in parallel, each thread might handle a portion of the matrix. Before moving to the next phase of computation, all threads need to complete their current phase to ensure data consistency.

```
Thread 1: [Computing A1] → [Barrier Wait] → [Computing B1]
Thread 2: [Computing A2] → [Barrier Wait] → [Computing B2]
Thread 3: [Computing A3] → [Barrier Wait] → [Computing B3]
```

### 2. Game Physics Engine
In a game physics engine, different threads might handle different aspects of the simulation:
- Thread 1: Position updates
- Thread 2: Collision detection
- Thread 3: Force calculations

All these computations must complete before moving to the next frame to maintain simulation accuracy.

### 3. Image Processing Pipeline
When processing images in parallel:
```
Stage 1: Multiple threads apply filters
Stage 2: All threads wait at barrier
Stage 3: Threads proceed with edge detection
Stage 4: All threads wait at barrier
Stage 5: Final composition
```

## Implementation Details

This repository provides two implementations of the Barrier Pattern:

### 1. Modern Implementation (C++20)
Uses the standard library's `std::barrier` class, which provides a clean and efficient implementation:

```cpp
std::barrier barrier(num_threads);
// ... in thread function:
barrier.arrive_and_wait();
```

Key features:
- Thread-safe by design
- Optimized performance
- Clear, modern syntax
- Automatic reset after all threads arrive

### 2. Custom Implementation
A manual implementation using traditional synchronization primitives:
- Uses `std::mutex` and `std::condition_variable`
- Maintains internal counter and phase
- Demonstrates the underlying mechanics of a barrier

```cpp
class CustomBarrier {
private:
    const size_t m_threadCount;
    size_t m_counter;
    // ... other members

public:
    void wait();
    // ... other methods
};
```

### A special note on condition variable lambda usage

```cpp
m_cv.wait(lock, [this, phase_copy] { return phase_copy != m_phase; });
```
Let me break down this line of code which is part of the condition variable wait operation:

1. `m_cv.wait()` - This is calling the wait method on our condition variable (m_cv)

2. It takes two parameters:
    - `lock`: A unique_lock that will be automatically unlocked while waiting and relocked when waking up
    - A lambda function that serves as the predicate/condition to check

3. `[this, phase_copy]` - This is the lambda capture clause:
    - `this`: Captures the current object pointer so we can access member variables
    - `phase_copy`: Captures the local phase_copy variable by value

4. `{ return phase_copy != m_phase; }` - This is the lambda body:
    - Returns true when the stored phase_copy is different from the current m_phase
    - This indicates that the barrier has moved to a new phase

The line essentially means:
- Wait until the barrier's phase changes
- While waiting, release the lock so other threads can proceed
- Periodically wake up to check if the condition is true
- When the condition becomes true (phase has changed), continue execution

This is part of the barrier mechanism because:
1. When the last thread arrives, it changes m_phase
2. This makes the condition true for all waiting threads
3. All waiting threads can then proceed to the next phase

The use of phase_copy helps avoid the "spurious wakeup" problem by ensuring each thread only proceeds when the barrier has genuinely moved to a new phase, rather than just when it receives a notification.

Without this predicate, threads might wake up falsely due to spurious wakeups, or miss a notification if it comes before they start waiting. The condition variable predicate pattern ensures robust synchronization.

As we first saw above with the C++20 addition of `std::barrier` things have gotten a little easier.

## Usage Example

```cpp
const int NUM_THREADS = 4;

// Using modern barrier
ModernBarrierExample::demonstrate(NUM_THREADS);

// Using custom barrier
CustomBarrierExample::demonstrate(NUM_THREADS);
```

## Code Style Conventions
This implementation follows these coding conventions:
- Class member variables are prefixed with `m_`
- Comments use // style instead of /* */
- All comments are in lowercase
- Deleted functions are declared in public section

## Building and Running

### Prerequisites
- C++20 compatible compiler
- CMake 3.15 or higher

### Build Commands
```bash
mkdir build
cd build
cmake ..
make
```

### Running the Example
```bash
./barrier_example
```

## Output Example
```
Demonstrating std::barrier implementation:
Thread 0 completed phase 1
Thread 2 completed phase 1
Thread 1 completed phase 1
Thread 3 completed phase 1
Thread 0 starting phase 2
Thread 2 starting phase 2
Thread 1 starting phase 2
Thread 3 starting phase 2
...
```

## Thread Safety
Both implementations are thread-safe and provide the following guarantees:
- All threads will wait until the last thread arrives
- No thread can proceed until all threads have arrived
- The barrier automatically resets for the next phase

## Performance Considerations
- The modern `std::barrier` implementation is generally more efficient
- The custom implementation might have more overhead due to mutex operations
- Both implementations scale well with the number of threads

## License
This project is licensed under the MIT License - see the LICENSE file for details.

## Contributing
Contributions are welcome! Please feel free to submit pull requests with improvements or bug fixes.