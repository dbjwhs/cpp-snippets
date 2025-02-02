// MIT License
// Copyright (c) 2025 dbjwhs

#include <iostream>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <vector>
#include <chrono>
#include <fstream>

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

    DECLARE_NON_COPYABLE(Logger);
    DECLARE_NON_MOVEABLE(Logger);
};

// thread-safe queue implementation that handles concurrent access to data
template<typename Queue>
class SafeQueue {
private:
    // internal queue to store items
    std::queue<Queue> m_queue;
    // mutex for thread synchronization
    mutable std::mutex m_mutex;
    // condition variable for thread signaling
    std::condition_variable m_cond;
    // flag to indicate queue shutdown
    std::atomic<bool> m_done{false};

public:
    // adds an item to the queue in a thread-safe manner
    void push(Queue item) {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_queue.push(std::move(item));
        lock.unlock();
        m_cond.notify_one();
    }

    // removes and returns an item from the queue
    // returns false if queue is empty and done
    bool pop(Queue& item) {
        std::unique_lock<std::mutex> lock(m_mutex);
        while (m_queue.empty() && !m_done) {
            m_cond.wait(lock);
        }
        if (m_queue.empty()) {
            return false;
        }
        item = std::move(m_queue.front());
        m_queue.pop();
        return true;
    }

    // signals that no more items will be added to the queue
    void setDone() {
        m_done = true;
        m_cond.notify_all();
    }

    // checks if the queue is empty
    bool isEmpty() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_queue.empty();
    }
};

// base class for pipeline stages that defines common functionality
class PipelineStage {
protected:
    // queue for receiving input data
    SafeQueue<int>& m_input_queue;
    // queue for sending output data
    SafeQueue<int>& m_output_queue;
    // name identifier for the stage
    std::string m_stage_name;
    // customer logger
    Logger *m_logger;

public:
    PipelineStage(SafeQueue<int>& in_queue, SafeQueue<int>& out_queue, std::string name, Logger *logger)
        : m_input_queue(in_queue), m_output_queue(out_queue), m_stage_name(std::move(name)), m_logger(logger) {}

    // pure virtual function for stage-specific processing
    virtual void process(int item) = 0;

    // main processing loop that handles input and output
    void run() {
        int item;
        while (m_input_queue.pop(item)) {
            m_logger->log(LogLevel::INFO,  m_stage_name + " processing item: " + std::to_string(item));
            process(item);
        }
        m_output_queue.setDone();
    }

    virtual ~PipelineStage() = default;
};

// multiplication stage that doubles input values
class MultiplyStage final : public PipelineStage {
public:
    MultiplyStage(SafeQueue<int>& in_queue, SafeQueue<int>& out_queue, Logger *logger)
        : PipelineStage(in_queue, out_queue, "Multiply Stage", logger) {}

    // implements multiplication processing
    void process(const int item) override {
        // simulate processing time
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        m_output_queue.push(item * 2);
    }
};

// addition stage that adds 10 to input values
class AddStage final : public PipelineStage {
public:
    AddStage(SafeQueue<int>& in_queue, SafeQueue<int>& out_queue, Logger *logger)
        : PipelineStage(in_queue, out_queue, "Add Stage", logger) {}

    // implements addition processing
    void process(const int item) override {
        // simulate processing time
        std::this_thread::sleep_for(std::chrono::milliseconds(150));
        m_output_queue.push(item + 10);
    }
};

// filter stage that only passes even numbers
class FilterStage final : public PipelineStage {
public:
    FilterStage(SafeQueue<int>& in_queue, SafeQueue<int>& out_queue, Logger *logger)
        : PipelineStage(in_queue, out_queue, "Filter Stage", logger) {}

    // implements filtering logic
    void process(const int item) override {
        // simulate processing time
        std::this_thread::sleep_for(std::chrono::milliseconds(80));
        if (item % 2 == 0) {
            m_output_queue.push(item);
        }
    }
};

int main() {
    // initialize queues for each stage of the pipeline
    SafeQueue<int> input_queue;
    SafeQueue<int> multiply_queue;
    SafeQueue<int> add_queue;
    SafeQueue<int> output_queue;

    // thread safe logger
    Logger logger("../custom.log");

    // create pipeline stage objects
    MultiplyStage multiply_stage(input_queue, multiply_queue, &logger);
    AddStage add_stage(multiply_queue, add_queue, &logger);
    FilterStage filter_stage(add_queue, output_queue, &logger);


    // create and start threads for each pipeline stage
    std::thread multiply_thread(&MultiplyStage::run, &multiply_stage);
    std::thread add_thread(&AddStage::run, &add_stage);
    std::thread filter_thread(&FilterStage::run, &filter_stage);

    // feed input data into the pipeline
    for (int i = 1; i <= 10; ++i) {
        input_queue.push(i);
    }
    // signal that no more input data will be added
    input_queue.setDone();

    // create output processing thread
    std::thread output_thread([&output_queue, &logger]() {
        int item;
        while (output_queue.pop(item)) {
            logger.log(LogLevel::INFO, "Final output: " + std::to_string(item));
        }
    });

    // wait for all pipeline stages to complete
    multiply_thread.join();
    add_thread.join();
    filter_thread.join();
    output_thread.join();

    return 0;
}