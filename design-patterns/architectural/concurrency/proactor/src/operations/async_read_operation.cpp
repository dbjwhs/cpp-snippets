#include "../../include/operations/async_read_operation.hpp"
#include "../../include/proactor.hpp"
#include "../../include/completion_handler.hpp"
#include "../../include/logger.hpp"
#include <sys/socket.h>
#include <format>
#include <cerrno>
#include <cstring>

namespace proactor {

AsyncReadOperation::AsyncReadOperation(const std::shared_ptr<CompletionHandler> &handler, const Socket& socket)
    : AsyncOperation(handler, OperationType::READ), m_socket(socket.fd()) {}

bool AsyncReadOperation::initiate(const std::shared_ptr<Proactor>& proactor) {
    LOG_INFO(std::format("Initiating read operation on socket {}", m_socket));
        
    if (m_socket < 0) {
        LOG_ERROR("Invalid socket");
        return false;
    }
    
    // Check socket validity with a quick diagnostic
    int error = 0;
    socklen_t errLen = sizeof(error);
    if (getsockopt(m_socket, SOL_SOCKET, SO_ERROR, &error, &errLen) < 0) {
        LOG_ERROR(std::format("Socket diagnostic failed: {}", strerror(errno)));
        return false;
    }
    
    if (error != 0) {
        LOG_ERROR(std::format("Socket has error state: {}", strerror(error)));
        return false;
    }
    
    // Try a zero-byte read to see if the socket is readable
    char testBuf[1];
    if (const ssize_t result = recv(m_socket, testBuf, 0, MSG_PEEK); result < 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
        LOG_ERROR(std::format("Socket read test failed: {}", strerror(errno)));
        return false;
    }

    proactor->registerOperation(m_socket, shared_from_this());
    return true;
}

} // namespace proactor
