// MIT License
// Copyright (c) 2025 dbjwhs

#include <iostream>
#include <vector>
#include <cassert>
#include <coroutine>
#include <string>
#include <format>
#include "../../../headers/project_utils.hpp"

// coroutine pattern - detailed history and explanation
// ----------------------------------------------------------
// coroutines are functions that can suspend execution and later resume where they left off.
// they were standardized in c++20 after years of development, with early versions appearing
// in libraries like boost.coroutine and being influenced by similar features in other languages.
// 
// history:
// - first formalized by melvin conway in 1963
// - implemented in languages like simula, modula-2, and later python, c#, and javascript (async/await)
// - proposed for c++ by gor nishanov around 2014 (n4286)
// - evolved through several proposals until final approval for c++20
// 
// how coroutines work in c++20:
// - a coroutine is a function that contains at least one of: co_await, co_yield, or co_return
// - when a coroutine is called, it returns a coroutine handle that represents its state
// - the coroutine state is allocated on the heap (by default) and persists between suspensions
// - the coroutine frame contains local variables, parameters, and promise object
// 
// common usages:
// - asynchronous programming (similar to async/await in other languages)
// - lazy evaluation of sequences (generators)
// - event-driven programming
// - state machines with complex logic
// - cooperative multitasking without explicit threading
// - reactive programming patterns

// our custom generator implementation for returning sequences of values
template<typename GeneratorType>
class generator {
public:
    // the promise_type is required by the coroutine machinery
    struct promise_type {
    private:
        // m_value stores the value to be yielded
        GeneratorType m_value{};
        // m_exception stores any exceptions thrown within the coroutine
        std::exception_ptr m_exception = nullptr;

    public:
        // initial_suspend is called when the coroutine starts
        // suspending immediately allows lazy evaluation
        auto initial_suspend() noexcept {
            return std::suspend_always{};
        }

        // final_suspend is called when the coroutine ends
        // suspending here allows the handle to be destroyed properly
        auto final_suspend() noexcept {
            return std::suspend_always{};
        }

        // unhandled_exception is called if the coroutine throws an exception
        void unhandled_exception() noexcept {
            m_exception = std::current_exception();
        }

        // yield_value is called when the coroutine executes co_yield
        auto yield_value(GeneratorType value) noexcept {
            // store the yielded value
            m_value = std::move(value);
            // suspend the coroutine to return control to the caller
            return std::suspend_always{};
        }

        // return_void is called when the coroutine executes co_return without a value
        void return_void() noexcept {}

        // get_return_object creates the generator object that wraps the coroutine
        generator get_return_object() noexcept {
            return generator{std::coroutine_handle<promise_type>::from_promise(*this)};
        }

        // retrieve_value returns the currently yielded value
        GeneratorType& value() & noexcept {
            return m_value;
        }

        // check if there was an exception and rethrow it
        void check_exception() const {
            if (m_exception) {
                std::rethrow_exception(m_exception);
            }
        }
    };

    // default constructors and assignment operators
    generator() noexcept = default;
    generator(const generator&) = delete;
    generator& operator=(const generator&) = delete;

    // move constructor
    generator(generator&& other) noexcept : m_handle(other.m_handle) {
        other.m_handle = nullptr;
    }

    // move assignment
    generator& operator=(generator&& other) noexcept {
        if (this != &other) {
            if (m_handle) {
                m_handle.destroy();
            }
            m_handle = other.m_handle;
            other.m_handle = nullptr;
        }
        return *this;
    }

    // destructor to clean up the coroutine frame
    ~generator() {
        if (m_handle) {
            m_handle.destroy();
        }
    }

    // advance the generator to the next yielded value
    // returns true if there is a next value, false if done
    bool next() {
        if (m_handle) {
            m_handle.resume();
            if (m_handle.done()) {
                // check for exceptions before reporting done
                m_handle.promise().check_exception();
                return false;
            }
            return true;
        }
        return false;
    }

    // get the current value
    GeneratorType& value() & {
        return m_handle.promise().value();
    }

    // check if the generator is at the end
    [[nodiscard]] bool done() const noexcept {
        return !m_handle || m_handle.done();
    }

private:
    // the coroutine handle manages the state of the coroutine
    std::coroutine_handle<promise_type> m_handle = nullptr;

    // constructor that takes a handle (used by promise_type::get_return_object)
    explicit generator(std::coroutine_handle<promise_type> h) noexcept : m_handle(h) {}
};

// fibonacci generator using coroutines
// returns a lazy sequence of fibonacci numbers up to a specified limit
generator<int> fibonacci(const int limit) {
    // declare and initialize fibonacci sequence variables
    int q = 0;
    int b = 1;

    // yield the first value in the sequence
    co_yield q;

    // continue yielding values until we reach the limit
    while (b <= limit) {
        // yield the next value
        co_yield b;

        // calculate the next fibonacci number
        const int next = q + b;
        q = b;
        b = next;
    }
    
    // no explicit co_return needed - the coroutine ends naturally
}

// asynchronous task simulation using coroutines
// a simple implementation to demonstrate task-based coroutines
template<typename TaskType>
class task {
public:
    // the promise_type for task coroutines
    struct promise_type {
    private:
        // m_value stores the result of the task
        TaskType m_value;
        // m_exception stores any exceptions thrown within the coroutine
        std::exception_ptr m_exception = nullptr;

    public:
        // don't suspend at the beginning - start executing immediately
        static auto initial_suspend() noexcept {
            return std::suspend_never{};
        }

        // suspend at the end to ensure proper cleanup
        static auto final_suspend() noexcept {
            return std::suspend_always{};
        }

        // store any unhandled exceptions
        void unhandled_exception() noexcept {
            m_exception = std::current_exception();
        }

        // create the task object from the promise
        task get_return_object() noexcept {
            return task{std::coroutine_handle<promise_type>::from_promise(*this)};
        }

        // called when the task completes with a value
        auto return_value(TaskType value) noexcept {
            m_value = std::move(value);
            return std::suspend_never{};
        }

        // access the task result
        TaskType& result() & {
            if (m_exception) {
                std::rethrow_exception(m_exception);
            }
            return m_value;
        }
    };

    // constructors and assignment operators
    task() noexcept = default;
    task(const task&) = delete;
    task& operator=(const task&) = delete;

    // move constructor
    task(task&& other) noexcept : m_handle(other.m_handle) {
        other.m_handle = nullptr;
    }

    // move assignment
    task& operator=(task&& other) noexcept {
        if (this != &other) {
            if (m_handle) {
                m_handle.destroy();
            }
            m_handle = other.m_handle;
            other.m_handle = nullptr;
        }
        return *this;
    }

    // destructor
    ~task() {
        if (m_handle) {
            m_handle.destroy();
        }
    }

    // check if the task is done
    [[nodiscard]] bool is_done() const noexcept {
        return !m_handle || m_handle.done();
    }

    // get the result of the task
    TaskType result() & {
        // ensure the coroutine has completed
        if (m_handle && !m_handle.done()) {
            m_handle.resume();
        }
        
        // get the result from the promise
        if (m_handle) {
            return m_handle.promise().result();
        }
        
        throw std::runtime_error("task has no valid result");
    }

private:
    // the coroutine handle
    std::coroutine_handle<promise_type> m_handle = nullptr;

    // constructor from a handle (used by promise_type::get_return_object)
    explicit task(std::coroutine_handle<promise_type> h) noexcept : m_handle(h) {}
};

// a simple awaitable to simulate asynchronous delays
class delay_awaitable {
private:
    // m_ticks represents how many cycles to "wait"
    int m_ticks;

public:
    // constructor
    explicit delay_awaitable(int ticks) : m_ticks(ticks) {}

    // called by the coroutine machinery to check if we should suspend
    [[nodiscard]] bool await_ready() const noexcept {
        // if ticks is zero, we don't need to suspend
        return m_ticks <= 0;
    }

    // called when the coroutine is about to suspend
    void await_suspend(std::coroutine_handle<> handle) const {
        Logger::getInstance().log(LogLevel::INFO, std::format("Simulating delay of {} ticks", m_ticks));
        
        // in a real implementation, this might register with an event loop
        // for this example; we just immediately resume
        handle.resume();
    }

    // called when the coroutine resumes
    static void await_resume() noexcept {
        // nothing to do here in our simple implementation
    }
};

// utility function to create a delay awaitable
auto delay(int ticks) {
    return delay_awaitable(ticks);
}

// an example task that uses the delay awaitable
task<std::string> process_data(int id) {
    Logger::getInstance().log(LogLevel::INFO, std::format("Starting to process data {}", id));
    
    // simulate some asynchronous work
    co_await delay(3);
    
    // return the result
    co_return std::format("Data {} processed", id);
}

// test our coroutine implementations
void test_coroutines() {
    Logger::getInstance().log(LogLevel::INFO, "Starting coroutine tests");
    
    // test fibonacci generator
    {
        Logger::getInstance().log(LogLevel::INFO, "Testing fibonacci generator");
        
        // generate fibonacci numbers up to 100
        auto fib = fibonacci(100);
        
        // expected fibonacci sequence
        std::vector<int> expected = {0, 1, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89};
        
        // verify the sequence
        int idx = 0;
        while (fib.next()) {
            int value = fib.value();
            Logger::getInstance().log(LogLevel::INFO, std::format("Fibonacci[{}] = {}", idx, value));
            
            // assert that the values match our expected sequence
            assert(idx < expected.size() && "Generated more Fibonacci numbers than expected");
            assert(value == expected[idx] && "Fibonacci value doesn't match expected");
            
            idx++;
        }
        
        // verify we got all expected values
        assert(idx == expected.size() && "Did not generate enough Fibonacci numbers");
        Logger::getInstance().log(LogLevel::INFO, "Fibonacci generator test passed");
    }
    
    // test task with co_await
    {
        Logger::getInstance().log(LogLevel::INFO, "Testing asynchronous task");
        
        // create and start the task
        auto task = process_data(42);
        
        // get the result
        std::string result = task.result();
        
        // verify the result
        Logger::getInstance().log(LogLevel::INFO, std::format("Task result: {}", result));
        assert(result == "Data 42 processed" && "Task returned unexpected result");
        
        // verify the task is complete
        assert(task.is_done() && "Task should be done after getting result");
        
        Logger::getInstance().log(LogLevel::INFO, "Asynchronous task test passed");
    }
    
    Logger::getInstance().log(LogLevel::INFO, "All coroutine tests passed");
}

int main() {
    try {
        Logger::getInstance().log(LogLevel::INFO, "Starting C++20 Coroutines example");
        
        // run all our tests
        test_coroutines();
        
        Logger::getInstance().log(LogLevel::INFO, "C++20 Coroutines example completed successfully");
        return 0;
    } catch (const std::exception& e) {
        Logger::getInstance().log(LogLevel::ERROR, std::format("Exception caught: {}", e.what()));
        return 1;
    }
}
