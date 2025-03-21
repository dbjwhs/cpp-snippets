// MIT License
// Copyright (c) 2025 dbjwhs

#include "../../include/socket.hpp"
#include <stdexcept>

namespace proactor {

Socket Socket::createTcp() {
    const int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        throw std::runtime_error(Error::fromErrno("Failed to create socket").message());
    }
    return Socket(fd);
}

bool Socket::isValid() const {
    return m_fd >= 0;
}

int Socket::fd() const {
    return m_fd;
}

Error Socket::setNonBlocking() const {
    int flags = fcntl(m_fd, F_GETFL, 0);
    if (flags < 0) {
        return Error::fromErrno("Failed to get socket flags");
    }

    flags |= O_NONBLOCK;
    if (fcntl(m_fd, F_SETFL, flags) < 0) {
        return Error::fromErrno("Failed to set socket non-blocking");
    }

    return Error(0, "");
}

Error Socket::setReuseAddress() const {
    constexpr int value = 1;
    if (setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, &value, sizeof(value)) < 0) {
        return Error::fromErrno("Failed to set socket reuse address");
    }

    return Error(0, "");
}

Error Socket::bind(const std::string& address, int port) const {
    struct sockaddr_in addr{};
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    if (inet_pton(AF_INET, address.c_str(), &addr.sin_addr) <= 0) {
        return Error(1, std::format("Invalid address: {}", address));
    }

    if (::bind(m_fd, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr)) < 0) {
        return Error::fromErrno("Failed to bind socket");
    }

    return Error(0, "");
}

Error Socket::listen(int backlog) const {
    if (::listen(m_fd, backlog) < 0) {
        return Error::fromErrno("Failed to listen on socket");
    }

    return Error(0, "");
}

std::pair<Socket, Error> Socket::accept() const {
    struct sockaddr_in addr{};
    socklen_t addrLen = sizeof(addr);

    int clientFd = ::accept(m_fd, reinterpret_cast<struct sockaddr*>(&addr), &addrLen);
    if (clientFd < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // no pending connections, not an error in non-blocking mode
            return {Socket(), Error(0, "")};
        }
        return {Socket(), Error::fromErrno("Failed to accept connection")};
    }

    return {Socket(clientFd), Error(0, "")};
}

Error Socket::connect(const std::string& address, int port) const {
    struct sockaddr_in addr{};
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    if (inet_pton(AF_INET, address.c_str(), &addr.sin_addr) <= 0) {
        return Error(1, std::format("Invalid address: {}", address));
    }

    if (::connect(m_fd, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr)) < 0) {
        if (errno == EINPROGRESS) {
            // connection in progress, not an error in non-blocking mode
            return Error(0, "");
        }
        return Error::fromErrno("Failed to connect");
    }

    return Error(0, "");
}

std::pair<ssize_t, Error> Socket::read(char* buffer, size_t size) const {
    ssize_t bytesRead = ::read(m_fd, buffer, size);
    if (bytesRead < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // would block, not an error in non-blocking mode
            return {0, Error(0, "")};
        }
        return {0, Error::fromErrno("Failed to read from socket")};
    }

    return {bytesRead, Error(0, "")};
}

std::pair<ssize_t, Error> Socket::write(const char* buffer, size_t size) const {
    ssize_t bytesWritten = ::write(m_fd, buffer, size);
    if (bytesWritten < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // would block, not an error in non-blocking mode
            return {0, Error(0, "")};
        }
        return {0, Error::fromErrno("Failed to write to socket")};
    }

    return {bytesWritten, Error(0, "")};
}

void Socket::close() {
    if (m_fd >= 0) {
        ::close(m_fd);
        m_fd = -1;
    }
}

} // namespace proactor