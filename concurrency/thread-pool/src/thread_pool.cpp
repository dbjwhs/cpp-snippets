// MIT license
// copyright (c) 2025 dbjwhs
//

#include <thread>
#include <queue>
#include <functional>
#include <condition_variable>
#include <vector>
#include <future>
#include "../../../headers/project_utils.hpp"

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
    Logger& m_logger;

    // prevent default construction and copying of the thread pool
    DECLARE_NON_COPYABLE(ThreadPool);
    DECLARE_NON_MOVEABLE(ThreadPool);

public:
    // the constructor creates a specified number of worker threads
    explicit ThreadPool(const size_t numThreads, Logger& logger)
        : m_stop(false), m_logger(logger) {
        m_logger.log(LogLevel::INFO, "Initializing thread pool with " + std::to_string(numThreads) + " threads");

        for (size_t i = 0; i < numThreads; ++i) {
            // create worker threads using a lambda function
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
                            m_logger.log(LogLevel::INFO, "Worker thread " + threadIdToString(), " shutting down");
                            return;
                        }

                        // get next task from queue
                        task = std::move(m_tasks.front());
                        m_tasks.pop();
                        m_logger.log(LogLevel::INFO, "Worker thread " + threadIdToString(), " dequeued a task");
                    }

                    // execute the task
                    task();
                }
            });
            m_logger.log(LogLevel::INFO, "Created worker thread " + threadIdToString(m_workers.back().get_id()));
        }
    }

    // destructor ensures clean shutdown of the thread pool
    ~ThreadPool() {
        {
            // signal threads to stop
            std::unique_lock<std::mutex> lock(m_queueMutex);
            m_stop = true;
            m_logger.log(LogLevel::INFO, "Initiating thread pool shutdown");
        }

        // wake up all threads
        m_condition.notify_all();

        // wait for all threads to finish
        for (std::thread& worker : m_workers) {
            if (worker.joinable()) {
                worker.join();
                m_logger.log(LogLevel::INFO, "Worker thread " + threadIdToString(worker.get_id()) + " joined");
            }
        }
        m_logger.log(LogLevel::INFO, "Thread pool shutdown complete");
    }

    // enqueue a task and return a future for the result
    template<class TQueue, class... Args>
    auto enqueue(TQueue&& tqueue, Args&&... args)
        -> std::future<std::invoke_result_t<TQueue, Args...>> {
        // determine the return type of the task
        using return_type = std::invoke_result_t<TQueue, Args...>;

        // create a packaged task
        auto task = std::make_shared<std::packaged_task<return_type()>>(
            [Func = std::forward<TQueue>(tqueue)] {
                return Func();
            }
        );

        // get the future for the task result
        std::future<return_type> result = task->get_future();
        {
            // lock the queue mutex
            std::unique_lock<std::mutex> lock(m_queueMutex);
            // check if the thread pool is stopped
            if (m_stop) {
                throw std::runtime_error("Cannot enqueue on stopped ThreadPool");
            }

            // add task to queue via lambda
            m_tasks.emplace([task]() {
                (*task)();
            });
            m_logger.log(LogLevel::INFO, "Task enqueued");
        }

        // notify one waiting thread
        m_condition.notify_one();
        return result;
    }
};

int main() {
    Logger& logger = Logger::getInstance();

    try {
        // thread pool will use the maximum number of concurrent threads supported
        const unsigned int threadCount = std::thread::hardware_concurrency();

        if (threadCount == 0) {
            logger.log(LogLevel::INFO, "Failed to detect the number of concurrent threads supported");
            return 1;
        }

        logger.log(LogLevel::INFO, "This machine supports " + std::to_string(threadCount) + " concurrent threads");

        // vector to store future results
        std::vector<std::future<int>> results;

        // create new scope for thread pool
        {
            // create thread pool with worker threads
            ThreadPool pool(threadCount, logger);

            // add tasks to the thread pool
            for (int i = 0; i < threadCount * 2; ++i) {
                // enqueue a task that prints the thread ID and returns the square of the input
                results.emplace_back(
                    pool.enqueue([i, &logger] {
                        logger.log(LogLevel::INFO, "Task " + std::to_string(i) + " running on thread ", threadIdToString());

                        // do some work
                        std::this_thread::sleep_for(std::chrono::seconds(1));
                        return i * i;
                    })
                );
            }
        }

        // get and print results
        for (size_t i = 0; i < results.size(); ++i) {
            logger.log(LogLevel::INFO, "Result ", std::to_string(i) + ": " + std::to_string(results[i].get()));
        }
    } catch (const std::exception& e) {
        // handle any exceptions
        logger.log(LogLevel::CRITICAL, "Error: " + std::string(e.what()));
        return 1;
    }

    return 0;
}
