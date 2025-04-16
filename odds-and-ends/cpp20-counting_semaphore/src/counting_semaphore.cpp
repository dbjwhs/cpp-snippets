// MIT License
// Copyright (c) 2025 dbjwhs

#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <semaphore>
#include <mutex>
#include <atomic>
#include <cassert>
#include <format>
#include "../../../headers/project_utils.hpp"

// counting semaphore pattern history:
// the counting semaphore is a synchronization primitive that was first introduced by dutch computer scientist
// edsger w. dijkstra in 1965. it extends the concept of a binary semaphore (mutex) by allowing multiple
// threads to access a resource simultaneously, up to a predefined limit. essentially, it maintains a counter
// that represents the number of available resources. when a thread acquires the semaphore, the counter is
// decremented; when a thread releases it, the counter is incremented. if the counter would go below zero
// upon acquisition, the requesting thread is blocked until another thread releases the semaphore.
//
// in c++20, std::counting_semaphore was introduced as part of the standard library's synchronization
// primitives, along with std::binary_semaphore (which is essentially a counting_semaphore with a max count
// of 1). this implementation provides a portable, efficient way to use counting semaphores in modern c++
// applications. common applications include:
// - limiting concurrent access to a finite pool of resources
// - implementing producer-consumer patterns
// - controlling access to thread pools
// - implementing throttling mechanisms for i/o or network operations

// maximum number of threads that can access the resource simultaneously
constexpr int MAX_CONCURRENT_THREADS = 3;
constexpr int MAX_NUMBER_OF_THREADS_MULTIPLE = 10;

class ResourceManager {
private:
    // m_semaphore limits the number of threads that can access the resource simultaneously to MAX_CONCURRENT_THREADS
    std::counting_semaphore<MAX_CONCURRENT_THREADS> m_semaphore{MAX_CONCURRENT_THREADS};
    
    // m_mutex protects m_active_threads for thread-safe access
    // declared mutable because we need to lock it in const methods
    mutable std::mutex m_mutex;

    // m_active_threads tracks the number of threads currently using the resource
    int m_active_threads = 0;

    // m_total_acquisitions tracks the total number of successful resource acquisitions
    std::atomic<int> m_total_acquisitions{0};

public:
    // acquire attempts to acquire the resource
    // returns true if successful, false if it would block
    bool acquire(const bool non_blocking = false) {
        // try to acquire the semaphore
        if (non_blocking) {
            // non-blocking acquisition attempt
            if (!m_semaphore.try_acquire()) {
                return false;
            }
        } else {
            // blocking acquisition - wait until a resource is available
            m_semaphore.acquire();
        }

        // update active threads counter safely
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_active_threads++;

            // verify that we never exceed our maximum concurrent threads
            assert(m_active_threads <= MAX_CONCURRENT_THREADS && "active threads exceeds maximum allowed");
        }

        // increment total acquisitions counter
        ++m_total_acquisitions;

        return true;
    }

    // release returns a previously acquired resource back to the pool
    void release() {
        // update active threads counter safely
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            // verify we have resources to release
            assert(m_active_threads > 0 && "trying to release without active acquisitions");
            m_active_threads--;
        }

        // release the semaphore, potentially unblocking waiting threads
        m_semaphore.release();
    }

    // get_active_threads returns the current number of threads using the resource
    int get_active_threads() const {
        std::lock_guard<std::mutex> lock(m_mutex);  // now works with mutable mutex
        return m_active_threads;
    }

    // get_total_acquisitions returns the total number of successful acquisitions
    int get_total_acquisitions() const {
        return m_total_acquisitions;
    }
};

// worker_thread simulates a thread that uses the resource for a period of time
void worker_thread(ResourceManager& resource, int thread_id, int work_duration_ms) {
    // log the thread start
    LOG_INFO(std::format("Thread {} starting", thread_id));

    // attempt to acquire the resource
    if (resource.acquire()) {
        // log successful acquisition
        LOG_INFO(std::format("Thread {} acquired resource, active threads: {}",
                                 thread_id, resource.get_active_threads()));

        // simulate doing some work with the resource
        std::this_thread::sleep_for(std::chrono::milliseconds(work_duration_ms));

        // release the resource
        resource.release();

        // log release
        LOG_INFO(std::format("Thread {} released resource, active threads: {}",
                                 thread_id, resource.get_active_threads()));
    } else {
        // this case only happens with non-blocking acquisition attempts
        LOG_INFO(std::format("Thread {} failed to acquire resource", thread_id));
    }
}

// test_basic_functionality tests the basic acquired and release functionality
void test_basic_functionality() {
    LOG_INFO("Starting basic functionality test");

    ResourceManager resource;

    // verify initial state
    assert(resource.get_active_threads() == 0);
    assert(resource.get_total_acquisitions() == 0);

    // acquire all available resources
    for (int ndx = 0; ndx < MAX_CONCURRENT_THREADS; ndx++) {
        const bool acquired = resource.acquire();
        assert(acquired && "Should be able to acquire resource");
        assert(resource.get_active_threads() == ndx + 1);
        assert(resource.get_total_acquisitions() == ndx + 1);
    }

    // verify that non-blocking acquire fails when all resources are taken
    const bool acquired = resource.acquire(true);
    assert(!acquired && "Should not be able to acquire more than MAX_CONCURRENT_THREADS resources");

    // release all resources
    for (int ndx = 0; ndx < MAX_CONCURRENT_THREADS; ndx++) {
        resource.release();
        LOG_INFO(std::format("Release threads: active count {}", resource.get_active_threads()));
        assert(resource.get_active_threads() == MAX_CONCURRENT_THREADS - ndx - 1);
    }

    LOG_INFO("Basic functionality test completed successfully");
}

// test_concurrent_access tests the semaphore with multiple threads
void test_concurrent_access() {
    LOG_INFO("Starting concurrent access test");

    ResourceManager resource;
    std::vector<std::thread> threads;

    // create more threads than the semaphore allows (5x MAX_CONCURRENT_THREADS)
    constexpr int total_threads = MAX_CONCURRENT_THREADS * MAX_NUMBER_OF_THREADS_MULTIPLE;

    // launch all threads
    for (int ndx = 0; ndx < total_threads; ndx++) {
        // vary work duration to create overlapping resource usage
        int work_duration = 100 + (ndx * 50);
        threads.emplace_back(worker_thread, std::ref(resource), ndx, work_duration);
    }

    // wait for all threads to complete
    for (auto& thread : threads) {
        thread.join();
    }

    // verify final state
    assert(resource.get_active_threads() == 0 && "All threads should have released the resource");
    assert(resource.get_total_acquisitions() == total_threads && "All threads should have acquired the resource once");

    LOG_INFO(std::format("Concurrent access test completed successfully with {} threads", total_threads));
}

// test_custom_max_value tests dsemaphore with a custom max value
void test_custom_max_value() {
    LOG_INFO("Starting custom max value test");

    // create semaphore with a custom max value
    constexpr int custom_max = 5;
    std::counting_semaphore<custom_max> custom_semaphore(custom_max);

    // acquire all available permits
    for (int ndx = 0; ndx < custom_max; ndx++) {
        custom_semaphore.acquire();
        LOG_INFO(std::format("Acquired semaphore permit {}", ndx + 1));
    }

    // this would block if we tried to acquire another permit

    // release all permits
    for (int ndx = 0; ndx < custom_max; ndx++) {
        custom_semaphore.release();
        LOG_INFO(std::format("Released semaphore permit {}", custom_max - ndx));
    }

    LOG_INFO("Custom max value test completed successfully");
}

int main() {
    LOG_INFO("Starting counting semaphore example");

    // run the tests
    test_basic_functionality();
    test_concurrent_access();
    test_custom_max_value();

    LOG_INFO("All tests completed successfully");

    return 0;
}
