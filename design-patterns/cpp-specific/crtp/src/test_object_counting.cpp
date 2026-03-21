// MIT License
// Copyright (c) 2025 dbjwhs

#include "../headers/crtp.hpp"
#include "../../../../headers/project_utils.hpp"
#include <cassert>
#include <vector>
#include <memory>
#include <thread>

using namespace crtp_pattern;

void test_basic_counting();
void test_copy_and_move_counting();
void test_independent_counters();
void test_scope_lifetime();
void test_vector_counting();
void test_unique_ptr_counting();
void test_thread_safe_counting();

int main() {
    LOG_INFO_PRINT("=== Object Counting Test Suite ===");

    try {
        test_basic_counting();
        test_copy_and_move_counting();
        test_independent_counters();
        test_scope_lifetime();
        test_vector_counting();
        test_unique_ptr_counting();
        test_thread_safe_counting();

        LOG_INFO_PRINT("=== All object counting tests passed ===");
        return 0;

    } catch (const std::exception& e) {
        LOG_ERROR_PRINT("Test failed with exception: {}", e.what());
        return 1;
    }
}

void test_basic_counting() {
    LOG_INFO_PRINT("\n--- Basic Counting ---");

    Widget::reset_count();
    assert(Widget::count() == 0);

    {
        Widget w1("first");
        assert(Widget::count() == 1);

        Widget w2("second");
        assert(Widget::count() == 2);

        Widget w3("third");
        assert(Widget::count() == 3);
    }

    assert(Widget::count() == 0);
    LOG_INFO_PRINT("  Create and destroy: passed");
}

void test_copy_and_move_counting() {
    LOG_INFO_PRINT("\n--- Copy and Move Counting ---");

    Widget::reset_count();

    {
        Widget w1("original");
        assert(Widget::count() == 1);

        // copy constructor — new object, count goes up
        auto w2 = w1;
        assert(Widget::count() == 2);
        assert(w2.name() == "original");
        LOG_INFO_PRINT("  Copy increments: passed");

        // move constructor — new object created (old one still exists until scope end)
        auto w3 = std::move(w2);
        assert(Widget::count() == 3);
        LOG_INFO_PRINT("  Move increments: passed");
    }

    assert(Widget::count() == 0);
    LOG_INFO_PRINT("  All destroyed after scope: passed");
}

void test_independent_counters() {
    LOG_INFO_PRINT("\n--- Independent Counters ---");

    Widget::reset_count();
    Gadget::reset_count();

    {
        Widget w1("w1");
        Widget w2("w2");
        Widget w3("w3");
        Gadget g1(1);
        Gadget g2(2);

        assert(Widget::count() == 3);
        assert(Gadget::count() == 2);
        LOG_INFO_PRINT("  Different types have independent counts: passed");
    }

    assert(Widget::count() == 0);
    assert(Gadget::count() == 0);
    LOG_INFO_PRINT("  Both reset to zero: passed");

    // creating one type doesn't affect the other
    {
        Widget w("solo");
        assert(Widget::count() == 1);
        assert(Gadget::count() == 0);
    }
    LOG_INFO_PRINT("  No cross-type interference: passed");
}

void test_scope_lifetime() {
    LOG_INFO_PRINT("\n--- Scope Lifetime ---");

    Widget::reset_count();

    {
        Widget w1("outer");
        assert(Widget::count() == 1);

        {
            Widget w2("middle");
            assert(Widget::count() == 2);

            {
                Widget w3("inner");
                assert(Widget::count() == 3);
            }
            assert(Widget::count() == 2);
        }
        assert(Widget::count() == 1);
    }
    assert(Widget::count() == 0);
    LOG_INFO_PRINT("  Nested scopes: passed");
}

void test_vector_counting() {
    LOG_INFO_PRINT("\n--- Vector Counting ---");

    Widget::reset_count();

    {
        std::vector<Widget> widgets;
        widgets.reserve(10); // prevent reallocation copies

        for (int i = 0; i < 10; ++i) {
            widgets.emplace_back(std::format("widget_{}", i));
        }
        assert(Widget::count() == 10);
        LOG_INFO_PRINT("  10 emplaced: count={}", Widget::count());

        // erase some
        widgets.erase(widgets.begin(), widgets.begin() + 3);
        assert(Widget::count() == 7);
        LOG_INFO_PRINT("  After erasing 3: count={}", Widget::count());

        widgets.clear();
        assert(Widget::count() == 0);
        LOG_INFO_PRINT("  After clear: count={}", Widget::count());
    }

    assert(Widget::count() == 0);
    LOG_INFO_PRINT("  Vector lifecycle: passed");
}

void test_unique_ptr_counting() {
    LOG_INFO_PRINT("\n--- unique_ptr Counting ---");

    Widget::reset_count();

    {
        auto w1 = std::make_unique<Widget>("heap1");
        assert(Widget::count() == 1);

        auto w2 = std::make_unique<Widget>("heap2");
        assert(Widget::count() == 2);

        w1.reset(); // destroys the Widget
        assert(Widget::count() == 1);
        LOG_INFO_PRINT("  unique_ptr reset: passed");

        auto w3 = std::move(w2); // transfers ownership, no new Widget
        assert(Widget::count() == 1);
        LOG_INFO_PRINT("  unique_ptr move (no count change): passed");
    }

    assert(Widget::count() == 0);
    LOG_INFO_PRINT("  Heap cleanup: passed");
}

void test_thread_safe_counting() {
    LOG_INFO_PRINT("\n--- Thread-Safe Counting ---");

    Widget::reset_count();

    constexpr int threads_count = 8;
    constexpr int widgets_per_thread = 100;

    {
        std::vector<std::thread> threads;
        std::vector<std::vector<Widget>> thread_widgets(threads_count);

        for (int t = 0; t < threads_count; ++t) {
            threads.emplace_back([&thread_widgets, t]() {
                thread_widgets[t].reserve(widgets_per_thread);
                for (int i = 0; i < widgets_per_thread; ++i) {
                    thread_widgets[t].emplace_back(std::format("t{}_w{}", t, i));
                }
            });
        }

        for (auto& thread : threads) {
            thread.join();
        }

        assert(Widget::count() == threads_count * widgets_per_thread);
        LOG_INFO_PRINT("  {} threads x {} widgets = {} total: passed",
            threads_count, widgets_per_thread, Widget::count());
    }

    assert(Widget::count() == 0);
    LOG_INFO_PRINT("  All threads cleaned up: passed");
}
