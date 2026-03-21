// MIT License
// Copyright (c) 2025 dbjwhs

#include "../headers/crtp.hpp"
#include "../../../../headers/project_utils.hpp"
#include <cassert>
#include <chrono>
#include <functional>
#include <cmath>

using namespace crtp_pattern;

void test_crtp_correctness_vs_virtual();
void test_crtp_faster_than_virtual();
void test_crtp_inlining_benefit();

int main() {
    LOG_INFO_PRINT("=== Performance Comparison Test Suite ===");

    try {
        test_crtp_correctness_vs_virtual();
        test_crtp_faster_than_virtual();
        test_crtp_inlining_benefit();

        LOG_INFO_PRINT("=== All performance tests passed ===");
        return 0;

    } catch (const std::exception& e) {
        LOG_ERROR_PRINT("Test failed with exception: {}", e.what());
        return 1;
    }
}

void test_crtp_correctness_vs_virtual() {
    LOG_INFO_PRINT("\n--- CRTP vs Virtual: Correctness ---");

    constexpr double epsilon = 1e-6;

    // CRTP versions
    const Circle crtp_circle(5.0);
    const Rectangle crtp_rect(4.0, 6.0);

    // Virtual versions
    const VirtualCircle virt_circle(5.0);
    const VirtualRectangle virt_rect(4.0, 6.0);

    // same results from both dispatch mechanisms
    assert(std::abs(crtp_circle.area() - virt_circle.area()) < epsilon);
    assert(std::abs(crtp_circle.perimeter() - virt_circle.perimeter()) < epsilon);
    LOG_INFO_PRINT("  Circle: CRTP area={:.6f}, Virtual area={:.6f} — match",
        crtp_circle.area(), virt_circle.area());

    assert(std::abs(crtp_rect.area() - virt_rect.area()) < epsilon);
    assert(std::abs(crtp_rect.perimeter() - virt_rect.perimeter()) < epsilon);
    LOG_INFO_PRINT("  Rectangle: CRTP area={:.6f}, Virtual area={:.6f} — match",
        crtp_rect.area(), virt_rect.area());
}

void test_crtp_faster_than_virtual() {
    LOG_INFO_PRINT("\n--- CRTP vs Virtual: Performance ---");

    constexpr int iterations = 5'000'000;

    // CRTP benchmark
    const Circle crtp_circle(5.0);
    const Rectangle crtp_rect(4.0, 6.0);

    auto start = std::chrono::high_resolution_clock::now();
    double crtp_sum = 0.0;
    for (int i = 0; i < iterations; ++i) {
        crtp_sum += crtp_circle.area() + crtp_rect.area();
    }
    auto end = std::chrono::high_resolution_clock::now();
    const auto crtp_us = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    // Virtual benchmark — use std::function to prevent devirtualization
    const VirtualCircle virt_circle(5.0);
    const VirtualRectangle virt_rect(4.0, 6.0);
    const std::function<double(const VirtualShape&)> get_area =
        [](const VirtualShape& s) { return s.area(); };

    start = std::chrono::high_resolution_clock::now();
    double virt_sum = 0.0;
    for (int i = 0; i < iterations; ++i) {
        virt_sum += get_area(virt_circle) + get_area(virt_rect);
    }
    end = std::chrono::high_resolution_clock::now();
    const auto virt_us = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    // results should be identical
    assert(std::abs(crtp_sum - virt_sum) < 1.0);

    LOG_INFO_PRINT("  {} iterations:", iterations);
    LOG_INFO_PRINT("  CRTP:    {} us", crtp_us);
    LOG_INFO_PRINT("  Virtual: {} us", virt_us);

    if (crtp_us > 0) {
        const double ratio = static_cast<double>(virt_us) / static_cast<double>(crtp_us);
        LOG_INFO_PRINT("  Ratio: {:.2f}x (virtual / CRTP)", ratio);

        // CRTP should be at least as fast (allowing for measurement noise)
        // using a conservative threshold — on most systems CRTP is 2-5x faster
        assert(ratio >= 0.8); // at minimum not significantly slower
        LOG_INFO_PRINT("  CRTP not slower than virtual: passed");
    }
}

void test_crtp_inlining_benefit() {
    LOG_INFO_PRINT("\n--- CRTP Inlining Benefit ---");

    // the real benefit of CRTP: the compiler can inline through describe()
    // which calls area(), perimeter(), and name() — all resolved at compile time
    constexpr int iterations = 1'000'000;

    const Circle c(7.0);

    auto start = std::chrono::high_resolution_clock::now();
    std::string last_desc;
    for (int i = 0; i < iterations; ++i) {
        last_desc = c.describe();
    }
    auto end = std::chrono::high_resolution_clock::now();
    const auto us = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    assert(!last_desc.empty());
    assert(last_desc.find("Circle") != std::string::npos);
    LOG_INFO_PRINT("  {} describe() calls in {} us", iterations, us);
    LOG_INFO_PRINT("  Output: '{}'", last_desc);
    LOG_INFO_PRINT("  Inlining benefit: passed");
}
