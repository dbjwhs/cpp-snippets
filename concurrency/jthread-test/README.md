# C++20 `std::jthread` on macOS

Time of writing May 18th, 2025.

This README explains why `std::jthread` is not supported on macOS/Apple Clang and provides workarounds for those who need this functionality.

## The Problem

When trying to use `std::jthread` on macOS with Apple Clang, you might encounter compilation errors like this:

```
error: no member named 'jthread' in namespace 'std'
std::jthread thread{[] { /* ... */ }};
     ^
```

This happens because `std::jthread`, despite being part of the C++20 standard, is not yet fully supported in Apple's Clang implementation.

## Why `jthread` Is Not Supported on macOS/Apple Clang

### 1. Experimental Status in libc++

The C++20 standard introduced `std::jthread` (P0660) as part of the standard library. However, in the libc++ C++20 implementation, this feature is currently marked as experimental. As noted in the libc++ documentation: "P0660: The paper is implemented but the features are experimental and can be enabled via `-fexperimental-library`".

The implementation is guarded behind this conditional compilation block:

```cpp
#if _LIBCPP_STD_VER >= 20 && !defined(_LIBCPP_HAS_NO_EXPERIMENTAL_STOP_TOKEN)
// jthread implementation
#endif
```

### 2. Apple Clang vs. Upstream LLVM Clang

Apple maintains its own fork of Clang which often lags behind the upstream LLVM Clang in terms of feature implementation. While `jthread` may be available as an experimental feature in newer versions of LLVM Clang, Apple Clang typically includes fewer C++20 features, especially newer ones.

### 3. Missing Experimental Library Support

The implementation depends on a separate library called `libc++experimental.a` which contains experimental features. On macOS, either:
- This library is missing entirely, or
- It's only available for specific architectures (like arm64e but not arm64)

### 4. No Flag Support in Apple Clang

While in upstream LLVM Clang (18+), you can enable `jthread` with:

```bash
clang++ -std=c++20 -stdlib=libc++ -fexperimental-library your_file.cpp
```

This `-fexperimental-library` flag is not supported in Apple Clang, which is the default compiler on macOS.

## How to Verify the Issue

You can confirm this issue by compiling the following test program:

```cpp
#include <iostream>

int main() {
    std::cout << "C++ Standard: " << __cplusplus << "\n";

#if defined(__cpp_lib_jthread)
    std::cout << "jthread is supported (version " << __cpp_lib_jthread << ")\n";
#else
    std::cout << "jthread is not supported\n";
#endif

    return 0;
}
```

Compile with:
```bash
clang++ -std=c++20 main.cpp -o jthread_test
```

## Solutions and Workarounds

There are several approaches to work around this limitation:

### 1. Use LLVM Clang with Homebrew

Install and use LLVM Clang instead of Apple Clang:

```bash
brew install llvm
/usr/local/opt/llvm/bin/clang++ -std=c++20 -stdlib=libc++ -fexperimental-library main.cpp -o jthread_test
```

Note that this approach may require additional configuration to ensure proper linking with experimental libraries.

### 2. Use a Third-Party Implementation

Consider using a library like [jthread-lite](https://github.com/martinmoene/jthread-lite) which provides a C++20 `jthread` implementation for C++11 and later:

```bash
git clone https://github.com/martinmoene/jthread-lite.git
```

Then include their header in your project.

### 3. Create a Custom `joining_thread` Implementation

You can implement your own version of `jthread` that provides the auto-joining functionality (though without the cancellation features):

```cpp
#include <thread>

class joining_thread {
private:
    std::thread thread_;

public:
    // Default constructor
    joining_thread() noexcept = default;
    
    // Variadic template constructor
    template<typename Function, typename... Args>
    explicit joining_thread(Function&& f, Args&&... args) 
        : thread_(std::forward<Function>(f), std::forward<Args>(args)...) {}
    
    // Move constructor
    joining_thread(joining_thread&& other) noexcept 
        : thread_(std::move(other.thread_)) {}
    
    // Move assignment
    joining_thread& operator=(joining_thread&& other) noexcept {
        if (joinable())
            join();
        thread_ = std::move(other.thread_);
        return *this;
    }
    
    // Destructor - automatically joins if joinable
    ~joining_thread() {
        if (joinable())
            join();
    }
    
    // Delete copy operations
    joining_thread(const joining_thread&) = delete;
    joining_thread& operator=(const joining_thread&) = delete;
    
    // Thread operations
    bool joinable() const noexcept { return thread_.joinable(); }
    void join() { thread_.join(); }
    void detach() { thread_.detach(); }
    std::thread::id get_id() const noexcept { return thread_.get_id(); }
    std::thread::native_handle_type native_handle() { return thread_.native_handle(); }
    
    // Static helper
    static unsigned hardware_concurrency() noexcept { 
        return std::thread::hardware_concurrency(); 
    }
};
```

This implementation provides the auto-joining feature of `std::jthread` without the cancellation functionality that depends on `std::stop_token` and related components.

### 4. Simple RAII Wrapper

For an even simpler approach, you can use a basic RAII wrapper:

```cpp
#include <thread>

class thread_guard {
    std::thread& t;
public:
    explicit thread_guard(std::thread& t_) : t(t_) {}
    ~thread_guard() {
        if (t.joinable()) {
            t.join();
        }
    }
    // Delete copy operations
    thread_guard(const thread_guard&) = delete;
    thread_guard& operator=(const thread_guard&) = delete;
};

// Usage:
// std::thread t{[] { /* work */ }};
// thread_guard g{t};
```

## What Features Are You Missing?

The primary features of `std::jthread` that you might be missing without proper support:

1. **Automatic Joining**: `std::jthread` automatically joins on destruction, which you can replicate with the solutions above.

2. **Cancellation Support**: `std::jthread` works with `std::stop_token`, `std::stop_source`, and `std::stop_callback` to provide a cooperative cancellation mechanism, which is harder to replicate manually.

## Future Outlook

As Apple updates their Clang implementation, `std::jthread` support may eventually be included. Until then, the workarounds outlined above should provide viable alternatives.

## References

- [C++20 Status in libc++](https://libcxx.llvm.org/Status/Cxx20.html)
- [std::jthread Documentation (cppreference)](https://en.cppreference.com/w/cpp/thread/jthread)
- [jthread-lite GitHub Repository](https://github.com/martinmoene/jthread-lite)
- [Paper P0660R10: Stop Token and Joining Thread](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2019/p0660r10.pdf)
