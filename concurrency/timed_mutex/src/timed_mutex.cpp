// MIT License
// Copyright (c) 2025 dbjwhs

#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <cassert>
#include <vector>
#include <atomic>
#include "../../../headers/project_utils.hpp"

// std::timed_mutex history and usage:
// timed_mutex was introduced in c++11 as part of the concurrency support library. it extends the basic
// mutex by adding the ability to specify a timeout when attempting to acquire the lock. this is particularly
// useful in scenarios where a thread should not wait indefinitely for a resource but instead should time out
// after a certain period and proceed with alternative actions. common use cases include resource accessing
// with timeouts, deadlock prevention, and implementing responsive applications that cannot afford unbounded
// wait times. timed_mutex provides two timeout methods: try_lock_for (which takes a duration) and try_lock_until
// (which takes an absolute time point). unlike regular mutex, timed_mutex allows threads to attempt acquisition
// for a specified time period before giving up, making it ideal for real-time systems and applications where
// responsiveness is critical.

class SharedResource {
private:
    // mutex for protecting shared data
    std::timed_mutex m_mutex;

    // shared counter that will be accessed by multiple threads
    int m_counter;

    // flag to track if a resource is in a valid state
    bool m_valid;

    // atomic flag to track if tests passed
    std::atomic<bool> m_tests_passed;

public:
    // constructor initializes the shared resource
    SharedResource() : m_counter{0}, m_valid{true}, m_tests_passed{true} {
        LOG_INFO("shared resource initialized");
    }

    // an attempt to increment counter with a timeout
    // returns true if the operation was successful
    bool tryIncrementWithTimeout(const std::chrono::milliseconds timeout) {
        // attempt to acquire the mutex with a timeout
        if (m_mutex.try_lock_for(timeout)) {
            // critical section - we got the lock
            m_counter++;
            LOG_INFO(std::format("counter incremented to {}", m_counter));

            // simulate some work being done
            std::this_thread::sleep_for(std::chrono::milliseconds(50));

            // release the mutex
            m_mutex.unlock();
            return true;
        }
        // failed to acquire the lock within the timeout period
        LOG_WARNING("failed to acquire lock within timeout period");
        return false;
    }

    // an attempt to increment counter with an absolute timeout point
    // returns true if the operation was successful
    bool tryIncrementUntil(const std::chrono::steady_clock::time_point timePoint) {
        // attempt to acquire the mutex with an absolute timeout
        if (m_mutex.try_lock_until(timePoint)) {
            // critical section - we got the lock
            m_counter++;
            LOG_INFO(std::format("counter incremented to {}", m_counter));

            // simulate some work being done
            std::this_thread::sleep_for(std::chrono::milliseconds(50));

            // release the mutex
            m_mutex.unlock();
            return true;
        }
        // failed to acquire the lock before the specified time point
        LOG_WARNING("failed to acquire lock before the specified time point");
        return false;
    }

    // get the current counter-value (with timeout protection)
    int getCounterWithTimeout(const std::chrono::milliseconds timeout, const int defaultValue = -1) {
        // attempt to acquire the mutex with a timeout
        if (m_mutex.try_lock_for(timeout)) {
            // critical section - we got the lock
            const int value = m_counter;

            // release the mutex
            m_mutex.unlock();
            return value;
        }
        // failed to acquire the lock within the timeout period
        LOG_WARNING("failed to read counter within timeout period");
        return defaultValue;
    }

    // deliberately hold the lock for an extended period to test timeout scenarios
    void holdLockFor(const std::chrono::milliseconds duration) {
        // acquire the mutex (blocking call, no timeout)
        m_mutex.lock();

        LOG_INFO(std::format("acquired lock, holding for {} ms", duration.count()));

        // hold the lock for the specified duration
        std::this_thread::sleep_for(duration);

        // release the mutex
        m_mutex.unlock();
        LOG_INFO("lock released after hold period");
    }

    // run comprehensive tests for timed_mutex functionality
    void runTests() {
        LOG_INFO("starting timed_mutex tests");

        // test 1: basic lock acquisition
        {
            const bool acquired = tryIncrementWithTimeout(std::chrono::milliseconds(100));
            LOG_INFO(std::format("test 1 - basic acquisition: {}", acquired ? "passed" : "failed"));
            assert(acquired && "test 1 failed: basic lock acquisition");
        }

        // test 2: timeout when lock is held
        {
            // start a thread that holds the lock for a while
            std::thread lockHolder([this]() {
                this->holdLockFor(std::chrono::milliseconds(300));
            });

            // give the thread time to acquire the lock
            std::this_thread::sleep_for(std::chrono::milliseconds(50));

            // try to acquire with a short timeout - should fail
            if (const bool acquired = tryIncrementWithTimeout(std::chrono::milliseconds(50))) {
                LOG_ERROR("test 2 failed: timeout did not occur when expected");
                m_tests_passed = false;
                assert(!acquired && "test 2 failed: timeout did not occur when expected");
            } else {
                LOG_INFO("test 2 - timeout when lock held: passed");
            }

            // wait for the lock-holding thread to finish
            lockHolder.join();
        }

        // test 3: successful acquisition after lock is released
        {
            // start a thread that holds the lock briefly
            std::thread lockHolder([this]() {
                this->holdLockFor(std::chrono::milliseconds(100));
            });

            // give the thread time to acquire the lock
            std::this_thread::sleep_for(std::chrono::milliseconds(50));

            // try to acquire with a longer timeout - should succeed after the first thread releases
            if (const bool acquired = tryIncrementWithTimeout(std::chrono::milliseconds(200)); !acquired) {
                LOG_ERROR("test 3 failed: lock not acquired after being released");
                m_tests_passed = false;
                assert(acquired && "test 3 failed: lock not acquired after being released");
            } else {
                LOG_INFO("test 3 - acquisition after release: passed");
            }

            // wait for the lock-holding thread to finish
            lockHolder.join();
        }

        // test 4: try_lock_until with absolute time point
        {
            const auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(100);
            const bool acquired = tryIncrementUntil(deadline);
            LOG_INFO(std::format("test 4 - try_lock_until: {}", acquired ? "passed" : "failed"));
            assert(acquired && "test 4 failed: try_lock_until with valid deadline");
        }

        // test 5: try_lock_until with expired time point
        {
            // to ensure the mutex is unlocked before this test
            // we need to make sure no other thread is holding the lock
            std::this_thread::sleep_for(std::chrono::milliseconds(50));

            // acquire the lock first (to ensure the test is consistent)
            m_mutex.lock();

            // create a thread that will try to acquire with an expired deadline
            std::thread expiredDeadlineThread([this]() {
                // create a time point in the past
                const auto pastDeadline = std::chrono::steady_clock::now() - std::chrono::milliseconds(100);
                if (const bool acquired = tryIncrementUntil(pastDeadline)) {
                    LOG_ERROR("test 5 failed: lock acquired with expired deadline");
                    m_tests_passed = false;
                    assert(false && "test 5 failed: lock acquired with expired deadline");
                }
                LOG_INFO("test 5 - expired time point: passed");
            });

            // give the thread time to run the test
            std::this_thread::sleep_for(std::chrono::milliseconds(200));

            // release the lock
            m_mutex.unlock();

            // wait for the test thread to complete
            expiredDeadlineThread.join();
        }

        // test 6: multiple threads with timeouts
        {
            constexpr int numThreads = 5;
            std::vector<std::thread> threads;
            std::atomic<int> successCount = 0;

            // start a thread that holds the lock for a while
            std::thread lockHolder([this]() {
                this->holdLockFor(std::chrono::milliseconds(200));
            });

            // give the lock holder time to acquire the lock
            std::this_thread::sleep_for(std::chrono::milliseconds(50));

            // create multiple threads trying to acquire the lock
            threads.reserve(numThreads);
            for (int ndx = 0; ndx < numThreads; ndx++) {
                threads.emplace_back([this, ndx, &successCount]() {
                    // each thread tries with progressively longer timeouts
                    const bool success = tryIncrementWithTimeout(std::chrono::milliseconds(100 + ndx * 50));
                    if (success) {
                        ++successCount;
                    }
                    LOG_INFO(std::format("thread {} acquisition: {}", ndx, success ? "succeeded" : "failed"));
                });
            }

            // wait for all threads to complete
            for (auto& t : threads) {
                t.join();
            }
            lockHolder.join();

            // at least some threads should have succeeded (those with longer timeouts)
            LOG_INFO(std::format("test 6 - multiple threads: {} out of {} succeeded", successCount.load(), numThreads));
            assert(successCount > 0 && "test 6 failed: no threads acquired the lock");
        }

        // test 7: performance under contention
        {
            constexpr int numThreads = 10;
            constexpr int attemptsPerThread = 5;
            std::vector<std::thread> threads;
            std::atomic<int> totalSuccessCount = 0;

            threads.reserve(numThreads);
            for (int ndx = 0; ndx < numThreads; ndx++) {
                threads.emplace_back([this, ndx, attemptsPerThread, &totalSuccessCount]() {
                    int threadSuccesses = 0;
                    for (int attempt = 0; attempt < attemptsPerThread; attempt++) {
                        // vary timeout based on thread id and attempt number
                        if (const auto timeout = std::chrono::milliseconds(50 + (ndx % 3) * 20); tryIncrementWithTimeout(timeout)) {
                            threadSuccesses++;
                        }
                        // small delay between attempts
                        std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    }
                    totalSuccessCount += threadSuccesses;
                    LOG_INFO(std::format("thread {} completed with {}/{} successes",
                                        ndx, threadSuccesses, attemptsPerThread));
                });
            }

            // wait for all threads to complete
            for (auto& t : threads) {
                t.join();
            }

            // check that we had a reasonable success rate
            int totalAttempts = numThreads * attemptsPerThread;
            LOG_INFO(std::format("test 7 - contention: {}/{} successful acquisitions ({}%)",
                               totalSuccessCount.load(), totalAttempts,
                               (totalSuccessCount.load() * 100) / totalAttempts));

            // we can't assert exact numbers due to timing variations, but we expect some successes
            assert(totalSuccessCount > 0 && "test 7 failed: no successful acquisitions under contention");
        }

        LOG_INFO(std::format("all tests completed. result: {}",
                           m_tests_passed.load() ? "all tests passed" : "some tests failed"));
    }

    // check if all tests passed
    bool allTestsPassed() const {
        return m_tests_passed.load();
    }
};

void basic_timed_mutex_usage() {
    // demonstrate basic timed_mutex usage
    LOG_INFO("demonstrating basic timed_mutex usage");

    // create a shared resource
    SharedResource resource;

    // create a thread that holds the lock for an extended period
    std::thread longHolder([&resource]() {
        resource.holdLockFor(std::chrono::milliseconds(500));
    });

    // give the thread time to acquire the lock
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    // try to increment with different timeouts
    const bool shortTimeout = resource.tryIncrementWithTimeout(std::chrono::milliseconds(100));
    LOG_INFO(std::format("short timeout attempt (100ms): {}", shortTimeout ? "succeeded" : "failed"));

    const bool longTimeout = resource.tryIncrementWithTimeout(std::chrono::milliseconds(1000));
    LOG_INFO(std::format("long timeout attempt (1000ms): {}", longTimeout ? "succeeded" : "failed"));

    // wait for the long-holding thread to complete
    longHolder.join();

    LOG_INFO("timed_mutex demonstration completed");
}

int run_tests() {
    // create a shared resource
    SharedResource resource;

    // run comprehensive tests
    resource.runTests();

    // verify test results
    if (!resource.allTestsPassed()) {
        LOG_ERROR("some timed_mutex tests failed!");
        return 1;
    }
    LOG_INFO("all timed_mutex tests passed successfully!");
    return 0;
}

int main() {
    LOG_INFO("starting timed_mutex demonstration");

    basic_timed_mutex_usage();
    const int result = run_tests();

    return result;
}
