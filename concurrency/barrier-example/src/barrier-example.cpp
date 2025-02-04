// MIT License
// Copyright (c) 2025 dbjwhs

#include <iostream>
#include <thread>
#include <vector>
#include <barrier>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <fstream>
#include <random>
#include "../../../headers/project_utils.hpp"

// manual implementation of a barrier
class CustomBarrier {
private:
    const size_t m_threadCount;
    size_t m_counter;
    size_t m_waiting;
    std::mutex m_mutex;
    std::condition_variable m_cv;
    bool m_phase;

public:
    explicit CustomBarrier(const size_t count)
        : m_threadCount(count), m_counter(count), m_waiting(0), m_phase(false) {}

    void wait() {
        std::unique_lock<std::mutex> lock(m_mutex);
        bool phase_copy = m_phase;

        if (--m_counter == 0) {
            // last thread to arrive
            m_counter = m_threadCount;
            m_waiting = m_threadCount - 1;
            m_phase = !m_phase;
            m_cv.notify_all();
        } else {
            // not the last thread, need to wait
            ++m_waiting;

            // wait on the condition variable m_cv. the unique_lock 'lock' is automatically
            // unlocked during waiting and relocked when waking up. the lambda [this, phase_copy]
            // captures the current object pointer and phase_copy by value. the lambda returns true
            // only when phase_copy != m_phase, meaning the barrier has moved to a new phase. this
            // ensures threads only proceed after a genuine phase change, protecting against spurious
            // wakeups. when the last thread arrives and changes m_phase, all waiting threads will
            // have their conditions satisfied and can proceed
            m_cv.wait(lock, [this, phase_copy] { return phase_copy != m_phase; });

            --m_waiting;
        }
    }

    // delete copy and move operations
    DECLARE_NON_COPYABLE(CustomBarrier);
    DECLARE_NON_MOVEABLE(CustomBarrier);
};

class CustomBarrierExample {
private:
    static void worker(CustomBarrier& barrier, int id, Logger *logger) {
        for (int phase = 1; phase <= 3; ++phase) {
            // simulate some work
            RandomGenerator random(100, 1000);
            std::this_thread::sleep_for(std::chrono::milliseconds(random.getNumber()));

            logger->log(LogLevel::INFO, "CustomBarrierExample Thread ", id, " completed phase ", phase);

            // wait for all threads at the barrier
            barrier.wait();
            logger->log(LogLevel::INFO, "CustomBarrierExample Thread ", id, " starting phase ", phase + 1);
        }
    }

public:
    static void demonstrate(const int num_threads, Logger *logger) {
        CustomBarrier barrier(num_threads);
        std::vector<std::thread> threads;

        logger->log(LogLevel::INFO, "\nDemonstrating custom barrier implementation:");

        // create threads
        threads.reserve(num_threads);
        for (int ndx = 0; ndx < num_threads; ++ndx) {
            threads.emplace_back(worker, std::ref(barrier), ndx, logger);
        }

        // join threads
        for (auto& thread : threads) {
            thread.join();
        }
    }

    // delete copy and move operations
    DECLARE_NON_COPYABLE(CustomBarrierExample);
    DECLARE_NON_MOVEABLE(CustomBarrierExample);
};

// modern implementation using std::barrier (c++20)
class ModernBarrierExample {
private:
    static void worker(std::barrier<>& barrier, const int id, Logger *logger) {
        for (int phase = 1; phase <= 3; ++phase) {
            // simulate some work
            RandomGenerator random(100, 1000);
            std::this_thread::sleep_for(std::chrono::milliseconds(random.getNumber()));

            logger->log(LogLevel::INFO, "Thread ", id, " completed phase ", phase);

            // wait for all threads at the barrier
            barrier.arrive_and_wait();

            logger->log(LogLevel::INFO, "Thread ", id, " starting phase ", (phase + 1));
        }
    }

public:
    static void demonstrate(const int num_threads, Logger *logger) {
        logger->log(LogLevel::INFO, "Demonstrating std::barrier implementation:");
        std::barrier barrier(num_threads);
        std::vector<std::thread> threads;

        // create threads
        threads.reserve(num_threads);
        for (int ndx = 0; ndx < num_threads; ++ndx) {
            threads.emplace_back(worker, std::ref(barrier), ndx, logger);
        }

        // join threads
        for (auto& thread : threads) {
            thread.join();
        }
    }

    // delete copy and move operations
    DECLARE_NON_COPYABLE(ModernBarrierExample);
    DECLARE_NON_MOVEABLE(ModernBarrierExample);

};

int main() {
    constexpr int NUM_THREADS = 4;

    // thread safe logger
    Logger logger("../custom.log");

    // demonstrate both implementations
    CustomBarrierExample::demonstrate(NUM_THREADS, &logger);
    logger.log(LogLevel::INFO, "\n-----------------------------------");
    ModernBarrierExample::demonstrate(NUM_THREADS, &logger);

    return 0;
}
