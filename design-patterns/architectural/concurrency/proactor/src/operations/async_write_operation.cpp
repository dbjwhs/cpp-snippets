#include "../../include/operations/async_write_operation.hpp"
#include "../../include/proactor.hpp"
#include "../../include/completion_handler.hpp"
#include "../../include/logger.hpp"
#include <format>
#include <utility>

namespace proactor {

AsyncWriteOperation::AsyncWriteOperation(std::shared_ptr<CompletionHandler> handler,
                                        const Socket& socket,
                                        Buffer buffer)
    : AsyncOperation(std::move(handler), OperationType::WRITE),
      m_socket(socket.fd()),
      m_buffer(std::move(buffer)),
      m_bytesWritten(0) {}

bool AsyncWriteOperation::initiate(const std::shared_ptr<Proactor>& proactor) {
    Logger::getInstance().log(LogLevel::INFO, 
        std::format("Initiating write operation on socket {} ({} bytes)",
            m_socket, m_buffer.size()));
            
    if (m_socket < 0) {
        Logger::getInstance().log(LogLevel::ERROR, "Invalid socket");
        return false;
    }

    // Create a socket wrapper for the file descriptor
    const Socket socket(m_socket);

    // Try to write the data synchronously first
    auto [bytesWritten, error] = socket.write(
        m_buffer.data() + m_bytesWritten,
        m_buffer.size() - m_bytesWritten);

    if (error) {
        Logger::getInstance().log(LogLevel::ERROR, 
            std::format("Write error: {}", error.message()));
        return false;
    }

    // Update the number of bytes written
    m_bytesWritten += bytesWritten;
    Logger::getInstance().log(LogLevel::INFO, 
        std::format("Wrote {} bytes synchronously", bytesWritten));

    // Check if all data has been written
    if (m_bytesWritten >= m_buffer.size()) {
        // All data written, complete the operation
        Logger::getInstance().log(LogLevel::INFO, 
            std::format("All data written ({} bytes), completing operation", m_bytesWritten));
        complete(m_bytesWritten, Buffer());
        return true;
    }

    // More data to write, register for write events
    Logger::getInstance().log(LogLevel::INFO, 
        std::format("{} bytes remaining, registering for write events", 
            m_buffer.size() - m_bytesWritten));
    proactor->registerOperation(m_socket, shared_from_this());
    return true;
}

void AsyncWriteOperation::handleWriteEvent() {
    // Create a socket wrapper for the file descriptor
    const Socket socket(m_socket);

    // Write more data
    auto [bytesWritten, error] = socket.write(
        m_buffer.data() + m_bytesWritten,
        m_buffer.size() - m_bytesWritten);

    if (error) {
        // Write failed, complete with error
        complete(-1, Buffer());
        return;
    }

    // Update the number of bytes written
    m_bytesWritten += bytesWritten;

    // Check if all data has been written
    if (m_bytesWritten >= m_buffer.size()) {
        // All data written, complete the operation
        complete(m_bytesWritten, Buffer());
    }
}

} // namespace proactor
