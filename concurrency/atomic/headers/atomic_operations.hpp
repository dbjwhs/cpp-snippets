// MIT License
// Copyright (c) 2025 dbjwhs

#pragma once

#include <atomic>
#include <memory>
#include <thread>
#include <expected>

//
// Atomic Operations with Memory Ordering Semantics
//
// This pattern represents one of the most fundamental low-level synchronization mechanisms in modern
// concurrent programming. Atomic operations were first formalized in the academic literature in the 1970s
// and 1980s, with early work by Leslie Lamport on sequential consistency and happens-before relationships.
// The concept gained practical importance with the rise of multicore processors in the 2000s.
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

namespace atomic_examples {

    //
    // AtomicCounter - demonstrates different memory ordering semantics for simple atomic operations
    //
    // This class showcases the performance vs. guarantee tradeoffs in atomic operations:
    // - relaxed ordering: maximum performance, no synchronization guarantees between threads
    // - sequential consistency: strongest guarantees, potential performance cost
    //
    // Use cases:
    // - statistics counters where exact ordering doesn't matter (relaxed)
    // - critical counters where total ordering is required (seq_cst)
    // - performance benchmarking of different memory orderings
    //
    class AtomicCounter {
    private:
        // atomic counter using braced initialization for thread-safe counting
        // relaxed operations on this counter provide no synchronization guarantees
        // but offer maximum performance for simple increment/decrement operations
        std::atomic<int> m_count{0};
        
    public:
        //
        // increment using relaxed memory ordering
        //
        // relaxed ordering provides no synchronization or ordering guarantees:
        // - fastest atomic operation possible
        // - no memory barriers inserted
        // - other threads may observe this increment at any time relative to other memory operations
        // - perfect for statistics counters where exact ordering doesn't matter
        //
        void increment_relaxed() {
            m_count.fetch_add(1, std::memory_order_relaxed);
        }
        
        //
        // increment using sequential consistency memory ordering
        //
        // sequential consistency provides the strongest guarantees:
        // - all threads observe a single total order of all seq_cst operations
        // - acts as both acquire and release operation
        // - creates memory barriers that can impact performance
        // - easiest to reason about but potentially slowest
        // - use when correctness is more important than performance
        //
        void increment_seq_cst() {
            m_count.fetch_add(1, std::memory_order_seq_cst);
        }
        
        //
        // get current count value using relaxed memory ordering
        //
        // returns the current counter value without any synchronization guarantees:
        // - may not reflect the most recent increments from other threads
        // - suitable for approximate readings where exact precision isn't critical
        // - marked [[nodiscard]] to prevent ignoring the return value
        // - const and noexcept for safe usage in const contexts
        //
        [[nodiscard]] int get_count() const noexcept {
            return m_count.load(std::memory_order_relaxed);
        }
        
        //
        // reset counter to zero using relaxed memory ordering
        //
        // sets the counter back to zero without synchronization guarantees:
        // - other threads may see the reset at any time relative to other operations
        // - typically used for test setup or periodic counter resets
        // - relaxed ordering is sufficient since reset is usually not time-critical
        //
        void reset() noexcept {
            m_count.store(0, std::memory_order_relaxed);
        }
    };

    //
    // LockFreeProducerConsumer - a thread-safe circular buffer using acquire-release semantics
    //
    // This class implements a classic lock-free producer-consumer pattern that demonstrates
    // the power of acquire-release memory ordering for synchronization without locks.
    //
    // Key Design Elements:
    // - circular buffer with one slot reserved to distinguish full from empty
    // - single producer, single consumer design (SPSC)
    // - acquire-release semantics ensure data visibility and ordering
    // - std::expected for Railway-Oriented Programming error handling
    //
    // Memory Ordering Strategy:
    // - relaxed: for loading indices when no synchronization needed
    // - acquire: when checking other thread's index (ensures visibility)
    // - release: when publishing updates (makes writes visible to other thread)
    //
    // Use Cases:
    // - high-performance message passing between threads
    // - lock-free queues in real-time systems
    // - producer-consumer pipelines in concurrent applications
    //
    template<std::copyable ElementType>
    class LockFreeProducerConsumer {
    private:
        // heap-allocated buffer for storing queue elements
        // using unique_ptr for automatic memory management and exception safety
        std::unique_ptr<ElementType[]> m_buffer;
        
        // total buffer size (actual usable size is m_size - 1 due to circular buffer design)
        // one slot is reserved to distinguish between full and empty states
        const size_t m_size;
        
        // producer index - points to next slot where producer will write
        // only the producer thread should modify this index
        // other threads may read it with appropriate memory ordering
        std::atomic<size_t> m_producer_index{0};
        
        // consumer index - points to next slot where consumer will read
        // only the consumer thread should modify this index  
        // other threads may read it with appropriate memory ordering
        std::atomic<size_t> m_consumer_index{0};
        
    public:
        //
        // constructor - creates buffer and initializes indices
        //
        // allocates buffer on heap using make_unique for exception safety
        // buffer size should be power of 2 for optimal performance (though not required)
        // actual capacity is size-1 due to circular buffer full/empty detection
        //
        explicit LockFreeProducerConsumer(const size_t size)
            : m_buffer{std::make_unique<ElementType[]>(size)}, m_size{size} {}
        
        //
        // produce - adds an item to the queue using acquire-release semantics
        //
        // This method demonstrates the producer side of acquire-release synchronization:
        // 1. loads producer index with relaxed (no sync needed, only producer writes it)
        // 2. checks consumer index with acquire (ensures we see consumer's latest position)
        // 3. stores item into buffer (regular store, no atomics needed)
        // 4. publishes producer index with release (makes item visible to consumer)
        //
        // The release store ensures that:
        // - the item store happens-before the index update
        // - consumer will see the item when it acquires the updated index
        //
        // Returns std::expected for error handling without exceptions
        //
        [[nodiscard]] std::expected<void, std::string> produce(const ElementType& item) {
            // load our current position - relaxed is safe since only producer writes this
            const size_t current_producer = m_producer_index.load(std::memory_order_relaxed);
            const size_t next_producer = (current_producer + 1) % m_size;
            
            // check if buffer would be full after this write
            // acquire ensures we see consumer's latest progress
            // if next_producer == consumer_index, buffer is full (circular buffer invariant)
            if (next_producer == m_consumer_index.load(std::memory_order_acquire)) {
                return std::unexpected("buffer full");
            }
            
            // store the item at current producer position
            // this is a regular store - the release below will ensure visibility
            m_buffer[current_producer] = item;
            
            // publish the new producer index with release semantics
            // this creates a happens-before relationship with consumers acquire
            // guarantees that item store above is visible when consumer sees this index
            m_producer_index.store(next_producer, std::memory_order_release);
            
            // return success (empty expected value)
            return {};
        }
        
        //
        // consume - removes an item from the queue using acquire-release semantics
        //
        // This method demonstrates the consumer side of acquire-release synchronization:
        // 1. loads consumer index with relaxed (no sync needed, only consumer writes it)
        // 2. checks producer index with acquire (ensures we see producer's latest items)
        // 3. reads item from buffer (regular load, acquire above ensures visibility)
        // 4. publishes consumer index with release (signals space available to producer)
        //
        // The acquire load ensures that:
        // - we see all producer writes that happened-before the producer's release
        // - the item read will be the value written by the producer
        //
        // Returns std::expected containing the item or error message
        //
        [[nodiscard]] std::expected<ElementType, std::string> consume() {
            // load our current position - relaxed is safe since only consumer writes this
            const size_t current_consumer = m_consumer_index.load(std::memory_order_relaxed);
            
            // check if buffer is empty
            // acquire ensures we see producer's latest items
            // if consumer_index == producer_index, buffer is empty
            if (current_consumer == m_producer_index.load(std::memory_order_acquire)) {
                return std::unexpected("buffer empty");
            }
            
            // read the item at current consumer position
            // the acquire above ensures this sees the producer's write
            ElementType item = m_buffer[current_consumer];
            
            // publish the new consumer index with release semantics
            // this signals to producer that this slot is now available
            // creates happens-before relationship for future producer operations
            const size_t next_consumer = (current_consumer + 1) % m_size;
            m_consumer_index.store(next_consumer, std::memory_order_release);
            
            // return the consumed item
            return item;
        }
        
        //
        // is_empty - checks if buffer is currently empty (approximate)
        //
        // This is a utility method for testing purposes - in lock-free programming,
        // the state can change between checking and acting, so this should not be
        // used for control flow in production code.
        //
        // Uses acquire ordering to ensure we see the latest state from both threads.
        // The result is only a snapshot and may be immediately outdated.
        //
        [[nodiscard]] bool is_empty() const noexcept {
            return m_consumer_index.load(std::memory_order_acquire) == 
                   m_producer_index.load(std::memory_order_acquire);
        }
    };

    //
    // ThreadSynchronizer - coordinates thread execution using atomic flags
    //
    // This class demonstrates thread synchronization patterns using atomic operations
    // instead of traditional mutex/condition variable combinations. It's particularly
    // useful for coordinating parallel work where you need:
    // - a start signal for multiple worker threads
    // - completion tracking across threads
    // - low-latency synchronization without kernel calls
    //
    // Synchronization Strategy:
    // - ready flag uses acquire-release for start coordination
    // - completion counter uses acq_rel for read-modify-write operations
    // - reset uses relaxed since no synchronization needed during cleanup
    //
    // Use Cases:
    // - parallel algorithm coordination (fork-join patterns)
    // - benchmark synchronization (start all threads simultaneously)
    // - barrier-like synchronization without heavy primitives
    //
    class ThreadSynchronizer {
    private:
        // atomic flag controlling when worker threads should begin execution
        // false = threads should wait, true = threads can start working
        // uses acquire-release semantics for proper synchronization
        std::atomic<bool> m_ready_flag{false};
        
        // atomic counter tracking how many threads have completed their work
        // incremented by each thread when it finishes
        // uses acq_rel semantics for read-modify-write operations
        std::atomic<int> m_completed_count{0};
        
    public:
        //
        // signal_start - releases all waiting threads to begin work
        //
        // Sets the ready flag to true using release semantics, which ensures that:
        // - all memory operations before this call are visible to threads that acquire the flag
        // - creates a happens-before relationship with threads calling wait_for_start()
        // - any setup work done before this call will be visible to worker threads
        //
        // Typically called by a coordinator thread after setup is complete.
        //
        void signal_start() noexcept {
            m_ready_flag.store(true, std::memory_order_release);
        }
        
        //
        // wait_for_start - blocks until start signal is received
        //
        // Spins until the ready flag becomes true using acquire semantics:
        // - acquire synchronizes-with the release in signal_start()
        // - ensures thread sees all memory writes that happened before signal_start()
        // - uses std::this_thread::yield() to be friendly to other threads
        //
        // Worker threads call this to wait for coordination.
        // In production code, consider adding exponential backoff or sleeping.
        //
        void wait_for_start() const noexcept {
            while (!m_ready_flag.load(std::memory_order_acquire)) {
                // yield to other threads - prevents busy spinning from starving other work
                // in real applications, might want exponential backoff or short sleeps
                std::this_thread::yield();
            }
        }
        
        //
        // signal_completion - increments completion counter atomically
        //
        // Uses acquire-release (acq_rel) memory ordering for the read-modify-write operation:
        // - acquire: ensures we see all previous completions and related memory writes
        // - release: makes this completion visible to threads reading the counter
        // - provides sequential consistency for completion counting
        //
        // Called by each worker thread when it finishes its assigned work.
        //
        void signal_completion() noexcept {
            m_completed_count.fetch_add(1, std::memory_order_acq_rel);
        }
        
        //
        // get_completion_count - returns current number of completed threads
        //
        // Uses acquire memory ordering to ensure we see all completion signals:
        // - synchronizes-with release operations in signal_completion()
        // - provides consistent view of completion state
        // - marked [[nodiscard]] since ignoring the count is likely a bug
        //
        // Typically used by coordinator to check if all workers have finished.
        //
        [[nodiscard]] int get_completion_count() const noexcept {
            return m_completed_count.load(std::memory_order_acquire);
        }
        
        //
        // reset - prepares synchronizer for reuse
        //
        // Resets both flag and counter to initial state using relaxed ordering:
        // - relaxed is sufficient since reset typically happens when no other threads are active
        // - allows reusing the same synchronizer object for multiple rounds
        // - should only be called when all threads are known to be inactive
        //
        void reset() noexcept {
            m_ready_flag.store(false, std::memory_order_relaxed);
            m_completed_count.store(0, std::memory_order_relaxed);
        }
    };

    //
    // MemoryOrderingDemo - demonstrates acquire-release synchronization patterns
    //
    // This class is specifically designed to test and demonstrate the crucial
    // acquire-release memory ordering pattern that forms the backbone of most
    // lock-free algorithms. It validates that memory ordering guarantees work
    // correctly under concurrent stress.
    //
    // The Test Pattern (used by test_memory_ordering):
    // 1. Producer thread: sets data values, then releases flag
    // 2. Consumer thread: acquires flag, then reads data values
    // 3. Validation: consumer should ALWAYS see consistent data when flag is set
    //
    // This proves the fundamental happens-before relationship:
    // Producer writes → Release flag → Acquire flag → Consumer reads
    //
    // Without proper memory ordering, consumer might see:
    // - flag=true but stale data (x=0, y=0)
    // - flag=true but partial updates (x=42, y=0)
    // - flag=true but reordered writes (x=0, y=24)
    //
    // With acquire-release, consumer ALWAYS sees complete, consistent data.
    //
    class MemoryOrderingDemo {
    private:
        // first data variable - will be set to 42 during test
        // uses relaxed ordering for writes since acquire-release on flag provides synchronization
        std::atomic<int> m_x{0};
        
        // second data variable - will be set to 24 during test
        // demonstrates that multiple writes are synchronized by single acquire-release pair
        std::atomic<int> m_y{0};
        
        // synchronization flag - the key to the acquire-release pattern
        // when true, indicates that data (x,y) has been completely written
        // this flag creates the happens-before relationship between producer and consumer
        std::atomic<bool> m_flag{false};
        
    public:
        //
        // relaxed_operations - demonstrates operations with no ordering guarantees
        //
        // This method shows what happens WITHOUT synchronization:
        // - stores use relaxed ordering, providing no guarantees about visibility order
        // - other threads might see these stores in any order, or not at all
        // - demonstrates the opposite of what acquire-release provides
        //
        // Used for educational comparison - shows why synchronization is needed.
        //
        void relaxed_operations() noexcept {
            // store values with no ordering guarantees
            m_x.store(1, std::memory_order_relaxed);
            m_y.store(2, std::memory_order_relaxed);
            // WARNING: other threads may see these stores in any order or not at all!
        }
        
        //
        // release_acquire_pattern - the producer side of acquire-release synchronization
        //
        // This method implements the classic "data setup + release flag" pattern:
        //
        // 1. Write data using relaxed ordering (no sync needed yet)
        // 2. Release the flag using memory_order_release
        //
        // The release operation ensures that:
        // - ALL previous writes (including x=42, y=24) happen-before the flag store
        // - Any thread that acquires this flag will see the complete data writes
        // - Creates a synchronization point that orders memory operations
        //
        // This is the fundamental building block of lock-free data structures.
        //
        void release_acquire_pattern() noexcept {
            // setup data values - these will be synchronized by the release below
            // using relaxed ordering here because the release provides the synchronization
            m_x.store(42, std::memory_order_relaxed);
            m_y.store(24, std::memory_order_relaxed);
            
            // CRITICAL: release the flag - this creates the happens-before relationship
            // the release guarantees that both x=42 and y=24 are visible to any thread
            // that subsequently acquires this flag
            m_flag.store(true, std::memory_order_release);
        }
        
        //
        // try_consume - the consumer side of acquire-release synchronization
        //
        // This method implements the classic "acquire flag + read data" pattern:
        //
        // 1. Acquire the flag using memory_order_acquire
        // 2. If flag is set, read the data values
        //
        // The acquire operation ensures that:
        // - It synchronizes-with any release operation on the same flag
        // - ALL writes that happened-before the release are now visible
        // - Data reads will see the values written by the producer
        //
        // This method is called repeatedly by test_memory_ordering() to validate
        // that the acquire-release pattern works correctly under concurrent stress.
        // Success means we ALWAYS see x=42, y=24 when flag=true.
        //
        [[nodiscard]] std::expected<std::pair<int, int>, std::string> try_consume() const noexcept {
            // CRITICAL: acquire the flag - this creates the synchronization point
            // if this load returns true, we are guaranteed to see all writes that
            // happened-before the corresponding release operation
            if (m_flag.load(std::memory_order_acquire)) {
                // read the data values - acquire above guarantees these see producer's writes
                // relaxed ordering is sufficient here because acquire provided the synchronization
                const int x_val = m_x.load(std::memory_order_relaxed);
                const int y_val = m_y.load(std::memory_order_relaxed);
                
                // return the synchronized data - should always be (42, 24) if acquire-release works
                return std::make_pair(x_val, y_val);
            }
            
            // flag not set - no data available
            return std::unexpected("flag not set");
        }
        
        //
        // reset - prepares demo for next test iteration
        //
        // Resets all values to initial state using relaxed ordering:
        // - relaxed is sufficient since reset typically happens during test setup
        // - allows reusing the same demo object for multiple test iterations
        // - should be called when no other threads are concurrently accessing the object
        //
        void reset() noexcept {
            m_x.store(0, std::memory_order_relaxed);
            m_y.store(0, std::memory_order_relaxed);
            m_flag.store(false, std::memory_order_relaxed);
        }
    };

} // namespace atomic_examples