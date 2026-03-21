// MIT License
// Copyright (c) 2025 dbjwhs

#include "../headers/crtp.hpp"
#include "../../../../headers/project_utils.hpp"
#include <cassert>
#include <sstream>
#include <vector>
#include <memory>
#include <algorithm>

using namespace crtp_pattern;

void test_equality_basic();
void test_equality_properties();
void test_equality_epsilon();
void test_printable_format();
void test_printable_special_values();
void test_cloneable_basic();
void test_cloneable_independence();
void test_mixin_composition();

int main() {
    LOG_INFO_PRINT("=== Mixin Classes Test Suite ===");

    try {
        test_equality_basic();
        test_equality_properties();
        test_equality_epsilon();
        test_printable_format();
        test_printable_special_values();
        test_cloneable_basic();
        test_cloneable_independence();
        test_mixin_composition();

        LOG_INFO_PRINT("=== All mixin tests passed ===");
        return 0;

    } catch (const std::exception& e) {
        LOG_ERROR_PRINT("Test failed with exception: {}", e.what());
        return 1;
    }
}

void test_equality_basic() {
    LOG_INFO_PRINT("\n--- Equality: Basic ---");

    const Point p1(1.0, 2.0);
    const Point p2(1.0, 2.0);
    const Point p3(3.0, 4.0);

    assert(p1 == p2);
    assert(!(p1 != p2));
    assert(p1 != p3);
    assert(!(p1 == p3));
    LOG_INFO_PRINT("  Equal and not-equal: passed");

    const Point origin(0.0, 0.0);
    const Point also_origin(0.0, 0.0);
    assert(origin == also_origin);
    LOG_INFO_PRINT("  Origin equality: passed");
}

void test_equality_properties() {
    LOG_INFO_PRINT("\n--- Equality: Mathematical Properties ---");

    const Point a(1.0, 2.0);
    const Point b(1.0, 2.0);
    const Point c(1.0, 2.0);
    const Point d(5.0, 6.0);

    // reflexive: a == a
    assert(a == a);
    LOG_INFO_PRINT("  Reflexive: passed");

    // symmetric: a == b implies b == a
    assert(a == b);
    assert(b == a);
    assert(a != d);
    assert(d != a);
    LOG_INFO_PRINT("  Symmetric: passed");

    // transitive: a == b and b == c implies a == c
    assert(a == b);
    assert(b == c);
    assert(a == c);
    LOG_INFO_PRINT("  Transitive: passed");
}

void test_equality_epsilon() {
    LOG_INFO_PRINT("\n--- Equality: Epsilon Tolerance ---");

    // within epsilon (1e-9)
    const Point p1(1.0, 2.0);
    const Point p2(1.0 + 1e-10, 2.0 + 1e-10);
    assert(p1 == p2);
    LOG_INFO_PRINT("  Within epsilon (1e-10 diff): passed");

    // just outside epsilon
    const Point p3(1.0, 2.0);
    const Point p4(1.0 + 1e-8, 2.0);
    assert(p3 != p4);
    LOG_INFO_PRINT("  Outside epsilon (1e-8 diff): passed");

    // negative coordinates
    const Point neg1(-5.0, -10.0);
    const Point neg2(-5.0, -10.0);
    assert(neg1 == neg2);
    LOG_INFO_PRINT("  Negative coordinates: passed");

    // mixed sign
    const Point pos(1.0, 1.0);
    const Point neg(-1.0, -1.0);
    assert(pos != neg);
    LOG_INFO_PRINT("  Mixed sign: passed");
}

void test_printable_format() {
    LOG_INFO_PRINT("\n--- Printable: Format ---");

    const Point p(3.14, 2.72);
    std::ostringstream oss;
    oss << p;
    const std::string output = oss.str();

    assert(output == "(3.14, 2.72)");
    LOG_INFO_PRINT("  Standard format: passed ('{}')", output);

    // integer-like values
    const Point whole(5.0, 10.0);
    std::ostringstream oss2;
    oss2 << whole;
    assert(oss2.str() == "(5.00, 10.00)");
    LOG_INFO_PRINT("  Whole numbers: passed ('{}')", oss2.str());
}

void test_printable_special_values() {
    LOG_INFO_PRINT("\n--- Printable: Special Values ---");

    const Point origin(0.0, 0.0);
    std::ostringstream oss;
    oss << origin;
    assert(oss.str() == "(0.00, 0.00)");
    LOG_INFO_PRINT("  Origin: passed ('{}')", oss.str());

    const Point neg(-1.5, -2.5);
    std::ostringstream oss2;
    oss2 << neg;
    assert(oss2.str() == "(-1.50, -2.50)");
    LOG_INFO_PRINT("  Negative: passed ('{}')", oss2.str());

    // chained output
    const Point a(1.0, 2.0);
    const Point b(3.0, 4.0);
    std::ostringstream oss3;
    oss3 << a << " -> " << b;
    assert(oss3.str() == "(1.00, 2.00) -> (3.00, 4.00)");
    LOG_INFO_PRINT("  Chained output: passed ('{}')", oss3.str());
}

void test_cloneable_basic() {
    LOG_INFO_PRINT("\n--- Cloneable: Basic ---");

    const Point original(5.0, 10.0);
    auto cloned = original.clone();

    assert(cloned != nullptr);
    assert(original == *cloned);
    assert(&original != cloned.get());
    LOG_INFO_PRINT("  Clone equality and identity: passed");

    // clone returns unique_ptr
    std::unique_ptr<Point> moved = std::move(cloned);
    assert(moved != nullptr);
    assert(original == *moved);
    LOG_INFO_PRINT("  unique_ptr semantics: passed");
}

void test_cloneable_independence() {
    LOG_INFO_PRINT("\n--- Cloneable: Independence ---");

    const Point p1(7.0, 8.0);
    auto c1 = p1.clone();
    auto c2 = p1.clone();

    // both clones equal original
    assert(*c1 == p1);
    assert(*c2 == p1);
    // clones are different objects
    assert(c1.get() != c2.get());
    LOG_INFO_PRINT("  Multiple clones independent: passed");

    // clone of clone
    auto c3 = c1->clone();
    assert(*c3 == *c1);
    assert(c3.get() != c1.get());
    LOG_INFO_PRINT("  Clone of clone: passed");
}

void test_mixin_composition() {
    LOG_INFO_PRINT("\n--- Mixin Composition ---");

    // Point uses all three mixins together — verify no interference
    const Point p(1.0, 2.0);

    // EqualityComparable
    assert(p == Point(1.0, 2.0));

    // Printable
    std::ostringstream oss;
    oss << p;
    assert(!oss.str().empty());

    // Cloneable
    auto cloned = p.clone();
    assert(p == *cloned);

    // all three work together
    std::ostringstream oss2;
    oss2 << *cloned;
    assert(oss.str() == oss2.str());
    LOG_INFO_PRINT("  All three mixins cooperate: passed");

    // vector of Points using all mixins
    std::vector<Point> points = {{1.0, 2.0}, {3.0, 4.0}, {5.0, 6.0}};
    auto clones = std::vector<std::unique_ptr<Point>>{};
    for (const auto& pt : points) {
        clones.push_back(pt.clone());
    }
    for (size_t i = 0; i < points.size(); ++i) {
        assert(points[i] == *clones[i]);
    }
    LOG_INFO_PRINT("  Vector of mixed-in objects: passed");
}
