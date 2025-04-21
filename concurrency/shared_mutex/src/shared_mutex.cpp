// MIT License
// Copyright (c) 2025 dbjwhs

#include <iostream>
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <vector>
#include <atomic>
#include <chrono>
#include <cassert>
#include <string>
#include "../../../headers/project_utils.hpp"

// shared_mutex pattern history:
// reader-writer locks have been around since the 1960s when they were first described by p.j. courtois,
// f. heymans, and d.l. parnas. the basic idea is to allow multiple readers to access shared data concurrently,
// while ensuring exclusive access for writers to prevent data corruption. this pattern is particularly useful
// in scenarios where read operations significantly outnumber write operations.
//
// in c++, this pattern was standardized in c++17 with std::shared_mutex and provides two-tiered access:
// 1. shared access: multiple threads can read the protected resource simultaneously
// 2. exclusive access: only one thread can write to the protected resource
//
// common use cases include:
// - caches that are read frequently but updated infrequently
// - configuration settings that rarely change
// - data structures with high read-to-write ratios
// - in-memory databases where reads are more common than writes
// - any scenario where you want to optimize for read performance without sacrificing thread safety

class ThreadSafeCounter {
private:
    // member variable for the counter-value
    std::atomic<int> m_counter{0};

    // shared mutex to protect the counter
    mutable std::shared_mutex m_mutex;

    // member variable to track the number of active readers
    mutable std::atomic<int> m_active_readers{0};

    // member variable to track the number of completed write operations
    mutable std::atomic<int> m_completed_writes{0};

    // member variable to track the number of completed read operations
    mutable std::atomic<int> m_completed_reads{0};

    // peak concurrent readers (for demonstration purposes)
    mutable std::atomic<int> m_peak_concurrent_readers{0};

public:
    // constructor to initialize counters
    ThreadSafeCounter() = default;

    // reset all counters (for testing purposes)
    void reset() {
        std::unique_lock<std::shared_mutex> lock(m_mutex);
        m_counter.store(0);
        m_active_readers.store(0);
        m_completed_writes.store(0);
        m_completed_reads.store(0);
        m_peak_concurrent_readers.store(0);
        LOG_INFO("counter reset to initial state");
    }

    // increment the counter (write operation)
    void increment() {
        // acquire exclusive lock for writing
        std::unique_lock<std::shared_mutex> lock(m_mutex);

        // simulate some work being done
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        // increment the counter
        ++m_counter;

        // increment the write operations counter
        ++m_completed_writes;

        LOG_INFO("write operation completed. counter value: " + std::to_string(m_counter));
    }

    // get the counter-value (read operation)
    int get() const {
        // acquire shared lock for reading
        std::shared_lock<std::shared_mutex> lock(m_mutex);

        // increment active readers counter
        const int current_readers = ++m_active_readers;

        // update peak readers if needed
        int peak = m_peak_concurrent_readers.load();
        while (current_readers > peak &&
               !m_peak_concurrent_readers.compare_exchange_weak(peak, current_readers)) {
            // If compare_exchange_weak fails, peak is updated with the current value,
            // so we retry the comparison with the updated peak
        }

        // simulate some work being done (longer to increase chances of concurrency)
        std::this_thread::sleep_for(std::chrono::milliseconds(5));

        // read the counter-value
        const int value = m_counter;

        // decrement active readers counter
        --m_active_readers;

        // increment completed reads counter
        ++m_completed_reads;

        LOG_INFO("read operation completed. active readers: " + std::to_string(current_readers) +
                 ", counter value: " + std::to_string(value));

        return value;
    }

    // get the number of active readers
    int get_active_readers() const {
        return m_active_readers;
    }

    // get the number of completed write operations
    int get_completed_writes() const {
        return m_completed_writes;
    }

    // get the number of completed read operations
    int get_completed_reads() const {
        return m_completed_reads;
    }

    // get the peak number of concurrent readers observed
    int get_peak_concurrent_readers() const {
        return m_peak_concurrent_readers;
    }
};

// test function to verify that multiple readers can access simultaneously
void test_concurrent_readers(ThreadSafeCounter& counter) {
    LOG_INFO("starting test for concurrent readers...");

    // reset counter-state for this test
    counter.reset();

    // vector to hold our reader threads
    std::vector<std::thread> readers;

    // vector to hold the results from each reader
    std::vector<int> results(20, 0);

    // set a non-zero initial value
    for (int ndx = 0; ndx < 5; ++ndx) {
        counter.increment();
    }

    LOG_INFO("counter initialized to 5, starting concurrent reader test");

    // atomic flag to synchronize the start of all readers
    std::atomic<bool> start_flag{false};

    // create more reader threads to increase concurrency chances
    readers.reserve(results.size());
    for (int ndx = 0; ndx < results.size(); ++ndx) {
        readers.emplace_back([&counter, &results, ndx, &start_flag]() {

            // wait until the start flag is set
            while (!start_flag.load()) {
                std::this_thread::yield();
            }

            // small randomized delay to spread out the read operations
            std::this_thread::sleep_for(std::chrono::milliseconds(ndx % 3));

            // perform read operation
            results[ndx] = counter.get();
        });
    }

    // set the start flag to begin all readers simultaneously
    start_flag.store(true);

    // wait for all readers to complete
    for (auto& thread : readers) {
        thread.join();
    }

    // ensure all threads have completed their operations
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    // verify that we had multiple readers active simultaneously
    const int reads = counter.get_completed_reads();
    const int peak_readers = counter.get_peak_concurrent_readers();

    LOG_INFO("completed reads: " + std::to_string(reads));
    LOG_INFO("peak concurrent readers: " + std::to_string(peak_readers));

    // verify we achieved actual concurrent reading
    assert(peak_readers > 1);
    LOG_INFO("achieved " + std::to_string(peak_readers) + " concurrent readers - shared lock is working!");

    // verify all readers got the same value
    for (int ndx = 1; ndx < results.size(); ++ndx) {
        assert(results[ndx] == results[0]);
    }

    LOG_INFO("all " + std::to_string(results.size()) + " readers got the same value: " + std::to_string(results[0]));
    LOG_INFO("concurrent readers test passed!");
}

// test function to verify that writers get exclusive access
void test_exclusive_writers(ThreadSafeCounter& counter) {
    LOG_INFO("starting test for exclusive writers...");

    // reset counter-state for this test
    counter.reset();

    // vector to hold our writer threads
    std::vector<std::thread> writers;

    // the number of increments each writer will perform
    constexpr int increments_per_writer = 100;

    // number of writer threads
    constexpr int num_writers = 5;

    // create multiple writer threads
    writers.reserve(num_writers);
    for (int ndx = 0; ndx < num_writers; ++ndx) {
        writers.emplace_back([&counter, increments_per_writer]() {
            // each writer increments the counter multiple times
            for (int i = 0; i < increments_per_writer; ++i) {
                counter.increment();
            }
        });
    }

    // wait for all writers to complete
    for (auto& thread : writers) {
        thread.join();
    }

    // verify the final counter-value
    const int final_value = counter.get();
    constexpr int expected_value = num_writers * increments_per_writer;

    LOG_INFO("final counter value: " + std::to_string(final_value) + ", expected: " + std::to_string(expected_value));
    assert(final_value == expected_value);

    // verify the number of completed write operations
    assert(counter.get_completed_writes() == expected_value);

    LOG_INFO("exclusive writers test passed!");
}

// test function to verify readers and writers interaction
void test_readers_and_writers(ThreadSafeCounter& counter) {
    LOG_INFO("starting test for readers and writers interaction...");

    // reset counter-state for this test
    counter.reset();

    // vector to hold all threads
    std::vector<std::thread> threads;

    // number of reader and writer threads
    constexpr int num_readers = 20;
    constexpr int num_writers = 5;

    // number of operations per thread
    constexpr int ops_per_thread = 50;

    // start reader threads
    threads.reserve(num_readers);
    for (int ndx = 0; ndx < num_readers; ++ndx) {
        threads.emplace_back([&counter, ops_per_thread]() {
            for (int i = 0; i < ops_per_thread; ++i) {
                counter.get();
                // small sleep to increase the chance of thread interleaving
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        });
    }

    // start writer threads
    for (int ndx = 0; ndx < num_writers; ++ndx) {
        threads.emplace_back([&counter, ops_per_thread]() {
            for (int i = 0; i < ops_per_thread; ++i) {
                counter.increment();
                // small sleep to increase the chance of thread interleaving
                std::this_thread::sleep_for(std::chrono::milliseconds(3));
            }
        });
    }

    // wait for all threads to complete
    for (auto& thread : threads) {
        thread.join();
    }

    // verify the final counter-value
    const int final_value = counter.get();
    constexpr int expected_value = num_writers * ops_per_thread;

    LOG_INFO("final counter value: " + std::to_string(final_value) + ", expected: " + std::to_string(expected_value));
    assert(final_value == expected_value);

    // verify the number of completed operations
    assert(counter.get_completed_writes() == expected_value);
    assert(counter.get_completed_reads() == num_readers * ops_per_thread + 1); // +1 for the final get()

    LOG_INFO("readers and writers interaction test passed!");
}

// performance test to demonstrate the benefits of shared_mutex
void performance_test() {
    LOG_INFO("starting performance test...");

    // create a counter protected by shared_mutex
    ThreadSafeCounter shared_counter;

    // create a counter protected by a regular mutex for comparison
    class MutexCounter {
    private:
        // member variable for the counter-value
        int m_value{0};

        // regular mutex to protect the counter
        mutable std::mutex m_mutex;

    public:
        // reset counters
        void reset() {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_value = 0;
        }

        // increment operation
        void increment() {
            std::lock_guard<std::mutex> lock(m_mutex);
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            ++m_value;
        }

        // get operation
        int get() const {
            std::lock_guard<std::mutex> lock(m_mutex);
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            return m_value;
        }
    } mutex_counter;

    // function to test performance with a given read/write ratio
    auto test_with_ratio = [](ThreadSafeCounter& shrd_counter, MutexCounter& mtx_counter,
                             const int num_readers, const int num_writers, int ops_per_thread) {
        // reset counters before the test
        shrd_counter.reset();
        mtx_counter.reset();

        // vector to hold all threads
        std::vector<std::thread> shared_threads;
        std::vector<std::thread> mutex_threads;

        LOG_INFO("testing with " + std::to_string(num_readers) + " readers and " +
                 std::to_string(num_writers) + " writers (" +
                 std::to_string(ops_per_thread) + " ops per thread)");

        // timing variables
        const auto shared_start = std::chrono::high_resolution_clock::now();

        // start reader threads for a shared_mutex version
        shared_threads.reserve(num_readers);
        for (int ndx = 0; ndx < num_readers; ++ndx) {
            shared_threads.emplace_back([&shrd_counter, ops_per_thread]() {
                for (int i = 0; i < ops_per_thread; ++i) {
                    shrd_counter.get();
                }
            });
        }

        // start writer threads for a shared_mutex version
        for (int ndx = 0; ndx < num_writers; ++ndx) {
            shared_threads.emplace_back([&shrd_counter, ops_per_thread]() {
                for (int i = 0; i < ops_per_thread; ++i) {
                    shrd_counter.increment();
                }
            });
        }

        // wait for all shared_mutex threads to complete
        for (auto& thread : shared_threads) {
            thread.join();
        }

        const auto shared_end = std::chrono::high_resolution_clock::now();

        // now test with regular mutex
        const auto mutex_start = std::chrono::high_resolution_clock::now();

        // start reader threads for a regular mutex version
        mutex_threads.reserve(num_readers);
        for (int ndx = 0; ndx < num_readers; ++ndx) {
            mutex_threads.emplace_back([&mtx_counter, ops_per_thread]() {
                for (int i = 0; i < ops_per_thread; ++i) {
                    mtx_counter.get();
                }
            });
        }

        // start writer threads for a regular mutex version
        for (int ndx = 0; ndx < num_writers; ++ndx) {
            mutex_threads.emplace_back([&mtx_counter, ops_per_thread]() {
                for (int i = 0; i < ops_per_thread; ++i) {
                    mtx_counter.increment();
                }
            });
        }

        // wait for all regular mutex threads to complete
        for (auto& thread : mutex_threads) {
            thread.join();
        }

        const auto mutex_end = std::chrono::high_resolution_clock::now();

        // calculate durations
        const auto shared_duration = std::chrono::duration_cast<std::chrono::milliseconds>(shared_end - shared_start).count();
        const auto mutex_duration = std::chrono::duration_cast<std::chrono::milliseconds>(mutex_end - mutex_start).count();

        // calculate speedup
        double speedup = static_cast<double>(mutex_duration) / static_cast<double>(shared_duration);

        LOG_INFO("read/write ratio " + std::to_string(num_readers) + ":" + std::to_string(num_writers));
        LOG_INFO("shared_mutex time: " + std::to_string(shared_duration) + " ms");
        LOG_INFO("regular mutex time: " + std::to_string(mutex_duration) + " ms");
        LOG_INFO("speedup: " + std::to_string(speedup) + "x");

        // Return peak concurrent readers as well to demonstrate the concurrency
        int peak_readers = shrd_counter.get_peak_concurrent_readers();
        LOG_INFO("peak concurrent readers with shared_mutex: " + std::to_string(peak_readers));

        return std::make_pair(speedup, peak_readers);
    };

    // test with different read/write ratios
    constexpr int ops_per_thread = 100;

    // test with 1:1 ratio (equal reads and writes)
    auto [speedup_1_1, peak_1_1] = test_with_ratio(shared_counter, mutex_counter, 10, 10, ops_per_thread);

    // test with 10:1 ratio (10x more reads than writes)
    auto [speedup_10_1, peak_10_1] = test_with_ratio(shared_counter, mutex_counter, 50, 5, ops_per_thread);

    // test with 100:1 ratio (100x more reads than writes)
    auto [speedup_100_1, peak_100_1] = test_with_ratio(shared_counter, mutex_counter, 100, 1, ops_per_thread);

    // output performance summary
    LOG_INFO("performance summary:");
    LOG_INFO("1:1 ratio - speedup: " + std::to_string(speedup_1_1) + "x, peak readers: " + std::to_string(peak_1_1));
    LOG_INFO("10:1 ratio - speedup: " + std::to_string(speedup_10_1) + "x, peak readers: " + std::to_string(peak_10_1));
    LOG_INFO("100:1 ratio - speedup: " + std::to_string(speedup_100_1) + "x, peak readers: " + std::to_string(peak_100_1));

    // verify that shared_mutex performs better as read operations increase
    LOG_INFO("verifying performance scaling with read/write ratio");
    assert(speedup_10_1 >= speedup_1_1);
    assert(speedup_100_1 >= speedup_10_1);

    LOG_INFO("performance test passed!");
}

int main() {
    LOG_INFO("starting std::shared_mutex demonstration");

    // create a counter protected by std::shared_mutex
    ThreadSafeCounter counter;

    // run tests
    test_concurrent_readers(counter);

    // reset counter state between tests
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    test_exclusive_writers(counter);

    // reset counter state between tests
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    test_readers_and_writers(counter);

    // reset counter state between tests
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    performance_test();

    LOG_INFO("all tests passed! std::shared_mutex demonstration completed");

    return 0;
}
