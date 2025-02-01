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

#define DECLARE_NON_COPYABLE(ClassType) \
ClassType(const ClassType&) = delete; \
ClassType& operator=(const ClassType&) = delete

#define DECLARE_NON_MOVEABLE(ClassType) \
ClassType(ClassType&) = delete; \
ClassType& operator=(ClassType&) = delete

enum class LogLevel {
    INFO,
    NORMAL,
    CRITICAL
};

class Logger {
public:
    // default constructor uses executable path
    Logger() {
        m_log_file.open("app.log", std::ios::app);
        if (!m_log_file.is_open()) {
            throw std::runtime_error("Failed to open default log file");
        }
    }

    // constructor with custom path
    explicit Logger(const std::string& path) {
        if (!std::filesystem::exists(std::filesystem::path(path).parent_path())) {
            throw std::runtime_error("Invalid path provided: " + path);
        }

        m_log_file.open(path, std::ios::app);
        if (!m_log_file.is_open()) {
            throw std::runtime_error("Failed to open log file: " + path);
        }
    }

    ~Logger() {
        if (m_log_file.is_open()) {
            m_log_file.close();
        }
    }

    // variadic template for logging, way better than overriding log() methods to except
    // all potential arguments. See README.md for more details.
    template<typename... Args>
    void log(LogLevel level, const Args&... args) {
        std::stringstream message;
        message << get_utc_timestamp()
                << " [" << log_level_to_string(level)
                << "] [Thread:"
                << std::this_thread::get_id() << "] ";
        (message << ... << args);
        message << std::endl;

        std::lock_guard<std::mutex> lock(m_mutex);

        // write to file
        m_log_file << message.str();
        m_log_file.flush();

        // write to console
        if (level == LogLevel::CRITICAL) {
            std::cerr << message.str();
        } else {
            std::cout << message.str();
        }
    }

private:
    std::ofstream m_log_file;
    std::mutex m_mutex;

    // convert log level to string
    static std::string log_level_to_string(const LogLevel level) {
        switch (level) {
            case LogLevel::INFO:
                return "INFO";
            case LogLevel::NORMAL:
                return "NORMAL";
            case LogLevel::CRITICAL:
                return "CRITICAL";
            default:
                return "UNKNOWN";
        }
    }

    // get current utc timestamp
    static std::string get_utc_timestamp() {
        const auto now = std::chrono::system_clock::now();
        const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
        auto time = std::chrono::system_clock::to_time_t(now);

        std::stringstream ss;
        struct tm tm_buf;
#ifdef _WIN32
        gmtime_s(&tm_buf, &time);
#else
        gmtime_r(&time, &tm_buf);
#endif

        ss << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S");
        ss << '.' << std::setfill('0') << std::setw(3) << ms.count() << " UTC";
        return ss.str();
    }
};

// simple random generator for int's
class RandomGenerator {
private:
    std::mt19937 m_gen;
    std::uniform_int_distribution<int> m_dist;  // for integers
    // or
    // std::uniform_real_distribution<double> m_dist;  // for floating point

public:
    RandomGenerator(const int min, const int max)
        : m_gen(std::random_device{}())
        , m_dist(min, max) {}

    int getNumber() {
        return m_dist(m_gen);
    }

    // delete copy and move operations
    DECLARE_NON_COPYABLE(RandomGenerator);
    DECLARE_NON_MOVEABLE(RandomGenerator);
};

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
