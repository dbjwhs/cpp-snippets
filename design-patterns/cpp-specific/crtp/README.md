# CRTP (Curiously Recurring Template Pattern)

The Curiously Recurring Template Pattern (CRTP) is a C++ idiom in which a class X derives from a class template
instantiation using X itself as a template argument: `class X : public Base<X>`. The name was coined by Jim Coplien
in his February 1995 column in the C++ Report, though the technique had been used earlier by Timothy Budd and was
present in Barton and Nackman's "Scientific and Engineering C++" (1994). CRTP is one of the most distinctive idioms
in C++ — it has no direct equivalent in other mainstream languages because it depends on the unique interaction
between C++ templates and inheritance.

The pattern works because when the base class template `Base<Derived>` is instantiated, `Derived` is an incomplete
type — but this is sufficient for the base to use it as a template parameter. The base can then `static_cast<Derived*>(this)`
to call methods on the derived class, achieving **static (compile-time) polymorphism** without virtual function tables.
The compiler resolves all calls at compile time, enabling full inlining and zero-cost abstraction.

## Use Cases and Problems Solved

### Static Polymorphism (Eliminating Virtual Dispatch)
- **Performance-critical code**: Game engines, embedded systems, HPC where vtable overhead matters
- **Template algorithms**: Functions that work with any type satisfying an interface, resolved at compile time
- **Zero-cost abstractions**: Polymorphic behavior with the same performance as hand-written non-polymorphic code

### Mixin Classes (Composable Behavior)
- **Operator generation**: Provide `==`, `!=`, `<`, `<=`, `>`, `>=` from a single `compare()` method
- **Interface injection**: Add `operator<<`, `clone()`, `hash()`, serialization without virtual inheritance
- **Boost.Operators**: The canonical real-world example — generates dozens of operators from a few primitives

### Object Counting (Per-Type Static State)
- **Instance tracking**: Each derived class gets its own independent static counter
- **Resource monitoring**: Track how many objects of each type are alive at any point
- **Memory leak detection**: Verify all objects are destroyed at shutdown

### Static Interface Enforcement
- **Compile-time contracts**: Verify derived classes implement required methods with clear error messages
- **Concept-constrained CRTP**: C++20 concepts + CRTP for the best of both worlds
- **Documentation as code**: The base class template documents the required interface

### Real-World STL Usage
- **`std::enable_shared_from_this<T>`**: The most well-known CRTP in the C++ standard library
- **Boost.Iterator**: CRTP base for writing custom iterators with minimal boilerplate
- **Boost.Operators**: Generates arithmetic and comparison operators from primitives

## Basic Usage Example

```cpp
#include "headers/crtp.hpp"
using namespace crtp_pattern;

// static polymorphism — no virtual dispatch
const Circle c(5.0);
const Rectangle r(4.0, 6.0);

// free function template works with any Shape<Derived>
print_shape_info(c);  // calls Circle::area_impl() directly
print_shape_info(r);  // calls Rectangle::area_impl() directly

// describe() defined once in base, dispatches statically
std::cout << c.describe() << "\n";
```

## Mixin Composition Example

```cpp
// Point inherits from 3 CRTP bases — no virtual functions
class Point : public EqualityComparable<Point>,
              public Printable<Point>,
              public Cloneable<Point> {
    double m_x, m_y;
public:
    // provide equals() → get == and != for free
    bool equals(const Point& other) const;
    // provide print_to() → get operator<< for free
    void print_to(std::ostream& os) const;
    // Cloneable works automatically via copy constructor
};

Point p1(1.0, 2.0), p2(1.0, 2.0);
assert(p1 == p2);           // from EqualityComparable
std::cout << p1;            // from Printable
auto clone = p1.clone();    // from Cloneable
```

## Object Counting Example

```cpp
class Widget : public ObjectCounter<Widget> { ... };
class Gadget : public ObjectCounter<Gadget> { ... };

{
    Widget w1("a"), w2("b");
    Gadget g1(1);
    assert(Widget::count() == 2);  // independent counter
    assert(Gadget::count() == 1);  // independent counter
}
assert(Widget::count() == 0);  // all destroyed
```

## Static Interface Enforcement with Concepts

```cpp
template<typename T>
concept Serializable = requires(const T& t, const std::string& data) {
    { t.serialize() } -> std::convertible_to<std::string>;
    { T::deserialize(data) } -> std::same_as<T>;
};

template<Serializable Derived>
class SerializableBase {
public:
    std::string to_string() const {
        return static_cast<const Derived*>(this)->serialize();
    }
};

// If Config lacks serialize(), you get a clear compile error:
// "constraints not satisfied for 'Serializable'"
class Config : public SerializableBase<Config> {
    std::string serialize() const { return key + "=" + value; }
    static Config deserialize(const std::string& data) { ... }
};
```

## CRTP vs Virtual Dispatch

| Aspect | CRTP (Static) | Virtual (Dynamic) |
|--------|--------------|-------------------|
| **Dispatch** | Compile-time | Runtime (vtable) |
| **Inlining** | Full inlining possible | Not possible through base pointer |
| **Memory** | No vtable pointer per object | 8 bytes per object for vptr |
| **Heterogeneous containers** | Not directly (different types) | Yes (`vector<Base*>`) |
| **Adding new types** | Recompile required | Link-time extensible |
| **Use when** | Types known at compile time | Types determined at runtime |

## Best Practices

### When to Use CRTP
1. **Performance matters**: Hot loops, embedded systems, game engines
2. **Types are known at compile time**: No need for runtime type discovery
3. **Composing behavior**: Adding operators, serialization, counting via mixins
4. **Interface enforcement**: Compile-time verification of derived class contracts

### When to Prefer Virtual Functions
1. **Heterogeneous collections**: Need `vector<unique_ptr<Base>>`
2. **Plugin architectures**: Types loaded at runtime from shared libraries
3. **Simpler mental model**: Virtual dispatch is more widely understood

### Common Pitfalls
1. **Object slicing**: CRTP shapes are different types — can't store in a common container without type erasure
2. **Protected destructors**: Base class destructors should be protected, not virtual (prevent deletion through base pointer)
3. **Forgetting the derived type**: `class X : public Base<Y>` compiles but is wrong if X != Y

## Building and Testing

### Prerequisites
- C++23 compatible compiler (GCC 13+, Clang 16+, MSVC 19.34+)
- CMake 3.28 or higher
- Threading support

### Build Instructions

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .

# run tests
ctest --verbose

# run demonstration
./bin/crtp_pattern_demo

# run specific tests
./bin/crtp_pattern_test
```

### Custom CMake Targets

```bash
make test_verbose       # tests with verbose output
make run_pattern_tests  # just the pattern tests
make run_demo           # demonstration only
```

## Performance Characteristics

### Compile-Time Costs
- **Template instantiation**: One instantiation per derived type (linear in number of types)
- **Code bloat**: Each `Shape<Derived>` generates its own describe() — minimal for small bases
- **Compile time**: Slightly longer than virtual dispatch due to template instantiation

### Runtime Costs
- **Dispatch overhead**: Zero — all calls resolved at compile time and inlined
- **Memory overhead**: Zero — no vtable pointer, no RTTI
- **Cache behavior**: Better than virtual dispatch (no vtable cache misses)

### Benchmarks
Based on performance tests with 10,000,000 iterations:
- CRTP dispatch: Typically 2-5x faster than virtual dispatch
- The gap widens when the function body is small (dispatch overhead dominates)

## Related Patterns

### Strategy Pattern
CRTP achieves the same goal as Strategy — interchangeable algorithms — but at compile time. Use CRTP when the strategy is known at compile time, virtual Strategy when determined at runtime.

### Template Method Pattern
Template Method uses virtual functions for customization points. CRTP achieves the same structure with static dispatch — the base class defines the skeleton, derived classes provide steps.

### Policy-Based Design
Policy-Based Design (Alexandrescu) generalizes CRTP mixins. Where CRTP uses inheritance, policies use template parameters as behavioral axes. Often combined: a policy class can itself use CRTP internally.

## References and Further Reading

### Books
- **"A Curiously Recurring Template Pattern"** by James O. Coplien, C++ Report, February 1995
  - The original naming and documentation of the pattern
- **"Modern C++ Design"** by Andrei Alexandrescu (2001)
  - Advanced CRTP techniques, policy-based design, type lists
- **"Scientific and Engineering C++"** by Barton and Nackman (1994)
  - Early use of the pattern for dimensional analysis
- **"C++ Templates: The Complete Guide"** by Vandevoorde, Josuttis, and Gregor (2nd Edition, 2017)
  - Comprehensive template mechanics behind CRTP

### Academic Papers
- **"Curiously Recurring Template Patterns"** — Todd Veldhuizen, C++ Report (1995)
- **"Static Polymorphism with CRTP"** — ISO C++ Core Guidelines

### Online Resources
- **C++ Core Guidelines**: https://isocpp.github.io/CppCoreGuidelines/
- **cppreference — enable_shared_from_this**: The STL's own CRTP usage
- **Barton-Nackman trick**: Related technique for operator overloading via friend functions

## License

This code is provided under the MIT License. Feel free to use, modify, and distribute as needed.

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.
