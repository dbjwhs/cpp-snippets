# C++23 Features TODO List

This document outlines missing C++23 features that would be valuable additions to the repository. Features are prioritized based on educational value, practical utility, and demonstration of modern C++ capabilities.

## 📊 **Current C++23 Features (Implemented)**

✅ **alias-labels** - Named universal character escapes and related features  
✅ **byteswap** - std::byteswap functionality for byte order operations  
✅ **expected** - std::expected vocabulary type for error handling  
✅ **if-consteval** - if consteval language feature for compile-time branching  
✅ **mdspan** - std::mdspan multidimensional array reference  
✅ **multi-subscript** - Multidimensional subscript operator `arr[1, 2]`  
✅ **new-preprocessor** - New preprocessor directives (#elifdef, #elifndef, #warning)  
✅ **print-println** - std::print and std::println formatted output  
✅ **this** - Deducing this/explicit object parameters  
✅ **turing-machines** - Complex constexpr example (educational)

---

## 🚀 **High Priority - Core Language Features**

### **1. std::size_t Literals** (`size-t-literals`)
- **Feature:** Literal suffix 'z'/'Z' for std::size_t literals
- **Value:** Eliminates signed/unsigned mismatch warnings in loops and containers
- **Implementation:** Demonstrate usage in loops, container operations, and numeric code
- **Example:**
  ```cpp
  auto size = 42z;        // std::size_t literal
  for (auto i = 0z; i < container.size(); ++i) { /* no warnings */ }
  ```

### **2. [[assume]] Attribute** (`assume-attribute`)
- **Feature:** Compiler optimization hints with `[[assume(expression)]]`
- **Value:** Demonstrates compiler optimization techniques and undefined behavior
- **Implementation:** Show performance improvements and optimization examples
- **Example:**
  ```cpp
  void process(int* ptr) {
      [[assume(ptr != nullptr)]];  // Compiler can optimize
      *ptr = calculate();
  }
  ```

### **3. constexpr Improvements** (`constexpr-static-thread-local`)
- **Feature:** Static and thread_local variables in constexpr functions
- **Value:** Advanced constexpr programming patterns and compile-time state
- **Implementation:** Demonstrate constexpr functions with static counters/caches
- **Example:**
  ```cpp
  constexpr int counter() {
      static int count = 0;  // Now allowed in C++23
      return ++count;
  }
  ```

### **4. UTF-8 String Literals** (`utf8-string-literals`)
- **Feature:** UTF-8 string literal improvements and char array initialization
- **Value:** Modern text processing, internationalization, and Unicode handling
- **Implementation:** Show UTF-8 text processing, emoji handling, and encoding
- **Example:**
  ```cpp
  char greeting[] = u8"Hello 🌍";  // Direct UTF-8 initialization
  ```

---

## 📚 **High Priority - Standard Library**

### **5. std::stacktrace** (`stacktrace`)
- **Feature:** Stack trace library for debugging and error reporting
- **Value:** Essential debugging tool, exception handling, and diagnostics
- **Implementation:** Debug utilities, exception traces, and call stack analysis
- **Example:**
  ```cpp
  #include <stacktrace>
  void debug_function() {
      auto trace = std::stacktrace::current();
      std::println("{}", trace);
  }
  ```

### **6. std::generator** (`generator`)
- **Feature:** Synchronous coroutine generator for ranges
- **Value:** Modern coroutine-based iteration patterns and lazy evaluation
- **Implementation:** Generator functions, infinite sequences, and range pipelines
- **Example:**
  ```cpp
  std::generator<int> fibonacci() {
      int a = 0, b = 1;
      while (true) {
          co_yield a;
          std::tie(a, b) = std::make_pair(b, a + b);
      }
  }
  ```

### **7. Flat Containers** (`flat-containers`)
- **Feature:** std::flat_set, std::flat_map, std::flat_multiset, std::flat_multimap
- **Value:** Performance-optimized container alternatives with better cache locality
- **Implementation:** Performance comparisons, cache-friendly algorithms
- **Example:**
  ```cpp
  std::flat_map<int, std::string> cache_friendly_map;
  // Better performance for many use cases
  ```

### **8. std::move_only_function** (`move-only-function`)
- **Feature:** Move-only function wrapper
- **Value:** Performance optimization for function objects with move-only capture
- **Implementation:** Unique resource management, move semantics examples
- **Example:**
  ```cpp
  std::move_only_function<void()> f = [ptr = std::make_unique<int>(42)]() {
      std::println("Value: {}", *ptr);
  };
  ```

### **9. std::unreachable** (`unreachable`)
- **Feature:** Mark unreachable code paths for optimization
- **Value:** Compiler optimization hints and defensive programming
- **Implementation:** Switch statements, error handling, optimization examples
- **Example:**
  ```cpp
  switch (enum_value) {
      case A: return handle_a();
      case B: return handle_b();
      default: std::unreachable();  // Optimization hint
  }
  ```

---

## 📊 **Medium Priority - Container & Utility Enhancements**

### **10. std::basic_string::contains** (`string-contains`)
- **Feature:** Contains member function for strings and string_view
- **Value:** Convenient string searching without iterator complexity
- **Implementation:** Text processing, parsing, and search algorithms
- **Example:**
  ```cpp
  if (text.contains("substring")) { /* found */ }
  if (text.contains('x')) { /* character found */ }
  ```

### **11. std::optional Monadic Operations** (`optional-monadic`)
- **Feature:** transform, or_else, and_then for std::optional
- **Value:** Functional programming patterns and error handling chains
- **Implementation:** Option chaining, functional pipelines, error propagation
- **Example:**
  ```cpp
  auto result = parse_int(str)
      .and_then([](int x) { return safe_divide(100, x); })
      .transform([](double d) { return std::to_string(d); })
      .or_else([]() { return std::optional<std::string>{"error"}; });
  ```

### **12. std::spanstream** (`spanstream`)
- **Feature:** std::span-based string streams for zero-copy operations
- **Value:** Efficient stream operations without memory allocation
- **Implementation:** Zero-copy parsing, efficient text processing
- **Example:**
  ```cpp
  std::span<char> buffer = get_buffer();
  std::ospanstream stream{buffer};
  stream << "Hello " << 42;
  ```

### **13. Extended Floating-Point Types** (`extended-float-types`)
- **Feature:** std::float16_t, std::float32_t, std::float64_t, std::bfloat16_t
- **Value:** Specialized numeric computations for ML, graphics, and scientific computing
- **Implementation:** Precision comparisons, numeric algorithms, type conversions
- **Example:**
  ```cpp
  #include <stdfloat>
  std::float16_t half_precision = 3.14f16;
  std::bfloat16_t brain_float = 2.71bf16;
  ```

---

## 🔧 **Lower Priority - Specialized Features**

### **14. Named Universal Character Escapes** (`named-unicode-escapes`)
- **Feature:** \N{CHARACTER NAME} syntax for Unicode characters
- **Value:** Better Unicode character handling and code readability
- **Implementation:** Unicode text processing, internationalization examples
- **Example:**
  ```cpp
  auto smile = "\N{SMILING FACE}";  // 😊
  auto cat = "\N{CAT FACE}";        // 🐱
  ```

### **15. Delimited Escape Sequences** (`delimited-escapes`)
- **Feature:** \o{}, \x{}, \u{} delimited escape syntax
- **Value:** Clearer escape sequence syntax and reduced ambiguity
- **Implementation:** String processing, binary data handling
- **Example:**
  ```cpp
  auto hex = "\x{C0DE}";    // vs "\xC0DE"
  auto octal = "\o{777}";   // vs "\777"
  ```

### **16. import std; Module** (`import-std`)
- **Feature:** Direct standard library import
- **Value:** Modern module system demonstration and build time improvements
- **Implementation:** Module usage examples, build system integration
- **Example:**
  ```cpp
  import std;  // Instead of individual #include statements
  int main() {
      std::println("Hello, modules!");
  }
  ```

### **17. Ranges Library Improvements** (`ranges-improvements`)
- **Feature:** Constant iterators, sentinels, and ranges consistency fixes
- **Value:** More robust ranges programming and iterator safety
- **Implementation:** Range algorithms, view compositions, iterator safety
- **Example:**
  ```cpp
  auto view = container | std::views::filter(pred) | std::views::take(5);
  auto const_it = std::ranges::cbegin(view);  // Guaranteed const iterator
  ```

---

## 📝 **Recommended Implementation Order**

### **Phase 1: Foundation Features**
1. **std::stacktrace** - Essential debugging tool
2. **size-t-literals** - Simple but widely useful
3. **string-contains** - Common string operation
4. **assume-attribute** - Compiler optimization demonstration

### **Phase 2: Advanced Language Features**
5. **std::generator** - Modern coroutine patterns
6. **move-only-function** - Advanced move semantics
7. **constexpr-static-thread-local** - Advanced constexpr
8. **unreachable** - Optimization and safety

### **Phase 3: Container and Performance**
9. **flat-containers** - Performance-focused containers
10. **optional-monadic** - Functional programming
11. **spanstream** - Zero-copy operations
12. **utf8-string-literals** - Modern text handling

### **Phase 4: Specialized Features**
13. **extended-float-types** - Specialized numeric types
14. **named-unicode-escapes** - Advanced Unicode
15. **delimited-escapes** - Syntax improvements
16. **import-std** - Module system
17. **ranges-improvements** - Advanced ranges

---

## 🎯 **Implementation Guidelines**

Each new feature should include:

- **Complete working example** with comprehensive demonstrations
- **CMakeLists.txt** with appropriate C++23 compiler flags
- **README.md** explaining the feature, use cases, and benefits
- **Error handling** and edge case demonstrations
- **Performance comparisons** where applicable
- **Integration** with existing project_utils.hpp logging
- **Cross-platform compatibility** considerations

## 📚 **References**

- [C++23 - cppreference.com](https://en.cppreference.com/w/cpp/23)
- [C++23 Language Features - C++ Stories](https://www.cppstories.com/2024/cpp23_lang/)
- [The C++23 Standard Breakdown](https://mariusbancila.ro/blog/2022/12/23/the-cpp23-standard-break-down/)
- [C++23 Wikipedia](https://en.wikipedia.org/wiki/C++23)

---

*This todo list prioritizes features based on educational value, practical utility, and demonstration of modern C++ capabilities. Each feature represents an opportunity to showcase C++23's improvements to the language and standard library.*
