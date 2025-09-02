// MIT License
// Copyright (c) 2025 dbjwhs

#include "../../include/event_queue.hpp"
#include <stdexcept>
#include <format>
#include <cerrno>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include "../../../../headers/project_utils.hpp"

namespace proactor {

EventQueue::EventQueue() : m_running(false) {
    m_kqueueFd = kqueue();
    if (m_kqueueFd < 0) {
        throw std::runtime_error(Error::fromErrno("Failed to create kqueue").message());
    }

    // create a pipe for signaling the event loop to wake up
    if (pipe(m_wakePipe) < 0) {
        ::close(m_kqueueFd);
        throw std::runtime_error(Error::fromErrno("Failed to create wake pipe").message());
    }

    // set the read end of the pipe to non-blocking
    if (const int flags = fcntl(m_wakePipe[0], F_GETFL, 0); flags < 0 || fcntl(m_wakePipe[0], F_SETFL, flags | O_NONBLOCK) < 0) {
        ::close(m_wakePipe[0]);
        ::close(m_wakePipe[1]);
        ::close(m_kqueueFd);
        throw std::runtime_error(Error::fromErrno("Failed to set pipe non-blocking").message());
    }

    // register the read end of the pipe with kqueue
    struct kevent event{};
    EV_SET(&event, m_wakePipe[0], EVFILT_READ, EV_ADD, 0, 0, nullptr);
    if (kevent(m_kqueueFd, &event, 1, nullptr, 0, nullptr) < 0) {
        ::close(m_wakePipe[0]);
        ::close(m_wakePipe[1]);
        ::close(m_kqueueFd);
        throw std::runtime_error(Error::fromErrno("Failed to register wake pipe with kqueue").message());
    }
}

EventQueue::~EventQueue() {
    stop();

    ::close(m_wakePipe[0]);
    ::close(m_wakePipe[1]);
    ::close(m_kqueueFd);
}

Error EventQueue::registerForRead(int fd, void* userData) const {
    struct kevent event{};
    EV_SET(&event, fd, EVFILT_READ, EV_ADD, 0, 0, userData);

    if (kevent(m_kqueueFd, &event, 1, nullptr, 0, nullptr) < 0) {
        return Error::fromErrno("Failed to register for read events");
    }

    return Error(0, "");
}

Error EventQueue::registerForWrite(const int fd, void* userData) const {
    struct kevent event{};
    EV_SET(&event, fd, EVFILT_WRITE, EV_ADD, 0, 0, userData);

    if (kevent(m_kqueueFd, &event, 1, nullptr, 0, nullptr) < 0) {
        return Error::fromErrno("Failed to register for write events");
    }

    return Error(0, "");
}

Error EventQueue::unregisterForRead(int fd) const {
    struct kevent event{};
    EV_SET(&event, fd, EVFILT_READ, EV_DELETE, 0, 0, nullptr);

    if (kevent(m_kqueueFd, &event, 1, nullptr, 0, nullptr) < 0) {
        if (errno == ENOENT) {
            // the event was not registered, not an error
            return Error(0, "");
        }
        return Error::fromErrno("Failed to unregister for read events");
    }

    return Error(0, "");
}

Error EventQueue::unregisterForWrite(int fd) const {
    struct kevent event{};
    EV_SET(&event, fd, EVFILT_WRITE, EV_DELETE, 0, 0, nullptr);

    if (kevent(m_kqueueFd, &event, 1, nullptr, 0, nullptr) < 0) {
        if (errno == ENOENT) {
            // the event was not registered, not an error
            return Error(0, "");
        }
        return Error::fromErrno("Failed to unregister for write events");
    }

    return Error(0, "");
}

void EventQueue::start(const std::function<void(int, int, void*)>& callback) {
    m_running = true;
    m_thread = std::thread([this, callback]() {
        Logger::getInstance().log(LogLevel::INFO, "event loop started");

        struct timespec timeout{};
        timeout.tv_sec = 0;  // 500ms timeout for kevent to be more responsive
        timeout.tv_nsec = 500000000;

        while (m_running) {
            struct kevent events[64];
            Logger::getInstance().log(LogLevel::INFO, "Waiting for events...");
            
            // Use a timeout so we don't block indefinitely and can process other tasks
            int numEvents = kevent(m_kqueueFd, nullptr, 0, events, 64, &timeout);

            if (numEvents < 0) {
                if (errno == EINTR) {
                    // interrupted by a signal, just retry
                    Logger::getInstance().log(LogLevel::INFO, "Interrupted by signal, retrying");
                    continue;
                }

                Logger::getInstance().log(LogLevel::ERROR,
                                      std::format("kevent error: {}", strerror(errno)));
                break;
            }
            
            if (numEvents == 0) {
                // Timeout, no events - Check state of sockets periodically
                Logger::getInstance().log(LogLevel::INFO, "Timeout - no events");
                // This is a good place to add any maintenance or timeout handling logic
                continue;
            }
            
            Logger::getInstance().log(LogLevel::INFO, 
                std::format("Received {} events", numEvents));

            for (int i = 0; i < numEvents; ++i) {
                int fd = static_cast<int>(events[i].ident);

                if (fd == m_wakePipe[0]) {
                    // wake-up event, drain the pipe
                    char buffer[256];
                    while (read(m_wakePipe[0], buffer, sizeof(buffer)) > 0) {}
                    Logger::getInstance().log(LogLevel::INFO, "Wake-up event processed");
                    continue;
                }

                // determine the event filter (read or write)
                const int filter = events[i].filter;
                
                // Check for errors or special conditions
                int flags = events[i].flags;
                if (flags & EV_ERROR) {
                    Logger::getInstance().log(LogLevel::ERROR, 
                        std::format("Error on fd {}: {}", fd, strerror(events[i].data)));
                    continue;
                }
                
                Logger::getInstance().log(LogLevel::INFO, 
                    std::format("Event on fd {}, filter {}, flags 0x{:x}, data {}", 
                        fd, 
                        (filter == EVFILT_READ) ? "READ" : 
                        (filter == EVFILT_WRITE) ? "WRITE" : "UNKNOWN",
                        flags,
                        events[i].data));

                // get the user data associated with the event
                void* userData = events[i].udata;
                
                // Log if userData is null
                if (userData == nullptr) {
                    Logger::getInstance().log(LogLevel::WARNING, 
                        std::format("Event on fd {} has null userData", fd));
                }

                // invoke the callback
                callback(fd, filter, userData);
            }
        }

        Logger::getInstance().log(LogLevel::INFO, "event loop stopped");
    });
}

void EventQueue::stop() {
    if (m_running) {
        m_running = false;
        wakeUp();

        if (m_thread.joinable()) {
            m_thread.join();
        }
    }
}

void EventQueue::wakeUp() const {
    constexpr char byte = 1;
    [[maybe_unused]] ssize_t result = write(m_wakePipe[1], &byte, 1);
}

} // namespace proactor
