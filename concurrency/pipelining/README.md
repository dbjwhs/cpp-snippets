# Thread Pipeline Example

## Understanding Thread Pipelines

A thread pipeline is a concurrent programming pattern where a series of processing stages are connected in sequence, with each stage running in its own thread. Data flows through these stages like an assembly line, with each stage performing a specific operation on the data before passing it to the next stage.

### Key Concepts

1. **Pipeline Stages**: Each stage is an independent processing unit that:
    - Receives input from the previous stage
    - Performs its specific processing
    - Passes results to the next stage
    - Runs concurrently with other stages

2. **Thread-Safe Queues**: Communication between stages is managed through thread-safe queues that:
    - Buffer data between stages
    - Handle synchronization
    - Prevent data races
    - Manage backpressure

3. **Data Flow**: Items move through the pipeline sequentially, enabling:
    - Parallel processing of different items at different stages
    - Continuous streaming of data
    - Efficient resource utilization

### Common Use Cases

1. **Image Processing**
    - Stage 1: Load image data
    - Stage 2: Apply filters
    - Stage 3: Perform feature detection
    - Stage 4: Save processed results

2. **Data ETL (Extract, Transform, Load)**
    - Stage 1: Extract data from source
    - Stage 2: Clean and validate
    - Stage 3: Transform format
    - Stage 4: Load into destination

3. **Video Processing**
    - Stage 1: Decode video frames
    - Stage 2: Apply effects
    - Stage 3: Encode processed frames
    - Stage 4: Write to output file

4. **Network Packet Processing**
    - Stage 1: Receive packets
    - Stage 2: Decrypt
    - Stage 3: Process payload
    - Stage 4: Route to destination

## Implementation Details

### SafeQueue Class
```cpp
template<typename T>
class SafeQueue {
    // Thread-safe queue implementation
    // Handles concurrent access to data
}
```

The SafeQueue class provides:
- Thread-safe push and pop operations
- Condition variables for synchronization
- Atomic shutdown signaling

### PipelineStage Base Class
```cpp
class PipelineStage {
    // Base class for pipeline stages
    // Defines common functionality
}
```

Features:
- Virtual processing interface
- Input/output queue management
- Stage identification
- Thread management

### Example Stages
1. MultiplyStage: Multiplies input by 2
2. AddStage: Adds 10 to input
3. FilterStage: Passes only even numbers

## Building and Running

### Prerequisites
- C++17 compatible compiler
- Standard library with thread support

### Compilation
```bash
g++ -std=c++17 pipeline.cpp -pthread -o pipeline
```

### Execution
```bash
./pipeline
```

## Example Output
```
Multiply Stage processing item: 1
Multiply Stage processing item: 2
Add Stage processing item: 2
Add Stage processing item: 4
Filter Stage processing item: 12
Filter Stage processing item: 14
Final output: 12
Final output: 14
...
```

## Performance Considerations

1. **Queue Size**
    - Larger queues increase memory usage
    - Smaller queues may cause blocking
    - Balance based on data flow characteristics

2. **Thread Scheduling**
    - CPU core availability affects performance
    - Consider processor affinity for critical stages
    - Monitor thread contention

3. **Data Batch Size**
    - Larger batches improve throughput
    - Smaller batches reduce latency
    - Choose based on application requirements

## Best Practices

1. **Error Handling**
    - Implement robust error propagation
    - Handle stage failures gracefully
    - Maintain pipeline integrity

2. **Resource Management**
    - Clean shutdown of all stages
    - Proper thread joining
    - Memory leak prevention

3. **Monitoring**
    - Track queue depths
    - Measure stage processing times
    - Identify bottlenecks

## Contributing

Feel free to submit issues and enhancement requests!

## License

This project is licensed under the MIT License - see the LICENSE file for details.
