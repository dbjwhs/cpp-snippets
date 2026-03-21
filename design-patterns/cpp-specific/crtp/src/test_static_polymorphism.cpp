// MIT License
// Copyright (c) 2025 dbjwhs

#include "../headers/crtp.hpp"
#include "../../../../headers/project_utils.hpp"
#include <cassert>
#include <cmath>
#include <string>

using namespace crtp_pattern;

void test_circle_geometry();
void test_rectangle_geometry();
void test_triangle_geometry();
void test_describe_output();
void test_free_function_dispatch();
void test_zero_dimension_shapes();
void test_large_and_small_values();
void test_degenerate_triangles();

int main() {
    LOG_INFO_PRINT("=== Static Polymorphism Test Suite ===");

    try {
        test_circle_geometry();
        test_rectangle_geometry();
        test_triangle_geometry();
        test_describe_output();
        test_free_function_dispatch();
        test_zero_dimension_shapes();
        test_large_and_small_values();
        test_degenerate_triangles();

        LOG_INFO_PRINT("=== All static polymorphism tests passed ===");
        return 0;

    } catch (const std::exception& e) {
        LOG_ERROR_PRINT("Test failed with exception: {}", e.what());
        return 1;
    }
}

void test_circle_geometry() {
    LOG_INFO_PRINT("\n--- Circle Geometry ---");

    constexpr double pi = 3.14159265358979323846;
    constexpr double epsilon = 1e-6;

    const Circle c1(1.0);
    assert(std::abs(c1.area() - pi) < epsilon);
    assert(std::abs(c1.perimeter() - 2.0 * pi) < epsilon);
    LOG_INFO_PRINT("  Unit circle: passed");

    const Circle c5(5.0);
    assert(std::abs(c5.area() - pi * 25.0) < 0.001);
    assert(std::abs(c5.perimeter() - 10.0 * pi) < 0.001);
    LOG_INFO_PRINT("  Radius 5: passed");

    const Circle c100(100.0);
    assert(std::abs(c100.area() - pi * 10000.0) < 0.01);
    LOG_INFO_PRINT("  Radius 100: passed");

    assert(c1.radius() == 1.0);
    assert(c5.radius() == 5.0);
    assert(c1.name() == "Circle");
    LOG_INFO_PRINT("  Accessors: passed");
}

void test_rectangle_geometry() {
    LOG_INFO_PRINT("\n--- Rectangle Geometry ---");

    constexpr double epsilon = 1e-6;

    // unit square
    const Rectangle r1(1.0, 1.0);
    assert(std::abs(r1.area() - 1.0) < epsilon);
    assert(std::abs(r1.perimeter() - 4.0) < epsilon);
    LOG_INFO_PRINT("  Unit square: passed");

    // standard rectangle
    const Rectangle r2(4.0, 6.0);
    assert(std::abs(r2.area() - 24.0) < epsilon);
    assert(std::abs(r2.perimeter() - 20.0) < epsilon);
    LOG_INFO_PRINT("  4x6 rectangle: passed");

    // wide rectangle
    const Rectangle r3(100.0, 0.5);
    assert(std::abs(r3.area() - 50.0) < epsilon);
    assert(std::abs(r3.perimeter() - 201.0) < epsilon);
    LOG_INFO_PRINT("  Wide thin rectangle: passed");

    assert(r2.width() == 4.0);
    assert(r2.height() == 6.0);
    assert(r2.name() == "Rectangle");
    LOG_INFO_PRINT("  Accessors: passed");
}

void test_triangle_geometry() {
    LOG_INFO_PRINT("\n--- Triangle Geometry ---");

    constexpr double epsilon = 1e-6;

    // 3-4-5 right triangle — area = 6.0
    const Triangle t1(3.0, 4.0, 5.0);
    assert(std::abs(t1.area() - 6.0) < epsilon);
    assert(std::abs(t1.perimeter() - 12.0) < epsilon);
    LOG_INFO_PRINT("  3-4-5 right triangle: passed");

    // equilateral triangle — area = (sqrt(3)/4) * a^2
    const Triangle t2(10.0, 10.0, 10.0);
    const double expected_equilateral = (std::sqrt(3.0) / 4.0) * 100.0;
    assert(std::abs(t2.area() - expected_equilateral) < epsilon);
    assert(std::abs(t2.perimeter() - 30.0) < epsilon);
    LOG_INFO_PRINT("  Equilateral (side 10): passed");

    // isoceles triangle — 5-5-6, area = 12.0
    const Triangle t3(5.0, 5.0, 6.0);
    const double s = (5.0 + 5.0 + 6.0) / 2.0;
    const double expected_isoceles = std::sqrt(s * (s - 5.0) * (s - 5.0) * (s - 6.0));
    assert(std::abs(t3.area() - expected_isoceles) < epsilon);
    LOG_INFO_PRINT("  Isoceles 5-5-6: passed");

    // 5-12-13 right triangle — area = 30.0
    const Triangle t4(5.0, 12.0, 13.0);
    assert(std::abs(t4.area() - 30.0) < epsilon);
    LOG_INFO_PRINT("  5-12-13 right triangle: passed");

    assert(t1.name() == "Triangle");
    LOG_INFO_PRINT("  Accessors: passed");
}

void test_describe_output() {
    LOG_INFO_PRINT("\n--- describe() Output ---");

    const Circle c(5.0);
    const std::string desc = c.describe();
    assert(desc.find("Circle") != std::string::npos);
    assert(desc.find("area=") != std::string::npos);
    assert(desc.find("perimeter=") != std::string::npos);
    LOG_INFO_PRINT("  Circle describe: passed ('{}')", desc);

    const Rectangle r(3.0, 4.0);
    const std::string rdesc = r.describe();
    assert(rdesc.find("Rectangle") != std::string::npos);
    LOG_INFO_PRINT("  Rectangle describe: passed ('{}')", rdesc);

    const Triangle t(3.0, 4.0, 5.0);
    const std::string tdesc = t.describe();
    assert(tdesc.find("Triangle") != std::string::npos);
    LOG_INFO_PRINT("  Triangle describe: passed ('{}')", tdesc);
}

void test_free_function_dispatch() {
    LOG_INFO_PRINT("\n--- Free Function Template Dispatch ---");

    // print_shape_info works with any Shape<Derived> — static dispatch
    const Circle c(2.0);
    const Rectangle r(3.0, 4.0);
    const Triangle t(3.0, 4.0, 5.0);

    // these compile and run — proving the template works with all derived types
    print_shape_info(c);
    print_shape_info(r);
    print_shape_info(t);
    LOG_INFO_PRINT("  All shape types accepted: passed");
}

void test_zero_dimension_shapes() {
    LOG_INFO_PRINT("\n--- Zero-Dimension Shapes ---");

    const Circle c(0.0);
    assert(c.area() == 0.0);
    assert(c.perimeter() == 0.0);
    LOG_INFO_PRINT("  Zero-radius circle: passed");

    const Rectangle r(0.0, 0.0);
    assert(r.area() == 0.0);
    assert(r.perimeter() == 0.0);
    LOG_INFO_PRINT("  Zero-dimension rectangle: passed");

    // rectangle with one zero dimension
    const Rectangle r2(5.0, 0.0);
    assert(r2.area() == 0.0);
    assert(std::abs(r2.perimeter() - 10.0) < 1e-6);
    LOG_INFO_PRINT("  One-zero-dimension rectangle: passed");
}

void test_large_and_small_values() {
    LOG_INFO_PRINT("\n--- Large and Small Values ---");

    const Circle big(1e6);
    assert(big.area() > 0.0);
    assert(std::isfinite(big.area()));
    assert(big.perimeter() > 0.0);
    LOG_INFO_PRINT("  Large radius (1e6): passed");

    const Circle tiny(1e-10);
    assert(tiny.area() >= 0.0);
    assert(std::isfinite(tiny.area()));
    assert(tiny.perimeter() >= 0.0);
    LOG_INFO_PRINT("  Tiny radius (1e-10): passed");

    const Rectangle wide(1e8, 1e-8);
    assert(std::abs(wide.area() - 1.0) < 1e-6);
    LOG_INFO_PRINT("  Extreme aspect ratio: passed");
}

void test_degenerate_triangles() {
    LOG_INFO_PRINT("\n--- Degenerate Triangles ---");

    // near-degenerate triangle (very flat)
    const Triangle flat(10.0, 10.0, 19.99);
    assert(flat.area() >= 0.0);
    assert(std::isfinite(flat.area()));
    LOG_INFO_PRINT("  Near-degenerate flat triangle: passed (area={:.6f})", flat.area());

    // very small equilateral
    const Triangle tiny(0.001, 0.001, 0.001);
    assert(tiny.area() > 0.0);
    assert(std::isfinite(tiny.area()));
    LOG_INFO_PRINT("  Tiny equilateral: passed (area={:.12f})", tiny.area());
}
