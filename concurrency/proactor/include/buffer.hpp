// MIT License
// Copyright (c) 2025 dbjwhs

#ifndef PROACTOR_BUFFER_HPP
#define PROACTOR_BUFFER_HPP

#include <string>
#include <cstring>

namespace proactor {

class Buffer {
public:
    // default constructor
    Buffer() : m_data(nullptr), m_size(0), m_capacity(0) {}

    // constructor with capacity
    explicit Buffer(size_t capacity) : m_size(0), m_capacity(capacity) {
        m_data = new char[capacity];
    }

    // constructor with data
    Buffer(const char* data, size_t size) : m_size(size), m_capacity(size) {
        m_data = new char[size];
        std::memcpy(m_data, data, size);
    }

    // move constructor
    Buffer(Buffer&& other) noexcept
        : m_data(other.m_data), m_size(other.m_size), m_capacity(other.m_capacity) {
        other.m_data = nullptr;
        other.m_size = 0;
        other.m_capacity = 0;
    }

    // destructor
    ~Buffer() {
        delete[] m_data;
    }

    // move assignment operator
    Buffer& operator=(Buffer&& other) noexcept {
        if (this != &other) {
            delete[] m_data;
            m_data = other.m_data;
            m_size = other.m_size;
            m_capacity = other.m_capacity;
            other.m_data = nullptr;
            other.m_size = 0;
            other.m_capacity = 0;
        }
        return *this;
    }

    // get the data
    char* data() { return m_data; }

    // get the data (const version)
    const char* data() const { return m_data; }

    // get the size
    size_t size() const { return m_size; }

    // get the capacity
    size_t capacity() const { return m_capacity; }

    // set the size
    void setSize(size_t size) { m_size = std::min(size, m_capacity); }

    // resize the buffer
    void resize(size_t newCapacity) {
        if (newCapacity > m_capacity) {
            char* newData = new char[newCapacity];
            if (m_data) {
                std::memcpy(newData, m_data, m_size);
                delete[] m_data;
            }
            m_data = newData;
            m_capacity = newCapacity;
        }
    }

    // append data to the buffer
    void append(const char* data, size_t size) {
        if (m_size + size > m_capacity) {
            resize(m_size + size);
        }
        std::memcpy(m_data + m_size, data, size);
        m_size += size;
    }

    // clear the buffer
    void clear() {
        m_size = 0;
    }

    // convert buffer to string
    std::string toString() const {
        return std::string(m_data, m_size);
    }

private:
    // buffer data
    char* m_data;

    // current size of data in the buffer
    size_t m_size;

    // total capacity of the buffer
    size_t m_capacity;

    // disable copy operations
    Buffer(const Buffer&) = delete;
    Buffer& operator=(const Buffer&) = delete;
};

} // namespace proactor

#endif // PROACTOR_BUFFER_HPP
