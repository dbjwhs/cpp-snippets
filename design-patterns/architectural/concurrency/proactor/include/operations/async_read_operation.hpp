#ifndef ASYNC_READ_OPERATION_HPP
#define ASYNC_READ_OPERATION_HPP

#include <memory>
#include "../async_operation.hpp"
#include "../socket.hpp"

namespace proactor {

// Forward declarations
class CompletionHandler;
class Proactor;

/**
 * Concrete asynchronous read operation
 * Used to read data from a socket asynchronously
 */
class AsyncReadOperation : public AsyncOperation, public std::enable_shared_from_this<AsyncReadOperation> {
public:
    /**
     * Constructor taking a completion handler and socket
     * @param handler The completion handler to notify when the operation completes
     * @param socket The socket to read from
     */
    AsyncReadOperation(const std::shared_ptr<CompletionHandler> &handler, const Socket& socket);
    
    /**
     * Initiate the read operation
     * Registers the socket with the proactor for read events
     * @param proactor The proactor to register with
     * @return true if the operation was initiated successfully, false otherwise
     */
    bool initiate(const std::shared_ptr<Proactor>& proactor);

private:
    // The socket file descriptor
    int m_socket;
};

} // namespace proactor

#endif // ASYNC_READ_OPERATION_HPP
