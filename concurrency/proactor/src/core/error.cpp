// MIT License
// Copyright (c) 2025 dbjwhs

#include "../../include/error.hpp"
#include <format>
#include <cerrno>
#include <cstring>

namespace proactor {

// static method to create an error from errno
Error Error::fromErrno(const std::string& context) {
    return Error(errno, std::format("{}: {} ({})", context, strerror(errno), errno));
}

} // namespace proactor
