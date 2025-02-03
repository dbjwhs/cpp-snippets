// MIT License
// Copyright (c) 2025 dbjwhs

#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <chrono>
#include <atomic>
#include <utility>
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
};

// enhanced result holder with timeout and cancellation
template<typename ReturnType>
class MethodResult {
private:
    std::promise<ReturnType> m_promise;
    std::future<ReturnType> m_future;
    std::atomic<bool> m_cancelled{false};

public:
    MethodResult() : m_future(m_promise.get_future()) {}

    // get with timeout
    ReturnType get(const std::chrono::milliseconds& timeout) {
        if (m_future.wait_for(timeout) == std::future_status::timeout) {
            throw std::runtime_error("operation timed out");
        }
        return m_future.get();
    }

    // get without timeout
    ReturnType get() {
        return m_future.get();
    }

    void set(const ReturnType& value) {
        if (!m_cancelled) {
            m_promise.set_value(value);
        }
    }

    void setException(const std::exception& ex) {
        m_promise.set_exception(std::make_exception_ptr(ex));
    }

    // cancellation support
    void cancel() {
        m_cancelled = true;
        try {
            m_promise.set_exception(std::make_exception_ptr(
                std::runtime_error("operation cancelled")));
        } catch (...) {
            // promise might already be satisfied
        }
    }

    bool isCancelled() const {
        return m_cancelled;
    }
};

// priority enumeration
enum class Priority {
    LOW,
    MEDIUM,
    HIGH
};

// enhanced method request with priority and cancellation
template<typename RequestType>
class MethodRequest {
protected:
    std::shared_ptr<MethodResult<RequestType>> m_result;
    Priority m_priority;

public:
    MethodRequest(std::shared_ptr<MethodResult<RequestType>> result, const Priority priority)
        : m_result(std::move(result)), m_priority(priority) {}

    virtual ~MethodRequest() = default;
    virtual void call() = 0;

    [[nodiscard]] Priority getPriority() const { return m_priority; }
    [[nodiscard]] bool isCancelled() const { return m_result->isCancelled(); }
    void setException(const std::exception& e) {
        m_result->setException(e);
    }
};

// comparison functor for priority queue
template<typename CompareType>
struct RequestCompare {
    bool operator()(const std::shared_ptr<MethodRequest<CompareType>>& a,
                   const std::shared_ptr<MethodRequest<CompareType>>& b) {
        return static_cast<int>(a->getPriority()) < static_cast<int>(b->getPriority());
    }
};

// active object implementation
class Calculator {
private:
    // priority queue instead of regular queue
    std::priority_queue<
        std::shared_ptr<MethodRequest<int>>,
        std::vector<std::shared_ptr<MethodRequest<int>>>,
        RequestCompare<int>
    > m_activation_queue;

    std::mutex m_mutex;
    std::condition_variable m_condition;
    std::thread m_scheduler;
    bool m_is_running;

    void processMethodQueue() {
        while (m_is_running) {
            std::shared_ptr<MethodRequest<int>> method;
            {
                std::unique_lock<std::mutex> lock(m_mutex);
                m_condition.wait(lock, [this]() {
                    return !m_activation_queue.empty() || !m_is_running;
                });

                // first glance suggests !m_is_running would always evaluate to false, but that's not the case.
                //
                // processMethodQueue() processes the activation queue until a shutdown is requested (!m_is_running becomes true)
                // the condition !m_is_running && m_activation_queue.empty() means:
                // 1. we received a shutdown request (!m_is_running is true)
                // 2. AND we've processed all remaining items in the queue (m_activation_queue.empty() is true)
                // this ensures we:
                //   - don't exit if there's still work to do during shutdown
                //   - process all pending operations before shutting down
                //   - exit cleanly only when both shutdown is requested and all work is completed
                // the check is needed because m_is_running starts as true (set in constructor) and becomes
                // false when shutdown() is called, allowing for graceful completion of pending tasks
                if (!m_is_running && m_activation_queue.empty()) {
                    return;
                }

                if (!m_activation_queue.empty()) {
                    method = m_activation_queue.top();
                    m_activation_queue.pop();
                }
            }

            if (method && !method->isCancelled()) {
                try {
                    method->call();
                } catch (const std::exception& e) {
                    method->setException(e);
                }
            }
        }
    }

public:
    Calculator() : m_is_running(true) {
        m_scheduler = std::thread(&Calculator::processMethodQueue, this);
    }

    ~Calculator() {
        shutdown();
    }

    // enhanced add method with priority
    std::shared_ptr<MethodResult<int>> add(int x, int y, Priority priority = Priority::MEDIUM) {
        auto result = std::make_shared<MethodResult<int>>();
        auto request = std::make_shared<AddRequest>(result, x, y, priority);

        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_activation_queue.push(request);
        }
        m_condition.notify_one();
        return result;
    }

    // enhanced multiply method with priority
    std::shared_ptr<MethodResult<int>> multiply(int x, int y, Priority priority = Priority::MEDIUM) {
        auto result = std::make_shared<MethodResult<int>>();
        auto request = std::make_shared<MultiplyRequest>(result, x, y, priority);

        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_activation_queue.push(request);
        }
        m_condition.notify_one();
        return result;
    }

    void shutdown() {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_is_running = false;
        }
        m_condition.notify_one();
        if (m_scheduler.joinable()) {
            m_scheduler.join();
        }
    }

private:
    class AddRequest : public MethodRequest<int> {
    private:
        int m_x;
        int m_y;

    public:
        AddRequest(std::shared_ptr<MethodResult<int>> result, int x, int y, Priority priority)
            : MethodRequest<int>(result, priority), m_x(x), m_y(y) {}

        void call() override {
            if (!isCancelled()) {
                try {
                    // Check for overflow in addition
                    if ((m_x > 0 && m_y > std::numeric_limits<int>::max() - m_x) ||
                        (m_x < 0 && m_y < std::numeric_limits<int>::min() - m_x)) {
                        throw std::overflow_error("addition overflow");
                    }

                    // simulate shorter processing time (100ms)
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    m_result->set(m_x + m_y);
                } catch (const std::exception& e) {
                    setException(e);
                }
            }
        }
    };

    class MultiplyRequest : public MethodRequest<int> {
    private:
        int m_x;
        int m_y;

    public:
        MultiplyRequest(std::shared_ptr<MethodResult<int>> result, int x, int y, Priority priority)
            : MethodRequest<int>(result, priority), m_x(x), m_y(y) {}

        void call() override {
            if (!isCancelled()) {
                try {
                    // Check for overflow in multiplication
                    if (m_x != 0 && m_y != 0) {
                        if (m_x > std::numeric_limits<int>::max() / m_y ||
                            m_x < std::numeric_limits<int>::min() / m_y) {
                            throw std::overflow_error("multiplication overflow");
                        }
                    }

                    // simulate shorter processing time (100ms)
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    m_result->set(m_x * m_y);
                } catch (const std::exception& e) {
                    setException(e);
                }
            }
        }
    };
};

// enhanced test code
int main() {
    Calculator calculator;
    Logger logger("../custom.log");

    try {
        logger.log(LogLevel::INFO, "=== Comprehensive Test Suite ===");

        // 1. basic Operation Tests
        logger.log(LogLevel::INFO, "1. Basic Operations:");
        {
            auto add = calculator.add(5, 3);
            auto multiply = calculator.multiply(4, 2);
            logger.log(LogLevel::INFO, "Add result (5+3): " + std::to_string(add->get()));;
            logger.log(LogLevel::INFO, "Multiply result (4*2): " + std::to_string(multiply->get()));
        }

        // 2. priority Tests
        logger.log(LogLevel::INFO, "2. Priority Handling:");
        {
            // Queue multiple operations with different priorities
            std::vector<std::shared_ptr<MethodResult<int>>> results;
            results.push_back(calculator.add(1, 1, Priority::LOW));
            results.push_back(calculator.add(2, 2, Priority::MEDIUM));
            results.push_back(calculator.add(3, 3, Priority::HIGH));
            results.push_back(calculator.multiply(2, 2, Priority::LOW));
            results.push_back(calculator.multiply(3, 3, Priority::HIGH));

            // high priority operations should complete first
            logger.log(LogLevel::INFO, "High priority add (3+3): " + std::to_string(results[2]->get()));
            logger.log(LogLevel::INFO, "High priority multiply (3*3): " + std::to_string(results[4]->get()));
            logger.log(LogLevel::INFO, "Medium priority (2+2): " + std::to_string(results[1]->get()));
            logger.log(LogLevel::INFO, "Low priority add (1+1): " + std::to_string(results[0]->get()));
            logger.log(LogLevel::INFO, "Low priority multiply (2*2): " + std::to_string(results[3]->get()));
        }

        // 3. timeout Tests
        logger.log(LogLevel::INFO, "3. Timeout Handling:");
        {
            // test immediate timeout
            auto quickTimeout = calculator.multiply(5, 5);
            try {
                quickTimeout->get(std::chrono::milliseconds(1));
                logger.log(LogLevel::CRITICAL, "Error: Should have timed out!");
            } catch (const std::runtime_error& e) {
                logger.log(LogLevel::INFO, "Expected immediate timeout caught: " + std::string(e.what()));
            }

            // test successful completion within timeout
            auto successfulOp = calculator.add(1, 1);
            try {
                int result = successfulOp->get(std::chrono::milliseconds(500));
                logger.log(LogLevel::INFO, "Operation completed within timeout: " + std::to_string(result));
            } catch (const std::runtime_error& e) {
                logger.log(LogLevel::CRITICAL, "Error: Should not have timed out!");
            }

            // test multiple timeouts concurrently
            auto op1 = calculator.multiply(2, 3);
            auto op2 = calculator.multiply(4, 5);
            try {
                op1->get(std::chrono::milliseconds(50));
                op2->get(std::chrono::milliseconds(50));
            } catch (const std::runtime_error& e) {
                logger.log(LogLevel::INFO, "Multiple timeout handling working");
            }
        }

        // 4. cancellation Tests
        logger.log(LogLevel::INFO, "4. Cancellation Handling:");
        {
            // Test immediate cancellation
            auto immediateCancellation = calculator.add(7, 7);
            immediateCancellation->cancel();
            try {
                immediateCancellation->get();
                logger.log(LogLevel::CRITICAL, "Error: Should have been cancelled!");
            } catch (const std::runtime_error& e) {
                logger.log(LogLevel::INFO, "Immediate cancellation working: " + std::string(e.what()));
            }

            // test cancellation of multiple operations
            auto op1 = calculator.multiply(8, 8);
            auto op2 = calculator.add(9, 9);
            op1->cancel();
            op2->cancel();
            try {
                op1->get();
                op2->get();
                logger.log(LogLevel::CRITICAL, "Error: Operations should have been cancelled!");
            } catch (const std::runtime_error& e) {
                logger.log(LogLevel::INFO, "Multiple cancellation working");
            }

            // test cancellation with timeout
            auto cancelWithTimeout = calculator.multiply(10, 10);
            cancelWithTimeout->cancel();
            try {
                cancelWithTimeout->get(std::chrono::milliseconds(500));
                logger.log(LogLevel::CRITICAL, "Error: Should have been cancelled!");
            } catch (const std::runtime_error& e) {
                logger.log(LogLevel::INFO, "Cancellation with timeout working");
            }
        }

        // 5. error Handling Tests
        logger.log(LogLevel::INFO, "5. Error Handling:");
        {
            // Test integer overflow
            auto overflowTest = calculator.add(std::numeric_limits<int>::max(), 1);
            try {
                overflowTest->get();
                logger.log(LogLevel::CRITICAL, "Error: Should have caught overflow!");
            } catch (const std::exception& e) {
                logger.log(LogLevel::INFO, "Overflow handling working " + std::string(e.what()));
            }

            // test with different priorities
            auto overflowHighPriority = calculator.multiply(std::numeric_limits<int>::max(), 2, Priority::HIGH);
            try {
                overflowHighPriority->get();
                logger.log(LogLevel::CRITICAL, "Error: Should have caught overflow!");
            } catch (const std::exception& e) {
                logger.log(LogLevel::INFO, "High priority overflow handling working");
            }
        }

        // 6. mixed Operation Tests
        logger.log(LogLevel::INFO, "6. Mixed Operation Scenarios:");
        {
            // combine priorities, timeouts, and cancellations
            auto highPriorityOp = calculator.add(1, 1, Priority::HIGH);
            auto mediumPriorityOp = calculator.multiply(2, 2, Priority::MEDIUM);
            auto lowPriorityOp = calculator.add(3, 3, Priority::LOW);

            // cancel medium priority operation
            mediumPriorityOp->cancel();

            // try to get results with timeout
            try {
                logger.log(LogLevel::INFO, "High priority result: " + std::to_string(highPriorityOp->get(std::chrono::milliseconds(200))));
                mediumPriorityOp->get(std::chrono::milliseconds(200));
                logger.log(LogLevel::CRITICAL, "Error: Should have been cancelled!");
            } catch (const std::runtime_error& e) {
                logger.log(LogLevel::INFO, "Mixed scenario handling working");
            }

            // low priority should still complete
            logger.log(LogLevel::INFO, "Low priority result: " + std::to_string(lowPriorityOp->get()));
        }

        // 7. stress Test with Mixed Operations
        logger.log(LogLevel::INFO, "7. Stress Test:");
        {
            const int numOperations = 100;
            std::vector<std::shared_ptr<MethodResult<int>>> results;

            // queue mix of operations with different priorities
            for (int i = 0; i < numOperations; ++i) {
                Priority priority = static_cast<Priority>(i % 3);  // Cycle through priorities
                if (i % 2 == 0) {
                    results.push_back(calculator.add(i, i, priority));
                } else {
                    results.push_back(calculator.multiply(i, 2, priority));
                }

                // randomly cancel some operations
                if (i % 7 == 0) {  // Cancel ~14% of operations
                    results.back()->cancel();
                }
            }

            // try to get all results
            int completed = 0;
            int cancelled = 0;
            int timedOut = 0;

            for (auto& result : results) {
                try {
                    result->get(std::chrono::milliseconds(150));
                    completed++;
                } catch (const std::runtime_error& e) {
                    std::string error = e.what();
                    if (error.find("cancelled") != std::string::npos) {
                        cancelled++;
                    } else if (error.find("timed out") != std::string::npos) {
                        timedOut++;
                    }
                }
            }

            logger.log(LogLevel::INFO, "Completed: " + std::to_string(completed));
            logger.log(LogLevel::INFO, "Canceled: " + std::to_string(cancelled));
            logger.log(LogLevel::INFO, "Timed Out: " + std::to_string(timedOut));
        }

    } catch (const std::exception& e) {
        logger.log(LogLevel::CRITICAL, "Unexpected error: " + std::string(e.what()));
    }

    return 0;
}