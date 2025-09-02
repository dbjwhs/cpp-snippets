// MIT License
// Copyright (c) 2025 dbjwhs

#include <memory>
#include <string>
#include <cassert>
#include <iostream>
#include "../../../headers/project_utils.hpp"

class StringWrapper {
public:
    // constructors and destructor
    StringWrapper();
    explicit StringWrapper(const std::string& str);
    ~StringWrapper();
    
    // explicitly declare special member functions
    StringWrapper(const StringWrapper& other);
    StringWrapper& operator=(const StringWrapper& other);
    StringWrapper(StringWrapper&& other) noexcept;
    StringWrapper& operator=(StringWrapper&& other) noexcept;
    
    // public interface
    void append(const std::string& str) const;
    void clear() const;
    [[nodiscard]] std::string get() const;
    
    [[nodiscard]] size_t length() const;
    [[nodiscard]] bool empty() const;

private:
    // forward declaration of implementation class
    class Impl;
    // pointer to implementation
    std::unique_ptr<Impl> m_pImpl;
};


// implementation class definition note users of stringwrapper need not know these details
class StringWrapper::Impl {
public:
    explicit Impl(std::string  str = "") : m_data(std::move(str)) {}
    
    // implementation methods
    void append(const std::string& str) {
        m_data += str;
    }
    
    void clear() {
        m_data.clear();
    }

    [[nodiscard]] std::string get() const {
        return m_data;
    }
    
    [[nodiscard]] size_t length() const {
        return m_data.length();
    }
    
    [[nodiscard]] bool empty() const {
        return m_data.empty();
    }
    
private:
    std::string m_data;  // actual string storage
};

// stringwrapper method implementations
StringWrapper::StringWrapper() : m_pImpl(std::make_unique<Impl>()) {}
StringWrapper::StringWrapper(const std::string& str) : m_pImpl(std::make_unique<Impl>(str)) {}
StringWrapper::~StringWrapper() = default;

// copy constructor
StringWrapper::StringWrapper(const StringWrapper& other) : m_pImpl(std::make_unique<Impl>(*other.m_pImpl)) {}

// copy assignment
StringWrapper& StringWrapper::operator=(const StringWrapper& other) {
    if (this != &other) {
        m_pImpl = std::make_unique<Impl>(*other.m_pImpl);
    }
    return *this;
}

// move constructor
StringWrapper::StringWrapper(StringWrapper&& other) noexcept = default;

// move assignment
StringWrapper& StringWrapper::operator=(StringWrapper&& other) noexcept = default;

// delegate all public methods to implementation
void StringWrapper::append(const std::string& str) const {
    m_pImpl->append(str);
}

void StringWrapper::clear() const {
    m_pImpl->clear();
}

std::string StringWrapper::get() const {
    return m_pImpl->get();
}

size_t StringWrapper::length() const {
    return m_pImpl->length();
}

bool StringWrapper::empty() const {
    return m_pImpl->empty();
}

// main.cpp
int main() {
    LOG_INFO("testing stringwrapper implementation...\n");

    // test default constructor
    const StringWrapper empty;
    assert(empty.empty());
    assert(empty.empty());
    LOG_INFO("default constructor: passed");

    // test parameterized constructor
    const StringWrapper hello("hello");
    assert(!hello.empty());
    assert(hello.length() == 5);
    assert(hello.get() == "hello");
    LOG_INFO("parameterized constructor: passed");

    // test append
    hello.append(" world");
    assert(hello.length() == 11);
    assert(hello.get() == "hello world");
    LOG_INFO("append: passed");

    // test copy constructor
    StringWrapper copy(hello);
    assert(copy.get() == hello.get());
    LOG_INFO("copy constructor: passed");

    // test copy assignment
    StringWrapper assigned;
    assigned = hello;
    assert(assigned.get() == hello.get());
    LOG_INFO("copy assignment: passed");

    // test move constructor
    StringWrapper moved(std::move(copy));
    assert(moved.get() == "hello world");
    LOG_INFO("move constructor: passed");

    // test move assignment
    StringWrapper move_assigned;
    move_assigned = std::move(moved);
    assert(move_assigned.get() == "hello world");
    LOG_INFO("move assignment: passed");

    // test clear
    move_assigned.clear();
    assert(move_assigned.empty());
    assert(move_assigned.empty());
    LOG_INFO("clear: passed");

    LOG_INFO("all tests passed successfully!");
    return 0;
}
