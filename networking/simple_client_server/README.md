# TCP Client-Server Pattern with Hash-based Message Integrity Verification

This project demonstrates a comprehensive implementation of the TCP client-server networking pattern enhanced with 
hash-based message integrity verification using C++23 and Boost.Asio. The pattern evolved from early distributed 
computing challenges where network unreliability required robust error detection mechanisms. Originally developed for 
mainframe-to-terminal communication in the 1970s, this pattern gained prominence during the client-server revolution 
of the 1980s and 1990s. The addition of cryptographic hash verification addresses modern concerns about data integrity 
across potentially compromised or unreliable network infrastructure, making it particularly valuable for microservice 
architectures, IoT systems, and distributed databases where message corruption could have significant consequences.

This pattern solves several critical problems in distributed systems: network transmission errors that can corrupt 
data, the need for efficient error detection without complex error correction protocols, and the requirement for 
reliable acknowledgment mechanisms in asynchronous communication systems. Unlike simple checksums, hash verification 
provides strong integrity guarantees while maintaining low computational overhead. The Railway-Oriented Programming 
approach using std::expected eliminates exception-based error handling, making the code more predictable and easier 
to reason about in concurrent environments. This combination makes the pattern particularly suitable for high-frequency 
trading systems, real-time data streaming, IoT sensor networks, distributed logging systems, and any application 
requiring reliable message delivery with integrity verification.

## Features

- **Asynchronous TCP Server**: Built with Boost.Asio, handles multiple concurrent connections efficiently
- **Synchronous TCP Client**: Simple, reliable client implementation with comprehensive error handling  
- **Hash Verification**: SHA-256-based message integrity verification ensures data transmission accuracy
- **Railway-Oriented Programming**: Clean error handling using std::expected without exceptions
- **RAII Resource Management**: Automatic cleanup of network resources and memory management
- **Comprehensive Testing**: Extensive test suite covering success scenarios, error conditions, and edge cases
- **C++23 Compliance**: Modern C++ features including std::expected, std::format, and constexpr

## Building

The project uses CMake with automatic Boost dependency management:

```bash
mkdir build
cd build
cmake ..
make
```

This will create three executables in the `bin/` directory:
- `server`: The TCP server application
- `client`: The TCP client application  
- `main`: Comprehensive demonstration with testing

## Usage Examples

### Basic Server Operation

```bash
# Start server on default port 8080
./bin/server

# Start server on custom port
./bin/server 9000
```

### Client Communication

```bash
# Send message to default server (localhost:8080)
./bin/client "Hello, World!"

# Send message to custom server
./bin/client "Test message" localhost 9000

# Send message to remote server
./bin/client "Remote test" 192.168.1.100 8080
```

### Example Communication Flow

```bash
# Terminal 1 - Start server
$ ./bin/server 8080
[INFO] Initializing TCP server on port 8080
[INFO] Server listening on port 8080
[INFO] Accepted new client connection
[INFO] Received message: 'Hello, Network!'
[INFO] Computed hash: 1a2b3c4d
[INFO] Response sent successfully

# Terminal 2 - Send message
$ ./bin/client "Hello, Network!"
[INFO] Initializing TCP client for localhost:8080
[INFO] Connecting to server localhost:8080
[INFO] Successfully connected to server
[INFO] Sending message: 'Hello, Network!' with expected hash: 1a2b3c4d
[INFO] Message sent successfully
[INFO] Received response: 'ACK:1a2b3c4d'
[INFO] Hash verification successful - message integrity confirmed
```

## Testing and Demonstration

Run the comprehensive test suite:

```bash
./bin/main
```

The test suite includes:
- Basic client-server communication verification
- Multiple sequential message handling
- Hash verification and corruption detection
- Error handling for connection failures
- Network timeout and retry scenarios

## Good Practices Demonstrated

### Memory Management
- RAII-based resource cleanup with automatic socket management
- Smart pointer usage for connection lifetime management
- Container reserve() calls when size is known ahead of time
- Proper buffer initialization and bounds checking

### Error Handling
- Railway-Oriented Programming with std::expected
- Comprehensive error categorization and propagation
- Graceful degradation on network failures
- Proper cleanup in error scenarios

### Performance Optimizations
- Asynchronous I/O for server scalability
- Efficient hash computation algorithms
- Minimal memory allocations in hot paths
- Thread-safe connection handling

### Security Considerations
- Hash-based message integrity verification
- Input validation and sanitization
- Protection against buffer overflow attacks
- Secure random number generation for session management

## Bad Practices to Avoid

### Common Anti-patterns
```cpp
// BAD: Exception-based error handling in network code
try {
    socket.connect(endpoint);
} catch (...) {
    // Generic catch-all loses error context
}

// GOOD: Expected-based error handling
auto result = connect_to_server();
if (!result) {
    handle_specific_error(result.error());
}
```

### Resource Management Issues
```cpp
// BAD: Manual resource management
tcp::socket* socket = new tcp::socket(io_context);
// ... code that might throw
delete socket; // May never execute

// GOOD: RAII with smart pointers
auto socket = std::make_unique<tcp::socket>(io_context);
// Automatic cleanup regardless of exit path
```

### Hash Verification Mistakes
```cpp
// BAD: Ignoring hash verification
send_message(data);
// Assuming message arrived correctly

// GOOD: Always verify integrity
auto result = send_message_with_verification(data);
if (!result) {
    handle_transmission_error(result.error());
}
```

## Books and References

### Network Programming
- **"UNIX Network Programming, Volume 1" by W. Richard Stevens** - Foundational text covering socket programming, 
  TCP/IP protocols, and client-server architectures. Essential reading for understanding the underlying mechanics 
  of network communication.

- **"Effective TCP/IP Programming" by Jon C. Snader** - Practical guide to robust network programming with 
  comprehensive coverage of error handling, performance optimization, and security considerations.

- **"Network Programming with C++" by Jon Snader** - Modern C++ approaches to network programming including 
  asynchronous I/O patterns and Boost.Asio usage.

### Design Patterns and Architecture  
- **"Railway-Oriented Programming" by Scott Wlaschin** - Functional programming approach to error handling that 
  inspired the std::expected pattern used throughout this implementation.

- **"Patterns of Enterprise Application Architecture" by Martin Fowler** - Contains detailed discussion of 
  client-server patterns, message integrity, and distributed system design principles.

- **"Building Microservices" by Sam Newman** - Modern perspective on distributed communication patterns including 
  service-to-service communication and reliability patterns.

### C++ and Modern Programming
- **"C++23: The Complete Guide" by Nicolai M. Josuttis** - Comprehensive coverage of modern C++ features including 
  std::expected, concepts, and coroutines used in this implementation.

- **"Concurrency in Action" by Anthony Williams** - Essential for understanding thread safety, asynchronous 
  programming, and concurrent design patterns in network applications.

- **"Effective Modern C++" by Scott Meyers** - Best practices for modern C++ development including RAII, smart 
  pointers, and exception safety relevant to network programming.

## License

This code is provided under the MIT License. Feel free to use, modify, and distribute as needed.

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request
