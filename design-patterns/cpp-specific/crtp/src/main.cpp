// MIT License
// Copyright (c) 2025 dbjwhs

#include "../headers/crtp.hpp"
#include "../../../../headers/project_utils.hpp"
#include <cassert>
#include <vector>
#include <memory>
#include <chrono>
#include <numeric>
#include <sstream>

using namespace crtp_pattern;

void demonstrate_static_polymorphism();
void demonstrate_mixin_classes();
void demonstrate_object_counting();
void demonstrate_static_interface_enforcement();
void demonstrate_performance_comparison();

int main() {
    LOG_INFO_PRINT("=== CRTP (Curiously Recurring Template Pattern) Demonstration ===");

    try {
        demonstrate_static_polymorphism();
        demonstrate_mixin_classes();
        demonstrate_object_counting();
        demonstrate_static_interface_enforcement();
        demonstrate_performance_comparison();

        LOG_INFO_PRINT("=== All demonstrations completed successfully ===");

    } catch (const std::exception& e) {
        LOG_ERROR_PRINT("Exception caught in main: {}", e.what());
        return 1;
    } catch (...) {
        LOG_ERROR_PRINT("Unknown exception caught in main");
        return 1;
    }

    return 0;
}

void demonstrate_static_polymorphism() {
    LOG_INFO_PRINT("\n--- Static Polymorphism ---");
    LOG_INFO_PRINT("CRTP shapes use compile-time dispatch — no vtable, full inlining:");

    const Circle circle(5.0);
    const Rectangle rectangle(4.0, 6.0);
    const Triangle triangle(3.0, 4.0, 5.0);

    // free function template works with any Shape<Derived>
    print_shape_info(circle);
    print_shape_info(rectangle);
    print_shape_info(triangle);

    // describe() is defined once in the base, calls area()/perimeter()/name()
    // via static dispatch — the compiler resolves everything at compile time
    LOG_INFO_PRINT("Key insight: Shape<Circle> and Shape<Rectangle> are distinct types.");
    LOG_INFO_PRINT("No common base pointer, no virtual dispatch, no runtime overhead.");
}

void demonstrate_mixin_classes() {
    LOG_INFO_PRINT("\n--- Mixin Classes ---");
    LOG_INFO_PRINT("Multiple CRTP bases compose behavior without virtual inheritance:");

    const Point p1(3.0, 4.0);
    const Point p2(3.0, 4.0);
    const Point p3(1.0, 2.0);

    // EqualityComparable mixin provides == and != from equals()
    LOG_INFO_PRINT("  p1 == p2: {} (same coordinates)", p1 == p2 ? "true" : "false");
    LOG_INFO_PRINT("  p1 != p3: {} (different coordinates)", p1 != p3 ? "true" : "false");

    // Printable mixin provides operator<<
    std::ostringstream oss;
    oss << p1;
    LOG_INFO_PRINT("  operator<< output: {}", oss.str());

    // Cloneable mixin provides clone() returning unique_ptr
    auto p1_clone = p1.clone();
    LOG_INFO_PRINT("  clone: original {} == clone {}: {}",
        [&]{ std::ostringstream s; s << p1; return s.str(); }(),
        [&]{ std::ostringstream s; s << *p1_clone; return s.str(); }(),
        p1 == *p1_clone ? "true" : "false");

    LOG_INFO_PRINT("Key insight: Point inherits from 3 CRTP bases, each adding");
    LOG_INFO_PRINT("operators/methods. No virtual functions, no diamond problem.");
}

void demonstrate_object_counting() {
    LOG_INFO_PRINT("\n--- Object Counting ---");
    LOG_INFO_PRINT("Each class gets its own independent counter via CRTP:");

    Widget::reset_count();
    Gadget::reset_count();

    LOG_INFO_PRINT("  Initial: Widget count={}, Gadget count={}", Widget::count(), Gadget::count());

    {
        Widget w1("alpha");
        Widget w2("beta");
        Gadget g1(1);

        LOG_INFO_PRINT("  After creating 2 Widgets, 1 Gadget: Widget={}, Gadget={}",
            Widget::count(), Gadget::count());

        {
            Widget w3("gamma");
            auto w4 = w1; // copy increments count
            LOG_INFO_PRINT("  After creating 1 more Widget + 1 copy: Widget={}, Gadget={}",
                Widget::count(), Gadget::count());
        }

        LOG_INFO_PRINT("  After inner scope exits (2 destroyed): Widget={}, Gadget={}",
            Widget::count(), Gadget::count());
    }

    LOG_INFO_PRINT("  After outer scope exits (all destroyed): Widget={}, Gadget={}",
        Widget::count(), Gadget::count());

    LOG_INFO_PRINT("Key insight: ObjectCounter<Widget> and ObjectCounter<Gadget> each");
    LOG_INFO_PRINT("have their own static m_count — separate template instantiations.");
}

void demonstrate_static_interface_enforcement() {
    LOG_INFO_PRINT("\n--- Static Interface Enforcement ---");
    LOG_INFO_PRINT("C++20 concepts verify derived classes implement required methods:");

    const Config cfg("database.host", "localhost:5432");
    LOG_INFO_PRINT("  Config: key='{}', value='{}'", cfg.key(), cfg.value());

    // serialize via CRTP base
    const std::string serialized = cfg.to_string();
    LOG_INFO_PRINT("  Serialized: '{}'", serialized);

    // deserialize via CRTP base
    const auto restored = Config::from_string(serialized);
    LOG_INFO_PRINT("  Restored: key='{}', value='{}'", restored.key(), restored.value());

    assert(cfg.key() == restored.key());
    assert(cfg.value() == restored.value());

    LOG_INFO_PRINT("Key insight: If Config lacked serialize() or deserialize(), the");
    LOG_INFO_PRINT("Serializable concept would produce a clear compile-time error.");

    // uncomment to see the compile error:
    // class BadConfig : public SerializableBase<BadConfig> {};
    // ^^^ error: constraints not satisfied for 'Serializable'
}

void demonstrate_performance_comparison() {
    LOG_INFO_PRINT("\n--- Performance: CRTP vs Virtual Dispatch ---");

    constexpr int iterations = 10'000'000;

    // CRTP (static dispatch) benchmark
    const Circle crtp_circle(5.0);
    const Rectangle crtp_rect(4.0, 6.0);

    auto start = std::chrono::high_resolution_clock::now();
    double crtp_sum = 0.0;
    for (int i = 0; i < iterations; ++i) {
        crtp_sum += crtp_circle.area() + crtp_rect.area();
    }
    auto end = std::chrono::high_resolution_clock::now();
    const auto crtp_duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    // Virtual dispatch benchmark
    const VirtualCircle virt_circle(5.0);
    const VirtualRectangle virt_rect(4.0, 6.0);

    // use function pointers to prevent the compiler from devirtualizing
    const std::function<double(const VirtualShape&)> get_area =
        [](const VirtualShape& s) { return s.area(); };

    start = std::chrono::high_resolution_clock::now();
    double virt_sum = 0.0;
    for (int i = 0; i < iterations; ++i) {
        virt_sum += get_area(virt_circle) + get_area(virt_rect);
    }
    end = std::chrono::high_resolution_clock::now();
    const auto virt_duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    LOG_INFO_PRINT("  {} iterations of area() on Circle + Rectangle:", iterations);
    LOG_INFO_PRINT("  CRTP (static dispatch):    {} us (sum={:.2f})", crtp_duration.count(), crtp_sum);
    LOG_INFO_PRINT("  Virtual dispatch:          {} us (sum={:.2f})", virt_duration.count(), virt_sum);

    if (virt_duration.count() > 0) {
        const double ratio = static_cast<double>(virt_duration.count()) /
                             static_cast<double>(crtp_duration.count() > 0 ? crtp_duration.count() : 1);
        LOG_INFO_PRINT("  Virtual/CRTP ratio: {:.2f}x", ratio);
    }

    LOG_INFO_PRINT("Note: CRTP calls can be fully inlined; virtual calls go through");
    LOG_INFO_PRINT("the vtable and std::function wrapper prevents devirtualization.");
}
