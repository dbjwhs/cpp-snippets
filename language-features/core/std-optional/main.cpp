// MIT License
// Copyright (c) 2025 dbjwhs

#include <optional>
#include <string>
#include <iostream>

// std::optional is a template class introduced in C++17 that represents an optional value -
// a value that may or may not be present. It's essentially a wrapper that can either contain
// a value of a specified type or contain nothing.
//
// Primary use cases for std::optional:
// 1. Representing optional/nullable values: When a function might return a value of type T or
//    might not have a value to return, instead of using special values (like -1, nullptr, etc.)
//    or exceptions.
// 2. Avoiding out parameters: Instead of using output parameters passed by reference, a function
//    can return an optional value.
// 3. Delayed initialization: When an object might not be initialized at construction time but
//    later in its lifecycle.
// 4. Error handling: As an alternative to exceptions or error codes, especially for functions
//    where "no result" is a normal outcome rather than an exceptional situation.
//
std::optional<std::string> findUserName(const int userId) {
    // Database lookup logic
    if (userId == 1) {
        return "John Doe";
    } else {
        return std::nullopt; // No user found
    }
}

int main() {
    auto name = findUserName(1);
    if (name) {
        std::cout << "User found: " << *name << std::endl;
    } else {
        std::cout << "User not found" << std::endl;
    }

    name = findUserName(2);
    if (name) {
        std::cout << "User found: " << *name << std::endl;
    } else {
        std::cout << "User not found" << std::endl;
    }

    return 0;
}
