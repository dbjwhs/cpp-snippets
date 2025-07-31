// MIT License
// Copyright (c) 2025 dbjwhs

#include <execinfo.h>
#include <iostream>
#include <memory>
#include <cxxabi.h>
#include <cstdlib>
#include <string>
#include "../../headers/project_utils.hpp"

// Debug Stack Trace Pattern with Integrated Logging
//
// This pattern combines traditional Unix backtrace functionality with modern C++ logging to create a comprehensive
// debugging solution. The approach has its roots in early Unix debugging tools like dbx and gdb, but modernizes
// the concept by integrating with structured logging systems. This pattern emerged from the need to capture
// stack traces at runtime for error reporting while maintaining clean, readable log output.
//
// The pattern works by wrapping the POSIX backtrace() and backtrace_symbols() functions with C++ RAII principles
// and integrating them with macro-based logging systems. The backtrace() function captures the current call stack
// by walking the stack frames, while backtrace_symbols() converts the raw addresses into human-readable symbols.
// The integration with logging macros allows for consistent formatting and output routing.
//
// Key components of this pattern include:
// 1. Stack frame capture using backtrace() - captures raw stack addresses
// 2. Symbol resolution using backtrace_symbols() - converts addresses to function names
// 3. C++ name demangling using abi::__cxa_demangle() - makes C++ function names readable
// 4. Integration with structured logging - ensures consistent output formatting
// 5. RAII memory management - automatic cleanup of backtrace_symbols() allocated memory
//
// Common usage patterns:
// - Exception handling: Capture stack traces when exceptions are thrown
// - Error reporting: Include stack context in error logs for debugging
// - Performance debugging: Track call paths in performance-critical code
// - Debug builds: Enable detailed tracing in development environments
// - Crash reporting: Generate stack traces for post-mortem analysis
//
// This pattern is particularly valuable in large codebases where understanding the call chain leading to an
// error is crucial for debugging. It provides a bridge between low-level debugging information and high-level
// application logging, making it easier to diagnose issues in production environments.

class stacktrace_capture {
private:
    std::vector<void*> addresses_;
    std::vector<std::string> raw_symbols_;

    void capture_addresses(int max_frames = 20) {
        addresses_.resize(max_frames);
        const int actual_size = backtrace(addresses_.data(), max_frames);
        addresses_.resize(actual_size);
    }

    void resolve_symbols() {
        if (addresses_.empty()) return;

        // Use RAII wrapper for backtrace_symbols
        struct backtrace_symbols_deleter {
            void operator()(char** ptr) const {
                if (ptr) free(ptr);
            }
        };

        const std::unique_ptr<char *, backtrace_symbols_deleter> symbols(
            backtrace_symbols(addresses_.data(), static_cast<int>(addresses_.size()))
        );

        if (!symbols) {
            LOG_ERROR("Failed to resolve backtrace symbols");
            return;
        }

        raw_symbols_.reserve(addresses_.size());
        for (size_t i = 0; i < addresses_.size(); ++i) {
            raw_symbols_.emplace_back(symbols.get()[i]);
        }
    }

    [[nodiscard]] std::string demangle_symbol(const std::string& mangled_name) const {
        if (mangled_name.empty() || mangled_name.substr(0, 2) == "0x") {
            return mangled_name;
        }

        struct demangle_deleter {
            void operator()(char* ptr) const {
                if (ptr) {
                    free(ptr);
                }
            }
        };

        int status;
        const std::unique_ptr<char, demangle_deleter> demangled(
            abi::__cxa_demangle(mangled_name.c_str(), nullptr, nullptr, &status)
        );

        return (status == 0 && demangled) ? std::string(demangled.get()) : mangled_name;
    }

    [[nodiscard]] std::string extract_and_demangle_frame(const std::string& raw_frame) const {
        // macOS format: "frame_number   binary_name   address   mangled_name + offset"
        const size_t plus_pos = raw_frame.rfind(" + ");
        if (plus_pos == std::string::npos) {
            return raw_frame; // Can't parse, return as-is
        }

        size_t name_start = raw_frame.rfind(' ', plus_pos - 1);
        if (name_start == std::string::npos) {
            return raw_frame; // Can't find name start
        }

        name_start++; // Move past the space
        std::string mangled_name = raw_frame.substr(name_start, plus_pos - name_start);

        // Clean whitespace
        mangled_name.erase(0, mangled_name.find_first_not_of(" \t"));
        mangled_name.erase(mangled_name.find_last_not_of(" \t") + 1);

        // Reconstruct the frame with demangled name
        if (const std::string demangled_name = demangle_symbol(mangled_name); demangled_name != mangled_name) {
            return raw_frame.substr(0, name_start) + demangled_name + raw_frame.substr(plus_pos);
        }

        return raw_frame;
    }

public:
    stacktrace_capture() {
        capture_addresses();
        resolve_symbols();
    }

    [[nodiscard]] const std::vector<std::string>& get_raw_symbols() const {
        return raw_symbols_;
    }

    [[nodiscard]] std::vector<std::string> get_demangled_frames() const {
        std::vector<std::string> demangled_frames;
        demangled_frames.reserve(raw_symbols_.size());

        for (const auto& raw_frame : raw_symbols_) {
            demangled_frames.push_back(extract_and_demangle_frame(raw_frame));
        }

        return demangled_frames;
    }

    [[nodiscard]] size_t frame_count() const {
        return addresses_.size();
    }
};

void print_stacktrace() {
    const stacktrace_capture capture;

    LOG_INFO(std::format("Stack trace ({} frames):", capture.frame_count()));

    const auto& raw_symbols = capture.get_raw_symbols();
    auto demangled_frames = capture.get_demangled_frames();

    // Debug output showing the processing steps
    for (size_t ndx = 0; ndx < raw_symbols.size(); ++ndx) {
        LOG_INFO(std::format("  Raw frame: {}", raw_symbols[ndx]));

        // Extract mangled name for debug output
        const std::string& raw_frame = raw_symbols[ndx];
        if (const size_t plus_pos = raw_frame.rfind(" + "); plus_pos != std::string::npos) {
            if (size_t name_start = raw_frame.rfind(' ', plus_pos - 1); name_start != std::string::npos) {
                name_start++;
                std::string mangled = raw_frame.substr(name_start, plus_pos - name_start);
                mangled.erase(0, mangled.find_first_not_of(" \t"));
                mangled.erase(mangled.find_last_not_of(" \t") + 1);

                LOG_INFO(std::format("  Extracted symbol: '{}'", mangled));

                // Check if this looks like a mangled C++ name (starts with _Z)
                if (!mangled.empty() && mangled.length() > 2 && mangled.substr(0, 2) == "_Z") {
                    LOG_INFO(std::format("  [{}] Found mangled C++ symbol: {}", ndx, mangled));
                } else if (!mangled.empty() && mangled.substr(0, 2) != "0x") {
                    LOG_INFO(std::format("  [{}] Found C symbol or already demangled: {}", ndx, mangled));
                } else {
                    LOG_INFO(std::format("  [{}] Address only: {}", ndx, mangled));
                }
            }
        }
        LOG_INFO(std::format("  [{}] {}", ndx, demangled_frames[ndx]));
    }

    // Output the final cleaned stack trace
    LOG_INFO("=== FINAL DEMANGLED STACK TRACE ===");
    for (size_t ndx = 0; ndx < demangled_frames.size(); ++ndx) {
        LOG_INFO(std::format("[{}] {}", ndx, demangled_frames[ndx]));
    }
    LOG_INFO("=== END STACK TRACE ===");
}

// Test functions to demonstrate C++ name mangling
namespace test_namespace {
    template<typename T>
    class complex_class {
    public:
        static void template_method(const std::vector<T>& data, std::string_view name) {
            LOG_INFO(std::format("In template_method with {} items", data.size()));
            print_stacktrace();
        }
    };

    void overloaded_function(int x) {
        LOG_INFO(std::format("overloaded_function(int): {}", x));
        complex_class<std::string>::template_method({"test", "data"}, "example");
    }

    void overloaded_function(double x, const std::string& name) {
        LOG_INFO(std::format("overloaded_function(double, string): {} {}", x, name));
        print_stacktrace();
    }
}

void function_c() {
    LOG_INFO("In function_c, calling C++ template function");
    test_namespace::overloaded_function(42);
}

void function_b() {
    LOG_INFO("In function_b, calling function_c");
    function_c();
}

void function_a() {
    LOG_INFO("In function_a, calling function_b");
    function_b();
}


// some simple test cases
#define ENABLE_TESTS

int main() {
    LOG_INFO("Starting main, calling function_a");
    function_a();

    LOG_INFO("--- Direct stack trace from main ---");
    print_stacktrace();

#ifdef ENABLE_TESTS
    void run_tests();
    run_tests();
#endif

    return 0;
}

#ifdef ENABLE_TESTS
#include <cassert>

// my test function to verify demangling works
void test_demangling() {
    // Test with a known mangled name
    auto test_mangled = "_Z16print_stacktracev";
    int status;

    if (auto demangled = abi::__cxa_demangle(test_mangled, nullptr, nullptr, &status); status == 0 && demangled) {
        LOG_INFO(std::format("Demangling test: '{}' -> '{}'", test_mangled, demangled));
        free(demangled);
    } else {
        LOG_ERROR(std::format("Demangling test failed for '{}', status: {}", test_mangled, status));
    }
}

void test_stacktrace_capture() {
    // Test that we can capture at least a few frames
    constexpr int max_frames = 10;
    std::vector<void*> array;
    array.resize(max_frames);
    const int size = backtrace(array.data(), max_frames);
    array.resize(size);

    assert(size > 0);
    assert(size <= max_frames);

    // Test that backtrace_symbols doesn't return null
    //
    // *note* we are using a C raw pointer here :-( I played around with using RAII to
    // fix this, this is as far as I got, it too verbose and I don't like it for this
    // example, so leaving in the raw pointer/free code.
    //
    // std::vector<std::string> get_backtrace() {
    //     void* buffer[256];
    //     int nptrs = backtrace(buffer, 256);
    //
    //     std::unique_ptr<char*, decltype(&free)> symbols{
    //         backtrace_symbols(buffer, nptrs), &free
    //     };
    //
    //     std::vector<std::string> result{};
    //     if (symbols) {
    //         result.reserve(nptrs);
    //         for (int i = 0; i < nptrs; ++i) {
    //             result.emplace_back(symbols.get()[i]);
    //         }
    //     }
    //
    //     return result;  // Memory automatically freed
    // }
    //
    char** strings = backtrace_symbols(array.data(), size);
    assert(strings != nullptr);

    // Verify we get some meaningful output
    for (int ndx = 0; ndx < size; ndx++) {
        assert(strings[ndx] != nullptr);
        assert(strlen(strings[ndx]) > 0);
    }

    free(strings);
    LOG_INFO("✓ Stacktrace capture test passed");
}

void run_tests() {
    LOG_INFO("=== Running Tests ===");
    test_demangling();
    test_stacktrace_capture();
    LOG_INFO("=== All Tests Passed ===");
}
#endif
