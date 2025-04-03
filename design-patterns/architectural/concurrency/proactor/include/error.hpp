// MIT License
// Copyright (c) 2025 dbjwhs

#ifndef PROACTOR_ERROR_HPP
#define PROACTOR_ERROR_HPP

#include <string>
#include <utility>

namespace proactor {

class Error {
public:
    // constructor with error code and message
    explicit Error(const int code = 0, std::string  message = "")
        : m_code(code), m_message(std::move(message)) {}

    // get the error code
    [[nodiscard]] int code() const {
        return m_code;
    }

    // get the error message
    [[nodiscard]] const std::string& message() const {
        return m_message;
    }

    // check if there is an error
    explicit operator bool() const {
        return m_code != 0;
    }
    
    // create an error from errno
    static Error fromErrno(const std::string& context);

private:
    // error code
    int m_code;

    // error message
    std::string m_message;
};

} // namespace proactor

#endif // PROACTOR_ERROR_HPP
