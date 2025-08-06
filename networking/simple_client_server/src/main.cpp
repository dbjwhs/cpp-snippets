// MIT License
// Copyright (c) 2025 dbjwhs

#include <boost/asio.hpp>
#include <functional>
#include <iostream>
#include <string>
#include <expected>
#include <array>
#include <format>
#include <thread>
#include <chrono>
#include <vector>
#include <memory>
#include <cassert>
#include "../../../headers/project_utils.hpp"

//
// Comprehensive Client-Server Networking Pattern Example
//
// This demonstration showcases a complete implementation of the TCP client-server pattern with hash-based
// message integrity verification. The example includes both positive and negative test cases, demonstrating
// proper error handling, connection management, and data validation techniques.
//
// Historical Context:
// This pattern represents the evolution of network programming from simple request-response systems to
// robust, integrity-verified communication protocols. The integration of hash verification addresses
// the fundamental challenge of ensuring data integrity across unreliable network connections, while the
// Railway-Oriented Programming approach provides clean error propagation without exceptions.
//
// Pattern Components Demonstrated:
// - Asynchronous server with concurrent client handling
// - Synchronous client with hash verification
// - Comprehensive error handling using std::expected
// - RAII-based resource management
// - Robust testing with both success and failure scenarios
//
// Real-World Applications:
// - Microservice architectures requiring reliable messaging
// - IoT systems with data integrity requirements
// - Distributed databases with node communication
// - Financial systems requiring transaction verification
// - Game servers with player state synchronization

namespace networking_example {

// forward declarations from our networking components
class TcpServer;
class TcpClient;

// test utilities and helper functions
class NetworkingTestSuite {
private:
    // test result tracking
    std::vector<std::string> m_test_results{};
    
    // test counter for tracking progress
    std::size_t m_test_count{};

    // helper to run client test against server
    auto run_client_test(const std::string& message, const std::string& host, std::uint16_t port) -> bool;
    
    // helper to wait for server startup
    auto wait_for_server_ready(std::uint16_t port) -> bool;

public:
    // constructor initializing test suite
    NetworkingTestSuite() = default;
    
    // run comprehensive test suite
    auto run_all_tests() -> void;
    
    // test basic client-server communication
    auto test_basic_communication() -> void;
    
    // test multiple sequential messages
    auto test_multiple_messages() -> void;
    
    // test error handling scenarios
    auto test_error_scenarios() -> void;
    
    // test hash verification functionality
    auto test_hash_verification() -> void;
    
    // print final test results
    auto print_results() const -> void;
    
    // helper to log test results
    auto log_test_result(const std::string& test_name, bool passed) -> void;
};

auto NetworkingTestSuite::run_all_tests() -> void {
    LOG_INFO_PRINT("starting comprehensive networking test suite");
    
    // reserve space for test results to avoid reallocations
    m_test_results.reserve(20);
    
    // run individual test categories
    test_basic_communication();
    test_multiple_messages();
    test_hash_verification();
    test_error_scenarios();
    
    // display final results
    print_results();
}

auto NetworkingTestSuite::test_basic_communication() -> void {
    LOG_INFO_PRINT("testing basic client-server communication");
    
    // test simple message exchange
    constexpr std::string_view test_message{"hello_world"};
    constexpr std::uint16_t test_port{8081};
    
    // start server in background thread
    std::thread server_thread{[test_port]() {
        try {
            // create server instance
            boost::asio::io_context io_context{};
            boost::asio::ip::tcp::acceptor acceptor{io_context,
                boost::asio::ip::tcp::endpoint{boost::asio::ip::tcp::v4(), test_port}};
            
            LOG_INFO_PRINT("test server listening on port {}", test_port);
            
            // accept one connection for testing
            boost::asio::ip::tcp::socket socket{io_context};
            acceptor.accept(socket);
            
            // read a message from the client
            std::array<char, 1024> buffer{};
            buffer.fill(0);
            const auto bytes_read = socket.read_some(boost::asio::buffer(buffer));
            const std::string received_message{buffer.data(), bytes_read};
            
            // compute hash and send response
            constexpr std::hash<std::string> hasher{};
            auto hash_value = hasher(received_message);
            auto response = std::format("ACK:{:x}", hash_value);
            
            boost::asio::write(socket, boost::asio::buffer(response));
            
            LOG_INFO_PRINT("test server handled connection successfully");
        } catch (const std::exception& e) {
            LOG_ERROR_PRINT("test server error: {}", e.what());
        }
    }};
    
    // allow server to start up
    std::this_thread::sleep_for(std::chrono::milliseconds{100});
    
    // test client communication
    const bool test_passed = run_client_test(std::string{test_message}, "localhost", test_port);
    log_test_result("basic_communication", test_passed);
    
    // cleanup server thread
    if (server_thread.joinable()) {
        server_thread.join();
    }
}

auto NetworkingTestSuite::test_multiple_messages() -> void {
    LOG_INFO_PRINT("testing multiple sequential messages");
    
    // test messages to send in sequence
    const std::vector<std::string> test_messages{
        "first_message",
        "second_message", 
        "third_message_with_longer_content",
        "final_test_message"
    };
    
    constexpr std::uint16_t test_port{8082};
    
    // start server that handles multiple connections
    std::thread server_thread{[test_port, &test_messages]() {
        try {
            boost::asio::io_context io_context{};
            boost::asio::ip::tcp::acceptor acceptor{io_context,
                boost::asio::ip::tcp::endpoint{boost::asio::ip::tcp::v4(), test_port}};
            
            LOG_INFO_PRINT("multi-message test server listening on port {}", test_port);
            
            // handle each message sequentially
            for (std::size_t ndx = 0; ndx < test_messages.size(); ++ndx) {
                boost::asio::ip::tcp::socket socket{io_context};
                acceptor.accept(socket);
                
                std::array<char, 1024> buffer{};
                buffer.fill(0);
                const auto bytes_read = socket.read_some(boost::asio::buffer(buffer));
                const std::string received_message{buffer.data(), bytes_read};
                
                // verify we received expected message
                assert(received_message == test_messages[ndx]);
                
                std::hash<std::string> hasher{};
                auto hash_value = hasher(received_message);
                auto response = std::format("ACK:{:x}", hash_value);
                
                boost::asio::write(socket, boost::asio::buffer(response));
                
                LOG_INFO_PRINT("handled message {} of {}: '{}'", ndx + 1, test_messages.size(), received_message);
            }
            
        } catch (const std::exception& e) {
            LOG_ERROR_PRINT("multi-message test server error: {}", e.what());
        }
    }};
    
    // allow server startup time
    std::this_thread::sleep_for(std::chrono::milliseconds{100});
    
    // test each message individually
    bool all_tests_passed{true};
    for (const auto& message : test_messages) {
        if (const bool test_passed = run_client_test(message, "localhost", test_port); !test_passed) {
            all_tests_passed = false;
        }
        
        // small delay between messages
        std::this_thread::sleep_for(std::chrono::milliseconds{50});
    }
    
    log_test_result("multiple_messages", all_tests_passed);
    
    // cleanup server thread
    if (server_thread.joinable()) {
        server_thread.join();
    }
}

auto NetworkingTestSuite::test_hash_verification() -> void {
    LOG_INFO_PRINT("testing hash verification functionality");
    
    constexpr std::string_view test_message{"hash_verification_test"};
    constexpr std::uint16_t test_port{8083};
    
    // server that intentionally returns the wrong hash for testing
    std::thread server_thread{[test_port]() {
        try {
            boost::asio::io_context io_context{};
            boost::asio::ip::tcp::acceptor acceptor{io_context,
                boost::asio::ip::tcp::endpoint{boost::asio::ip::tcp::v4(), test_port}};
            
            LOG_INFO_PRINT("hash verification test server listening on port {}", test_port);
            
            // accept connection
            boost::asio::ip::tcp::socket socket{io_context};
            acceptor.accept(socket);
            
            // read a message
            std::array<char, 1024> buffer{};
            buffer.fill(0);
            const auto bytes_read = socket.read_some(boost::asio::buffer(buffer));
            const std::string received_message{buffer.data(), bytes_read};
            
            // intentionally return incorrect hash for testing
            constexpr auto wrong_hash{"deadbeef"};
            auto response = std::format("ACK:{}", wrong_hash);
            
            boost::asio::write(socket, boost::asio::buffer(response));
            
            LOG_INFO_PRINT("sent intentionally wrong hash for verification test");
            
        } catch (const std::exception& e) {
            LOG_ERROR_PRINT("hash verification test server error: {}", e.what());
        }
    }};
    
    // allow server startup
    std::this_thread::sleep_for(std::chrono::milliseconds{100});
    
    // test should fail due to hash mismatch - this is expected behavior
    {
        // suppress stderr for this test since we expect it to fail
        Logger::StderrSuppressionGuard stderr_guard{};
        
        const bool test_failed_as_expected = !run_client_test(std::string{test_message}, "localhost", test_port);
        log_test_result("hash_verification_detects_corruption", test_failed_as_expected);
    }
    
    // cleanup server thread
    if (server_thread.joinable()) {
        server_thread.join();
    }
}

auto NetworkingTestSuite::test_error_scenarios() -> void {
    LOG_INFO_PRINT("testing error handling scenarios");
    
    // test connection to non-existent server
    {
        Logger::StderrSuppressionGuard stderr_guard{};
        
        // this should fail since no server is running on this port
        constexpr std::uint16_t unused_port{9999};
        const bool connection_failed_as_expected = !run_client_test("test_message", "localhost", unused_port);
        log_test_result("connection_failure_handling", connection_failed_as_expected);
    }
    
    // test invalid hostname
    {
        Logger::StderrSuppressionGuard stderr_guard{};
        
        const bool hostname_resolution_failed = !run_client_test("test_message", "invalid.hostname.that.does.not.exist", 8080);
        log_test_result("hostname_resolution_failure", hostname_resolution_failed);
    }
}

auto NetworkingTestSuite::run_client_test(const std::string& message, const std::string& host, std::uint16_t port) -> bool {
    try {
        // create client and attempt connection
        boost::asio::io_context io_context{};
        boost::asio::ip::tcp::socket socket{io_context};
        
        // resolve and connect
        boost::asio::ip::tcp::resolver resolver{io_context};
        auto endpoints = resolver.resolve(host, std::to_string(port));
        boost::asio::connect(socket, endpoints);
        
        // compute expected hash
        std::hash<std::string> hasher{};
        auto expected_hash = hasher(message);
        auto expected_hash_str = std::format("{:x}", expected_hash);
        
        // send message
        boost::asio::write(socket, boost::asio::buffer(message));
        
        // receive response
        std::array<char, 1024> response_buffer{};
        response_buffer.fill(0);
        const auto bytes_received = socket.read_some(boost::asio::buffer(response_buffer));
        const std::string response{response_buffer.data(), bytes_received};
        
        // verify response format and hash
        constexpr std::string_view ack_prefix{"ACK:"};
        if (!response.starts_with(ack_prefix)) {
            return false;
        }
        
        const auto received_hash = response.substr(ack_prefix.size());
        return received_hash == expected_hash_str;
        
    } catch (const std::exception& e) {
        // test failed due to exception
        return false;
    }
}

auto NetworkingTestSuite::log_test_result(const std::string& test_name, bool passed) -> void {
    ++m_test_count;
    
    const auto result_msg = std::format("test {}: {} - {}", 
        m_test_count, test_name, passed ? "PASSED" : "FAILED");
    
    m_test_results.emplace_back(result_msg);
    
    if (passed) {
        LOG_INFO_PRINT("{}", result_msg);
    } else {
        LOG_ERROR_PRINT("{}", result_msg);
    }
}

auto NetworkingTestSuite::print_results() const -> void {
    LOG_INFO_PRINT("test suite completed - {} tests run", m_test_count);
    
    // count passed and failed tests
    std::size_t passed_count{};
    std::size_t failed_count{};
    
    for (const auto& result : m_test_results) {
        if (result.contains("PASSED")) {
            ++passed_count;
        } else {
            ++failed_count;
        }
    }
    
    LOG_INFO_PRINT("results: {} passed, {} failed", passed_count, failed_count);
    
    // print summary
    if (failed_count == 0) {
        LOG_INFO_PRINT("all tests passed successfully!");
    } else {
        LOG_WARNING_PRINT("some tests failed - review error logs for details");
    }
    
    // assert all tests passed for verification
    assert(failed_count == 0 && "some tests failed - check implementation");
}

} // namespace networking_example

// main demonstration function
auto main() -> int {
    LOG_INFO_PRINT("starting comprehensive networking pattern demonstration");
    
    // create and run test suite
    networking_example::NetworkingTestSuite test_suite{};
    test_suite.run_all_tests();
    
    LOG_INFO_PRINT("demonstration completed successfully");
    
    return 0;
}