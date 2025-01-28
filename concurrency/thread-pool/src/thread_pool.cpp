// mit license
// copyright (c) 2025 dbjwhs
//

// ensure c++17 or later is being used
#if __cplusplus < 201703L
    #error This file requires C++17 or later
#endif

#include <iostream>
#include <thread>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <future>
#include <type_traits>

// thread-safe stream wrapper
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

class ThreadPool {
private:
    // vector to store worker threads
    std::vector<std::thread> m_workers;
    // queue to store pending tasks
    std::queue<std::function<void()>> m_tasks;
    // mutex to protect access to the task queue
    std::mutex m_queueMutex;
    // condition variable for thread synchronization
    std::condition_variable m_condition;
    // flag to signal thread pool shutdown
    bool m_stop;
    // thread-safe logger
    ThreadSafeStream m_logger;

    // prevent default construction and copying of thread pool
    ThreadPool() = delete;
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool(ThreadPool&&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;
    ThreadPool& operator=(ThreadPool&&) = delete;

public:
    // constructor creates specified number of worker threads
    ThreadPool(size_t numThreads)
        : m_stop(false), m_logger(std::cout) {
        m_logger.print("Initializing thread pool with ", numThreads, " threads");

        for (size_t i = 0; i < numThreads; ++i) {
            // create worker threads using lambda function
            m_workers.emplace_back([this] {
                while (true) {
                    std::function<void()> task;

                    {
                        // lock the queue mutex
                        std::unique_lock<std::mutex> lock(m_queueMutex);

                        // wait for tasks or stop signal
                        m_condition.wait(lock, [this] {
                            return m_stop || !m_tasks.empty();
                        });

                        // exit if stopped and no tasks remain
                        if (m_stop && m_tasks.empty()) {
                            m_logger.print("Worker thread ", std::this_thread::get_id(), " shutting down");
                            return;
                        }

                        // get next task from queue
                        task = std::move(m_tasks.front());
                        m_tasks.pop();
                        m_logger.print("Worker thread ", std::this_thread::get_id(), " dequeued a task");
                    }

                    // execute the task
                    task();
                }
            });
            m_logger.print("Created worker thread ", m_workers.back().get_id());
        }
    }

    // destructor ensures clean shutdown of thread pool
    ~ThreadPool() {
        {
            // signal threads to stop
            std::unique_lock<std::mutex> lock(m_queueMutex);
            m_stop = true;
            m_logger.print("Initiating thread pool shutdown");
        }

        // wake up all threads
        m_condition.notify_all();

        // wait for all threads to finish
        for (std::thread& worker : m_workers) {
            if (worker.joinable()) {
                worker.join();
                m_logger.print("Worker thread ", worker.get_id(), " joined");
            }
        }
        m_logger.print("Thread pool shutdown complete");
    }

    // enqueue a task and return a future for the result
    template<class TQueue, class... Args>
    auto enqueue(TQueue&& tqueue, Args&&... args)
        -> std::future<typename std::invoke_result<TQueue, Args...>::type> {
        // determine the return type of the task
        using return_type = typename std::invoke_result<TQueue, Args...>::type;

        // create a packaged task
        auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<TQueue>(tqueue), std::forward<Args>(args)...)
        );

        // get future for the task result
        std::future<return_type> result = task->get_future();

        {
            // lock the queue mutex
            std::unique_lock<std::mutex> lock(m_queueMutex);
            // check if thread pool is stopped
            if (m_stop) {
                throw std::runtime_error("Cannot enqueue on stopped ThreadPool");
            }

            // add task to queue
            m_tasks.emplace([task]() { (*task)(); });
            m_logger.print("Task enqueued");
        }

        // notify one waiting thread
        m_condition.notify_one();
        return result;
    }
};

int main() {
    try {
        // thread pool will use maximum number of concurrent threads supported
        unsigned int threadCount = std::thread::hardware_concurrency();
        ThreadSafeStream logger(std::cout);

        if (threadCount == 0) {
            logger.print("Failed to detect the number of concurrent threads supported");
            return 1;
        }

        logger.print("This machine supports ", threadCount, " concurrent threads");

        // vector to store future results
        std::vector<std::future<int>> results;

        // Create new scope for thread pool
        {
            // create thread pool with worker threads
            ThreadPool pool(threadCount);

            // add tasks to the thread pool
            for (int i = 0; i < threadCount * 2; ++i) {
                // enqueue task that prints thread id and returns square of input
                results.emplace_back(
                    pool.enqueue([i, &logger] {
                        logger.print("Task ", i, " running on thread ", std::this_thread::get_id());

                        // do some work
                        std::this_thread::sleep_for(std::chrono::seconds(1));
                        return i * i;
                    })
                );
            }
        }

        // get and print results
        for (size_t i = 0; i < results.size(); ++i) {
            logger.print("Result ", i, ": ", results[i].get());
        }
    } catch (const std::exception& e) {
        // handle any exceptions
        ThreadSafeStream logger(std::cerr);
        logger.print("Error: ", e.what());
        return 1;
    }

    return 0;
}