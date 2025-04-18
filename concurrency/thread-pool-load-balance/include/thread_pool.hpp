// MIT License
// Copyright (c) 2025 dbjwhs

#ifndef THREAD_POOL_HPP
#define THREAD_POOL_HPP

// thread pool pattern history:
// the thread pool pattern emerged in the late 1990s as a solution to the performance overhead associated
// with thread creation and destruction. it was first formalized in the paper "half-sync/half-async: an
// architectural pattern for efficient and well-structured concurrent i/o" by douglas schmidt in 1995.
// the pattern became widely adopted in server applications to handle concurrent client requests efficiently.
// c++11 introduced standard threading capabilities, making thread pools easier to implement in standard c++.
// c++20's addition of semaphores and other synchronization primitives further enhanced thread pool implementations.
// thread pools are used extensively in web servers, database systems, game engines, and any application
// requiring efficient handling of multiple concurrent tasks.

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <atomic>
#include <cassert>
#include <optional>

#if __cplusplus >= 202002L
#include <semaphore>
#else
// simple semaphore implementation for c++17
class counting_semaphore {
private:
    // the current count of the semaphore
    std::atomic<int> m_count;

    // mutex and condition variable for thread synchronization
    std::mutex m_mutex;
    std::condition_variable m_cv;

public:
    // constructor initializes the semaphore with the given count
    explicit counting_semaphore(int count = 0) : m_count(count) {}

    // acquire decrements the counter or blocks until it can
    void acquire() {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_cv.wait(lock, [this] { return m_count > 0; });
        --m_count;
    }

    // try_acquire attempts to decrement the counter without blocking
    bool try_acquire() {
        int expected = m_count.load();
        while (expected > 0) {
            if (m_count.compare_exchange_weak(expected, expected - 1)) {
                return true;
            }
            expected = m_count.load();
        }
        return false;
    }

    // release increments the counter and potentially unblocks a waiting thread
    void release() {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            ++m_count;
        }
        m_cv.notify_one();
    }
};
#endif

// a thread pool implementation with task prioritization and load balancing
class ThreadPool {
public:
    // priority levels for tasks
    enum class Priority {
        LOW = 0,
        MEDIUM = 1,
        HIGH = 2
    };

private:
    // task class representing a unit of work
    class Task {
    public:
        // constructor sets the priority and function to execute
        Task(const Priority prio, std::function<void()> func)
            : m_priority(prio), m_function(std::move(func)) {}

        // execute the task
        void execute() const {
            m_function();
        }

        // get the priority of the task
        [[nodiscard]] Priority get_priority() const {
            return m_priority;
        }

    private:
        // the priority of this task
        Priority m_priority;

        // the function to execute
        std::function<void()> m_function;
    };

public:
    // comparison functor for priority queue
    struct TaskComparator {
        // compare tasks based on their priority
        bool operator()(const std::shared_ptr<Task>& t1, const std::shared_ptr<Task>& t2) const {
            // higher priority value means lower priority in std::priority_queue
            return t1->get_priority() < t2->get_priority();
        }
    };

    // constructor creates a thread pool with the specified number of threads
    explicit ThreadPool(size_t num_threads = std::thread::hardware_concurrency())
        : m_stop(false),
          m_paused(false),
          #if __cplusplus >= 202002L
          m_semaphore(num_threads),
          #else
          m_semaphore(num_threads),
          #endif
          m_active_tasks(0),
          m_busy_threads(0) {
        // ensure at least one thread is created
        num_threads = std::max(num_threads, static_cast<size_t>(1));

        // reserve space for threads to avoid reallocations
        m_threads.reserve(num_threads);

        // create the specified number of worker threads
        for (size_t ndx = 0; ndx < num_threads; ++ndx) {
            m_threads.emplace_back(&ThreadPool::worker_thread, this);
        }
    }

    // destructor ensures all threads are properly joined
    ~ThreadPool() {
        // set the stop flag and notify all threads
        {
            std::lock_guard<std::mutex> lock(m_queue_mutex);
            m_stop = true;
        }
        m_condition.notify_all();

        // join all worker threads
        for (auto& thread : m_threads) {
            if (thread.joinable()) {
                thread.join();
            }
        }
    }

    // disable copy construction and assignment to prevent multiple instances sharing the same threads
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

    // enqueue a task with a given priority and return a future to the result
    template<typename FutureTask, typename... Args>
    auto enqueue(Priority priority, FutureTask&& f, Args&&... args)
        -> std::future<std::invoke_result_t<FutureTask, Args...>> {
        // define the return type of the function
        using return_type = std::invoke_result_t<FutureTask, Args...>;

        // create a shared_ptr to the packaged_task
        auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<FutureTask>(f), std::forward<Args>(args)...)
        );

        // get the future from the task before potentially moving it
        std::future<return_type> result = task->get_future();

        // wrap the packaged task into a void function, create the task object with the given priority and add the task
        // to the queue
        {
            auto task_function = [task]() {
                (*task)();
            };
            auto task_obj = std::make_shared<Task>(priority, task_function);
            std::lock_guard<std::mutex> lock(m_queue_mutex);

            // check if the pool is stopping
            if (m_stop) {
                throw std::runtime_error("enqueue on stopped ThreadPool");
            }

            // add the task to the queue
            m_tasks.push(task_obj);

            // increment active task counter
            ++m_active_tasks;
        }

        // notify one waiting thread that there's a new task
        m_condition.notify_one();

        return result;
    }

    // get the current size of the task queue
    size_t get_queue_size() const {
        std::lock_guard<std::mutex> lock(m_queue_mutex);
        return m_tasks.size();
    }

    // get the number of worker threads
    size_t get_thread_count() const {
        return m_threads.size();
    }

    // get the number of busy threads
    size_t get_busy_count() const {
        return m_busy_threads.load();
    }

    // wait for all tasks to complete
    void wait_all() const {
        while (true) {
            {
                std::lock_guard<std::mutex> lock(m_queue_mutex);
                if (m_active_tasks == 0) {
                    return;
                }
            }
            std::this_thread::yield();
        }
    }

    // pause execution of new tasks
    void pause() {
        m_paused = true;
    }

    // resume execution of tasks
    void resume() {
        m_paused = false;
        m_condition.notify_all();
    }

private:
    // worker thread function
    void worker_thread() {
        while (true) {
            std::shared_ptr<Task> task;

            // scope for queue lock
            {
                std::unique_lock<std::mutex> lock(m_queue_mutex);

                // wait for a task or stop signal
                m_condition.wait(lock, [this] {
                    return m_stop || (!m_tasks.empty() && !m_paused);
                });

                // check if we need to stop
                if (m_stop && m_tasks.empty()) {
                    return;
                }

                // check if the pool is paused
                if (m_paused) {
                    continue;
                }

                // try to get a task from the queue
                if (!m_tasks.empty()) {
                    task = m_tasks.top();
                    m_tasks.pop();
                }
            }

            // if we got a task, acquire a semaphore slot and execute it
            if (task) {
                // acquire the semaphore to limit concurrent execution
                #if __cplusplus >= 202002L
                m_semaphore.acquire();
                #else
                m_semaphore.acquire();
                #endif

                // mark this thread as busy
                ++m_busy_threads;

                try {
                    // execute the task
                    task->execute();
                } catch (...) {
                    // decrement counters even if a task throws
                    --m_busy_threads;

                    // release the semaphore
                    #if __cplusplus >= 202002L
                    m_semaphore.release();
                    #else
                    m_semaphore.release();
                    #endif

                    // decrement active task counter
                    {
                        std::lock_guard<std::mutex> lock(m_queue_mutex);
                        --m_active_tasks;
                    }

                    // rethrow the exception
                    throw;
                }

                // mark this thread as no longer busy
                --m_busy_threads;

                // release the semaphore
                #if __cplusplus >= 202002L
                m_semaphore.release();
                #else
                m_semaphore.release();
                #endif

                // decrement active task counter
                {
                    std::lock_guard<std::mutex> lock(m_queue_mutex);
                    --m_active_tasks;
                }
            } else if (!try_steal_task()) {
                // if there's no task, and we can't steal one, yield to other threads
                std::this_thread::yield();
            }
        }
    }

    // attempt to balance the load by stealing tasks from other threads
    static bool try_steal_task() {
        // in a more complex implementation, this would try to steal tasks from another thread-local queue
        // for this simplified version; we just return false
        return false;
    }

    // vector of worker threads
    std::vector<std::thread> m_threads;

    // priority queue of tasks
    std::priority_queue<std::shared_ptr<Task>, std::vector<std::shared_ptr<Task>>, TaskComparator> m_tasks;

    // mutex to protect access to the task queue
    mutable std::mutex m_queue_mutex;

    // condition variable for thread synchronization
    std::condition_variable m_condition;

    // flag indicating whether the pool is shutting down
    std::atomic<bool> m_stop;

    // flag indicating whether the pool is paused
    std::atomic<bool> m_paused;

    // semaphore to limit concurrent task execution for load balancing
    #if __cplusplus >= 202002L
    std::counting_semaphore<> m_semaphore;
    #else
    counting_semaphore m_semaphore;
    #endif

    // counter for active tasks
    std::atomic<size_t> m_active_tasks;

    // counter for busy threads
    std::atomic<size_t> m_busy_threads;
};

#endif // THREAD_POOL_HPP
