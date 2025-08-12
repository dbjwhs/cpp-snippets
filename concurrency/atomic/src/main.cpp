// MIT License
// Copyright (c) 2025 dbjwhs

//
// Atomic Operations with Memory Ordering Semantics
//
// This pattern represents one of the most fundamental low-level synchronization mechanisms in modern
// concurrent programming. Atomic operations were first formalized in the academic literature in the 1970s
// and 1980s, with early work by Leslie Lamport on sequential consistency and happens-before relationships.
// The concept gained practical importance with the rise of multi-core processors in the 2000s.
//
// The C++11 standard introduced atomic operations to the language, providing a standardized interface
// for lock-free programming. The memory ordering model, based on the C++ memory model defined by
// Hans Boehm and others, allows precise control over how memory operations are ordered across threads.
//
// This pattern enables lock-free data structures and algorithms, which are crucial for high-performance
// systems where traditional mutex-based synchronization would create bottlenecks. The memory ordering
// semantics (relaxed, acquire, release, acq_rel, seq_cst) provide different guarantees about the
// visibility and ordering of memory operations across threads.
//
// Common usage patterns include:
// - Producer-consumer queues with acquire-release semantics
// - Reference counting in smart pointers
// - Flags for thread synchronization
// - Lock-free data structures (stacks, queues, hash tables)
// - Memory barriers for ensuring operation ordering
//

#include "../../../headers/project_utils.hpp"
#include "../headers/atomic_operations.hpp"
#include <thread>
#include <vector>
#include <chrono>
#include <random>
#include <algorithm>
#include <cassert>

using namespace atomic_examples;

// test atomic counter with multiple threads
void test_atomic_counter() {
    LOG_INFO_PRINT("testing atomic counter operations...");
    
    AtomicCounter counter;
    constexpr int num_threads = 4;
    constexpr int increments_per_thread = 1000;
    
    std::vector<std::thread> threads;
    threads.reserve(num_threads);
    
    // create threads that increment the counter
    for (int ndx = 0; ndx < num_threads; ++ndx) {
        threads.emplace_back([&counter]() {
            for (int tdex = 0; tdex < increments_per_thread; ++tdex) {
                if (tdex % 2 == 0) {
                    counter.increment_relaxed();
                } else {
                    counter.increment_seq_cst();
                }
            }
        });
    }
    
    // wait for all threads to complete
    for (auto& thread : threads) {
        thread.join();
    }
    
    constexpr int expected_count = num_threads * increments_per_thread;
    const int actual_count = counter.get_count();
    
    LOG_INFO_PRINT("atomic counter test - expected: {}, actual: {}", expected_count, actual_count);
    assert(actual_count == expected_count);
    LOG_INFO_PRINT("atomic counter test passed!");
}

// test producer-consumer with acquire-release semantics
void test_producer_consumer() {
    LOG_INFO_PRINT("testing producer-consumer with acquire-release semantics...");
    
    constexpr size_t buffer_size = 100;
    constexpr int items_to_produce = 1000;
    
    LockFreeProducerConsumer<int> pc_queue{buffer_size};
    std::atomic<bool> producer_done{false};
    std::atomic<int> items_consumed{0};
    
    // producer thread
    std::thread producer([&pc_queue, &producer_done, items_to_produce]() {
        std::random_device rd;
        std::mt19937 gen{rd()};
        std::uniform_int_distribution<int> dist{1, 1000};
        
        for (int ndx = 0; ndx < items_to_produce; ++ndx) {
            const int item = dist(gen);
            
            // keep trying until successful
            while (true) {
                if (auto result = pc_queue.produce(item); result.has_value()) {
                    break;
                }
                // brief pause if buffer is full
                std::this_thread::sleep_for(std::chrono::microseconds{1});
            }
        }
        
        producer_done.store(true, std::memory_order_release);
        LOG_INFO_PRINT("producer finished producing {} items", items_to_produce);
    });
    
    // consumer thread
    std::thread consumer([&pc_queue, &producer_done, &items_consumed]() {
        int consumed_count = 0;
        
        while (true) {
            if (auto result = pc_queue.consume(); result.has_value()) {
                consumed_count++;
                // simulate some work with the consumed item
                [[maybe_unused]] const int item = result.value();
            } else {
                // check if producer is done and queue is empty
                if (producer_done.load(std::memory_order_acquire) && pc_queue.is_empty()) {
                    break;
                }
                // brief pause if buffer is empty
                std::this_thread::yield();
            }
        }
        
        items_consumed.store(consumed_count, std::memory_order_release);
        LOG_INFO_PRINT("consumer finished consuming {} items", consumed_count);
    });
    
    producer.join();
    consumer.join();
    
    const int consumed_count = items_consumed.load(std::memory_order_acquire);
    LOG_INFO_PRINT("producer-consumer test - produced: {}, consumed: {}", items_to_produce, consumed_count);
    assert(consumed_count == items_to_produce);
    LOG_INFO_PRINT("producer-consumer test passed!");
}

// test thread synchronization using atomic flags
void test_thread_synchronization() {
    LOG_INFO_PRINT("testing thread synchronization with atomic flags...");
    
    ThreadSynchronizer synchronizer;
    constexpr int num_workers = 6;
    std::atomic<int> work_completed{0};
    
    std::vector<std::thread> workers;
    workers.reserve(num_workers);
    
    // create worker threads that wait for signal
    for (int ndx = 0; ndx < num_workers; ++ndx) {
        workers.emplace_back([&synchronizer, &work_completed, ndx]() {
            LOG_INFO_PRINT("worker {} waiting for start signal...", ndx);
            
            // wait for start signal
            synchronizer.wait_for_start();
            
            LOG_INFO_PRINT("worker {} starting work...", ndx);
            
            // simulate some work
            std::this_thread::sleep_for(std::chrono::milliseconds{10 + (ndx * 5)});
            
            // increment work counter
            work_completed.fetch_add(1, std::memory_order_relaxed);
            
            // signal completion
            synchronizer.signal_completion();
            
            LOG_INFO_PRINT("worker {} completed work", ndx);
        });
    }
    
    // let workers start and wait
    std::this_thread::sleep_for(std::chrono::milliseconds{50});
    
    LOG_INFO_PRINT("sending start signal to all workers...");
    synchronizer.signal_start();
    
    // wait for all workers to complete
    for (auto& worker : workers) {
        worker.join();
    }
    
    const int completed_work = work_completed.load(std::memory_order_acquire);
    const int completion_signals = synchronizer.get_completion_count();
    
    LOG_INFO_PRINT("synchronization test - workers: {}, completed: {}, signals: {}", 
                   num_workers, completed_work, completion_signals);
    assert(completed_work == num_workers);
    assert(completion_signals == num_workers);
    LOG_INFO_PRINT("thread synchronization test passed!");
}

// test memory ordering demonstration
void test_memory_ordering() {
    LOG_INFO_PRINT("testing memory ordering patterns...");
    
    MemoryOrderingDemo demo;
    constexpr int num_iterations = 1000;
    std::atomic<int> successful_reads{0};
    
    std::thread producer([&demo]() {
        for (int ndx = 0; ndx < num_iterations; ++ndx) {
            demo.reset();
            
            // brief delay to let consumer potentially observe intermediate state
            std::this_thread::sleep_for(std::chrono::microseconds{1});
            
            demo.release_acquire_pattern();
            
            std::this_thread::sleep_for(std::chrono::microseconds{1});
        }
    });
    
    std::thread consumer([&demo, &successful_reads]() {
        int reads = 0;
        int attempts = 0;
        
        while (attempts < num_iterations * 10) { // more attempts to catch the data
            auto result = demo.try_consume();
            if (result.has_value()) {
                const auto [x, y] = result.value();
                // verify the data consistency from release-acquire
                if (x == 42 && y == 24) {
                    reads++;
                }
            }
            attempts++;
            std::this_thread::yield();
        }
        
        successful_reads.store(reads, std::memory_order_release);
    });
    
    producer.join();
    consumer.join();
    
    const int reads = successful_reads.load(std::memory_order_acquire);
    LOG_INFO_PRINT("memory ordering test - successful reads: {} out of {} iterations", reads, num_iterations);
    
    // we should have caught some of the data due to acquire-release semantics
    assert(reads > 0);
    LOG_INFO_PRINT("memory ordering test passed!");
}

// comprehensive stress test with error scenarios
void test_error_conditions() {
    LOG_INFO_PRINT("testing error conditions and edge cases...");
    
    // test producer-consumer buffer overflow/underflow
    {
        Logger::StderrSuppressionGuard guard; // suppress expected error output
        
        LockFreeProducerConsumer<int> small_queue{3};
        
        // fill the buffer
        auto result1 = small_queue.produce(1);
        assert(result1.has_value());
        
        auto result2 = small_queue.produce(2);
        assert(result2.has_value());
        
        // should fail - buffer full
        auto result3 = small_queue.produce(3);
        assert(!result3.has_value());
        assert(result3.error() == "buffer full");
        
        // consume one item
        auto consumed = small_queue.consume();
        assert(consumed.has_value());
        assert(consumed.value() == 1);
        
        // should succeed now
        auto result4 = small_queue.produce(4);
        assert(result4.has_value());
        
        LOG_INFO_PRINT("buffer overflow/underflow tests passed");
    }
    
    // test empty buffer consumption
    {
        Logger::StderrSuppressionGuard guard; // suppress expected error output
        
        LockFreeProducerConsumer<int> empty_queue{5};
        
        // should fail - buffer empty  
        auto result = empty_queue.consume();
        assert(!result.has_value());
        assert(result.error() == "buffer empty");
        
        LOG_INFO_PRINT("empty buffer consumption test passed");
    }
    
    LOG_INFO_PRINT("error condition tests passed!");
}

// performance comparison between different memory orderings
void performance_benchmark() {
    LOG_INFO_PRINT("running performance benchmark...");
    
    AtomicCounter counter;
    constexpr int iterations = 100000;
    constexpr int num_threads = 4;
    
    // benchmark relaxed ordering
    auto start_relaxed = std::chrono::high_resolution_clock::now();
    {
        std::vector<std::thread> threads;
        threads.reserve(num_threads);
        
        for (int ndx = 0; ndx < num_threads; ++ndx) {
            threads.emplace_back([&counter]() {
                for (int cdex = 0; cdex < iterations; ++cdex) {
                    counter.increment_relaxed();
                }
            });
        }
        
        for (auto& thread : threads) {
            thread.join();
        }
    }
    const auto end_relaxed = std::chrono::high_resolution_clock::now();
    const auto relaxed_duration = std::chrono::duration_cast<std::chrono::microseconds>(end_relaxed - start_relaxed);
    
    counter.reset();
    
    // benchmark sequential consistency
    const auto start_seq_cst = std::chrono::high_resolution_clock::now();
    {
        std::vector<std::thread> threads;
        threads.reserve(num_threads);
        
        for (int ndx = 0; ndx < num_threads; ++ndx) {
            threads.emplace_back([&counter]() {
                for (int cdex = 0; cdex < iterations; ++cdex) {
                    counter.increment_seq_cst();
                }
            });
        }
        
        for (auto& thread : threads) {
            thread.join();
        }
    }
    const auto end_seq_cst = std::chrono::high_resolution_clock::now();
    const auto seq_cst_duration = std::chrono::duration_cast<std::chrono::microseconds>(end_seq_cst - start_seq_cst);
    
    LOG_INFO_PRINT("performance comparison:");
    LOG_INFO_PRINT("  relaxed ordering: {} microseconds", relaxed_duration.count());
    LOG_INFO_PRINT("  seq_cst ordering: {} microseconds", seq_cst_duration.count());
    
    const double speedup = static_cast<double>(seq_cst_duration.count()) / static_cast<double>(relaxed_duration.count());
    LOG_INFO_PRINT("  relaxed is {:.2f}x faster than seq_cst", speedup);
}

int main() {
    LOG_INFO_PRINT("starting atomic operations demonstration and testing...");
    LOG_INFO_PRINT("========================================================");
    
    try {
        // run comprehensive tests
        test_atomic_counter();
        LOG_INFO_PRINT("");
        
        test_producer_consumer();
        LOG_INFO_PRINT("");
        
        test_thread_synchronization();
        LOG_INFO_PRINT("");
        
        test_memory_ordering();
        LOG_INFO_PRINT("");
        
        test_error_conditions();
        LOG_INFO_PRINT("");
        
        performance_benchmark();
        LOG_INFO_PRINT("");
        
        LOG_INFO_PRINT("========================================================");
        LOG_INFO_PRINT("all atomic operations tests passed successfully!");
        LOG_INFO_PRINT("demonstrated memory ordering semantics:");
        LOG_INFO_PRINT("  - relaxed: maximum performance, no ordering guarantees");
        LOG_INFO_PRINT("  - acquire: synchronizes-with release operations");
        LOG_INFO_PRINT("  - release: makes writes visible to acquire operations");
        LOG_INFO_PRINT("  - acq_rel: combines acquire and release semantics");
        LOG_INFO_PRINT("  - seq_cst: strongest guarantee, total ordering");
        
    } catch (const std::exception& ex) {
        LOG_ERROR_PRINT("test failed with exception: {}", ex.what());
        return 1;
    }
    
    return 0;
}
