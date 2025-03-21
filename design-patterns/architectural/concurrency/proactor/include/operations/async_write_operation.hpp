#ifndef ASYNC_WRITE_OPERATION_HPP
#define ASYNC_WRITE_OPERATION_HPP

#include <memory>
#include "../async_operation.hpp"
#include "../socket.hpp"
#include "../buffer.hpp"

namespace proactor {

// Forward declarations
class CompletionHandler;
class Proactor;

/**
 * Concrete asynchronous write operation
 * Used to write data to a socket asynchronously
 */
class AsyncWriteOperation : public AsyncOperation, public std::enable_shared_from_this<AsyncWriteOperation> {
public:
    /**
     * Constructor taking a completion handler, socket, and data to write
     * @param handler The completion handler to notify when the operation completes
     * @param socket The socket to write to
     * @param buffer The data to write
     */
    AsyncWriteOperation(std::shared_ptr<CompletionHandler> handler,
                        const Socket& socket,
                        Buffer buffer);
    
    /**
     * Initiate the write operation
     * Writes as much data as possible synchronously, then registers for write events if needed
     * @param proactor The proactor to register with
     * @return true if the operation was initiated successfully, false otherwise
     */
    bool initiate(const std::shared_ptr<Proactor>& proactor);

    /**
     * Handle a write event
     * This is called by the proactor when the socket is ready for writing
     */
    void handleWriteEvent();

private:
    // The socket file descriptor
    int m_socket;

    // The data buffer to write
    Buffer m_buffer;

    // The number of bytes written so far
    size_t m_bytesWritten;
};

} // namespace proactor

#endif // ASYNC_WRITE_OPERATION_HPP