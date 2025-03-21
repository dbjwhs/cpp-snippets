#include "../../include/operations/async_accept_operation.hpp"
#include "../../include/proactor.hpp"
#include "../../include/completion_handler.hpp"
#include "../../include/logger.hpp"
#include <format>
#include <utility>

namespace proactor {

AsyncAcceptOperation::AsyncAcceptOperation(std::shared_ptr<CompletionHandler> handler, const Socket& serverSocket)
    : AsyncOperation(std::move(handler), OperationType::ACCEPT), m_serverSocket(serverSocket.fd()) {}

bool AsyncAcceptOperation::initiate(const std::shared_ptr<Proactor>& proactor) {
    Logger::getInstance().log(LogLevel::INFO, 
        std::format("Initiating accept operation on server socket {}", m_serverSocket));
        
    if (m_serverSocket < 0) {
        Logger::getInstance().log(LogLevel::ERROR, "Invalid server socket");
        return false;
    }
    
    proactor->registerOperation(m_serverSocket, shared_from_this());
    return true;
}

} // namespace proactor