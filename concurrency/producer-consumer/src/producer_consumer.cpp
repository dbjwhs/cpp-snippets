// mit license
// copyright (c) 2025 dbjwhs
//

#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <random>
#include <chrono>
#include <atomic>

// Thread-safe stream wrapper
class ThreadSafeStream {
private:
    std::mutex m_mutex;
    std::ostream& m_stream;

public:
    explicit ThreadSafeStream(std::ostream& stream) : m_stream(stream) {}

    template<typename... Args>
    void print(Args&&... args) {
        std::lock_guard<std::mutex> lock(m_mutex);
        (m_stream << ... << std::forward<Args>(args)) << std::endl;
    }
};

// Thread-safe queue implementation
template <typename T>
class ThreadSafeQueue {
private:
    std::queue<T> m_queue;
    mutable std::mutex m_mutex;
    std::condition_variable m_notEmpty;
    std::condition_variable m_notFull;
    size_t m_capacity;
    ThreadSafeStream& m_output;  // queue-specific logging

public:
    explicit ThreadSafeQueue(size_t max_size, ThreadSafeStream& output)
        : m_capacity(max_size), m_output(output) {}

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

// Producer class
class Producer {
private:
    ThreadSafeQueue<int>& m_queue;
    std::atomic<bool>& m_running;
    int m_id;
    ThreadSafeStream& m_output;

public:
    Producer(ThreadSafeQueue<int>& q, std::atomic<bool>& run,
             int producer_id, ThreadSafeStream& output)
        : m_queue(q), m_running(run), m_id(producer_id), m_output(output) {}

    void operator()() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(1, 100);

        while (m_running) {
            int value = dis(gen);
            m_queue.push(value);
            m_output.print("Producer ", m_id, " produced: ", value);

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
    ThreadSafeStream& m_output;

public:
    Consumer(ThreadSafeQueue<int>& q, std::atomic<bool>& run,
             int consumer_id, ThreadSafeStream& output)
        : m_queue(q), m_running(run), m_id(consumer_id), m_output(output) {}

    void operator()() {
        while (m_running || !m_queue.empty()) {
            int value = m_queue.pop();
            m_output.print("Consumer ", m_id, " consumed: ", value);

            // simulate some work
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    }
};

int main() {
    const size_t QUEUE_CAPACITY = 10;
    const int NUM_PRODUCERS = 2;
    const int NUM_CONSUMERS = 3;

    ThreadSafeStream output(std::cout);
    ThreadSafeQueue<int> queue(QUEUE_CAPACITY, output);
    std::atomic<bool> running(true);

    // Reserve vector capacity upfront to prevent reallocation during emplace_back.
    // While the performance impact is minimal for small thread counts, reserving
    // known sizes upfront eliminates vector resizing, memory moves, and potential
    // iterator invalidation. This is especially important for larger thread pools
    // or performance-critical systems.
    std::vector<std::thread> producers;
    std::vector<std::thread> consumers;
    producers.reserve(NUM_PRODUCERS);
    consumers.reserve(NUM_CONSUMERS);

    // start producers
    for (int i = 0; i < NUM_PRODUCERS; ++i) {
        producers.emplace_back(Producer(queue, running, i + 1, output));
    }

    // start consumers
    for (int i = 0; i < NUM_CONSUMERS; ++i) {
        consumers.emplace_back(Consumer(queue, running, i + 1, output));
    }

    // let the simulation run for a while
    std::this_thread::sleep_for(std::chrono::seconds(10));

    // signal threads to stop
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