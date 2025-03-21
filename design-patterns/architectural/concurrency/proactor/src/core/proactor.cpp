// MIT License
// Copyright (c) 2025 dbjwhs

#include "../../include/proactor.hpp"
#include "../../include/socket.hpp"
#include "../../../../../headers/project_utils.hpp"
#include <format>
#include <sys/socket.h>

namespace proactor {

Proactor::Proactor() : m_eventQueue(std::make_unique<EventQueue>()) {}

void Proactor::start() {
    // set the running flag to true
    m_running = true;

    // start the event queue
    m_eventQueue->start([this](int fd, int filter, void* userData) {
        handleEvent(fd, filter, userData);
    });

    Logger::getInstance().log(LogLevel::INFO, "proactor started");
}

void Proactor::stop() {
    if (m_running.exchange(false)) {
        // stop the event queue
        m_eventQueue->stop();

        Logger::getInstance().log(LogLevel::INFO, "proactor stopped");
    }
}

void Proactor::registerOperation(int fd, const std::shared_ptr<AsyncOperation>& operation) {
    std::lock_guard<std::mutex> lock(m_mutex);

    // store the operation in the operation map
    m_operations[fd] = operation;

    // register the socket with the event queue based on the operation type

    switch (OperationType type = operation->type()) {
        case OperationType::ACCEPT:
        case OperationType::READ:
            Logger::getInstance().log(LogLevel::INFO, 
                std::format("Registering fd {} for READ events", fd));
            m_eventQueue->registerForRead(fd, operation.get());  // Pass the operation as userData
            break;

        case OperationType::CONNECT:
        case OperationType::WRITE:
            Logger::getInstance().log(LogLevel::INFO, 
                std::format("Registering fd {} for WRITE events", fd));
            m_eventQueue->registerForWrite(fd, operation.get());  // Pass the operation as userData
            break;
    }
}

void Proactor::cancelOperation(int fd) {
    std::lock_guard<std::mutex> lock(m_mutex);

    // find the operation in the map
    if (const auto it = m_operations.find(fd); it != m_operations.end()) {
        // get the operation type

        // unregister the socket from the event queue based on the operation type
        switch (OperationType type = it->second->type()) {
            case OperationType::ACCEPT:
            case OperationType::READ:
                m_eventQueue->unregisterForRead(fd);
                break;

            case OperationType::CONNECT:
            case OperationType::WRITE:
                m_eventQueue->unregisterForWrite(fd);
                break;
        }

        // cancel the operation
        it->second->cancel();

        // remove the operation from the map
        m_operations.erase(it);
    }
}

void Proactor::handleEvent(int fd, const int filter, void* userData) {
    std::shared_ptr<AsyncOperation> operation;

    Logger::getInstance().log(LogLevel::INFO, 
        std::format("Handling event for fd {}, filter {}", fd, 
            (filter == EVFILT_READ) ? "READ" : 
            (filter == EVFILT_WRITE) ? "WRITE" : "UNKNOWN"));

    {
        std::lock_guard<std::mutex> lock(m_mutex);

        // find the operation in the map
        auto it = m_operations.find(fd);
        if (it == m_operations.end()) {
            Logger::getInstance().log(LogLevel::ERROR, 
                std::format("No operation found for fd {}", fd));
            return;
        }

        // get the operation
        operation = it->second;

        // remove the operation from the map
        m_operations.erase(it);

        // unregister the socket from the event queue
        if (filter == EVFILT_READ) {
            Logger::getInstance().log(LogLevel::INFO, 
                std::format("Unregistering fd {} for READ events", fd));
            m_eventQueue->unregisterForRead(fd);
        } else if (filter == EVFILT_WRITE) {
            Logger::getInstance().log(LogLevel::INFO, 
                std::format("Unregistering fd {} for WRITE events", fd));
            m_eventQueue->unregisterForWrite(fd);
        }
    }

    // handle the operation based on its type
    OperationType type = operation->type();
    
    Logger::getInstance().log(LogLevel::INFO, 
        std::format("Processing operation of type {}", 
            (type == OperationType::ACCEPT) ? "ACCEPT" :
            (type == OperationType::CONNECT) ? "CONNECT" :
            (type == OperationType::READ) ? "READ" :
            (type == OperationType::WRITE) ? "WRITE" : "UNKNOWN"));

    switch (type) {
        case OperationType::ACCEPT:
            handleAccept(fd, operation);
            break;

        case OperationType::CONNECT:
            handleConnect(fd, operation);
            break;

        case OperationType::READ:
            handleRead(fd, operation);
            break;

        case OperationType::WRITE:
            handleWrite(fd, operation);
            break;
    }
}

// Private methods for handling different operation types
void Proactor::handleAccept(const int fd, const std::shared_ptr<AsyncOperation>& operation) {
    // create a socket wrapper for the server socket
    Socket serverSocket(fd);
    
    Logger::getInstance().log(LogLevel::INFO, "Handling accept operation");

    // accept the incoming connection
    auto [clientSocket, error] = serverSocket.accept();

    if (error) {
        // accept failed
        Logger::getInstance().log(LogLevel::ERROR, 
            std::format("Accept failed: {}", error.message()));
        operation->complete(-1, Buffer());
        return;
    }

    if (!clientSocket.isValid()) {
        // no pending connections, try again
        Logger::getInstance().log(LogLevel::INFO, "No pending connections, retrying");
        this->registerOperation(fd, operation);
        return;
    }
    
    Logger::getInstance().log(LogLevel::INFO, 
        std::format("Accepted connection on socket {}", clientSocket.fd()));

    // set the client socket to non-blocking mode
    error = clientSocket.setNonBlocking();
    if (error) {
        // failed to set non-blocking mode
        operation->complete(-1, Buffer());
        return;
    }

    // create a buffer with the client socket fd
    Buffer buffer(sizeof(int));
    *reinterpret_cast<int*>(buffer.data()) = clientSocket.fd();
    buffer.setSize(sizeof(int));

    // complete the operation successfully
    operation->complete(0, std::move(buffer));

    // don't close the client socket, it's now owned by the handler
}

void Proactor::handleConnect(int fd, const std::shared_ptr<AsyncOperation>& operation) {
    Logger::getInstance().log(LogLevel::INFO, 
        std::format("Handling connect operation for socket {}", fd));
        
    // Check if socket is valid first
    if (fd < 0) {
        Logger::getInstance().log(LogLevel::ERROR, "Invalid socket descriptor");
        operation->complete(-1, Buffer());
        return;
    }
    
    // Check if socket is writable (indicating connection success)
    // First attempt: use getsockopt to check for connection errors
    int error = 0;
    socklen_t errorLen = sizeof(error);
    if (getsockopt(fd, SOL_SOCKET, SO_ERROR, &error, &errorLen) < 0) {
        // failed to get a socket option
        Logger::getInstance().log(LogLevel::ERROR, 
            std::format("Failed to get socket option: {} ({})", strerror(errno), errno));
        operation->complete(-1, Buffer());
        return;
    }

    if (error != 0) {
        // connection failed
        Logger::getInstance().log(LogLevel::ERROR, 
            std::format("Connection failed: {} ({})", strerror(error), error));
        operation->complete(-1, Buffer());
        return;
    }
    
    // Second verification: try a zero-byte write to confirm connection
    if (const int writeResult = write(fd, nullptr, 0); writeResult < 0 && errno != EINTR) {
        Logger::getInstance().log(LogLevel::ERROR, 
            std::format("Zero-byte write test failed: {} ({})", strerror(errno), errno));
        operation->complete(-1, Buffer());
        return;
    }

    // connection established successfully
    Logger::getInstance().log(LogLevel::INFO, 
        std::format("Connection established successfully on socket {}", fd));
    operation->complete(0, Buffer());
}

void Proactor::handleRead(int fd, const std::shared_ptr<AsyncOperation>& operation) {
    Logger::getInstance().log(LogLevel::INFO, 
        std::format("Handling read operation for socket {}", fd));
        
    // Create a buffer for reading
    constexpr size_t bufferSize = 4096;
    Buffer buffer(bufferSize);
    
    // Create a socket wrapper for the client socket
    Socket socket(fd);
    
    // Read data from the socket
    auto [bytesRead, error] = socket.read(buffer.data(), buffer.capacity());
    
    if (error) {
        // read failed
        Logger::getInstance().log(LogLevel::ERROR, 
            std::format("Read failed: {}", error.message()));
        operation->complete(-1, Buffer());
        return;
    }
    
    if (bytesRead == 0) {
        // connection closed by peer or would block
        Logger::getInstance().log(LogLevel::INFO, "Connection closed by peer or would block");
        operation->complete(0, Buffer());
        return;
    }
    
    // Set the size of the buffer to the number of bytes read
    buffer.setSize(bytesRead);
    
    // Complete the operation successfully
    Logger::getInstance().log(LogLevel::INFO, 
        std::format("Read {} bytes", bytesRead));
    operation->complete(bytesRead, std::move(buffer));
}

void Proactor::handleWrite(int fd, const std::shared_ptr<AsyncOperation>& operation) {
    // Implement write operation handling
    // Similar to handleRead but for writing data
}

} // namespace proactor