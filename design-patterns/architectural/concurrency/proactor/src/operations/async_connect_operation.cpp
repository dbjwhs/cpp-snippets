#include "../../include/operations/async_connect_operation.hpp"
#include "../../include/proactor.hpp"
#include "../../include/completion_handler.hpp"
#include "../../include/logger.hpp"
#include "../../include/error.hpp"
#include <sys/socket.h>
#include <format>
#include <cerrno>
#include <cstring>
#include <utility>

namespace proactor {

AsyncConnectOperation::AsyncConnectOperation(const std::shared_ptr<CompletionHandler>& handler,
                                            const Socket& socket,
                                            std::string  address,
                                            const int port)
    : AsyncOperation(handler, OperationType::CONNECT),
      m_socket(socket.fd()),
      m_address(std::move(address)),
      m_port(port) {}

bool AsyncConnectOperation::initiate(const std::shared_ptr<Proactor>& proactor) {
    Logger::getInstance().log(LogLevel::INFO, 
        std::format("Initiating connect operation to {}:{} on socket {}", 
            m_address, m_port, m_socket));
            
    if (m_socket < 0) {
        Logger::getInstance().log(LogLevel::ERROR, "Invalid socket");
        return false;
    }

    // Create a socket wrapper for the file descriptor
    const Socket socket(m_socket);

    // Set the socket to non-blocking mode
    Error error = socket.setNonBlocking();
    if (error) {
        Logger::getInstance().log(LogLevel::ERROR, 
            std::format("Failed to set socket non-blocking: {}", error.message()));
        return false;
    }
    
    // Run diagnostics on the socket
    int value = 0;
    socklen_t valueLen = sizeof(value);
    if (getsockopt(m_socket, SOL_SOCKET, SO_TYPE, &value, &valueLen) < 0) {
        Logger::getInstance().log(LogLevel::ERROR, 
            std::format("Socket diagnostic failed: {} ({})", strerror(errno), errno));
        return false;
    }
    Logger::getInstance().log(LogLevel::INFO, 
        std::format("Socket {} diagnostics - type: {}", m_socket, value));

    // Ensure we're using TCP
    if (value != SOCK_STREAM) {
        Logger::getInstance().log(LogLevel::ERROR, "Not a TCP socket");
        return false;
    }

    // Start the asynchronous connection
    error = socket.connect(m_address, m_port);
    if (error) {
        Logger::getInstance().log(LogLevel::ERROR, 
            std::format("Failed to connect: {}", error.message()));
        return false;
    }
    
    Logger::getInstance().log(LogLevel::INFO, "Connection initiated");

    // Check connection status immediately - if already connected, it's a loopback 
    // connection which might succeed immediately
    int err = 0;
    socklen_t errLen = sizeof(err);
    if (getsockopt(m_socket, SOL_SOCKET, SO_ERROR, &err, &errLen) >= 0 && err == 0) {
        if (const int result = connect(m_socket, nullptr, 0); result == 0 || (result < 0 && errno == EISCONN)) {
            Logger::getInstance().log(LogLevel::INFO, "Connection completed immediately");
            // For immediate connections, we still register with the proactor
            // because the proactor pattern requires all operations to complete asynchronously
        }
    }

    // Register the socket with the proactor for write events
    // (write events are triggered when the connection is established)
    proactor->registerOperation(m_socket, shared_from_this());
    Logger::getInstance().log(LogLevel::INFO, "Socket registered with proactor");
    return true;
}

} // namespace proactor
