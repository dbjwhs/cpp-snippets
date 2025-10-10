// MIT License
// Copyright (c) 2025 dbjwhs

#ifndef HAPPENS_BEFORE_DEMO_HPP
#define HAPPENS_BEFORE_DEMO_HPP

//
// comprehensive history and explanation of happens-before relationships in c++
//
// the happens-before relationship is a fundamental concept in the c++ memory model (introduced in c++11)
// that defines the ordering constraints between operations in concurrent programs. it establishes when
// one operation's effects are guaranteed to be visible to another operation, preventing data races and
// ensuring program correctness in multithreaded environments.
//
// historical context:
// prior to c++11, c++ had no formal memory model, and threading was provided by platform-specific
// libraries (pthreads, windows threads, etc.). the c++11 standard introduced a comprehensive memory
// model based on the happens-before relationship, heavily influenced by java's memory model (jsr-133)
// and academic research on weak memory models. the key contributors include hans boehm, whose work
// on "threads cannot be implemented as a library" highlighted the need for language-level support.
//
// the happens-before relationship forms a strict partial ordering on operations:
// 1. single-threaded rule: if operation a is sequenced before operation b in the same thread, then a
//    happens-before b (program order)
// 2. synchronizes-with: certain synchronization operations create happens-before relationships across
//    threads (mutex locks/unlocks, atomic operations with acquire/release semantics, thread start/join)
// 3. transitivity: if a happens-before b and b happens-before c, then a happens-before c
//
// memory orderings in c++ (from weakest to strongest):
// - memory_order_relaxed: no synchronization or ordering constraints, only atomicity guaranteed
// - memory_order_consume: data-dependency ordering (rarely used, deprecated in c++17)
// - memory_order_acquire: prevents reads/writes after this operation from being reordered before it
// - memory_order_release: prevents reads/writes before this operation from being reordered after it
// - memory_order_acq_rel: combines acquire and release semantics
// - memory_order_seq_cst: sequential consistency - all threads observe all modifications in the same order
//
// what happens-before guarantees:
// - visibility: if operation a happens-before operation b, then the effects of a are visible to b
// - no data races: properly synchronized programs (where conflicting accesses are ordered by
//   happens-before) are race-free
// - compiler and hardware reordering constraints: operations cannot be reordered across happens-before
//   boundaries in ways that would violate the relationship
//
// common usage patterns:
// 1. mutex-based synchronization: unlock on one thread happens-before lock acquisition on another
// 2. release-acquire pairs: atomic release store happens-before atomic acquire load of the same value
// 3. thread creation: operations before std::thread construction happen-before the thread's entry point
// 4. thread joining: thread completion happens-before the join() operation returns
// 5. fence operations: atomic_thread_fence establishes happens-before relationships
//
// this implementation demonstrates these concepts through carefully crafted examples that show both
// correct synchronization (with happens-before guarantees) and incorrect patterns (data races).
//

#include <atomic>
#include <thread>
#include <vector>
#include <array>
#include <chrono>
#include <expected>
#include <string>
#include <cstdint>

namespace HappensBefore {

// error types for railway-oriented programming
enum class SyncError {
    timeout,
    thread_creation_failed,
    invalid_state,
    memory_ordering_violation
};

// helper to convert error to string
[[nodiscard]] constexpr auto error_to_string(SyncError error) noexcept -> const char* {
    switch (error) {
        case SyncError::timeout:
            return "operation timed out";
        case SyncError::thread_creation_failed:
            return "failed to create thread";
        case SyncError::invalid_state:
            return "invalid synchronization state";
        case SyncError::memory_ordering_violation:
            return "memory ordering violation detected";
    }
    return "unknown error";
}

//
// demonstrates sequential consistency (strongest ordering)
// all threads observe all modifications in the same total order
//
class SequentialConsistencyDemo {
public:
    SequentialConsistencyDemo() noexcept
        : m_x{0}
        , m_y{0}
        , m_r1{0}
        , m_r2{0}
    {}

    // thread 1 writes to x, then reads y
    auto writer_reader_1() noexcept -> void {
        m_x.store(1, std::memory_order_seq_cst);
        m_r1 = m_y.load(std::memory_order_seq_cst);
    }

    // thread 2 writes to y, then reads x
    auto writer_reader_2() noexcept -> void {
        m_y.store(1, std::memory_order_seq_cst);
        m_r2 = m_x.load(std::memory_order_seq_cst);
    }

    // with seq_cst, it's impossible for both r1 and r2 to be 0
    // because there must be a total order of all seq_cst operations
    [[nodiscard]] auto get_results() const noexcept -> std::pair<int, int> {
        return {m_r1, m_r2};
    }

    auto reset() noexcept -> void {
        m_x.store(0, std::memory_order_seq_cst);
        m_y.store(0, std::memory_order_seq_cst);
        m_r1 = 0;
        m_r2 = 0;
    }

private:
    std::atomic<int> m_x;
    std::atomic<int> m_y;
    int m_r1;
    int m_r2;
};

//
// demonstrates release-acquire synchronization
// creates a happens-before relationship between release store and acquire load
//
class ReleaseAcquireDemo {
public:
    ReleaseAcquireDemo() noexcept
        : m_data{0}
        , m_ready{false}
    {}

    // producer writes data and signals readiness
    // the release store ensures all previous writes are visible to consumers
    auto produce(int value) noexcept -> void {
        m_data.store(value, std::memory_order_relaxed);

        // release: all previous writes (including to m_data) happen-before
        // any acquire load that reads true from m_ready
        m_ready.store(true, std::memory_order_release);
    }

    // consumer waits for readiness and reads data
    // the acquire load ensures we see all writes that happened-before the release
    [[nodiscard]] auto consume() noexcept -> std::expected<int, SyncError> {
        // acquire: if this reads true, then all writes that happened-before
        // the release store are visible to us
        while (!m_ready.load(std::memory_order_acquire)) {
            std::this_thread::yield();
        }

        // safe to read m_data because of happens-before relationship
        return m_data.load(std::memory_order_relaxed);
    }

    auto reset() noexcept -> void {
        m_data.store(0, std::memory_order_relaxed);
        m_ready.store(false, std::memory_order_relaxed);
    }

private:
    std::atomic<int> m_data;
    std::atomic<bool> m_ready;
};

//
// demonstrates relaxed ordering (weakest ordering)
// provides atomicity but no synchronization or ordering guarantees
//
class RelaxedOrderingDemo {
public:
    RelaxedOrderingDemo() noexcept
        : m_counter{0}
    {}

    // relaxed increment - only guarantees atomicity of the operation
    // no happens-before relationships created
    auto increment() noexcept -> void {
        m_counter.fetch_add(1, std::memory_order_relaxed);
    }

    // relaxed read - may see any value, no ordering guarantees
    [[nodiscard]] auto get_count() const noexcept -> int {
        return m_counter.load(std::memory_order_relaxed);
    }

    auto reset() noexcept -> void {
        m_counter.store(0, std::memory_order_relaxed);
    }

private:
    std::atomic<int> m_counter;
};

//
// demonstrates thread synchronization happens-before relationships
// thread creation and joining establish happens-before edges
//
class ThreadSyncDemo {
public:
    ThreadSyncDemo() noexcept
        : m_shared_data{0}
        , m_ready{false}
    {}

    // run demonstrates:
    // 1. everything before thread construction happens-before thread entry
    // 2. thread completion happens-before join() returns
    [[nodiscard]] auto run() noexcept -> std::expected<int, SyncError> {
        // write to shared data before creating thread
        m_shared_data = 42;

        // all writes above happen-before the thread's entry point
        std::thread worker([this]() {
            // safe to read m_shared_data due to happens-before from thread creation
            int local_copy = m_shared_data;

            // do some work
            local_copy *= 2;

            // write back result
            m_shared_data = local_copy;

            // all writes in thread happen-before join() returns
            m_ready.store(true, std::memory_order_release);
        });

        // wait for thread to complete
        worker.join();

        // safe to read m_shared_data because thread completion happens-before join() return
        if (!m_ready.load(std::memory_order_acquire)) {
            return std::unexpected(SyncError::invalid_state);
        }

        return m_shared_data;
    }

private:
    int m_shared_data;
    std::atomic<bool> m_ready;
};

//
// demonstrates fence operations for happens-before synchronization
// fences can establish happens-before without using atomic variables
//
class FenceDemo {
public:
    FenceDemo() noexcept
        : m_data{0}
        , m_flag{0}
    {}

    // writer uses release fence
    auto write_with_fence(int value) noexcept -> void {
        m_data = value;

        // release fence: all writes before this fence happen-before
        // an acquire operation that reads the flag we're about to set
        std::atomic_thread_fence(std::memory_order_release);

        m_flag.store(1, std::memory_order_relaxed);
    }

    // reader uses acquire fence
    [[nodiscard]] auto read_with_fence() noexcept -> std::expected<int, SyncError> {
        while (m_flag.load(std::memory_order_relaxed) == 0) {
            std::this_thread::yield();
        }

        // acquire fence: all writes that happened-before the release fence
        // are now visible to us
        std::atomic_thread_fence(std::memory_order_acquire);

        return m_data;
    }

    auto reset() noexcept -> void {
        m_data = 0;
        m_flag.store(0, std::memory_order_relaxed);
    }

private:
    int m_data;
    std::atomic<int> m_flag;
};

//
// demonstrates message passing idiom using happens-before
// a common pattern for transferring ownership of data between threads
//
template<typename MessageType>
class MessagePassing {
public:
    MessagePassing() noexcept
        : m_message{}
        , m_message_ready{false}
    {}

    // send message using release semantics
    auto send(const MessageType& msg) noexcept -> void {
        m_message = msg;

        // release: message write happens-before this store
        m_message_ready.store(true, std::memory_order_release);
    }

    // receive message using acquire semantics
    [[nodiscard]] auto receive() noexcept -> std::expected<MessageType, SyncError> {
        // acquire: see all writes that happened-before the release
        if (!m_message_ready.load(std::memory_order_acquire)) {
            return std::unexpected(SyncError::timeout);
        }

        return m_message;
    }

    auto reset() noexcept -> void {
        m_message = MessageType{};
        m_message_ready.store(false, std::memory_order_relaxed);
    }

private:
    MessageType m_message;
    std::atomic<bool> m_message_ready;
};

//
// demonstrates transitivity of happens-before
// if a hb b and b hb c, then a hb c
//
class TransitivityDemo {
public:
    TransitivityDemo() noexcept
        : m_value1{0}
        , m_value2{0}
        , m_value3{0}
        , m_step1_done{false}
        , m_step2_done{false}
    {}

    // thread 1: writes value1 and signals step1_done
    auto step1() noexcept -> void {
        m_value1 = 100;
        m_step1_done.store(true, std::memory_order_release);
    }

    // thread 2: waits for step1, writes value2, signals step2_done
    auto step2() noexcept -> void {
        while (!m_step1_done.load(std::memory_order_acquire)) {
            std::this_thread::yield();
        }

        // can safely read m_value1 due to happens-before from step1
        m_value2 = m_value1 + 50;
        m_step2_done.store(true, std::memory_order_release);
    }

    // thread 3: waits for step2, reads all values
    [[nodiscard]] auto step3() noexcept -> std::expected<std::array<int, 3>, SyncError> {
        while (!m_step2_done.load(std::memory_order_acquire)) {
            std::this_thread::yield();
        }

        // transitivity: step1 hb step2, step2 hb step3, therefore step1 hb step3
        // safe to read both m_value1 and m_value2
        m_value3 = m_value1 + m_value2;

        return std::array<int, 3>{m_value1, m_value2, m_value3};
    }

    auto reset() noexcept -> void {
        m_value1 = 0;
        m_value2 = 0;
        m_value3 = 0;
        m_step1_done.store(false, std::memory_order_relaxed);
        m_step2_done.store(false, std::memory_order_relaxed);
    }

private:
    int m_value1;
    int m_value2;
    int m_value3;
    std::atomic<bool> m_step1_done;
    std::atomic<bool> m_step2_done;
};

//
// demonstrates acq_rel ordering (acquire-release combined)
// useful for read-modify-write operations
//
class AcqRelDemo {
public:
    AcqRelDemo() noexcept
        : m_shared_value{0}
        , m_modification_count{0}
    {}

    // modify using acq_rel - both acquires previous writes and releases our writes
    auto modify_value(int delta) noexcept -> int {
        // acq_rel: acquire semantics for the read part, release for the write part
        // this means we see all previous modifications and our modification is visible to future ops
        return m_modification_count.fetch_add(1, std::memory_order_acq_rel) +
               m_shared_value.fetch_add(delta, std::memory_order_acq_rel);
    }

    [[nodiscard]] auto get_value() const noexcept -> int {
        return m_shared_value.load(std::memory_order_acquire);
    }

    [[nodiscard]] auto get_modification_count() const noexcept -> int {
        return m_modification_count.load(std::memory_order_acquire);
    }

    auto reset() noexcept -> void {
        m_shared_value.store(0, std::memory_order_relaxed);
        m_modification_count.store(0, std::memory_order_relaxed);
    }

private:
    std::atomic<int> m_shared_value;
    std::atomic<int> m_modification_count;
};

} // namespace HappensBefore

#endif // HAPPENS_BEFORE_DEMO_HPP
