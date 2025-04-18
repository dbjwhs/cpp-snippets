// MIT License
// Copyright (c) 2025 dbjwhs

#include "../include/thread_pool.hpp"
#include "../../../headers/project_utils.hpp"

#include <iostream>
#include <chrono>
#include <string>
#include <cassert>
#include <atomic>
#include <array>
#include <numeric>

// simulated work function that takes a certain amount of time
int simulate_work(int id, int sleep_ms) {
    // log the start of the task
    LOG_INFO(std::format("Task {} started", id));

    // simulate work by sleeping
    std::this_thread::sleep_for(std::chrono::milliseconds(sleep_ms));

    // log the completion of the task
    LOG_INFO(std::format("Task {} completed after {}ms", id, sleep_ms));

    // return the task id to verify results
    return id;
}

// test basic thread pool functionality
void test_basic_functionality() {
    // log the start of the test
    LOG_INFO("Starting basic functionality test");

    // create a thread pool with the default number of threads
    ThreadPool pool;

    // get the number of threads for logging
    const auto thread_count = pool.get_thread_count();
    LOG_INFO(std::format("Thread pool created with {} threads", thread_count));

    // verify thread count is as expected
    assert(thread_count > 0);

    // create a vector to hold futures
    std::vector<std::future<int>> results;

    // enqueue 10 tasks
    for (int ndx = 0; ndx < 10; ++ndx) {
        // enqueue task with medium priority
        auto future = pool.enqueue(
            ThreadPool::Priority::MEDIUM,
            simulate_work,
            ndx,
            100  // 100 ms of work
        );

        // store the future
        results.push_back(std::move(future));
    }

    // verify all tasks completed successfully
    for (int ndx = 0; ndx < 10; ++ndx) {
        // get the result from the future
        const int result = results[ndx].get();

        // log the result
        LOG_INFO(std::format("Result for task {}: {}", ndx, result));

        // verify the result is correct
        assert(result == ndx);
    }

    // verify queue is empty after tasks complete
    assert(pool.get_queue_size() == 0);

    // log the completion of the test
    LOG_INFO("Basic functionality test completed successfully!");
}

// test task prioritization
void test_prioritization() {
    // log the start of the test
    LOG_INFO("Starting prioritization test");

    // create a thread pool with only 1 thread to ensure sequential execution
    ThreadPool pool(1);

    // verify thread count
    assert(pool.get_thread_count() == 1);

    // vector to track task processing order
    std::vector<int> process_order;
    std::mutex order_mutex;

    // ensure tasks are all created before any start executing
    // by making the first task wait until all are enqueued
    std::atomic<bool> all_enqueued(false);

    // enqueue a low priority task first
    LOG_INFO("Enqueueing LOW priority task");
    auto low_future = pool.enqueue(
        ThreadPool::Priority::LOW,
        [&process_order, &order_mutex, &all_enqueued]() {
            // wait until all tasks are enqueued
            while (!all_enqueued.load()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }

            // record LOW priority task processed
            {
                std::lock_guard<std::mutex> lock(order_mutex);
                process_order.push_back(0); // 0 = LOW
            }
            LOG_INFO("LOW priority task executed");
        }
    );

    // ensure the first task won't complete immediately
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    // now enqueue a medium priority task
    LOG_INFO("Enqueueing MEDIUM priority task");
    auto medium_future = pool.enqueue(
        ThreadPool::Priority::MEDIUM,
        [&process_order, &order_mutex, &all_enqueued]() {
            // wait until all tasks are enqueued
            while (!all_enqueued.load()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }

            // record MEDIUM priority task processed
            {
                std::lock_guard<std::mutex> lock(order_mutex);
                process_order.push_back(1); // 1 = MEDIUM
            }
            LOG_INFO("MEDIUM priority task executed");
        }
    );

    // ensure the second task won't complete immediately
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    // finally enqueue a high priority task
    LOG_INFO("Enqueueing HIGH priority task");
    auto high_future = pool.enqueue(
        ThreadPool::Priority::HIGH,
        [&process_order, &order_mutex, &all_enqueued]() {
            // wait until all tasks are enqueued
            while (!all_enqueued.load()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }

            // record HIGH priority task processed
            {
                std::lock_guard<std::mutex> lock(order_mutex);
                process_order.push_back(2); // 2 = HIGH
            }
            LOG_INFO("HIGH priority task executed");
        }
    );

    // ensure the third task won't complete immediately
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    // signal that all tasks are enqueued and can proceed
    all_enqueued.store(true);

    // wait for all tasks to complete
    low_future.wait();
    medium_future.wait();
    high_future.wait();

    // log the processing order
    LOG_INFO("Task processing order (by priority level):");
    for (size_t ndx = 0; ndx < process_order.size(); ++ndx) {
        std::string priority_name;
        switch (process_order[ndx]) {
            case 0:
                priority_name = "LOW";
                break;
            case 1:
                priority_name = "MEDIUM";
                break;
            case 2:
                priority_name = "HIGH";
                break;
            default:
                priority_name = "UNKNOWN";
                break;
        }
        LOG_INFO(std::format("Position #{}: {} priority", ndx, priority_name));
    }

    // verify tasks were processed in priority order (highest first)
    assert(process_order.size() == 3);

    // we need to check that the pool is correctly prioritizing tasks
    // in a way that works with how the priority queue is implemented
    // the issue is that higher priorities might not execute until the first task is finished

    // there are two valid scenarios:
    // 1. HIGH, MEDIUM, LOW (perfect prioritization)
    // 2. LOW, HIGH, MEDIUM (if LOW already started executing)

    // check if we have perfect prioritization
    if (process_order[0] == 2) {
        // perfect prioritization case
        assert(process_order[0] == 2); // HIGH first
        assert(process_order[1] == 1); // MEDIUM second
        assert(process_order[2] == 0); // LOW last
        LOG_INFO("Perfect prioritization observed!");
    } else {
        // first task already executing a case
        assert(process_order[0] == 0); // LOW first (was already executing)
        assert(process_order[1] == 2); // HIGH second (highest priority of remaining tasks)
        assert(process_order[2] == 1); // MEDIUM last
        LOG_INFO("First-task-started prioritization observed!");
    }

    // log the completion of the test
    LOG_INFO("Prioritization test completed successfully!");
}

// test load balancing
void test_load_balancing() {
    // log the start of the test
    LOG_INFO("Starting load balancing test");

    // create a thread pool with hardware concurrency threads
    ThreadPool pool;

    // get thread count for logging
    const auto thread_count = pool.get_thread_count();
    LOG_INFO(std::format("Thread pool created with {} threads", thread_count));

    // atomic counter for busy threads
    std::atomic<size_t> max_busy_threads(0);

    // monitor busy threads in a separate thread
    std::atomic<bool> monitoring(true);
    std::thread monitor_thread([&pool, &max_busy_threads, &monitoring]() {
        while (monitoring.load()) {
            size_t busy = pool.get_busy_count();
            size_t expected = max_busy_threads.load();

            // update max_busy_threads if the current busy count is higher
            while (busy > expected) {
                if (max_busy_threads.compare_exchange_weak(expected, busy)) {
                    break;
                }
                expected = max_busy_threads.load();
            }

            // sleep briefly
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    });

    // futures for tasks
    std::vector<std::future<int>> futures;

    // enqueue many short tasks to encourage concurrency
    const int num_tasks = thread_count * 10;
    futures.reserve(num_tasks);
    for (int ndx = 0; ndx < num_tasks; ++ndx) {
        futures.push_back(pool.enqueue(
            ThreadPool::Priority::MEDIUM,
            simulate_work,
            ndx,
            50  // 50 ms of work
        ));
    }

    // wait for all tasks to complete
    for (auto& future : futures) {
        future.get();
    }

    // stop monitoring
    monitoring = false;
    monitor_thread.join();

    // log max concurrency achieved
    LOG_INFO(std::format("Maximum busy threads observed: {} out of {}", max_busy_threads.load(), thread_count));

    // verify reasonable thread utilization (at least half of threads were used)
    assert(max_busy_threads.load() >= thread_count / 2);

    // log the completion of the test
    LOG_INFO("Load balancing test completed successfully!");
}

// test pausing and resuming the thread pool
void test_pause_resume() {
    // log the start of the test
    LOG_INFO("Starting pause/resume test");

    // create a thread pool
    ThreadPool pool(4);

    // atomic flag to check if tasks run during pause
    std::atomic<bool> task_ran_during_pause(false);

    // pause the thread pool
    pool.pause();
    LOG_INFO("Thread pool paused");

    // enqueue a task that sets the flag
    auto future = pool.enqueue(
        ThreadPool::Priority::HIGH,
        [&task_ran_during_pause]() {
            task_ran_during_pause = true;
            return true;
        }
    );

    // sleep to give a task a chance to run (it shouldn't)
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // verify task didn't run during pause
    assert(!task_ran_during_pause.load());
    LOG_INFO("Verified task did not run while paused");

    // resume the thread pool
    pool.resume();
    LOG_INFO("Thread pool resumed");

    // wait for the task to complete
    const bool result = future.get();

    // verify task ran after resume
    assert(result == true);
    assert(task_ran_during_pause.load() == true);

    // log the completion of the test
    LOG_INFO("Pause/resume test completed successfully!");
}

// main function to run all tests
int main() {
    try {
        // log the start of the program
        LOG_INFO("Thread pool testing started");

        // run the tests
        test_basic_functionality();
        test_prioritization();
        test_load_balancing();
        test_pause_resume();

        // log successful completion
        LOG_INFO("All tests passed successfully!");

        return 0;
    } catch (const std::exception& e) {
        // log any exceptions
        LOG_ERROR(std::format("Exception: {}", e.what()));
        return 1;
    }
}
