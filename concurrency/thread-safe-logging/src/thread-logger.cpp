// MIT License
// Copyright (c) 2025 dbjwhs

#include <iostream>
#include <fstream>
#include <mutex>
#include <string>
#include <ctime>
#include <filesystem>
#include <thread>
#include <vector>
#include <stdexcept>
#include <sstream>
#include <chrono>
#include <random>

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

    // variadic template for logging, way better than overiding log() methods to except
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
    static std::string log_level_to_string(LogLevel level) {
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
        auto now = std::chrono::system_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
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

// test function to simulate work and logging
void test_logging(Logger& logger, int job_id) {
    // setup random number generation
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> delay_dist(1, 100);

    for (int ndx = 0; ndx < 10; ++ndx) {
        // simulate random work using modern random generation
        std::this_thread::sleep_for(std::chrono::milliseconds(delay_dist(gen)));

        // log different levels
        switch (ndx % 3) {
            case 0:
                logger.log(LogLevel::INFO, "Job ", job_id, " - Info message ", ndx);
                break;
            case 1:
                logger.log(LogLevel::NORMAL, "Job ", job_id, " - Normal message ", ndx);
                break;
            case 2:
                logger.log(LogLevel::CRITICAL, "Job ", job_id, " - Critical message ", ndx);
                break;
            default:
                throw std::runtime_error("Invalid data test_logging" + std::to_string(ndx % 3));
        }
    }
}

int main() {
    try {
        // test default constructor
        Logger default_logger;
        std::cout << "Default logger created successfully\n";

        // test custom path constructor
        Logger custom_logger("./custom.log");
        std::cout << "Custom path logger created successfully\n";

        // test invalid path
        try {
            Logger invalid_logger("/invalid/path/log.txt");
        } catch (const std::runtime_error& e) {
            std::cout << "Successfully caught invalid path: " << e.what() << std::endl;
        }

        // test multi-threading
        constexpr int num_threads = 20;
        std::vector<std::thread> threads;

        std::cout << "Starting multi-threaded test with " << num_threads << " threads\n";

        // create and start threads
        threads.reserve(num_threads);
        for (int ndx = 0; ndx < num_threads; ++ndx) {
            threads.emplace_back(test_logging, std::ref(custom_logger), ndx);
        }

        // wait for all threads to complete
        for (auto& thread : threads) {
            thread.join();
        }

        std::cout << "All threads completed successfully\n";

        // test different log levels
        custom_logger.log(LogLevel::INFO, "Test info message with multiple ", "arguments ", 42);
        custom_logger.log(LogLevel::NORMAL, "Test normal message");
        custom_logger.log(LogLevel::CRITICAL, "Test critical message");

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
