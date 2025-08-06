// MIT License
// Copyright (c) 2025 dbjwhs

#include <boost/asio.hpp>
#include <functional>
#include <iostream>
#include <string>
#include <expected>
#include <array>
#include <format>
#include "../../../headers/project_utils.hpp"

//
// TCP Client with Hash Verification Pattern
//
// This pattern implements a robust TCP client that sends messages to a server and verifies the integrity
// of the communication through hash comparison. The client computes a hash of the message before sending,
// then validates that the server computed the same hash, ensuring data was transmitted without corruption.
//
// Historical Context:
// This client-side verification pattern emerged from the need to detect network transmission errors in
// distributed systems. By comparing hashes computed on both ends, applications can quickly detect data
// corruption without implementing complex checksums or error correction codes. The Railway-Oriented
// Programming approach with std::expected provides clean error handling throughout the network operations.
//
// Key Components:
// - Synchronous TCP client using Boost.Asio for reliable connection management
// - Hash computation and verification for message integrity
// - Railway-Oriented Programming with std::expected for error propagation
// - RAII-based resource management with automatic socket cleanup
// - Comprehensive error handling for all network operations
//
// Common Usage Patterns:
// - Command-line tools requiring reliable message delivery
// - Data synchronization between distributed systems
// - IoT device communication with integrity verification
// - Automated testing of network services
// - Batch processing systems with error detection

namespace networking {

// error types for railway-oriented programming
enum class ClientError {
    CONNECTION_FAILED,
    SEND_FAILED,
    RECEIVE_FAILED,
    HASH_MISMATCH,
    HASH_COMPUTATION_FAILED,
    INVALID_RESPONSE
};

// tcp client class for sending messages with hash verification
class TcpClient {
private:
    // boost asio context for network operations
    boost::asio::io_context m_io_context{};
    
    // tcp socket for server communication
    boost::asio::ip::tcp::socket m_socket{m_io_context};
    
    // server hostname or ip address
    std::string m_host{};
    
    // server port number
    std::uint16_t m_port{};

    // compute hash of given data using the same algorithm as server
    [[nodiscard]] auto compute_hash(const std::string& data) const -> std::expected<std::string, ClientError>;
    
    // extract hash from server response
    [[nodiscard]] auto extract_hash_from_response(const std::string& response) const -> std::expected<std::string, ClientError>;

public:
    // constructor initializing a client with server details
    TcpClient(std::string host, std::uint16_t port);
    
    // connect to the server
    auto connect() -> std::expected<void, ClientError>;
    
    // send message and verify response hash
    auto send_message(const std::string& message) -> std::expected<void, ClientError>;
    
    // disconnect from server
    auto disconnect() -> void;
};

// tcpclient implementation
TcpClient::TcpClient(std::string host, std::uint16_t port) 
    : m_host{std::move(host)}, m_port{port} {
    LOG_INFO_PRINT("initializing tcp client for {}:{}", m_host, m_port);
}

auto TcpClient::connect() -> std::expected<void, ClientError> {
    try {
        // resolve server hostname to ip address
        boost::asio::ip::tcp::resolver resolver{m_io_context};
        auto endpoints = resolver.resolve(m_host, std::to_string(m_port));
        
        LOG_INFO_PRINT("connecting to server {}:{}", m_host, m_port);
        
        // attempt to connect to server
        boost::asio::connect(m_socket, endpoints);
        
        LOG_INFO_PRINT("successfully connected to server");
        return {};
    } catch (const std::exception& e) {
        LOG_ERROR_PRINT("connection failed: {}", e.what());
        return std::unexpected{ClientError::CONNECTION_FAILED};
    }
}

auto TcpClient::send_message(const std::string& message) -> std::expected<void, ClientError> {
    try {
        // compute hash of a message before sending
        auto hash_result = compute_hash(message);
        if (!hash_result) {
            return std::unexpected{hash_result.error()};
        }
        
        const auto expected_hash = hash_result.value();
        LOG_INFO_PRINT("sending message: '{}' with expected hash: {}", message, expected_hash);
        
        // send a message to server
        boost::asio::write(m_socket, boost::asio::buffer(message));
        LOG_INFO_PRINT("message sent successfully");
        
        // receive response from server
        std::array<char, 1024> response_buffer{};
        response_buffer.fill(0);
        
        const auto bytes_received = m_socket.read_some(boost::asio::buffer(response_buffer));
        const std::string response{response_buffer.data(), bytes_received};
        
        LOG_INFO_PRINT("received response: '{}'", response);
        
        // extract hash from server response
        auto received_hash_result = extract_hash_from_response(response);
        if (!received_hash_result) {
            return std::unexpected{received_hash_result.error()};
        }
        
        const auto received_hash = received_hash_result.value();
        
        // verify hash matches what we computed
        if (expected_hash != received_hash) {
            LOG_ERROR_PRINT("hash mismatch! expected: {}, received: {}", expected_hash, received_hash);
            return std::unexpected{ClientError::HASH_MISMATCH};
        }
        
        LOG_INFO_PRINT("hash verification successful - message integrity confirmed");
        return {};
        
    } catch (const std::exception& e) {
        LOG_ERROR_PRINT("send operation failed: {}", e.what());
        return std::unexpected{ClientError::SEND_FAILED};
    }
}

auto TcpClient::compute_hash(const std::string& data) const -> std::expected<std::string, ClientError> {
    try {
        // use the same hash algorithm as server
        std::hash<std::string> hasher{};
        auto hash_value = hasher(data);
        
        // convert hash to hex string
        auto hex_hash = std::format("{:x}", hash_value);
        LOG_INFO_PRINT("computed client hash: {}", hex_hash);
        
        return hex_hash;
    } catch (const std::exception& e) {
        LOG_ERROR_PRINT("hash computation failed: {}", e.what());
        return std::unexpected{ClientError::HASH_COMPUTATION_FAILED};
    }
}

auto TcpClient::extract_hash_from_response(const std::string& response) const -> std::expected<std::string, ClientError> {
    // expected response format: "ACK:hash_value"
    constexpr std::string_view ack_prefix{"ACK:"};
    
    if (response.size() <= ack_prefix.size() || !response.starts_with(ack_prefix)) {
        LOG_ERROR_PRINT("invalid response format: '{}'", response);
        return std::unexpected{ClientError::INVALID_RESPONSE};
    }
    
    // extract a hash portion after "ACK":
    auto hash = response.substr(ack_prefix.size());
    LOG_INFO_PRINT("extracted hash from response: {}", hash);
    
    return hash;
}

auto TcpClient::disconnect() -> void {
    try {
        if (m_socket.is_open()) {
            LOG_INFO_PRINT("disconnecting from server");
            m_socket.close();
        }
    } catch (const std::exception& e) {
        LOG_ERROR_PRINT("disconnect failed: {}", e.what());
    }
}

} // namespace networking

// main function for client application
auto main(int argc, char* argv[]) -> int {
    // validate command line arguments
    if (argc < 2) {
        LOG_ERROR_PRINT("usage: {} <message> [host] [port]", argv[0]);
        return 1;
    }
    
    // extract a message from command line
    const std::string message{argv[1]};
    
    // default connection parameters
    constexpr std::string_view default_host{"localhost"};
    constexpr std::uint16_t default_port{8080};
    
    // parse optional host and port arguments
    const std::string host{argc > 2 ? argv[2] : std::string{default_host}};
    std::uint16_t port{default_port};
    
    if (argc > 3) {
        try {
            port = static_cast<std::uint16_t>(std::stoi(argv[3]));
        } catch (const std::exception& e) {
            LOG_ERROR_PRINT("invalid port number: {}", e.what());
            return 1;
        }
    }
    
    LOG_INFO_PRINT("client starting with message: '{}'", message);
    
    // create client and attempt connection
    networking::TcpClient client{host, port};
    
    // connect to server
    auto connect_result = client.connect();
    if (!connect_result) {
        LOG_ERROR_PRINT("failed to connect to server");
        return 1;
    }
    
    // send message and verify response
    auto send_result = client.send_message(message);
    if (!send_result) {
        LOG_ERROR_PRINT("failed to send message or verify response");
        client.disconnect();
        return 1;
    }
    
    LOG_INFO_PRINT("message sent and verified successfully");
    
    // clean disconnection
    client.disconnect();
    
    return 0;
}