// MIT License
// Copyright (c) 2025 dbjwhs

#ifndef PROACTOR_SOCKET_HPP
#define PROACTOR_SOCKET_HPP

#include "error.hpp"
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <format>
#include <utility>

namespace proactor {

class Socket {
public:
    // default constructor
    Socket() : m_fd(-1) {}

    // constructor with existing file descriptor
    explicit Socket(int fd) : m_fd(fd) {}

    // move constructor
    Socket(Socket&& other) noexcept : m_fd(other.m_fd) {
        other.m_fd = -1;
    }

    // destructor
    ~Socket() {
        close();
    }

    // move assignment operator
    Socket& operator=(Socket&& other) noexcept {
        if (this != &other) {
            close();
            m_fd = other.m_fd;
            other.m_fd = -1;
        }
        return *this;
    }

    // create a tcp socket
    static Socket createTcp();

    // check if the socket is valid
    bool isValid() const;

    // get the file descriptor
    int fd() const;

    // set the socket to non-blocking mode
    Error setNonBlocking() const;

    // set the socket to reuse address
    Error setReuseAddress() const;

    // bind the socket to an address and port
    Error bind(const std::string& address, int port) const;

    // listen for incoming connections
    Error listen(int backlog = 128) const;

    // accept an incoming connection
    std::pair<Socket, Error> accept() const;

    // connect to a remote address and port
    Error connect(const std::string& address, int port) const;

    // read data from the socket
    std::pair<ssize_t, Error> read(char* buffer, size_t size) const;

    // write data to the socket
    std::pair<ssize_t, Error> write(const char* buffer, size_t size) const;

    // close the socket
    void close();

private:
    // socket file descriptor
    int m_fd;
    
    // disable copy operations
    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;
};

} // namespace proactor

#endif // PROACTOR_SOCKET_HPP
