#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <cassert>
#include <atomic>
#include <mutex>
#include <format>
#include "../../../headers/project_utils.hpp"

// simplified stop token mechanism
class stop_token {
public:
    stop_token() : m_stop(std::make_shared<std::atomic<bool>>(false)) {}

    void request_stop() const {
        m_stop->store(true);
    }

    [[nodiscard]] bool stop_requested() const {
        return m_stop->load();
    }

private:
    std::shared_ptr<std::atomic<bool>> m_stop;
};

// simplified jthread implementation
class jthread {
public:
    jthread() = default;

    template<typename F>
    explicit jthread(F&& f) : m_stop(std::make_shared<stop_token>()) {
        m_thread = std::thread([f = std::forward<F>(f), stop = m_stop]() {
            f(*stop);
        });
    }

    ~jthread() {
        if (joinable()) {
            request_stop();
            join();
        }
    }

    // non-copyable
    jthread(const jthread&) = delete;
    jthread& operator=(const jthread&) = delete;

    // movable
    jthread(jthread&& other) noexcept :
        m_thread(std::move(other.m_thread)),
        m_stop(std::move(other.m_stop)) {}

    jthread& operator=(jthread&& other) noexcept {
        if (joinable()) {
            request_stop();
            join();
        }
        m_thread = std::move(other.m_thread);
        m_stop = std::move(other.m_stop);
        return *this;
    }

    void join() {
        if (m_thread.joinable()) {
            m_thread.join();
        }
    }

    [[nodiscard]] bool joinable() const {
        return m_thread.joinable();
    }

    void request_stop() const {
        if (m_stop) {
            m_stop->request_stop();
        }
    }

private:
    std::thread m_thread;
    std::shared_ptr<stop_token> m_stop;
};

// thread safe queue processor
class ThreadSafeQueue {
public:
    ThreadSafeQueue() {
        LOG_INFO(std::format("thread safe queue initialized"));
        startProcessing();
    }

    ~ThreadSafeQueue() {
        try {
            LOG_INFO(std::format("thread safe queue destructor called"));
            stop();
            LOG_INFO(std::format("thread safe queue destroyed"));
        } catch (...) {
            // ensure no exceptions escape destructor
        }
    }

    void addData(const std::vector<std::string>& data) {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_queue.insert(m_queue.end(), data.begin(), data.end());
        }
        LOG_INFO(std::format("added {} items to queue", data.size()));
    }

    void stop() {
        if (m_processor.joinable()) {
            LOG_INFO(std::format("stopping processor"));
            m_processor.request_stop();
            m_processor.join();
            LOG_INFO(std::format("processor stopped"));
        }
    }

    size_t getProcessedCount() const {
        return m_processed_count.load();
    }

    size_t getCurrentQueueSize() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_queue.size();
    }

private:
    void startProcessing() {
        m_processor = jthread([this](const stop_token& token) {
            LOG_INFO(std::format("processing thread started"));

            while (!token.stop_requested()) {
                std::string item;
                {
                    std::lock_guard<std::mutex> lock(m_mutex);
                    if (!m_queue.empty()) {
                        item = m_queue.front();
                        m_queue.erase(m_queue.begin());
                    }
                }

                if (!item.empty()) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    ++m_processed_count;
                    LOG_INFO(std::format("processed item: {}", item));
                } else {
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                }
            }

            LOG_INFO(std::format("processing thread ending"));
        });
    }

    mutable std::mutex m_mutex;
    std::vector<std::string> m_queue;
    std::atomic<size_t> m_processed_count{0};
    jthread m_processor;
};

void testVectorProcessing() {
    try {
        LOG_INFO(std::format("starting basic test"));

        ThreadSafeQueue queue;
        const std::vector<std::string> test_data{"test1", "test2", "test3"};

        queue.addData(test_data);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        size_t processed = queue.getProcessedCount();
        size_t remaining = queue.getCurrentQueueSize();

        Logger::getInstance().log(LogLevel::INFO,
            std::format("processed {} items, {} remaining", processed, remaining));

        assert(processed == 3 && "basic processing failed");
        assert(remaining == 0 && "queue should be empty");

        LOG_INFO(std::format("basic test completed"));
    } catch (const std::exception& e) {
        LOG_CRITICAL(std::format("test error: {}", e.what()));
        throw;
    }
}

int main() {
    try {
        LOG_INFO(std::format("starting custom jthread example tests"));
        testVectorProcessing();
        LOG_INFO(std::format("all tests completed successfully"));
        return 0;
    } catch (const std::exception& e) {
        LOG_CRITICAL(std::format("test failure: {}", e.what()));
        return 1;
    }
}
