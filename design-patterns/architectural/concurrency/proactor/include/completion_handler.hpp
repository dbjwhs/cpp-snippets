// MIT License
// Copyright (c) 2025 dbjwhs

#ifndef PROACTOR_COMPLETION_HANDLER_HPP
#define PROACTOR_COMPLETION_HANDLER_HPP

#include "buffer.hpp"
#include <memory>

namespace proactor {

// Completion handler interface
class CompletionHandler {
public:
    // virtual destructor for proper cleanup in derived classes
    virtual ~CompletionHandler() = default;

    // handle completion of an asynchronous operation
    // parameters: result - the result of the operation (bytes transferred or error code)
    //             buffer - the data buffer associated with the operation
    virtual void handleCompletion(ssize_t result, Buffer buffer) = 0;
};

} // namespace proactor

#endif // PROACTOR_COMPLETION_HANDLER_HPP
