// MIT License
// Copyright (c) 2025 dbjwhs

#ifndef PROACTOR_EVENT_QUEUE_HPP
#define PROACTOR_EVENT_QUEUE_HPP

#include "error.hpp"
#include <functional>
#include <thread>
#include <atomic>
#include <sys/event.h>
#include <sys/time.h>

namespace proactor {

// Event queue using kqueue
class EventQueue {
public:
    // constructor creates the kqueue descriptor
    EventQueue();

    // destructor cleans up resources
    ~EventQueue();

    // register a socket for read events
    Error registerForRead(int fd, void* userData) const;

    // register a socket for write events
    Error registerForWrite(int fd, void* userData) const;

    // unregister a socket for read events
    Error unregisterForRead(int fd) const;

    // unregister a socket for write events
    Error unregisterForWrite(int fd) const;

    // start the event loop in a separate thread
    void start(const std::function<void(int, int, void*)>& callback);

    // stop the event loop
    void stop();

    // wake up the event loop
    void wakeUp() const;

private:
    // kqueue file descriptor
    int m_kqueueFd;

    // pipe for waking up the event loop [read_fd, write_fd]
    int m_wakePipe[2]{};

    // flag indicating whether the event loop is running
    std::atomic<bool> m_running;

    // thread for running the event loop
    std::thread m_thread;
};

} // namespace proactor

#endif // PROACTOR_EVENT_QUEUE_HPP
