# C++20 Features TODO List

This document outlines missing C++20 features that would be valuable additions to the repository. C++20 is a major release with approximately 80 library features and 70 language changes, making it one of the most substantial updates to C++ in recent years.

## 📊 **Current C++20 Features (Implemented)**

✅ **concepts** - Template constraints and concept definitions  
✅ **coroutines** - Stackless coroutines with co_await, co_yield, co_return  
✅ **counting-semaphore** - std::counting_semaphore for resource management  
✅ **designated-initializers** - C99-style designated initialization syntax  
✅ **modules** - Module system for improved compilation and encapsulation  
✅ **ranges** - Ranges library with views and algorithms  
✅ **spaceship** - Three-way comparison operator (<=>) and defaulted comparisons  
✅ **test-macros** - Testing utilities and macro improvements  

---

## 🚀 **High Priority - The Big Four Completions**

C++20's "Big Four" major features are: **Concepts**, **Ranges**, **Coroutines**, and **Modules**. While you have basic implementations, these can be significantly expanded.

### **1. Advanced Concepts** (`concepts-advanced`)
- **Feature:** Advanced concept patterns, requires clauses, concept specialization
- **Value:** Deep dive into template metaprogramming and constraint design
- **Implementation:** Concept composition, SFINAE replacement, type trait concepts
- **Example:**
  ```cpp
  template<typename T>
  concept Serializable = requires(T t) {
      { t.serialize() } -> std::same_as<std::string>;
      { T::deserialize(std::string{}) } -> std::same_as<T>;
  };
  ```

### **2. Advanced Ranges** (`ranges-advanced`)
- **Feature:** Custom ranges, range adaptors, view composition, range-based algorithms
- **Value:** Functional programming patterns and lazy evaluation
- **Implementation:** Custom views, range pipelines, infinite ranges
- **Example:**
  ```cpp
  auto result = data 
      | std::views::filter(is_valid)
      | std::views::transform(process)
      | std::views::take(10);
  ```

### **3. Advanced Coroutines** (`coroutines-advanced`)
- **Feature:** Custom coroutine types, async/await patterns, generators
- **Value:** Asynchronous programming and generator patterns
- **Implementation:** Task systems, async I/O, coroutine schedulers
- **Example:**
  ```cpp
  Task<int> async_computation() {
      auto result = co_await fetch_data();
      co_return process(result);
  }
  ```

### **4. Advanced Modules** (`modules-advanced`)
- **Feature:** Module interfaces, module partitions, header units
- **Value:** Modern build systems and compilation improvements
- **Implementation:** Complex module hierarchies, private module fragments
- **Example:**
  ```cpp
  export module math.geometry;
  export import math.basic;
  export namespace geometry { /* ... */ }
  ```

---

## 📚 **High Priority - Core Language Features**

### **5. consteval Functions** (`consteval`)
- **Feature:** Immediate functions that must be evaluated at compile-time
- **Value:** Stronger compile-time guarantees than constexpr
- **Implementation:** Compile-time validation, metaprogramming utilities
- **Example:**
  ```cpp
  consteval int compile_time_factorial(int n) {
      return n <= 1 ? 1 : n * compile_time_factorial(n - 1);
  }
  ```

### **6. constinit Variables** (`constinit`)
- **Feature:** Guaranteed constant initialization for static/thread_local variables
- **Value:** Eliminates static initialization order fiasco
- **Implementation:** Global variable initialization, thread-local storage
- **Example:**
  ```cpp
  constinit thread_local int counter = calculate_initial_value();
  ```

### **7. char8_t and UTF-8** (`char8-utf8`)
- **Feature:** Dedicated char8_t type for UTF-8 strings
- **Value:** Type-safe UTF-8 string handling and Unicode support
- **Implementation:** UTF-8 string processing, encoding conversions
- **Example:**
  ```cpp
  char8_t utf8_string[] = u8"Hello 🌍";
  std::u8string text = u8"Unicode text";
  ```

### **8. [[likely]] / [[unlikely]] Attributes** (`likely-unlikely`)
- **Feature:** Branch prediction hints for optimization
- **Value:** Performance optimization and compiler hints
- **Implementation:** Hot path optimization, performance-critical code
- **Example:**
  ```cpp
  if (common_condition) [[likely]] {
      // Frequently taken path
  } else [[unlikely]] {
      // Rarely taken path
  }
  ```

---

## 🛠️ **High Priority - Standard Library Essentials**

### **9. std::span** (`span`)
- **Feature:** Non-owning view over contiguous memory
- **Value:** Safe array/vector access without ownership
- **Implementation:** Buffer processing, C-style array wrapping, subviews
- **Example:**
  ```cpp
  void process_data(std::span<const int> data) {
      for (auto value : data.subspan(1, 5)) { /* ... */ }
  }
  ```

### **10. std::format** (`format`)
- **Feature:** Type-safe string formatting library
- **Value:** Modern alternative to printf with type safety
- **Implementation:** Custom formatters, locale support, performance comparisons
- **Example:**
  ```cpp
  auto text = std::format("Value: {:.2f}, Count: {}", 3.14159, 42);
  std::println("{:%Y-%m-%d}", std::chrono::system_clock::now());
  ```

### **11. std::jthread** (`jthread`)
- **Feature:** Automatically joining thread with cancellation support
- **Value:** Safer threading with RAII and cooperative cancellation
- **Implementation:** Thread pools, cancellation tokens, safe threading patterns
- **Example:**
  ```cpp
  std::jthread worker([](std::stop_token token) {
      while (!token.stop_requested()) { /* work */ }
  });
  // Automatically joins on destruction
  ```

### **12. std::source_location** (`source-location`)
- **Feature:** Capture source code location information
- **Value:** Better debugging, logging, and assertion messages
- **Implementation:** Debug utilities, assertion macros, error reporting
- **Example:**
  ```cpp
  void log_error(const std::string& msg, 
                 std::source_location loc = std::source_location::current()) {
      std::println("Error at {}:{}: {}", loc.file_name(), loc.line(), msg);
  }
  ```

---

## 🔢 **Medium Priority - Utility and Container Features**

### **13. Mathematical Constants** (`math-constants`)
- **Feature:** std::numbers namespace with π, e, and other constants
- **Value:** Precise mathematical constants and scientific computing
- **Implementation:** Mathematical algorithms, scientific calculations
- **Example:**
  ```cpp
  #include <numbers>
  auto area = std::numbers::pi * radius * radius;
  auto exp_value = std::numbers::e;
  ```

### **14. Bit Operations** (`bit-operations`)
- **Feature:** std::bit_cast, std::popcount, std::rotl, std::rotr, etc.
- **Value:** Low-level bit manipulation and type punning
- **Implementation:** Binary algorithms, cryptography, bit fields
- **Example:**
  ```cpp
  auto bits_set = std::popcount(0b11010110u);  // 5
  auto rotated = std::rotl(0b10010011u, 2);
  ```

### **15. Calendar and Timezone** (`calendar-timezone`)
- **Feature:** Extended chrono library with calendar and timezone support
- **Value:** Date/time manipulation and timezone-aware programming
- **Implementation:** Date calculations, timezone conversions, scheduling
- **Example:**
  ```cpp
  using namespace std::chrono;
  auto today = year_month_day{floor<days>(system_clock::now())};
  auto christmas = year{2024}/December/25;
  ```

### **16. std::atomic_ref** (`atomic-ref`)
- **Feature:** Atomic operations on non-atomic objects
- **Value:** Thread-safe operations on existing data structures
- **Implementation:** Lock-free algorithms, concurrent data structures
- **Example:**
  ```cpp
  int value = 42;
  std::atomic_ref atomic_value(value);
  atomic_value.fetch_add(10);  // Thread-safe increment
  ```

### **17. Container Improvements** (`container-improvements`)
- **Feature:** std::erase/std::erase_if, uniform container erasure
- **Value:** Simplified element removal across all containers
- **Implementation:** Generic algorithms, container utilities
- **Example:**
  ```cpp
  std::vector<int> vec{1, 2, 3, 4, 5};
  std::erase_if(vec, [](int x) { return x % 2 == 0; });
  ```

---

## 🧵 **Medium Priority - Concurrency Features**

### **18. Synchronization Primitives** (`sync-primitives`)
- **Feature:** std::latch, std::barrier, std::stop_token, std::stop_source
- **Value:** Advanced synchronization patterns and thread coordination
- **Implementation:** Producer-consumer patterns, thread pools, task systems
- **Example:**
  ```cpp
  std::latch completion_latch(worker_count);
  std::barrier sync_point(thread_count, []() { std::println("Phase complete"); });
  ```

### **19. Atomic Smart Pointers** (`atomic-smart-pointers`)
- **Feature:** std::atomic<std::shared_ptr<T>>, std::atomic<std::weak_ptr<T>>
- **Value:** Thread-safe smart pointer operations
- **Implementation:** Lock-free data structures, concurrent programming
- **Example:**
  ```cpp
  std::atomic<std::shared_ptr<Data>> atomic_ptr;
  auto data = std::make_shared<Data>();
  atomic_ptr.store(data);
  ```

### **20. Safe Integer Comparison** (`safe-integer-comparison`)
- **Feature:** std::cmp_equal, std::cmp_less, std::cmp_greater for safe comparisons
- **Value:** Eliminates signed/unsigned comparison issues
- **Implementation:** Generic algorithms, safe numeric programming
- **Example:**
  ```cpp
  if (std::cmp_less(signed_value, unsigned_value)) {
      // Safe comparison without warnings
  }
  ```

---

## 🔧 **Lower Priority - Specialized Features**

### **21. std::midpoint and std::lerp** (`midpoint-lerp`)
- **Feature:** Safe midpoint calculation and linear interpolation
- **Value:** Numeric algorithms without overflow
- **Implementation:** Graphics programming, numerical methods
- **Example:**
  ```cpp
  auto mid = std::midpoint(INT_MAX, INT_MAX - 1);  // No overflow
  auto interpolated = std::lerp(start, end, 0.5);
  ```

### **22. Feature Test Macros** (`feature-test-macros`)
- **Feature:** Standardized feature detection macros
- **Value:** Portable feature detection and conditional compilation
- **Implementation:** Cross-platform compatibility, feature detection
- **Example:**
  ```cpp
  #if __cpp_concepts >= 201907L
      // Use concepts
  #else
      // Fallback implementation
  #endif
  ```

### **23. std::identity and std::type_identity** (`identity-utilities`)
- **Feature:** Identity function object and type identity utility
- **Value:** Perfect forwarding and template metaprogramming
- **Implementation:** Generic programming utilities, SFINAE helpers
- **Example:**
  ```cpp
  std::ranges::sort(vec, {}, std::identity{});  // Sort by identity
  ```

### **24. Constexpr Improvements** (`constexpr-improvements`)
- **Feature:** constexpr dynamic_cast, virtual functions, try/catch, unions
- **Value:** Expanded compile-time programming capabilities
- **Implementation:** Advanced metaprogramming, compile-time polymorphism
- **Example:**
  ```cpp
  constexpr auto result = []() {
      try { return risky_calculation(); }
      catch (...) { return default_value(); }
  }();
  ```

---

## 📝 **Recommended Implementation Order**

### **Phase 1: Essential Library Features**
1. **std::span** - Fundamental for safe memory access
2. **std::format** - Modern string formatting
3. **std::jthread** - Improved threading
4. **std::source_location** - Better debugging
5. **consteval** - Stronger compile-time guarantees

### **Phase 2: Advanced Core Features**
6. **concepts-advanced** - Advanced template constraints
7. **ranges-advanced** - Functional programming patterns
8. **bit-operations** - Low-level bit manipulation
9. **math-constants** - Mathematical programming
10. **container-improvements** - Generic algorithms

### **Phase 3: Concurrency and Safety**
11. **sync-primitives** - Advanced synchronization
12. **atomic-ref** - Lock-free programming
13. **safe-integer-comparison** - Numeric safety
14. **char8-utf8** - Unicode and text processing
15. **calendar-timezone** - Date/time programming

### **Phase 4: Advanced and Specialized**
16. **coroutines-advanced** - Async programming patterns
17. **modules-advanced** - Modern build systems
18. **atomic-smart-pointers** - Concurrent data structures
19. **likely-unlikely** - Performance optimization
20. **constexpr-improvements** - Advanced metaprogramming

---

## 🎯 **Implementation Guidelines**

Each new feature should include:

- **Comprehensive examples** demonstrating practical usage
- **Performance comparisons** with older alternatives where applicable
- **Best practices** and common pitfalls
- **Integration** with existing utilities and error handling
- **Cross-platform considerations** and compiler support notes
- **CMakeLists.txt** with appropriate C++20 compiler flags
- **Detailed README.md** with feature explanation and use cases

## 📚 **References**

- [C++20 - cppreference.com](https://en.cppreference.com/w/cpp/20)
- [All C++20 core language features with examples](https://oleksandrkvl.github.io/2021/04/02/cpp-20-overview.html)
- [20 Smaller yet Handy C++20 Features](https://www.cppstories.com/2022/20-smaller-cpp20-features/)
- [C++20: The Big Four](https://www.modernescpp.com/index.php/thebigfour/)
- [C++20 Wikipedia](https://en.wikipedia.org/wiki/C++20)
- [Programming with C++20](https://andreasfertig.com/books/programming-with-cpp20/)

---

*This todo list emphasizes the most impactful C++20 features for educational and practical purposes. C++20 represents a paradigm shift in C++ programming with its major features (Concepts, Ranges, Coroutines, Modules) and numerous quality-of-life improvements.*