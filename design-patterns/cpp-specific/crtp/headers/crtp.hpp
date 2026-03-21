// MIT License
// Copyright (c) 2025 dbjwhs

#ifndef CRTP_HPP
#define CRTP_HPP

// The Curiously Recurring Template Pattern (CRTP): A Comprehensive Historical Analysis
//
// The Curiously Recurring Template Pattern (CRTP) is a C++ idiom in which a class X derives from
// a class template instantiation using X itself as a template argument. The name was coined by
// Jim Coplien in his 1995 column in the C++ Report, though the technique had been used earlier
// by Timothy Budd in his multiparadigm programming work and was present in early template
// libraries. The pattern's discovery arose naturally from the intersection of C++'s template
// system and inheritance, and it has since become one of the most distinctive and powerful
// idioms in the C++ language — with no direct equivalent in other mainstream languages.
//
// Historical Development:
// The pattern first appeared in practice in the early 1990s as C++ templates matured. Jim
// Coplien formally named and documented it in "A Curiously Recurring Template Pattern" (C++
// Report, February 1995). The technique was subsequently adopted by the STL (std::enable_shared_from_this),
// by Barton and Nackman in their "Fallible" class, and extensively by the Boost libraries
// (Boost.Operators, Boost.Iterator). Andrei Alexandrescu popularized advanced CRTP techniques
// in "Modern C++ Design" (2001), particularly for policy-based design and type lists.
//
// Core Mechanics:
// CRTP works because at the point where the base class template is instantiated, the derived
// class is an incomplete type — but this is sufficient for the base class to use it as a
// template parameter. The base class can then cast `this` to `Derived*` or `Derived&` to
// call methods on the derived class, achieving static (compile-time) polymorphism without
// the overhead of virtual function tables. The key insight is that the base class "knows"
// its derived type at compile time, enabling devirtualization, inlining, and zero-cost
// abstraction.
//
// Modern C++23 Implementation Features:
// This implementation uses C++23 concepts to constrain CRTP base classes, ensuring that
// derived types actually provide the required interface at compile time with clear error
// messages. It demonstrates static polymorphism, mixin classes, object counting, and
// compile-time interface enforcement — the four canonical CRTP use cases.
//
// Common Use Cases:
// - Static polymorphism (eliminating virtual dispatch overhead)
// - Mixin classes (adding reusable behavior without virtual inheritance)
// - Object counting per type
// - Static interface enforcement (compile-time "abstract base class")
// - Expression templates for lazy evaluation
// - std::enable_shared_from_this (STL's own CRTP usage)

#include <concepts>
#include <type_traits>
#include <string>
#include <format>
#include <iostream>
#include <atomic>
#include <functional>
#include "../../../../headers/project_utils.hpp"

namespace crtp_pattern {

// ============================================================================
// 1. Static Polymorphism — The core CRTP use case
// ============================================================================
//
// Instead of virtual dispatch (vtable lookup at runtime), the base class
// statically casts to the derived type and calls its methods directly.
// The compiler can inline everything — zero overhead compared to hand-written
// non-polymorphic code.

template<typename Derived>
class Shape {
public:
    // static dispatch: calls Derived::area_impl() with zero overhead
    [[nodiscard]] double area() const {
        return static_cast<const Derived*>(this)->area_impl();
    }

    [[nodiscard]] double perimeter() const {
        return static_cast<const Derived*>(this)->perimeter_impl();
    }

    [[nodiscard]] std::string name() const {
        return static_cast<const Derived*>(this)->name_impl();
    }

    [[nodiscard]] std::string describe() const {
        return std::format("{}: area={:.4f}, perimeter={:.4f}", name(), area(), perimeter());
    }

protected:
    // prevent deletion through base pointer (no virtual destructor needed)
    ~Shape() = default;
};

class Circle : public Shape<Circle> {
    double m_radius;
public:
    explicit constexpr Circle(const double radius) : m_radius(radius) {}

    [[nodiscard]] constexpr double area_impl() const {
        return 3.14159265358979323846 * m_radius * m_radius;
    }

    [[nodiscard]] constexpr double perimeter_impl() const {
        return 2.0 * 3.14159265358979323846 * m_radius;
    }

    [[nodiscard]] std::string name_impl() const { return "Circle"; }

    [[nodiscard]] constexpr double radius() const { return m_radius; }
};

class Rectangle : public Shape<Rectangle> {
    double m_width;
    double m_height;
public:
    constexpr Rectangle(const double width, const double height)
        : m_width(width), m_height(height) {}

    [[nodiscard]] constexpr double area_impl() const {
        return m_width * m_height;
    }

    [[nodiscard]] constexpr double perimeter_impl() const {
        return 2.0 * (m_width + m_height);
    }

    [[nodiscard]] std::string name_impl() const { return "Rectangle"; }

    [[nodiscard]] constexpr double width() const { return m_width; }
    [[nodiscard]] constexpr double height() const { return m_height; }
};

class Triangle : public Shape<Triangle> {
    double m_a, m_b, m_c; // side lengths
public:
    constexpr Triangle(const double a, const double b, const double c)
        : m_a(a), m_b(b), m_c(c) {}

    [[nodiscard]] double area_impl() const {
        // heron's formula
        const double s = (m_a + m_b + m_c) / 2.0;
        return std::sqrt(s * (s - m_a) * (s - m_b) * (s - m_c));
    }

    [[nodiscard]] constexpr double perimeter_impl() const {
        return m_a + m_b + m_c;
    }

    [[nodiscard]] std::string name_impl() const { return "Triangle"; }
};

// free function template — works with any Shape<Derived> without virtual dispatch
template<typename Derived>
void print_shape_info(const Shape<Derived>& shape) {
    LOG_INFO_PRINT("  {}", shape.describe());
}

// ============================================================================
// 2. Mixin Classes — Adding reusable behavior via CRTP
// ============================================================================
//
// CRTP mixins inject behavior into derived classes without virtual inheritance.
// Each mixin adds a specific capability. Classes can inherit from multiple
// mixins, composing behavior at compile time.

// EqualityComparable mixin: provides == and != from a single equals() method
template<typename Derived>
class EqualityComparable {
public:
    [[nodiscard]] friend bool operator==(const Derived& lhs, const Derived& rhs) {
        return lhs.equals(rhs);
    }

    [[nodiscard]] friend bool operator!=(const Derived& lhs, const Derived& rhs) {
        return !lhs.equals(rhs);
    }

protected:
    ~EqualityComparable() = default;
};

// Printable mixin: provides operator<< from a single print_to() method
template<typename Derived>
class Printable {
public:
    friend std::ostream& operator<<(std::ostream& os, const Derived& obj) {
        obj.print_to(os);
        return os;
    }

protected:
    ~Printable() = default;
};

// Cloneable mixin: provides clone() returning a unique_ptr
template<typename Derived>
class Cloneable {
public:
    [[nodiscard]] std::unique_ptr<Derived> clone() const {
        return std::make_unique<Derived>(static_cast<const Derived&>(*this));
    }

protected:
    ~Cloneable() = default;
};

// demonstration class using multiple CRTP mixins
class Point : public EqualityComparable<Point>,
              public Printable<Point>,
              public Cloneable<Point> {
    double m_x, m_y;
public:
    constexpr Point(const double x, const double y) : m_x(x), m_y(y) {}

    [[nodiscard]] constexpr double x() const { return m_x; }
    [[nodiscard]] constexpr double y() const { return m_y; }

    // required by EqualityComparable
    [[nodiscard]] bool equals(const Point& other) const {
        constexpr double epsilon = 1e-9;
        return std::abs(m_x - other.m_x) < epsilon &&
               std::abs(m_y - other.m_y) < epsilon;
    }

    // required by Printable
    void print_to(std::ostream& os) const {
        os << std::format("({:.2f}, {:.2f})", m_x, m_y);
    }
};

// ============================================================================
// 3. Object Counting — Track instance count per type
// ============================================================================
//
// A classic CRTP use case: each derived class gets its own static counter
// because the base template is instantiated separately for each derived type.

template<typename Derived>
class ObjectCounter {
    inline static std::atomic<int> m_count{0};

public:
    ObjectCounter() { ++m_count; }
    ObjectCounter(const ObjectCounter&) { ++m_count; }
    ObjectCounter(ObjectCounter&&) noexcept { ++m_count; }
    ObjectCounter& operator=(const ObjectCounter&) = default;
    ObjectCounter& operator=(ObjectCounter&&) noexcept = default;

    [[nodiscard]] static int count() { return m_count.load(); }
    static void reset_count() { m_count.store(0); }

protected:
    ~ObjectCounter() { --m_count; }
};

// each class gets its own independent counter
class Widget : public ObjectCounter<Widget> {
    std::string m_name;
public:
    explicit Widget(std::string name) : m_name(std::move(name)) {}
    [[nodiscard]] const std::string& name() const { return m_name; }
};

class Gadget : public ObjectCounter<Gadget> {
    int m_id;
public:
    explicit constexpr Gadget(const int id) : m_id(id) {}
    [[nodiscard]] constexpr int id() const { return m_id; }
};

// ============================================================================
// 4. Static Interface Enforcement — Compile-time "abstract base class"
// ============================================================================
//
// Uses C++20 concepts to verify that derived classes actually implement the
// required interface. Failures produce clear compile-time errors instead of
// link errors or undefined behavior from missing methods.

template<typename T>
concept Serializable = requires(const T& t, const std::string& data) {
    { t.serialize() } -> std::convertible_to<std::string>;
    { T::deserialize(data) } -> std::same_as<T>;
};

// note: the concept cannot be applied directly to the base template parameter
// because Derived is an incomplete type at the point of inheritance. Instead,
// we apply the concept check via static_assert in the member functions, where
// Derived is complete. This is a well-known CRTP + concepts interaction.
template<typename Derived>
class SerializableBase {
public:
    [[nodiscard]] std::string to_string() const {
        static_assert(Serializable<Derived>,
            "Derived must implement serialize() and static deserialize()");
        return static_cast<const Derived*>(this)->serialize();
    }

    [[nodiscard]] static Derived from_string(const std::string& data) {
        static_assert(Serializable<Derived>,
            "Derived must implement serialize() and static deserialize()");
        return Derived::deserialize(data);
    }

protected:
    ~SerializableBase() = default;
};

class Config : public SerializableBase<Config> {
    std::string m_key;
    std::string m_value;

public:
    Config(std::string key, std::string value)
        : m_key(std::move(key)), m_value(std::move(value)) {}

    [[nodiscard]] const std::string& key() const { return m_key; }
    [[nodiscard]] const std::string& value() const { return m_value; }

    [[nodiscard]] std::string serialize() const {
        return std::format("{}={}", m_key, m_value);
    }

    [[nodiscard]] static Config deserialize(const std::string& data) {
        const auto pos = data.find('=');
        if (pos == std::string::npos) {
            return Config{"", data};
        }
        return Config{data.substr(0, pos), data.substr(pos + 1)};
    }
};

// ============================================================================
// 5. Performance comparison helpers — virtual vs CRTP
// ============================================================================
//
// Virtual base class for runtime polymorphism comparison

class VirtualShape {
public:
    virtual ~VirtualShape() = default;
    [[nodiscard]] virtual double area() const = 0;
    [[nodiscard]] virtual double perimeter() const = 0;
    [[nodiscard]] virtual std::string name() const = 0;
};

class VirtualCircle : public VirtualShape {
    double m_radius;
public:
    explicit constexpr VirtualCircle(const double radius) : m_radius(radius) {}
    [[nodiscard]] double area() const override {
        return 3.14159265358979323846 * m_radius * m_radius;
    }
    [[nodiscard]] double perimeter() const override {
        return 2.0 * 3.14159265358979323846 * m_radius;
    }
    [[nodiscard]] std::string name() const override { return "VirtualCircle"; }
};

class VirtualRectangle : public VirtualShape {
    double m_width, m_height;
public:
    constexpr VirtualRectangle(const double w, const double h) : m_width(w), m_height(h) {}
    [[nodiscard]] double area() const override {
        return m_width * m_height;
    }
    [[nodiscard]] double perimeter() const override {
        return 2.0 * (m_width + m_height);
    }
    [[nodiscard]] std::string name() const override { return "VirtualRectangle"; }
};

} // namespace crtp_pattern

#endif // CRTP_HPP
