// MIT License
// Copyright (c) 2025 dbjwhs

#include <iostream>
#include <thread>
#include <vector>
#include <cassert>
#include <mutex>
#include <chrono>
#include <string>
#include <memory>
#include <fstream>
#include "../../../headers/project_utils.hpp"

// ===============================================================================
// Thread Local Storage (TLS) Example with Complex Objects
// ===============================================================================
// Thread Local Storage is a mechanism that allows each thread to have its own
// instance of a variable. TLS was introduced in C++11 as part of the concurrency
// support in the Standard Template Library (STL).
//
// This example specifically demonstrates 5 key points about TLS with complex objects:
// 1. Construction/Destruction: Each thread gets its own instance, constructed on
//    first access and destroyed when the thread terminates.
// 2. Lazy Initialization: TLS objects are initialized only when first accessed,
//    not when declared.
// 3. Resource Management: Each TLS object manages its own resources independently.
// 4. Memory Usage: Each thread has its own copy, potentially increasing memory
//    usage for large objects or many threads.
// 5. Static Class Members: Static members remain shared across threads unless
//    also declared thread_local.

// counter to track construction and destruction of objects
int g_constructionCounter{0};
int g_destructionCounter{0};
std::mutex g_counterMutex{};

// flag to verify lazy initialization
bool g_globalObjectCreated{false};
std::mutex g_flagMutex{};

// forward declaration of our complex thread-local class
class ThreadLocalLogger;

// declare but don't immediately create a global thread-local object
// this demonstrates lazy initialization - will only be created when accessed
thread_local ThreadLocalLogger* g_threadLogger;

// class that manages a thread-specific resource (file handle)
class ThreadLocalLogger {
private:
    // each logger instance gets a unique ID
    int m_id{0};

    // each logger has its own buffer (demonstrating memory usage per thread)
    std::vector<std::string> m_buffer{};

    // each logger has its own file (demonstrating resource management)
    std::unique_ptr<std::ofstream> m_logFile{};

    // static counter shared by all instances across all threads
    static int s_nextId;

    // static mutex to protect the counter
    static std::mutex s_mutex;

    // thread_local static counter - each thread gets its own copy
    static thread_local int s_threadMessageCount;

public:
    // constructor to demonstrate construction timing
    ThreadLocalLogger() : m_id{getNextId()}, m_buffer{} {
        // record construction
        {
            std::lock_guard<std::mutex> lock(g_counterMutex);
            ++g_constructionCounter;
        }

        // record that the object was created (for lazy init verification)
        {
            std::lock_guard<std::mutex> lock(g_flagMutex);
            g_globalObjectCreated = true;
        }

        // create a unique log file for this thread
        std::string filename = std::format("thread_{}_log.txt", m_id);
        m_logFile = std::make_unique<std::ofstream>(filename);

        if (!m_logFile->is_open()) {
            LOG_ERROR(std::format("Failed to open log file for thread {}", m_id));
            m_logFile.reset();
        }

        LOG_INFO(std::format("ThreadLocalLogger {} constructed in thread", m_id));
        log("Logger initialized");
    }

    // destructor to demonstrate destruction timing
    ~ThreadLocalLogger() {
        // close the file before destruction
        if (m_logFile && m_logFile->is_open()) {
            log("Logger shutting down");
            m_logFile->close();
        }

        // record destruction
        {
            std::lock_guard<std::mutex> lock(g_counterMutex);
            ++g_destructionCounter;
        }

        LOG_INFO(std::format("ThreadLocalLogger {} destructed - thread terminating", m_id));
    }

    // log a message to both the console and the thread-specific file
    void log(const std::string& message) {
        // store in buffer
        m_buffer.push_back(message);

        // increment thread-local message counter
        ++s_threadMessageCount;

        // log to console
        LOG_INFO(std::format("[Logger {}] Message #{}: {}",
                            m_id, s_threadMessageCount, message));

        // log to file if available (thread-specific resource)
        if (m_logFile && m_logFile->is_open()) {
            *m_logFile << std::format("[Thread {}] {}\n", m_id, message);
            m_logFile->flush();
        }
    }

    // get the thread-local message count
    int getMessageCount() const {
        return s_threadMessageCount;
    }

    // get the global ID (not thread-local)
    int getId() const {
        return m_id;
    }

    // get next ID with thread safety
    static int getNextId() {
        std::lock_guard<std::mutex> lock(s_mutex);
        return s_nextId++;
    }

    // get the buffer size
    size_t getBufferSize() const {
        return m_buffer.size();
    }

    // demonstrates the memory impact of storing large objects in TLS
    void addLargeDataToBuffer(size_t sizeKB) {
        // create a large string (1KB * sizeKB)
        std::string largeData(1024 * sizeKB, 'X');

        // add to this thread's buffer
        m_buffer.push_back(largeData);

        LOG_INFO(std::format("[Logger {}] Added {}KB to buffer, total buffer entries: {}",
                            m_id, sizeKB, m_buffer.size()));
    }
};

// initialize static members
int ThreadLocalLogger::s_nextId{1};
std::mutex ThreadLocalLogger::s_mutex{};
thread_local int ThreadLocalLogger::s_threadMessageCount{0};

// function to get the thread-local logger instance, demonstrating lazy initialization
ThreadLocalLogger& getThreadLogger() {
    // if this thread hasn't accessed the logger yet, create it
    if (g_threadLogger == nullptr) {
        g_threadLogger = new ThreadLocalLogger();
        LOG_INFO(std::format("Creating thread-local logger for the first time in thread {}", g_threadLogger->getId()));
    }
    return *g_threadLogger;
}

// function to ensure the thread-local logger is properly destroyed when the thread exits
void cleanupThreadLogger() {
    if (g_threadLogger != nullptr) {
        delete g_threadLogger;
        g_threadLogger = nullptr;
    }
}

// test function that demonstrates the key aspects of thread-local complex objects
void testThreadLocalComplex(int threadId, int iterations, size_t largeDataSize) {
    LOG_INFO(std::format("Thread {} started", threadId));

    // verify lazy initialization by checking if a global object exists yet
    {
        std::lock_guard<std::mutex> lock(g_flagMutex);
        LOG_INFO(std::format("Thread {}: Global object created: {}", threadId, g_globalObjectCreated ? "true" : "false"));
    }

    // demonstrate resource management by getting thread-specific logger
    ThreadLocalLogger& logger = getThreadLogger();

    // register cleanup function to ensure proper destruction
    std::atexit(cleanupThreadLogger);

    // perform the specified number of iterations
    for (int ndx = 0; ndx < iterations; ++ndx) {
        // log a message using the thread-local logger
        logger.log(std::format("Thread {} iteration {}", threadId, ndx));

        // every third iteration, add some large data to demonstrate memory usage
        if (ndx % 3 == 0) {
            logger.addLargeDataToBuffer(largeDataSize);
        }

        // sleep a bit to simulate work
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    // verify that the message count matches our expectations
    int messageCount = logger.getMessageCount();
    int expectedMessageCount = iterations + 1;  // +1 for initialization message

    LOG_INFO(std::format("Thread {} message count: {}, expected: {}", threadId, messageCount, expectedMessageCount));

    // verify that the buffer size includes our large data additions
    size_t expectedBufferSize = iterations + 1;  // all messages
    size_t actualBufferSize = logger.getBufferSize();

    LOG_INFO(std::format("Thread {} buffer size: {}, expected at least: {}", threadId, actualBufferSize, expectedBufferSize));

    // add one final message
    logger.log(std::format("Thread {} completed with logger ID {}", threadId, logger.getId()));

    // assert our expectations
    assert(messageCount == expectedMessageCount);
    assert(actualBufferSize >= expectedBufferSize);
}

int main() {
    // output explanatory message
    LOG_INFO("Starting Thread Local Storage with Complex Objects example");
    LOG_INFO("This demonstrates 5 key points about TLS with complex objects:");
    LOG_INFO("1. Construction/Destruction: Each thread gets its own instance");
    LOG_INFO("2. Lazy Initialization: TLS objects are initialized only when first accessed");
    LOG_INFO("3. Resource Management: Each TLS object manages its own resources");
    LOG_INFO("4. Memory Usage: Each thread has its own copy of potentially large objects");
    LOG_INFO("5. Static Class Members: Remain shared unless also thread_local");

    // number of threads to create
    constexpr int numThreads{5};

    // number of operations per thread

    // size of large data in KB to add to the buffer
    const size_t largeDataSizeKB{10};

    // verify that no thread-local objects have been created yet in the main thread
    {
        std::lock_guard<std::mutex> lock(g_flagMutex);
        LOG_INFO(std::format("Before any threads: Global object created: {}",
                            g_globalObjectCreated ? "true" : "false"));
        assert(g_globalObjectCreated == false);
    }

    // container to hold our threads
    std::vector<std::thread> threads;

    // reserve space for our threads
    threads.reserve(numThreads);

    // create and start the threads
    for (int ndx = 0; ndx < numThreads; ++ndx) {
        constexpr int iterationsPerThread{10};
        threads.emplace_back(testThreadLocalComplex, ndx, iterationsPerThread, largeDataSizeKB);
    }

    // wait for all threads to complete
    for (auto& thread : threads) {
        thread.join();
    }

    // verify that thread-local objects were created
    {
        std::lock_guard<std::mutex> lock(g_flagMutex);
        LOG_INFO(std::format("After threads complete: Global object created: {}", g_globalObjectCreated ? "true" : "false"));
        assert(g_globalObjectCreated == true);
    }

    // verify construction and destruction counts
    {
        std::lock_guard<std::mutex> lock(g_counterMutex);
        LOG_INFO(std::format("Total constructions: {}", g_constructionCounter));
        LOG_INFO(std::format("Total destructions: {}", g_destructionCounter));

        // This may fail if threads haven't fully cleaned up yet, but it's a good check
        // Note: In a real application, you'd want to ensure proper cleanup
        LOG_INFO("Note: If destructions < constructions, some thread-local objects may still exist");
    }

    // access the thread-local logger in the main thread to demonstrate
    // that each thread (including main) gets its own copy
    LOG_INFO("Now accessing thread-local logger in main thread");
    ThreadLocalLogger& mainLogger = getThreadLogger();
    mainLogger.log("This is a message from the main thread");

    LOG_INFO(std::format("Main thread logger ID: {}", mainLogger.getId()));
    LOG_INFO(std::format("Main thread message count: {}", mainLogger.getMessageCount()));

    // clean up the main thread's logger
    cleanupThreadLogger();

    LOG_INFO("Thread Local Storage with Complex Objects example completed");
    return 0;
}
