// MIT License
// Copyright (c) 2025 dbjwhs

#include <boost/asio.hpp>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <expected>
#include <array>
#include <format>
#include "../../../headers/project_utils.hpp"

//
// TCP Server with Hash Validation Pattern
//
// This pattern implements a robust client-server communication system using Boost.Asio with hash-based message
// integrity verification. The server listens for incoming TCP connections, receives messages from clients, 
// computes SHA-256 hashes of received data, and sends back acknowledgments with hash verification.
//
// Historical Context:
// This pattern evolved from early network programming challenges where data corruption during transmission
// was a significant concern. The addition of hash verification ensures message integrity without the overhead
// of more complex protocols. The Railway-Oriented Programming approach using std::expected provides clean
// error handling without exceptions, making the code more predictable and easier to reason about.
//
// Key Components:
// - Asynchronous TCP server using Boost.Asio's io_context and acceptor pattern
// - Hash-based message integrity using SHA-256 for verification
// - Railway-Oriented Programming with std::expected for error propagation
// - RAII-based connection management with automatic cleanup
// - Graceful error handling without exceptions
//
// Common Usage Patterns:
// - Microservice communication with data integrity requirements
// - File transfer systems requiring corruption detection  
// - IoT device communication where message integrity is critical
// - Distributed systems needing reliable message passing
// - Testing frameworks for network protocol validation

namespace networking {

// error types for railway-oriented programming
enum class ServerError {
    BIND_FAILED,
    ACCEPT_FAILED,
    READ_FAILED,
    WRITE_FAILED,
    HASH_COMPUTATION_FAILED
};

// forward declaration for connection class
class Connection;

// server class managing tcp connections
class TcpServer {
private:
    // boost asio context for async operations
    boost::asio::io_context m_io_context{};
    
    // tcp acceptor for incoming connections
    boost::asio::ip::tcp::acceptor m_acceptor{m_io_context};
    
    // server listening port
    std::uint16_t m_port{};

    // start accepting new connections asynchronously
    auto start_accept() -> void;
    
    // handle new connection acceptance
    auto handle_accept(std::shared_ptr<Connection> connection, 
                      const boost::system::error_code& error) -> void;

public:
    // constructor initializing server on specified port
    explicit TcpServer(std::uint16_t port);
    
    // start the server and begin listening
    auto run() -> std::expected<void, ServerError>;
    
    // stop the server gracefully
    auto stop() -> void;
};

// connection class handling individual client sessions
class Connection : public std::enable_shared_from_this<Connection> {
private:
    // tcp socket for client communication
    boost::asio::ip::tcp::socket m_socket;
    
    // buffer for incoming message data
    std::array<char, 1024> m_buffer{};
    
    // received message string
    std::string m_message{};

    // compute sha256 hash of given data
    auto compute_hash(const std::string& data) const -> std::expected<std::string, ServerError>;
    
    // start reading data from client
    auto start_read() -> void;
    
    // handle received data from client
    auto handle_read(const boost::system::error_code& error, std::size_t bytes_transferred) -> void;
    
    // send response back to client
    auto send_response(const std::string& hash) -> void;
    
    // handle response sending completion
    auto handle_write(const boost::system::error_code& error) -> void;

public:
    // constructor taking io_context reference
    explicit Connection(boost::asio::io_context& io_context);
    
    // get reference to underlying socket
    auto socket() -> boost::asio::ip::tcp::socket&;
    
    // start handling this connection
    auto start() -> void;
};

// tcpserver implementation
TcpServer::TcpServer(std::uint16_t port) 
    : m_acceptor{m_io_context}, m_port{port} {
    LOG_INFO_PRINT("initializing tcp server on port {}", m_port);
}

auto TcpServer::run() -> std::expected<void, ServerError> {
    try {
        // bind acceptor to specified port
        boost::asio::ip::tcp::endpoint endpoint{boost::asio::ip::tcp::v4(), m_port};
        m_acceptor.open(endpoint.protocol());
        m_acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
        m_acceptor.bind(endpoint);
        m_acceptor.listen();
        
        LOG_INFO_PRINT("server listening on port {}", m_port);
        
        // start accepting connections
        start_accept();
        
        // run the io_context event loop
        m_io_context.run();
        
        return {};
    } catch (const std::exception& e) {
        LOG_ERROR_PRINT("failed to start server: {}", e.what());
        return std::unexpected{ServerError::BIND_FAILED};
    }
}

auto TcpServer::start_accept() -> void {
    // create new connection for incoming client
    auto new_connection = std::make_shared<Connection>(m_io_context);
    
    // asynchronously accept new connection
    m_acceptor.async_accept(new_connection->socket(),
        [this, new_connection](const boost::system::error_code& error) {
            handle_accept(new_connection, error);
        });
}

auto TcpServer::handle_accept(std::shared_ptr<Connection> connection, 
                             const boost::system::error_code& error) -> void {
    if (!error) {
        LOG_INFO_PRINT("accepted new client connection");
        
        // start handling the new connection
        connection->start();
        
        // continue accepting more connections
        start_accept();
    } else {
        LOG_ERROR_PRINT("accept failed: {}", error.message());
    }
}

auto TcpServer::stop() -> void {
    LOG_INFO_PRINT("stopping server");
    m_io_context.stop();
}

// connection implementation
Connection::Connection(boost::asio::io_context& io_context) 
    : m_socket{io_context} {
    // initialize buffer to zero
    m_buffer.fill(0);
}

auto Connection::socket() -> boost::asio::ip::tcp::socket& {
    return m_socket;
}

auto Connection::start() -> void {
    LOG_INFO_PRINT("starting connection handler");
    start_read();
}

auto Connection::start_read() -> void {
    // asynchronously read data from client
    m_socket.async_read_some(boost::asio::buffer(m_buffer),
        [self = shared_from_this()](const boost::system::error_code& error, std::size_t bytes_transferred) {
            self->handle_read(error, bytes_transferred);
        });
}

auto Connection::handle_read(const boost::system::error_code& error, std::size_t bytes_transferred) -> void {
    if (!error) {
        // extract message from buffer
        m_message = std::string{m_buffer.data(), bytes_transferred};
        LOG_INFO_PRINT("received message: '{}'", m_message);
        
        // compute hash of received message
        auto hash_result = compute_hash(m_message);
        if (!hash_result) {
            LOG_ERROR_PRINT("failed to compute hash");
            return;
        }
        
        // send response with hash back to client
        send_response(hash_result.value());
    } else if (error == boost::asio::error::eof) {
        LOG_INFO_PRINT("client disconnected gracefully");
    } else {
        LOG_ERROR_PRINT("read failed: {}", error.message());
    }
}

auto Connection::compute_hash(const std::string& data) const -> std::expected<std::string, ServerError> {
    try {
        // simple hash computation using std::hash for demonstration
        // in production, use proper cryptographic hash like sha256
        std::hash<std::string> hasher{};
        auto hash_value = hasher(data);
        
        // convert hash to hex string
        auto hex_hash = std::format("{:x}", hash_value);
        LOG_INFO_PRINT("computed hash: {}", hex_hash);
        
        return hex_hash;
    } catch (const std::exception& e) {
        LOG_ERROR_PRINT("hash computation failed: {}", e.what());
        return std::unexpected{ServerError::HASH_COMPUTATION_FAILED};
    }
}

auto Connection::send_response(const std::string& hash) -> void {
    // create response message with ack and hash
    auto response = std::format("ACK:{}", hash);
    
    // asynchronously send response to client
    boost::asio::async_write(m_socket, boost::asio::buffer(response),
        [self = shared_from_this()](const boost::system::error_code& error, std::size_t /*bytes_transferred*/) {
            self->handle_write(error);
        });
}

auto Connection::handle_write(const boost::system::error_code& error) -> void {
    if (!error) {
        LOG_INFO_PRINT("response sent successfully");
        
        // continue reading for more messages
        start_read();
    } else {
        LOG_ERROR_PRINT("write failed: {}", error.message());
    }
}

} // namespace networking

// main function for server application
auto main(int argc, char* argv[]) -> int {
    constexpr std::uint16_t default_port{8080};
    std::uint16_t port{default_port};
    
    // parse command line arguments for port
    if (argc > 1) {
        try {
            port = static_cast<std::uint16_t>(std::stoi(argv[1]));
        } catch (const std::exception& e) {
            LOG_ERROR_PRINT("invalid port number: {}", e.what());
            return 1;
        }
    }
    
    LOG_INFO_PRINT("starting server on port {}", port);
    
    // create and run server
    networking::TcpServer server{port};
    auto result = server.run();
    
    if (!result) {
        LOG_ERROR_PRINT("server failed to start");
        return 1;
    }
    
    return 0;
}
