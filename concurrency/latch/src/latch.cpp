// MIT License
// Copyright (c) 2025 dbjwhs

#include <iostream>
#include <thread>
#include <vector>
#include <latch>
#include <functional>
#include <chrono>
#include <cassert>
#include <format>
#include "../../../headers/project_utils.hpp"

// std::latch - Historical and Technical Background
//
// The std::latch class was introduced in C++20 as part of the concurrency library enhancements.
// It's a downward counter that can be used to synchronize threads. The counter is initialized to a specified count,
// and threads can wait on the latch until the counter reaches zero. Once the counter reaches zero, it cannot be reset.
//
// A latch is a synchronization primitive that acts as a single-use barrier. Threads can wait on the latch until
// it is released by other threads decreasing the counter to zero. Unlike a barrier, a latch cannot be reset after
// it has been released.
//
// Common use cases include:
// 1. Thread coordination - Wait for a group of threads to complete a task before proceeding
// 2. Starting multiple threads simultaneously
// 3. Waiting for a specific number of events to occur before proceeding
// 4. Implementing the "fork-join" pattern where multiple worker threads perform tasks in parallel and then synchronize

// helper function to simulate work with a random duration
void simulate_work(const std::string& thread_name, int min_ms, int max_ms) {
    // generate a random duration between min_ms and max_ms
    RandomGenerator random(100, 500);
    auto duration = min_ms + random.getNumber() % (max_ms - min_ms);

    // log the start of work
    LOG_INFO(std::format("{} starting work for {} ms", thread_name, duration));

    // simulate work by sleeping
    std::this_thread::sleep_for(std::chrono::milliseconds(duration));

    // log the end of work
    LOG_INFO(std::format("{} finished work", thread_name));
}

// class demonstrating the use of std::latch for thread synchronization
class LatchExample {
public:
    // constructor initializes member variables
    explicit LatchExample(int thread_count)
        : m_thread_count{thread_count},
          m_start_latch{thread_count + 1},  // +1 for the main thread
          m_completion_latch{thread_count} {

        LOG_INFO(std::format("Created LatchExample with {} threads", thread_count));
    }

    // method to run the example
    void run() {
        LOG_INFO("Starting LatchExample::run()");

        // vector to store thread objects
        std::vector<std::thread> threads;

        // reserve space for the threads to avoid reallocation
        threads.reserve(m_thread_count);

        // create and start worker threads
        for (int ndx = 0; ndx < m_thread_count; ++ndx) {
            // create a thread and add it to the vector
            threads.emplace_back(&LatchExample::worker_function, this, ndx);
        }

        LOG_INFO("All worker threads created, waiting for 1 second before starting them");
        std::this_thread::sleep_for(std::chrono::seconds(1));

        // start time measurement
        auto start_time = std::chrono::high_resolution_clock::now();

        // release the start latch to allow all workers to begin simultaneously
        LOG_INFO("Releasing start latch to begin work");
        m_start_latch.count_down();
        m_start_latch.wait();  // wait with the worker threads

        // wait for all workers to complete
        LOG_INFO("Waiting for all workers to complete");
        m_completion_latch.wait();

        // end time measurement
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

        LOG_INFO(std::format("All workers completed in {} ms", duration));

        // join all threads
        for (auto& thread : threads) {
            thread.join();
        }

        LOG_INFO("All threads joined, LatchExample::run() completed");
    }

    // test the latch functionality with assertions
    void test() {
        LOG_INFO("Starting LatchExample::test()");

        // test case 1: verify threads are synchronized by the start latch
        {
            bool all_started_together = true;
            std::atomic<int> threads_ready{0};
            std::atomic<bool> main_thread_ready{false};
            std::atomic<int> simultaneous_starts{0};

            // the main thread will release the latch, worker threads will wait on it
            std::latch start_signal{1};
            std::latch completion_test{3};

            std::vector<std::thread> test_threads;
            test_threads.reserve(3);

            for (int ndx = 0; ndx < 3; ++ndx) {
                test_threads.emplace_back([&]() {
                    // indicate this thread is ready
                    threads_ready.fetch_add(1);

                    // wait for the start signal from the main thread
                    start_signal.wait();

                    // check if the main thread is ready when we start
                    if (main_thread_ready.load()) {
                        simultaneous_starts.fetch_add(1);
                    } else {
                        all_started_together = false;
                    }

                    // signal completion
                    completion_test.count_down();
                });
            }

            // wait for all threads to be ready
            while (threads_ready.load() < 3) {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }

            // set the main thread as ready and release the latch to start all worker threads
            LOG_INFO("All worker threads ready, releasing start signal");
            main_thread_ready.store(true);
            start_signal.count_down();  // This releases all waiting threads

            // wait for completion
            completion_test.wait();

            // join all test threads
            for (auto& thread : test_threads) {
                thread.join();
            }

            // verify all threads started simultaneously
            LOG_INFO(std::format("Test 1: {} out of 3 threads started simultaneously", simultaneous_starts.load()));
            assert(all_started_together && "All threads should start together");
            assert(simultaneous_starts.load() == 3 && "All 3 threads should have started simultaneously");
        }

        // test case 2: verify completion latch works correctly
        {
            int completion_count = 0;
            std::latch completion_test{3};

            std::vector<std::thread> test_threads;
            test_threads.reserve(3);

            for (int ndx = 0; ndx < 3; ++ndx) {
                test_threads.emplace_back([&, ndx]() {
                    // simulate different work durations
                    std::this_thread::sleep_for(std::chrono::milliseconds(100 * (ndx + 1)));

                    // count down the latch
                    completion_test.count_down();
                });
            }

            // wait for completion latch
            completion_test.wait();

            // all threads should have completed
            completion_count = 3;

            // join all test threads
            for (auto& thread : test_threads) {
                thread.join();
            }

            LOG_INFO(std::format("Test 2: Completion count is {}", completion_count));
            assert(completion_count == 3 && "All threads should have completed");
        }

        LOG_INFO("All tests passed successfully");
    }

private:
    // worker function that will be executed by each thread
    void worker_function(int thread_id) {
        // create a thread name for logging
        std::string thread_name = std::format("Worker-{}", thread_id);

        LOG_INFO(std::format("{} waiting for start signal (m_start_latch.arrive_and_wait())", thread_name));

        // count down and wait on the start latch
        m_start_latch.arrive_and_wait();

        LOG_INFO(std::format("{} started work", thread_name));

        // simulate varying workload
        simulate_work(thread_name, 500, 2000);

        // count down the completion latch
        LOG_INFO(std::format("{} completing task (m_completion_latch.count_down())", thread_name));
        m_completion_latch.count_down();
    }

    // member variables
    int m_thread_count{};
    std::latch m_start_latch;
    std::latch m_completion_latch;
};

int main() {
    // seed the random number generator
    //srand(static_cast<unsigned int>(time(nullptr)));

    LOG_INFO("Program started");

    try {
        // create an instance of the latch example
        LatchExample example(5);

        // run the tests
        example.test();

        // run the example
        example.run();

        LOG_INFO("Example completed successfully");
    } catch (const std::exception& e) {
        // log any exceptions
        LOG_ERROR(std::format("Exception caught: {}", e.what()));
        return 1;
    }

    LOG_INFO("Program finished");
    return 0;
}
