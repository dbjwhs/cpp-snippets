#ifndef ASYNC_ACCEPT_OPERATION_HPP
#define ASYNC_ACCEPT_OPERATION_HPP

#include <memory>
#include "../async_operation.hpp"
#include "../socket.hpp"

namespace proactor {

// Forward declarations
class CompletionHandler;
class Proactor;

/**
 * Concrete asynchronous accept operation
 * Used to accept incoming connections on a listening socket
 */
class AsyncAcceptOperation : public AsyncOperation, public std::enable_shared_from_this<AsyncAcceptOperation> {
public:
    /**
     * Constructor taking a completion handler and server socket
     * @param handler The completion handler to notify when the operation completes
     * @param serverSocket The server socket to accept connections on
     */
    AsyncAcceptOperation(std::shared_ptr<CompletionHandler> handler, const Socket& serverSocket);
    
    /**
     * Initiate the accept operation
     * Registers the server socket with the proactor for accept events
     * @param proactor The proactor to register with
     * @return true if the operation was initiated successfully, false otherwise
     */
    bool initiate(const std::shared_ptr<Proactor>& proactor);

private:
    // The server socket file descriptor
    int m_serverSocket;
};

} // namespace proactor

#endif // ASYNC_ACCEPT_OPERATION_HPP