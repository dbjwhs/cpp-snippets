# C++20 Coroutines

Coroutines represent one of the most significant additions to C++20, providing a standardized mechanism for cooperative task execution.
Originally formalized by Melvin Conway in 1963, coroutines are functions that can suspend their execution and later resume from the
same point, preserving their state between invocations. They evolved through various programming languages including Simula, Modula-2,
Python, C#, and JavaScript (as async/await) before their standardization in C++20. The C++ implementation began with proposals by Gor
Nishanov around 2014 (N4286) and underwent several revisions before final approval. C++20 coroutines employ a unique approach where
the compiler transforms coroutine functions (containing co_await, co_yield, or co_return) into state machines with heap-allocated
frames, allowing for powerful and flexible concurrency paradigms without explicit thread management.

Coroutines solve numerous programming challenges that traditional functions cannot elegantly address. They excel at expressing
asynchronous operations without callback hell, simplifying code that would otherwise require complex state machines. Generators
implemented with coroutines enable lazy evaluation of potentially infinite sequences, producing values on-demand while maintaining
minimal memory usage. Event-driven programming becomes more intuitive with coroutines, as they naturally model the "wait for event,
process, repeat" pattern. They're particularly valuable in GUI applications, network programming, and game development where they
facilitate cooperative multitasking without the overhead of thread creation and management. By decoupling logical and temporal flow,
coroutines make traditionally complex patterns like asynchronous loops and conditional execution straightforward, resulting in more
maintainable code with better separation of concerns.

## Basic Example

```cpp
#include <coroutine>
#include <iostream>

// A simple generator that yields integers
generator<int> sequence(int start, int end) {
    for (int i = start; i <= end; ++i) {
        co_yield i;  // Suspend and return the current value
    }
}

int main() {
    // Use the generator
    for (auto gen = sequence(1, 5); gen.next();) {
        std::cout << gen.value() << " ";  // Outputs: 1 2 3 4 5
    }
}
```

## Common Usage Patterns

### 1. Asynchronous Operations

```cpp
task<std::string> fetch_data(std::string url) {
    auto connection = co_await connect(url);
    auto data = co_await connection.get_data();
    co_return process(data);
}
```

### 2. Event Handling

```cpp
task<void> ui_interaction() {
    while (true) {
        auto event = co_await event_queue.next_event();
        switch (event.type) {
            case click: handle_click(event); break;
            case key_press: handle_key(event); break;
        }
    }
}
```

### 3. Lazy Evaluation

```cpp
generator<int> primes() {
    int n = 2;
    while (true) {
        if (is_prime(n)) {
            co_yield n;
        }
        ++n;
    }
}
```

## Best Practices

- **Use Symmetric Transfer**: When chaining coroutines, use symmetric transfer (`std::experimental::noop_coroutine()`) to avoid stack overflow.
- **Memory Management**: Be aware of the heap allocation for the coroutine frame; use custom allocators when needed.
- **Error Handling**: Propagate exceptions properly through the promise type's `unhandled_exception()` method.
- **Avoid Blocking**: Never perform blocking operations in coroutines; always use awaitable equivalents.
- **Performance Considerations**: Create coroutines judiciously, as each creation involves heap allocation.

## Common Pitfalls

- **Dangling Coroutines**: A coroutine must be properly destroyed when no longer needed.
- **Lifetime Issues**: The lifetime of objects referenced by a coroutine must exceed the coroutine's lifetime.
- **Mixing Concurrency Models**: Be careful when using coroutines with threads or other concurrency mechanisms.
- **Debug Complexity**: Debugging suspended coroutines can be challenging; add proper logging at suspension points.
- **Compiler Support Variations**: Different compilers may have varying levels of coroutine optimizations.

## Recommended Books and Resources

- "C++20: The Complete Guide" by Nicolai M. Josuttis
- "C++ Coroutines: A Practical Guide" by Andreas Fertig
- "Effective Modern C++" by Scott Meyers (for general C++ best practices)
- CppCoro library by Lewis Baker: A library of coroutine abstractions for C++20
- Cppcoro GitHub repository: https://github.com/lewissbaker/cppcoro
- C++ Reference coroutine documentation: https://en.cppreference.com/w/cpp/language/coroutines

## Implementation Details

The implementation of coroutines in C++20 involves three main components:

1. **Coroutine Handle**: A non-owning pointer-like object that allows resuming a suspended coroutine
2. **Promise Object**: Controls the behavior of the coroutine at different points (initialization, yielding, returning, etc.)
3. **Awaitable Objects**: Types that can be used with `co_await` to suspend the coroutine

### Detailed Generator Implementation

Let's explore a complete implementation of a prime number generator using C++20 coroutines:

```cpp
#include <iostream>
#include <vector>
#include <coroutine>
#include <cassert>
#include <cmath>

// Complete generator implementation
template<typename T>
class generator {
public:
    // Required promise_type for the coroutine machinery
    struct promise_type {
    private:
        // The current value to be yielded
        T m_value;
        // Storage for any exception thrown in the coroutine
        std::exception_ptr m_exception = nullptr;

    public:
        // Called when the coroutine starts - we want lazy execution, so suspend immediately
        auto initial_suspend() noexcept {
            return std::suspend_always{};
        }

        // Called when the coroutine reaches its end - suspend to allow proper destruction
        auto final_suspend() noexcept {
            return std::suspend_always{};
        }

        // Called if an exception is thrown within the coroutine
        void unhandled_exception() noexcept {
            m_exception = std::current_exception();
        }

        // Called when co_yield is used - stores the value and suspends execution
        auto yield_value(T value) noexcept {
            m_value = std::move(value);
            return std::suspend_always{};
        }

        // Called for co_return without a value
        void return_void() noexcept {}

        // Creates the generator object that wraps the coroutine
        generator get_return_object() noexcept {
            return generator{std::coroutine_handle<promise_type>::from_promise(*this)};
        }

        // Access the current value
        T& value() & noexcept {
            return m_value;
        }

        // Check and rethrow any exceptions
        void check_exception() {
            if (m_exception) {
                std::rethrow_exception(m_exception);
            }
        }
    };

    // Default constructor for an empty generator
    generator() noexcept = default;
    
    // No copying allowed (would create dangling handles)
    generator(const generator&) = delete;
    generator& operator=(const generator&) = delete;

    // Move constructor - transfers ownership of the coroutine handle
    generator(generator&& other) noexcept : m_handle(other.m_handle) {
        other.m_handle = nullptr;
    }

    // Move assignment - transfers ownership after destroying any existing handle
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

    // Destructor - ensures the coroutine frame is properly destroyed
    ~generator() {
        if (m_handle) {
            m_handle.destroy();
        }
    }

    // Advances the generator to the next value
    // Returns true if there is a next value, false if done
    bool next() {
        if (m_handle) {
            m_handle.resume();
            if (m_handle.done()) {
                // Check for any exceptions before reporting done
                m_handle.promise().check_exception();
                return false;
            }
            return true;
        }
        return false;
    }

    // Access the current value
    T& value() & {
        return m_handle.promise().value();
    }

    // Check if the generator is done
    bool done() const noexcept {
        return !m_handle || m_handle.done();
    }

private:
    // The coroutine handle that manages the coroutine state
    std::coroutine_handle<promise_type> m_handle = nullptr;

    // Constructor that takes a handle (used by promise_type::get_return_object)
    explicit generator(std::coroutine_handle<promise_type> h) noexcept : m_handle(h) {}
};

// Utility function to check if a number is prime
bool is_prime(int n) {
    if (n <= 1) return false;
    if (n <= 3) return true;
    if (n % 2 == 0 || n % 3 == 0) return false;
    
    for (int i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0) return false;
    }
    return true;
}

// Prime number generator using coroutines
// Yields prime numbers up to a specified limit
generator<int> primes_up_to(int limit) {
    // Special case for 2, the only even prime
    if (limit >= 2) {
        co_yield 2;
    }
    
    // Check odd numbers starting from 3
    for (int n = 3; n <= limit; n += 2) {
        if (is_prime(n)) {
            // Suspend execution and yield the prime number
            co_yield n;
        }
    }
    // Coroutine will automatically end when we reach the limit
}

// Example usage function
void demonstrate_prime_generator() {
    const int LIMIT = 50;
    std::cout << "Prime numbers up to " << LIMIT << ":\n";
    
    // Create and use the prime number generator
    auto prime_gen = primes_up_to(LIMIT);
    
    // Collect primes for verification
    std::vector<int> found_primes;
    
    // Iterate through the generator
    while (prime_gen.next()) {
        int prime = prime_gen.value();
        found_primes.push_back(prime);
        std::cout << prime << " ";
    }
    std::cout << "\n";
    
    // Verify our generator works correctly
    std::vector<int> expected_primes = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47};
    assert(found_primes == expected_primes && "Prime generator produced incorrect sequence");
    
    std::cout << "Prime generator verified successfully!\n";
}
```

### How It Works

1. **Compiler Transformation**: When the compiler encounters a function containing `co_yield`, `co_await`, or `co_return`, it transforms it into a state machine:
    - It creates a coroutine frame on the heap that stores local variables, parameters, and the promise object
    - It splits the function at each suspension point (co_yield/co_await)
    - It creates a finite state machine where each state represents the code between suspension points

2. **Execution Flow**: When `primes_up_to(50)` is called:
    - The coroutine frame is allocated
    - The promise object is created inside the frame
    - `get_return_object()` creates and returns the generator
    - `initial_suspend()` is called, which returns `std::suspend_always`, so the coroutine suspends immediately
    - The generator is returned to the caller without executing the coroutine body yet

3. **Resuming Execution**: When `prime_gen.next()` is called:
    - The coroutine resumes execution from where it was suspended
    - When `co_yield n` is executed:
        - The value `n` is passed to `yield_value(n)`
        - The value is stored in the promise
        - The coroutine suspends again, returning control to the caller
    - The caller can access the yielded value with `prime_gen.value()`

4. **Coroutine Lifetime**: The generator maintains ownership of the coroutine frame via its handle:
    - When the generator is destroyed, it calls `m_handle.destroy()` to free the coroutine frame
    - If the coroutine completes naturally, `final_suspend()` is called, which keeps it suspended so the handle can properly destroy it

This implementation showcases the power of coroutines for creating efficient, lazy-evaluated sequences without complex iterator classes or manual state tracking.

## License
This code is provided under the MIT License. Feel free to use, modify, and distribute as needed.

## Contributing
Contributions are welcome! Please feel free to submit a Pull Request.
