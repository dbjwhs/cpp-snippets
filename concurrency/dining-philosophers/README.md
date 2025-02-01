# Dining Philosophers Problem Implementation

## The Problem and Its Challenges

The Dining Philosophers Problem is a classic computer science problem that illustrates challenges in resource allocation and deadlock prevention. Originally formulated by Edsger Dijkstra in 1965, it models a scenario where five philosophers sit around a circular table, each facing a bowl of spaghetti. Between each pair of philosophers is a single fork, and a philosopher needs both their left and right forks to eat.

### The Basic Setup
- 5 philosophers sit around a circular table
- 5 forks are placed between them (one between each pair)
- To eat, a philosopher needs both adjacent forks
- When not eating, philosophers think
- Forks can only be used by one philosopher at a time

### Core Challenges

1. **Deadlock Prevention**
    - If each philosopher picks up their left fork simultaneously, no one can pick up their right fork
    - This results in a circular wait condition where no philosopher can proceed
    - This is a classic example of deadlock in concurrent systems

### Understanding the Circular Wait Condition

A circular wait condition occurs when each process in a set of processes is waiting for a resource held by another process in the set, forming a circular chain of dependencies. In the Dining Philosophers context:

1. **Chain of Dependencies**
    - Philosopher 0 holds Fork 0, waits for Fork 1
    - Philosopher 1 holds Fork 1, waits for Fork 2
    - Philosopher 2 holds Fork 2, waits for Fork 3
    - Philosopher 3 holds Fork 3, waits for Fork 4
    - Philosopher 4 holds Fork 4, waits for Fork 0

2. **Visualization of the Deadlock**
```
Phil 0 --holds--> Fork 0 <--needs-- Phil 4
   |                                  ^
   needs                             holds
   |                                  |
   v                                  |
Fork 1 <--needs-- Phil 1 ... and so on
```

3. **Properties of Circular Wait**
    - Each philosopher is in a state of waiting
    - The resources they wait for are held by another philosopher
    - The pattern forms an unbreakable cycle
    - No philosopher can proceed without the cycle being broken

4. **Breaking the Circular Wait**
   This solution breaks this condition by:
    - Introducing asymmetry in fork pickup order
    - Even-numbered philosophers: left fork first
    - Odd-numbered philosophers: right fork first
    - This prevents the formation of the complete cycle
    - I read about this technique a while back, I find the reference to the article I will circle back and add it here.
    - *TODO Find reference for above idea*

2. **Resource Competition**
    - Forks are shared resources between adjacent philosophers
    - Efficient resource allocation is necessary to maximize concurrent eating
    - Poor allocation strategies can lead to starvation (philosophers unable to eat)

3. **Starvation Prevention**
    - Solutions must ensure that no philosopher starves
    - Each philosopher should have a fair chance to eat
    - The system should maintain progress and liveness

4. **Concurrency Management**
    - Multiple philosophers must operate independently
    - Resource access must be synchronized
    - Race conditions must be prevented

## Implementation Details

This implementation uses modern C++ features to solve the Dining Philosophers Problem. Key aspects include:

### Resource Management
- Each fork is represented by a `std::mutex`
- `std::lock_guard` ensures proper mutex locking/unlocking
- Fork ownership is managed through pointer references

### Deadlock Prevention Strategy
- Even-numbered philosophers pick up left fork first
- Odd-numbered philosophers pick up right fork first
- This asymmetric approach prevents the circular wait condition

### Thread Safety
- Uses `std::unique_ptr` for safe philosopher object management
- Proper synchronization using mutex locks
- Thread-safe console output for status messages

## Building and Running

### Prerequisites
- C++11 or later compiler
- Standard library with threading support

### Compilation
```bash
g++ -std=c++11 dining_philosophers.cpp -o dining_philosophers -pthread
```

### Running
```bash
./dining_philosophers
```

## Code Structure

The implementation consists of two main components:

1. **Philosopher Class**
    - Manages individual philosopher behavior
    - Handles fork acquisition and release
    - Implements thinking and eating cycles

2. **Main Program**
    - Creates and initializes resources (forks)
    - Spawns philosopher threads
    - Manages program lifecycle

## Output Description

The program provides real-time status updates showing:
- When philosophers start/stop thinking
- When philosophers pick up/put down forks
- When philosophers start/finish eating
- Completion of the dining session

## Implementation Notes

1. **Fork Management**
    - Forks are managed using a vector of mutexes
    - Each philosopher has pointers to their left and right forks
    - Mutex pointers ensure stable references throughout execution

2. **Thread Handling**
    - Each philosopher runs in their own thread
    - Thread synchronization is handled through mutex locks
    - Proper thread joining ensures clean program termination

3. **Memory Management**
    - Smart pointers (`std::unique_ptr`) handle philosopher object lifetime
    - Vector container manages fork collection
    - Automatic resource cleanup through RAII

## Limitations and Considerations

1. **Fixed Configuration**
    - Number of philosophers is fixed at compile time
    - Number of meals per philosopher is hardcoded
    - Thinking and eating times are randomly generated within fixed ranges

2. **Resource Overhead**
    - Each philosopher requires a separate thread
    - Mutex operations have associated performance costs
    - Console output may impact performance

3. **Scalability**
    - Performance may degrade with larger numbers of philosophers
    - Console output becomes more cluttered with more philosophers
    - Resource consumption increases linearly with philosopher count

## License
This implementation is provided under the MIT License. See the LICENSE file for details.

## Contributing
Contributions are welcome! Please feel free to submit pull requests with improvements or bug fixes.