// MIT License
// Copyright (c) 2025 dbjwhs

#include "../../include/async_operation.hpp"

namespace proactor {

void AsyncOperation::complete(const ssize_t result, Buffer buffer) const {
    if (!m_cancelled && m_completionHandler) {
        m_completionHandler->handleCompletion(result, std::move(buffer));
    }
}

} // namespace proactor
