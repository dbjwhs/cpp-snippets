// MIT License
// Copyright (c) 2025 dbjwhs

#ifndef PROACTOR_PROACTOR_HPP
#define PROACTOR_PROACTOR_HPP

#include "event_queue.hpp"
#include "async_operation.hpp"
#include <memory>
#include <mutex>
#include <map>
#include <atomic>

namespace proactor {

// Proactor class
class Proactor {
public:
    // constructor initializes the event queue
    Proactor();

    // start the proactor event loop
    void start();

    // stop the proactor event loop
    void stop();

    // register an asynchronous operation with the proactor
    void registerOperation(int fd, const std::shared_ptr<AsyncOperation>& operation);

    // cancel an asynchronous operation
    void cancelOperation(int fd);

private:
    // handle an event from the event queue
    void handleEvent(int fd, int filter, void* userData);
    
    // handle different operation types
    void handleAccept(int fd, const std::shared_ptr<AsyncOperation>& operation);
    static void handleConnect(int fd, const std::shared_ptr<AsyncOperation>& operation);
    static void handleRead(int fd, const std::shared_ptr<AsyncOperation>& operation);
    void handleWrite(int fd, const std::shared_ptr<AsyncOperation>& operation);

    // event queue
    std::unique_ptr<EventQueue> m_eventQueue;

    // mutex for protecting the operations map
    std::mutex m_mutex;

    // map of file descriptors to operations
    std::map<int, std::shared_ptr<AsyncOperation>> m_operations;

    // flag indicating whether the proactor is running
    std::atomic<bool> m_running;
};

} // namespace proactor

#endif // PROACTOR_PROACTOR_HPP