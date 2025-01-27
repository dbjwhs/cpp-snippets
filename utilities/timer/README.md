# High Resolution Timer Class

A modern C++ implementation of a high-precision timer using the `std::chrono` library. This class provides accurate timing measurements with nanosecond precision and multiple output formats.

## Features

- High-precision timing using `std::chrono::high_resolution_clock`
- Multiple time unit outputs (nanoseconds, microseconds, milliseconds, seconds)
- Automatic formatting with appropriate units
- Runtime status checking
- Thread-safe implementation
- Simple start/stop/reset interface

## Requirements

- C++11 or later
- Standard C++ libraries (`chrono`, `string`, `sstream`, `iomanip`)

## Usage

### Basic Usage

```cpp
HighResolutionTimer timer;

// Start timing
timer.start();

// Your code to measure here
do_something();

// Stop timing
timer.stop();

// Get the elapsed time
std::cout << "Operation took: " << timer.elapsed_formatted() << std::endl;
```

### Available Methods

#### Timer Control
- `void start()`: Starts the timer
- `void stop()`: Stops the timer
- `void reset()`: Resets the timer
- `bool running()`: Checks if the timer is currently running

#### Time Measurements
- `double elapsed_nanoseconds()`: Get elapsed time in nanoseconds
- `double elapsed_microseconds()`: Get elapsed time in microseconds
- `double elapsed_milliseconds()`: Get elapsed time in milliseconds
- `double elapsed_seconds()`: Get elapsed time in seconds
- `std::string elapsed_formatted()`: Get formatted string with appropriate unit

### Example with Multiple Features

```cpp
HighResolutionTimer timer;

// Basic timing
timer.start();
complex_operation();
timer.stop();

// Get results in different formats
std::cout << "Raw nanoseconds: " << timer.elapsed_nanoseconds() << std::endl;
std::cout << "Formatted time: " << timer.elapsed_formatted() << std::endl;

// Check timer status
if (!timer.running()) {
    std::cout << "Timer is stopped" << std::endl;
}

// Multiple measurements
timer.reset();
timer.start();
operation_1();
std::cout << "Time so far: " << timer.elapsed_formatted() << std::endl;
operation_2();
timer.stop();
std::cout << "Final time: " << timer.elapsed_formatted() << std::endl;
```

## Output Format

The `elapsed_formatted()` method automatically selects the most appropriate unit:
- Under 1 microsecond: displays in nanoseconds (ns)
- Under 1 millisecond: displays in microseconds (µs)
- Under 1 second: displays in milliseconds (ms)
- 1 second or more: displays in seconds (s)

Example outputs:
```
856 ns
1.234 µs
12.345 ms
1.234 s
```

## Implementation Details

The timer uses `std::chrono::high_resolution_clock` for maximum precision. Time measurements are stored internally as time points and converted to the requested unit only when queried. This ensures no precision is lost during timing operations.

## Performance Considerations

- The timer has minimal overhead, making it suitable for measuring both very short and long operations
- Time point storage uses the system's highest precision clock
- All time unit conversions are performed using standard C++ duration casts
- The formatted output includes a small overhead for string formatting

## Thread Safety

The timer is thread-safe for independent instances. However, sharing a single timer instance between threads requires external synchronization.

## License

This code is provided under the MIT License. Feel free to use, modify, and distribute as needed.

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.