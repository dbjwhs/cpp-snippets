// MIT License
// Copyright (c) 2025 dbjwhs

#ifndef PROACTOR_ASYNC_OPERATION_HPP
#define PROACTOR_ASYNC_OPERATION_HPP

#include "completion_handler.hpp"
#include "operation_types.hpp"
#include "buffer.hpp"
#include <memory>
#include <atomic>

namespace proactor {

// Asynchronous operation base class
class AsyncOperation {
public:
    // constructor taking a completion handler
    AsyncOperation(std::shared_ptr<CompletionHandler> handler, OperationType type)
        : m_completionHandler(handler), m_type(type), m_cancelled(false) {}

    // virtual destructor for proper cleanup in derived classes
    virtual ~AsyncOperation() = default;

    // get the operation type
    OperationType type() const { return m_type; }

    // check if the operation is cancelled
    bool isCancelled() const { return m_cancelled; }

    // cancel the operation
    void cancel() { m_cancelled = true; }

    // complete the operation and notify the handler
    // parameters: result - the result of the operation (bytes transferred or error code)
    //             buffer - the data buffer associated with the operation
    void complete(ssize_t result, Buffer buffer) const;

protected:
    // the handler to be notified when this operation completes
    std::shared_ptr<CompletionHandler> m_completionHandler;

    // the type of operation
    OperationType m_type;

    // flag indicating whether the operation has been cancelled
    std::atomic<bool> m_cancelled;
};

} // namespace proactor

#endif // PROACTOR_ASYNC_OPERATION_HPP