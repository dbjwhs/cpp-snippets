// MIT License
// Copyright (c) 2025 dbjwhs

#include <iostream>
#include <thread>
#include <mutex>
#include <array>
#include <chrono>
#include <fstream>
#include <vector>
#include <memory>
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

// class representing a philosopher in the dining problem
class Philosopher {
private:
    int m_id;                    // unique identifier for each philosopher
    std::mutex *m_leftFork;      // left fork mutex pointer
    std::mutex *m_rightFork;     // right fork mutex pointer
    int m_mealsEaten;            // counter for meals eaten
    static constexpr int m_maxMeals = 3;  // maximum meals each philosopher will eat
    Logger *m_logger;

public:
    // constructor initializing philosopher with their id and adjacent forks
    Philosopher(const int id, std::mutex* leftFork, std::mutex* rightFork, Logger* logger)
        : m_id(id), m_leftFork(leftFork), m_rightFork(rightFork), m_mealsEaten(0), m_logger(logger) {}

    // main dining action for each philosopher
    void dine() {
        while (m_mealsEaten < m_maxMeals) {
            think();
            eat();
        }
    }

private:
    // simulate thinking process
    void think() const {
        RandomGenerator random(1, 100);
        m_logger->log(LogLevel::INFO, "Philosopher ", m_id, " is thinking...");
        // random thinking time between 1-3 seconds
        std::this_thread::sleep_for(std::chrono::seconds(random.getNumber() % 3 + 1));
    }

    // simulate eating process with resource management
    void eat() {
        RandomGenerator random(100, 1000);

        // try to pick up forks using resource hierarchy to prevent deadlock
        if (m_id % 2 == 0) {
            // even numbered philosophers pick up left fork first
            std::lock_guard<std::mutex> leftLock(*m_leftFork);
            m_logger->log(LogLevel::INFO, "Philosopher ", m_id, " picked up left fork");

            // delay to demonstrate deadlock prevention
            std::this_thread::sleep_for(std::chrono::milliseconds(random.getNumber()));

            std::lock_guard<std::mutex> rightLock(*m_rightFork);
            m_logger->log(LogLevel::INFO, "Philosopher ", m_id, " picked up right fork");
        } else {
            // odd numbered philosophers pick up right fork first
            std::lock_guard<std::mutex> rightLock(*m_rightFork);
            m_logger->log(LogLevel::INFO, "Philosopher ", m_id, " picked up right fork");

            // delay to demonstrate deadlock prevention
            std::this_thread::sleep_for(std::chrono::milliseconds(random.getNumber()));

            std::lock_guard<std::mutex> leftLock(*m_leftFork);
            m_logger->log(LogLevel::INFO, "Philosopher ", m_id, " picked up left fork");
        }

        // eating process
        m_logger->log(LogLevel::INFO, "Philosopher ", m_id, " is eating meal ", m_mealsEaten + 1);
        std::this_thread::sleep_for(std::chrono::seconds(random.getNumber() % 3 + 1));
        m_mealsEaten++;

        // forks are automatically released when lock_guards go out of scope
        m_logger->log(LogLevel::INFO, "Philosopher ", m_id, " finished eating and put down forks");
    }

    // delete copy and move operations
    DECLARE_NON_COPYABLE(Philosopher);
    DECLARE_NON_MOVEABLE(Philosopher);
};

int main() {
    constexpr int numPhilosophers = 5;

    // create forks (represented by mutexes)
    std::vector<std::mutex> forks(numPhilosophers);

    // create logger
    Logger custom_logger("../custom.log");

    // create philosophers first
    std::vector<std::unique_ptr<Philosopher>> philosophers;
    for (int ndx = 0; ndx < numPhilosophers; ++ndx) {
        philosophers.push_back(std::make_unique<Philosopher>(
            ndx,
            &forks[ndx],
            &forks[(ndx + 1) % numPhilosophers],
            &custom_logger
        ));
    }

    // create and store philosopher threads
    std::vector<std::thread> threads;
    for (int ndx = 0; ndx < numPhilosophers; ++ndx) {
        threads.emplace_back(&Philosopher::dine, philosophers[ndx].get());
    }

    // wait for all philosophers to finish dining
    for (auto& thread : threads) {
        thread.join();
    }

    custom_logger.log(LogLevel::INFO, "All philosophers have finished dining!\n");
    return 0;
}
