// MIT License
// Copyright (c) 2025 dbjwhs

#include "happens_before_demo.hpp"
#include "project_utils.hpp"
#include <print>
#include <cassert>
#include <thread>
#include <vector>

//
// happens-before relationship
//
// the happens-before relationship is the fundamental ordering guarantee in the c++ memory model.
// when operation a happens-before operation b, all side effects of a are guaranteed to be visible
// to b. this prevents data races and enables safe communication between threads.
//
// key mechanisms that establish happens-before:
// 1. sequenced-before: operations within a single thread execute in program order
// 2. synchronizes-with: mutex unlock happens-before subsequent lock, atomic release happens-before
//    acquire load, thread creation happens-before thread entry, thread completion happens-before join
// 3. transitivity: if a happens-before b and b happens-before c, then a happens-before c
//
// this demonstration provides comprehensive examples of all memory orderings (seq_cst, release-acquire,
// relaxed, acq_rel) and common synchronization patterns (message passing, fences, thread sync).
//

using namespace HappensBefore;

//
// test sequential consistency guarantees
// with seq_cst, at least one thread must observe the other's write
//
auto test_sequential_consistency() -> bool {
    LOG_INFO_PRINT("testing sequential consistency ordering...");

    SequentialConsistencyDemo demo{};

    constexpr int num_iterations{1000};
    int both_zero_count{0};

    for (int iteration{0}; iteration < num_iterations; ++iteration) {
        demo.reset();

        std::thread t1{[&demo]() { demo.writer_reader_1(); }};
        std::thread t2{[&demo]() { demo.writer_reader_2(); }};

        t1.join();
        t2.join();

        auto [r1, r2] = demo.get_results();

        // with seq_cst, it's impossible for both to be 0
        // because there must be a global total order
        if (r1 == 0 && r2 == 0) {
            both_zero_count++;
        }
    }

    // should never see both threads read 0 with sequential consistency
    LOG_INFO_PRINT("sequential consistency test: both_zero_count = {} (expected 0)", both_zero_count);
    assert(both_zero_count == 0);

    LOG_INFO_PRINT("sequential consistency test passed");
    return true;
}

//
// test release-acquire synchronization
// demonstrates proper data transfer between producer and consumer
//
auto test_release_acquire() -> bool {
    LOG_INFO_PRINT("testing release-acquire synchronization...");

    ReleaseAcquireDemo demo{};

    constexpr int test_value{12345};
    std::atomic<bool> producer_done{false};
    int consumed_value{0};

    std::thread producer{[&demo, &producer_done]() {
        demo.produce(test_value);
        producer_done.store(true, std::memory_order_release);
    }};

    std::thread consumer{[&demo, &consumed_value, &producer_done]() {
        // wait for producer to signal
        while (!producer_done.load(std::memory_order_acquire)) {
            std::this_thread::yield();
        }

        auto result = demo.consume();
        assert(result.has_value());
        consumed_value = result.value();
    }};

    producer.join();
    consumer.join();

    LOG_INFO_PRINT("release-acquire test: produced = {}, consumed = {}", test_value, consumed_value);
    assert(consumed_value == test_value);

    LOG_INFO_PRINT("release-acquire test passed");
    return true;
}

//
// test relaxed ordering
// demonstrates that relaxed operations provide atomicity but no ordering
//
auto test_relaxed_ordering() -> bool {
    LOG_INFO_PRINT("testing relaxed memory ordering...");

    RelaxedOrderingDemo demo{};

    constexpr int num_threads{4};
    constexpr int increments_per_thread{10000};

    std::vector<std::thread> threads{};
    threads.reserve(num_threads);

    for (int ndx{0}; ndx < num_threads; ++ndx) {
        threads.emplace_back([&demo]() {
            for (int count{0}; count < increments_per_thread; ++count) {
                demo.increment();
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    int final_count = demo.get_count();
    int expected_count = num_threads * increments_per_thread;

    LOG_INFO_PRINT("relaxed ordering test: final_count = {}, expected = {}", final_count, expected_count);

    // relaxed still guarantees atomicity of individual operations
    assert(final_count == expected_count);

    LOG_INFO_PRINT("relaxed ordering test passed");
    return true;
}

//
// test thread synchronization happens-before relationships
// demonstrates thread creation and join synchronization points
//
auto test_thread_sync() -> bool {
    LOG_INFO_PRINT("testing thread synchronization happens-before...");

    ThreadSyncDemo demo{};

    auto result = demo.run();
    assert(result.has_value());

    int final_value = result.value();
    int expected_value = 42 * 2;

    LOG_INFO_PRINT("thread sync test: final_value = {}, expected = {}", final_value, expected_value);
    assert(final_value == expected_value);

    LOG_INFO_PRINT("thread synchronization test passed");
    return true;
}

//
// test fence operations
// demonstrates that fences can establish happens-before without atomic variables
//
auto test_fences() -> bool {
    LOG_INFO_PRINT("testing fence-based synchronization...");

    FenceDemo demo{};

    constexpr int test_value{99999};
    std::atomic<bool> writer_done{false};
    int read_value{0};

    std::thread writer{[&demo, &writer_done]() {
        demo.write_with_fence(test_value);
        writer_done.store(true, std::memory_order_release);
    }};

    std::thread reader{[&demo, &read_value, &writer_done]() {
        // wait for writer to complete
        while (!writer_done.load(std::memory_order_acquire)) {
            std::this_thread::yield();
        }

        auto result = demo.read_with_fence();
        assert(result.has_value());
        read_value = result.value();
    }};

    writer.join();
    reader.join();

    LOG_INFO_PRINT("fence test: written = {}, read = {}", test_value, read_value);
    assert(read_value == test_value);

    LOG_INFO_PRINT("fence test passed");
    return true;
}

//
// test message passing pattern
// demonstrates safe data transfer using release-acquire
//
auto test_message_passing() -> bool {
    LOG_INFO_PRINT("testing message passing pattern...");

    struct Message {
        int id{};
        std::array<int, 4> data{};
    };

    MessagePassing<Message> mailbox{};

    Message sent_msg{42, {1, 2, 3, 4}};
    Message received_msg{};

    std::thread sender{[&mailbox, &sent_msg]() {
        mailbox.send(sent_msg);
    }};

    std::thread receiver{[&mailbox, &received_msg]() {
        // spin until message is ready
        while (true) {
            auto result = mailbox.receive();
            if (result.has_value()) {
                received_msg = result.value();
                break;
            }
            std::this_thread::yield();
        }
    }};

    sender.join();
    receiver.join();

    LOG_INFO_PRINT("message passing test: sent id = {}, received id = {}", sent_msg.id, received_msg.id);
    assert(sent_msg.id == received_msg.id);

    bool data_matches{true};
    for (std::size_t ndx{0}; ndx < sent_msg.data.size(); ++ndx) {
        if (sent_msg.data[ndx] != received_msg.data[ndx]) {
            data_matches = false;
            break;
        }
    }

    assert(data_matches);
    LOG_INFO_PRINT("message passing test passed");
    return true;
}

//
// test transitivity of happens-before
// demonstrates that happens-before relationships chain transitively
//
auto test_transitivity() -> bool {
    LOG_INFO_PRINT("testing happens-before transitivity...");

    TransitivityDemo demo{};

    std::thread t1{[&demo]() { demo.step1(); }};
    std::thread t2{[&demo]() { demo.step2(); }};

    std::array<int, 3> results{};
    std::thread t3{[&demo, &results]() {
        auto result = demo.step3();
        assert(result.has_value());
        results = result.value();
    }};

    t1.join();
    t2.join();
    t3.join();

    LOG_INFO_PRINT("transitivity test: value1 = {}, value2 = {}, value3 = {}",
                   results[0], results[1], results[2]);

    // verify the chain of dependencies
    assert(results[0] == 100);
    assert(results[1] == 150);
    assert(results[2] == 250);

    LOG_INFO_PRINT("transitivity test passed");
    return true;
}

//
// test acquire-release ordering
// demonstrates read-modify-write operations with acq_rel
//
auto test_acq_rel() -> bool {
    LOG_INFO_PRINT("testing acquire-release ordering...");

    AcqRelDemo demo{};

    constexpr int num_threads{8};
    constexpr int modifications_per_thread{100};

    std::vector<std::thread> threads{};
    threads.reserve(num_threads);

    for (int ndx{0}; ndx < num_threads; ++ndx) {
        threads.emplace_back([&demo]() {
            for (int count{0}; count < modifications_per_thread; ++count) {
                demo.modify_value(1);
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    int final_value = demo.get_value();
    int final_count = demo.get_modification_count();
    int expected_modifications = num_threads * modifications_per_thread;

    LOG_INFO_PRINT("acq_rel test: final_value = {}, modification_count = {}, expected_modifications = {}",
                   final_value, final_count, expected_modifications);

    // all modifications should be accounted for
    assert(final_count == expected_modifications);

    LOG_INFO_PRINT("acquire-release test passed");
    return true;
}

//
// comprehensive stress test
// runs multiple happens-before patterns concurrently
//
auto test_comprehensive_stress() -> bool {
    LOG_INFO_PRINT("running comprehensive stress test...");

    constexpr int iterations{100};
    bool all_passed{true};

    for (int ndx{0}; ndx < iterations; ++ndx) {
        // run multiple patterns in parallel
        std::vector<std::thread> test_threads{};
        test_threads.reserve(3);

        std::atomic<int> tests_passed{0};

        test_threads.emplace_back([&tests_passed]() {
            ReleaseAcquireDemo demo{};
            demo.produce(42);
            auto result = demo.consume();
            if (result.has_value() && result.value() == 42) {
                tests_passed.fetch_add(1, std::memory_order_relaxed);
            }
        });

        test_threads.emplace_back([&tests_passed]() {
            RelaxedOrderingDemo demo{};
            for (int count{0}; count < 100; ++count) {
                demo.increment();
            }
            if (demo.get_count() == 100) {
                tests_passed.fetch_add(1, std::memory_order_relaxed);
            }
        });

        test_threads.emplace_back([&tests_passed]() {
            FenceDemo demo{};
            demo.write_with_fence(123);
            auto result = demo.read_with_fence();
            if (result.has_value() && result.value() == 123) {
                tests_passed.fetch_add(1, std::memory_order_relaxed);
            }
        });

        for (auto& thread : test_threads) {
            thread.join();
        }

        if (tests_passed.load(std::memory_order_relaxed) != 3) {
            all_passed = false;
            LOG_ERROR_PRINT("stress test iteration {} failed", ndx);
            break;
        }
    }

    if (all_passed) {
        LOG_INFO_PRINT("comprehensive stress test passed ({} iterations)", iterations);
    }

    assert(all_passed);
    return all_passed;
}

//
// main test driver
// executes all happens-before demonstrations and tests
//
auto main() -> int {
    LOG_INFO_PRINT("=== happens-before guarantees demonstration ===");
    LOG_INFO_PRINT("");

    try {
        bool all_tests_passed{true};

        // run all test suites
        all_tests_passed &= test_sequential_consistency();
        LOG_INFO_PRINT("");

        all_tests_passed &= test_release_acquire();
        LOG_INFO_PRINT("");

        all_tests_passed &= test_relaxed_ordering();
        LOG_INFO_PRINT("");

        all_tests_passed &= test_thread_sync();
        LOG_INFO_PRINT("");

        all_tests_passed &= test_fences();
        LOG_INFO_PRINT("");

        all_tests_passed &= test_message_passing();
        LOG_INFO_PRINT("");

        all_tests_passed &= test_transitivity();
        LOG_INFO_PRINT("");

        all_tests_passed &= test_acq_rel();
        LOG_INFO_PRINT("");

        all_tests_passed &= test_comprehensive_stress();
        LOG_INFO_PRINT("");

        if (all_tests_passed) {
            LOG_INFO_PRINT("=== all happens-before tests passed ===");
            return 0;
        }
        else {
            LOG_ERROR_PRINT("=== some happens-before tests failed ===");
            return 1;
        }
    }
    catch (const std::exception& e) {
        LOG_ERROR_PRINT("exception caught in main: {}", e.what());
        return 1;
    }
    catch (...) {
        LOG_ERROR_PRINT("unknown exception caught in main");
        return 1;
    }
}
