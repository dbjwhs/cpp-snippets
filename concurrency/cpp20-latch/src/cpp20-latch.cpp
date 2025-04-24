// MIT License
// Copyright (c) 2025 dbjwhs

#include <iostream>
#include <thread>
#include <vector>
#include <latch>
#include <chrono>
#include <cassert>
#include <random>
#include <string>
#include <format>
#include "../../../headers/project_utils.hpp"

/*
 * std::latch - A Comprehensive Guide
 *
 * History:
 * The latch synchronization primitive was introduced in C++20 as part of the concurrency library.
 * It was inspired by similar constructs in other languages and threading libraries (like CountDownLatch
 * in Java). The concept dates back to early concurrent programming patterns and was formalized in the
 * C++ standard to provide a simple one-time use coordination mechanism.
 *
 * Purpose:
 * A latch is a downward counter that can be used to synchronize threads. The counter is initialized
 * with a count value. Threads can decrement the counter by calling count_down(). Once the counter
 * reaches zero, any threads waiting on the latch (via wait()) are released to continue execution.
 * Unlike barriers, latches are single-use only and cannot be reset once they reach zero.
 *
 * Common usage:
 * 1. Starting gate pattern: Holding back a group of threads until they're all ready to proceed
 * 2. Task completion synchronization: Main thread waits for a known number of tasks to complete
 * 3. Resource initialization: Ensuring resources are fully initialized before proceeding
 * 4. Thread coordination: Managing groups of threads that must wait for each other at specific points
 */

class LatchExample {
public:
    // constructor that takes the number of worker threads to create
    explicit LatchExample(const int numWorkers) :
        m_numWorkers{numWorkers},

        // initialize latch with the number of workers (the main thread will count down to release them)
        m_startLatch{static_cast<ptrdiff_t>(1)},

        // initialize completion latch with the number of workers
        m_completionLatch{static_cast<ptrdiff_t>(numWorkers)} {

        // reserve space for the worker threads to avoid reallocation
        m_workers.reserve(numWorkers);

        // reserve space for the results to avoid reallocation
        m_results.reserve(numWorkers);

        // initialize random number generator
        m_generator = std::mt19937(m_randomDevice());
        m_distribution = std::uniform_int_distribution<int>(100, 500);
    }

    // run the example
    void run() {
        LOG_INFO("starting latch example with {} workers", m_numWorkers);

        // create and start all worker threads
        for (int ndx = 0; ndx < m_numWorkers; ++ndx) {
            m_workers.emplace_back(&LatchExample::workerFunction, this, ndx);
        }

        // simulate some preparation work
        LOG_INFO("main thread preparing...");
        std::this_thread::sleep_for(std::chrono::milliseconds(200));

        // signal that the main thread is ready and releasing all workers
        LOG_INFO("main thread ready, releasing start latch");
        m_startLatch.count_down();

        // wait for completion of all worker threads
        LOG_INFO("main thread waiting for all workers to complete...");
        m_completionLatch.wait();

        // all workers have completed their tasks
        LOG_INFO("all workers completed their tasks");

        // perform validation
        validateResults();

        // join all worker threads
        for (auto& worker : m_workers) {
            if (worker.joinable()) {
                worker.join();
            }
        }

        LOG_INFO("latch example completed successfully");
    }

private:
    // number of worker threads
    int m_numWorkers{};

    // latch for synchronizing the start of worker threads
    std::latch m_startLatch;

    // latch for tracking completion of worker threads
    std::latch m_completionLatch;

    // collection of worker threads
    std::vector<std::thread> m_workers{};

    // collection to store results from worker threads
    std::vector<int> m_results{};

    // mutex to protect access to the results vector
    std::mutex m_resultsMutex{};

    // random number generation
    std::random_device m_randomDevice{};
    std::mt19937 m_generator;
    std::uniform_int_distribution<int> m_distribution;

    // function executed by each worker thread
    void workerFunction(int id) {
        LOG_INFO("worker {} initialized and waiting for start signal", id);

        // wait for the start signal from the main thread
        m_startLatch.wait();

        LOG_INFO("worker {} started processing", id);

        // simulate work by sleeping for a random duration
        const int workDuration = m_distribution(m_generator);
        std::this_thread::sleep_for(std::chrono::milliseconds(workDuration));

        // safely store the result
        {

            // calculate a result (just a simple calculation for demonstration)
            int result = id * 10 + workDuration % 10;

            std::lock_guard<std::mutex> lock(m_resultsMutex);
            m_results.emplace_back(result);
            LOG_INFO("worker {} completed with result {}", id, result);
        }

        // signal completion
        m_completionLatch.count_down();
        LOG_INFO("worker {} counted down completion latch", id);
    }

    // validate the results after all workers have completed
    void validateResults() const {
        // assert that we have the correct number of results
        assert(m_results.size() == m_numWorkers && "Incorrect number of results");
        LOG_INFO("validation: correct number of results ({})", m_results.size());

        // additional validation can be added here

        // print all results
        LOG_INFO("results from all workers:");
        for (int ndx = 0; ndx < m_results.size(); ++ndx) {
            LOG_INFO("worker {} result: {}", ndx, m_results[ndx]);
        }
    }
};

int main() {
    try {
        // create and run an example with 5 workers
        LatchExample example(5);
        example.run();

        // demonstrate another common use case: arriving at a synchronization point
        LOG_INFO("\ndemonstrating synchronization point example");

        constexpr int numThreads = 3;
        std::latch arrivalLatch(numThreads);
        std::vector<std::thread> threads;
        threads.reserve(numThreads);

        // start threads that will arrive at a synchronization point
        for (int ndx = 0; ndx < numThreads; ++ndx) {
            threads.emplace_back([ndx, &arrivalLatch]() {
                LOG_INFO("thread {} executing work", ndx);

                // simulate different work durations
                std::this_thread::sleep_for(std::chrono::milliseconds(100 * (ndx + 1)));

                LOG_INFO("thread {} arrived at synchronization point", ndx);

                // signal arrival and wait for all threads
                arrivalLatch.arrive_and_wait();

                LOG_INFO("thread {} continued after synchronization", ndx);
            });
        }

        // join all threads
        for (auto& t : threads) {
            if (t.joinable()) {
                t.join();
            }
        }

        LOG_INFO("all threads completed synchronization example");

        return 0;
    } catch (const std::exception& e) {
        LOG_ERROR("exception caught in main: {}", e.what());
        return 1;
    }
}
