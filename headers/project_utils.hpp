// MIT License
// Copyright (c) 2025 dbjwhs

#ifndef PROJECT_UTILS_HPP
#define PROJECT_UTILS_HPP

// standard library includes
#include <iostream>
#include <string>
#include <stdexcept>
#include <thread>
#include <mutex>
#include <fstream>
#include <chrono>
#include <random>
#include <cstddef>
#include <sstream>
#include <filesystem>
#include <iomanip>

// Version information
#define PROJECT_VERSION_MAJOR 1
#define PROJECT_VERSION_MINOR 0

// utility macros (use sparingly)
#define DECLARE_NON_COPYABLE(ClassType) \
ClassType(const ClassType&) = delete; \
ClassType& operator=(const ClassType&) = delete

#define DECLARE_NON_MOVEABLE(ClassType) \
ClassType(ClassType&) = delete; \
ClassType& operator=(ClassType&) = delete

// common constants
constexpr std::size_t DEFAULT_BUFFER_SIZE = 1024;
constexpr double EPSILON = 1e-6;

// transform thread id to std::string, note if no argument
// current thread id, else passed in thread id
template<typename ThreadType = std::thread::id>
inline std::string threadIdToString(ThreadType thread_id = std::this_thread::get_id()) {
    std::stringstream ss;
    ss << thread_id;
    return ss.str();
}

// simple random generator for int's
class RandomGenerator {
private:
    std::mt19937 m_gen;
    std::uniform_int_distribution<int> m_dist;  // for integers
    // or
    // std::uniform_real_distribution<double> m_dist; // for floating point

public:
    RandomGenerator(int min, int max)
        : m_gen(std::random_device{}())
        , m_dist(min, max) {}

    int getNumber() {
        return m_dist(m_gen);
    }

    // delete copy and move operations
    DECLARE_NON_COPYABLE(RandomGenerator);
    DECLARE_NON_MOVEABLE(RandomGenerator);
};

enum class LogLevel {
    INFO,
    NORMAL,
    ERROR,
    CRITICAL
};

class Logger {
    // singleton instance
    static Logger* m_instance;

	// do not allow default constructor
	Logger() = delete;

public:
    // constructor with a custom path
    explicit Logger(const std::string& path) {
        if (!std::filesystem::exists(std::filesystem::path(path).parent_path())) {
            throw std::runtime_error("Invalid path provided: " + path);
        }

        m_log_file.open(path, std::ios::app);
        if (!m_log_file.is_open()) {
            throw std::runtime_error("Failed to open log file: " + path);
        }
    }

    static Logger& getInstance() {
        if (m_instance == nullptr) {
            m_instance = new Logger("../custom.log");
        }
        return *m_instance;
    }

    ~Logger() {
        if (m_log_file.is_open()) {
            m_log_file.close();
        }
    }

    // variadic template for logging, way better than overriding log() methods to except
    // all potential arguments. see README.md for more details.
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
        if (level == LogLevel::CRITICAL || level == LogLevel::ERROR) {
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
            case LogLevel::ERROR:
                return "ERROR";
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

        struct tm tm_buf;
#ifdef _WIN32
        // ### not tested (did work few years ago)
        gmtime_s(&tm_buf, &time);
#else
        gmtime_r(&time, &tm_buf);
#endif

        std::stringstream ss;
        ss << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S");
        ss << '.' << std::setfill('0') << std::setw(3) << ms.count() << " UTC";
        return ss.str();
    }
};

// initialize static member
Logger* Logger::m_instance = nullptr;

#endif // PROJECT_UTILS_HPP
