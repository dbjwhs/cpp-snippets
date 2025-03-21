#ifndef ASYNC_CONNECT_OPERATION_HPP
#define ASYNC_CONNECT_OPERATION_HPP

#include <memory>
#include <string>
#include "../async_operation.hpp"
#include "../socket.hpp"

namespace proactor {

// Forward declarations
class CompletionHandler;
class Proactor;

/**
 * Concrete asynchronous connect operation
 * Used to connect a socket to a remote address asynchronously
 */
class AsyncConnectOperation : public AsyncOperation, public std::enable_shared_from_this<AsyncConnectOperation> {
public:
    /**
     * Constructor taking a completion handler, socket, address, and port
     * @param handler The completion handler to notify when the operation completes
     * @param socket The socket to connect
     * @param address The remote address to connect to
     * @param port The remote port to connect to
     */
    AsyncConnectOperation(const std::shared_ptr<CompletionHandler>& handler,
                          const Socket &socket,
                          std::string  address,
                          int port);
    
    /**
     * Initiate the connect operation
     * Starts the asynchronous connection and registers the socket with the proactor
     * @param proactor The proactor to register with
     * @return true if the operation was initiated successfully, false otherwise
     */
    bool initiate(const std::shared_ptr<Proactor>& proactor);

private:
    // The socket file descriptor
    int m_socket;

    // The remote address
    std::string m_address;

    // The remote port
    int m_port;
};

} // namespace proactor

#endif // ASYNC_CONNECT_OPERATION_HPP