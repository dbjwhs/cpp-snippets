# Simple Subnet Ping Scanner

A modern C++23 network discovery tool that scans local subnets for active devices using ICMP ping operations. This project demonstrates advanced C++ networking patterns with Boost.Asio, comprehensive error handling using `std::expected`, and cross-platform compatibility.

[![C++23](https://img.shields.io/badge/C++-23-blue.svg)](https://isocpp.org/std/the-standard)
[![Boost](https://img.shields.io/badge/Boost-1.70+-orange.svg)](https://www.boost.org/)
[![MIT License](https://img.shields.io/badge/License-MIT-green.svg)](https://opensource.org/licenses/MIT)

## 🎯 Features

- **Automatic Network Discovery**: Detects local IP address and scans the entire /24 subnet
- **High-Performance Scanning**: Concurrent ping operations with configurable limits
- **Comprehensive Error Handling**: Railway-oriented programming with `std::expected`
- **Modern C++23**: Leverages latest language features and best practices
- **Cross-Platform**: Works on Windows, macOS, and Linux
- **Rich Output**: Tabular results with hostname resolution and response times
- **Built-in Testing**: Comprehensive test suite with detailed verification

## 🏛️ Architecture & Design Patterns

### Historical Context

This implementation follows established network scanning patterns that date back to early Unix utilities:

- **ICMP Echo (Ping)**: Based on Mike Muuss's original 1983 ping utility
- **Subnet Discovery**: Common pattern used in network administration since the 1980s
- **Concurrent I/O**: Modern asynchronous networking patterns with Boost.Asio
- **Railway-Oriented Programming**: Functional error handling popularized by F# community

### Core Design Patterns

```cpp
// Railway-oriented programming with std::expected
std::expected<std::vector<DeviceInfo>, ScanError> scan_subnet(const std::string& subnet_base);

// RAII resource management
class SubnetScanner {
    boost::asio::io_context& m_io_context;
    std::chrono::milliseconds m_timeout{1000};
    // ...
};

// Modern C++ value semantics
struct DeviceInfo {
    std::string m_ip_address{};
    std::string m_hostname{};
    double m_response_time_ms{};
    bool m_is_active{false};
};
```

## 🚀 Quick Start

### Prerequisites

- **C++23 compatible compiler**: GCC 13+, Clang 16+, or MSVC 19.34+
- **CMake**: Version 3.31 or higher  
- **Boost Libraries**: Version 1.70+ (automatically downloaded if not found)
- **Root/Administrator privileges**: Required for ICMP socket operations

### Building

```bash
# Clone and build
git clone <repository-url>
cd simple_subnet_ping
mkdir build && cd build
cmake ..
make

# Or use system Boost (recommended)
# macOS: brew install boost
# Ubuntu: sudo apt install libboost-all-dev
# Then build as above
```

### Running

```bash
# Run with administrator privileges (required for ICMP)
sudo ./simple_subnet_ping

# Run comprehensive tests
./subnet_ping_tests
```

## 📖 Usage Examples

### Basic Subnet Scan

```bash
$ sudo ./simple_subnet_ping

═══════════════════════════════════════════════════════════════════════════════
                        Simple Subnet Ping Scanner v1.0                        
                               MIT License 2025                                
═══════════════════════════════════════════════════════════════════════════════

🔍 Detecting local network configuration...
✅ Local IP detected: 192.168.1.105
📡 Scanning subnet: 192.168.1.1-254
⏱️  This may take 30-60 seconds depending on network conditions...

🎯 Found 8 active device(s):

IP Address      Hostname                 Response    Status
-----------------------------------------------------------------
192.168.1.1     gateway.local           1.23ms      🟢 Active
192.168.1.2     router.home            2.45ms      🟢 Active
192.168.1.105   my-computer.local      0.12ms      🟢 Active
192.168.1.110   printer.local          15.67ms     🟢 Active
192.168.1.115   smart-tv.local         8.34ms      🟢 Active
192.168.1.120   phone.local            22.11ms     🟢 Active
192.168.1.125   tablet.local           12.89ms     🟢 Active
192.168.1.130   unknown                 45.23ms     🟢 Active

📊 Scan Summary:
   • Subnet: 192.168.1.0/24
   • Active devices: 8/254
   • Scan duration: 23456ms
   • Log file: subnet_ping.log

✨ Scan completed successfully!
```

### Test Suite Output

```bash
$ ./subnet_ping_tests

🧪 Running comprehensive test suite...

Test 1: SubnetScanner initialization... ✅ PASSED
Test 2: PingManager initialization... ✅ PASSED
Test 3: Local IP detection... ✅ PASSED (detected: 10.0.0.219)
Test 4: Subnet extraction... ✅ PASSED
Test 5: IP address generation... ✅ PASSED (generated 254 addresses)
Test 6: Loopback ping test... ⚠️  WARNING: Loopback ping failed (may require permissions)

🎉 Test suite completed!
```

## 🛠️ Configuration

### Scanning Parameters

The scanner can be configured programmatically:

```cpp
SubnetScanner scanner{io_context};
scanner.set_timeout(std::chrono::milliseconds{1500});  // Ping timeout
scanner.set_max_concurrent(50);                        // Max concurrent pings

PingManager ping_manager{io_context};
ping_manager.set_timeout(std::chrono::milliseconds{1000});
ping_manager.set_retry_count(3);  // Retry failed pings
```

### Error Handling

All operations use `std::expected` for clean error propagation:

```cpp
auto result = scanner.scan_subnet("192.168.1");
if (result) {
    // Success: process discovered devices
    for (const auto& device : result.value()) {
        // Handle device
    }
} else {
    // Error: handle specific error types
    switch (result.error()) {
        case ScanError::permission_denied:
            std::cout << "Run with sudo for ICMP access\n";
            break;
        // ... other error types
    }
}
```

## 🏗️ Project Structure

```
simple_subnet_ping/
├── README.md                 # This file
├── CMakeLists.txt           # Build configuration with Boost auto-download
├── main.cpp                 # Application entry point and UI
├── headers/                 # Header files
│   ├── subnet_scanner.hpp   # Network discovery and subnet scanning
│   └── ping_manager.hpp     # ICMP ping operations and packet handling
├── src/                     # Implementation files
│   ├── subnet_scanner.cpp   # SubnetScanner implementation
│   └── ping_manager.cpp     # PingManager implementation
└── build/                   # Build artifacts (generated)
    ├── simple_subnet_ping   # Main executable
    └── subnet_ping_tests    # Test executable
```

## 🔧 Technical Details

### Network Operations

- **Protocol**: ICMP Echo Request/Reply (RFC 792)
- **Concurrency**: Configurable async operations (default: 25 concurrent pings)
- **Timeout**: Per-ping timeout (default: 1000ms)
- **Retry Logic**: Configurable retry attempts for failed pings

### Memory Management

- **RAII**: All resources properly managed with RAII principles
- **Smart Pointers**: `std::unique_ptr` for socket management
- **Value Semantics**: Modern C++ value types throughout
- **Zero-Copy**: Efficient buffer management where possible

### Error Categories

```cpp
enum class ScanError {
    network_interface_error,  // Cannot detect local IP
    invalid_subnet,          // Malformed subnet specification
    permission_denied,       // ICMP socket requires root/admin
    timeout_exceeded,        // Network operation timed out
    system_error            // General system error
};
```

## 🧪 Testing

### Test Coverage

- ✅ Component initialization and teardown
- ✅ Network interface detection
- ✅ IP address validation and subnet extraction  
- ✅ ICMP packet creation and parsing
- ✅ Concurrent operation management
- ✅ Error handling and edge cases

### Running Tests

```bash
# Basic test run
./subnet_ping_tests

# With verbose logging
BOOST_LOG_TRIVIAL_SEVERITY=debug ./subnet_ping_tests

# Memory leak detection (with valgrind on Linux)
valgrind --leak-check=full ./subnet_ping_tests
```

## ⚠️ Security Considerations

### ICMP Socket Privileges

ICMP raw sockets require elevated privileges:

- **Linux/macOS**: Run with `sudo` 
- **Windows**: Run as Administrator
- **Alternative**: Use `setcap` on Linux: `sudo setcap cap_net_raw+ep ./simple_subnet_ping`

### Network Safety

- **Rate Limiting**: Built-in concurrency limits prevent network flooding
- **Timeout Control**: Prevents hanging on unresponsive hosts
- **Local Scope**: Only scans local subnet, doesn't cross network boundaries

## 🤝 Contributing

### Development Setup

```bash
# Install development dependencies
# macOS
brew install boost cmake ninja

# Ubuntu  
sudo apt update && sudo apt install -y \
    build-essential cmake ninja-build \
    libboost-all-dev

# Build with development flags
cmake -DCMAKE_BUILD_TYPE=Debug -GNinja ..
ninja
```

### Code Style

- **Modern C++**: C++23 features preferred
- **Const Correctness**: All methods marked `const` where applicable
- **Error Handling**: Use `std::expected`, avoid exceptions for control flow
- **Naming**: Snake_case for variables, PascalCase for classes
- **Documentation**: Comprehensive header comments with historical context

### Submitting Changes

1. Fork the repository
2. Create feature branch (`git checkout -b feature/amazing-feature`)
3. Run tests (`./subnet_ping_tests`)
4. Commit changes (`git commit -m 'Add amazing feature'`)  
5. Push to branch (`git push origin feature/amazing-feature`)
6. Open Pull Request

## 📚 Dependencies

### Required
- **C++23 Standard Library**: Modern language features
- **Boost.Asio**: Cross-platform networking (header-only components)
- **Boost.System**: Error code handling
- **Threading**: `std::thread`, `std::future` for concurrency

### Optional  
- **Boost Libraries**: Full installation (auto-downloaded if missing)
- **CMake**: Build system (3.31+)
- **Ninja**: Fast build backend (optional)

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

```
MIT License

Copyright (c) 2025 dbjwhs

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

## 🙏 Acknowledgments

- **Mike Muuss**: Original ping utility implementation (1983)
- **Boost Community**: Excellent networking libraries
- **C++ Standards Committee**: Modern language features enabling clean network code
- **Unix/Linux Community**: Decades of network programming patterns and practices
