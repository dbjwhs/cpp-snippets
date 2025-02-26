// mit license
// copyright (c) 2025 dbjwhs
//

#include <queue>
#include <thread>
#include <condition_variable>
#include <random>
#include <atomic>
#include "../../../headers/project_utils.hpp"

// thread-safe queue implementation
template <typename T>
class ThreadSafeQueue {
private:
    std::queue<T> m_queue;
    mutable std::mutex m_mutex;
    std::condition_variable m_notEmpty;
    std::condition_variable m_notFull;
    size_t m_capacity;
    Logger& m_logger = Logger::getInstance();

public:
    explicit ThreadSafeQueue(const size_t max_size) : m_capacity(max_size) {}

    void push(T value) {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_notFull.wait(lock, [this]() { return m_queue.size() < m_capacity; });
        m_queue.push(std::move(value));
        lock.unlock();
        m_notEmpty.notify_one();
    }

    T pop() {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_notEmpty.wait(lock, [this]() { return !m_queue.empty(); });
        T value = std::move(m_queue.front());
        m_queue.pop();
        lock.unlock();
        m_notFull.notify_one();
        return value;
    }

    bool empty() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_queue.empty();
    }

    size_t size() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_queue.size();
    }
};

// producer class
class Producer {
private:
    ThreadSafeQueue<int>& m_queue;
    std::atomic<bool>& m_running;
    int m_id;
    Logger& m_logger = Logger::getInstance();

public:
    Producer(ThreadSafeQueue<int>& q, std::atomic<bool>& run,
             int producer_id)
        : m_queue(q), m_running(run), m_id(producer_id) {}

    void operator()() const {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(1, 100);

        while (m_running) {
            int value = dis(gen);
            m_queue.push(value);
            m_logger.log(LogLevel::INFO, "Producer " + std::to_string(m_id) + " produced: " + std::to_string(value));

            // simulate some work
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    }
};

// consumer class
class Consumer {
private:
    ThreadSafeQueue<int>& m_queue;
    std::atomic<bool>& m_running;
    int m_id;
    Logger& m_logger = Logger::getInstance();

public:
    Consumer(ThreadSafeQueue<int>& q, std::atomic<bool>& run,
             const int consumer_id) : m_queue(q), m_running(run), m_id(consumer_id) {}

    void operator()() const {
        while (m_running || !m_queue.empty()) {
            int value = m_queue.pop();
            m_logger.log(LogLevel::INFO, "Consumer " + std::to_string(m_id) + " consumed: " + std::to_string(value));

            // simulate some work
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    }
};

int main() {
    constexpr size_t QUEUE_CAPACITY = 10;
    constexpr int NUM_PRODUCERS = 2;
    constexpr int NUM_CONSUMERS = 3;
    ThreadSafeQueue<int> queue(QUEUE_CAPACITY);
    std::atomic<bool> running(true);

    // reserve vector capacity upfront to prevent reallocation during emplace_back.
    // while the performance impact is minimal for small thread counts, reserving
    // known sizes upfront eliminates vector resizing, memory moves, and potential
    // iterator invalidation. this is especially important for larger thread pools
    // or performance-critical systems.
    std::vector<std::thread> producers;
    std::vector<std::thread> consumers;
    producers.reserve(NUM_PRODUCERS);
    consumers.reserve(NUM_CONSUMERS);

    // start producers
    for (int i = 0; i < NUM_PRODUCERS; ++i) {
        producers.emplace_back(Producer(queue, running, i + 1));
    }

    // start consumers
    for (int i = 0; i < NUM_CONSUMERS; ++i) {
        consumers.emplace_back(Consumer(queue, running, i + 1));
    }

    // let the simulation run for a while
    std::this_thread::sleep_for(std::chrono::seconds(10));

    // signal threads to stop, note std::atomic above and passed
    // to both producer and consumer ctor's
    // resharper disabling once cppdfaunusedvalue
    running = false;

    // wait for all threads to finish
    for (auto& producer : producers) {
        producer.join();
    }
    for (auto& consumer : consumers) {
        consumer.join();
    }

    return 0;
}
