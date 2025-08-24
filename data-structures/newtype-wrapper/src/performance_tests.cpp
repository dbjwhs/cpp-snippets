// MIT License
// Copyright (c) 2025 dbjwhs

#include <print>
#include <cassert>
#include <chrono>
#include <vector>
#include <random>
#include <algorithm>
#include <numeric>

#include "../headers/strong_type.hpp"
#include <project_utils.hpp>

// performance testing namespace
namespace PerformanceTests {
    
    // timer utility for performance measurements
    class Timer {
    private:
        std::chrono::high_resolution_clock::time_point m_start{};
        
    public:
        Timer() : m_start{std::chrono::high_resolution_clock::now()} {}
        
        [[nodiscard]] double elapsed_ms() const {
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - m_start);
            return static_cast<double>(duration.count()) / 1000.0;
        }
    };
    
    // test data size for performance benchmarks
    constexpr size_t PERFORMANCE_TEST_SIZE = 1000000;
    
    // performance test for construction and destruction
    void test_construction_performance() {
        LOG_INFO_PRINT("testing construction and destruction performance");
        
        Timer timer;
        
        // test strong type construction performance
        {
            std::vector<StrongTypes::Integer> strong_integers;
            strong_integers.reserve(PERFORMANCE_TEST_SIZE);
            
            for (size_t ndx = 0; ndx < PERFORMANCE_TEST_SIZE; ++ndx) {
                strong_integers.emplace_back(StrongTypes::Integer{static_cast<int>(ndx)});
            }
        } // destruction happens here
        
        double strong_type_time = timer.elapsed_ms();
        LOG_INFO_PRINT("strong type construction/destruction: {:.2f} ms for {} elements", 
                      strong_type_time, PERFORMANCE_TEST_SIZE);
        
        // test raw type construction for comparison
        Timer raw_timer;
        {
            std::vector<int> raw_integers;
            raw_integers.reserve(PERFORMANCE_TEST_SIZE);
            
            for (size_t ndx = 0; ndx < PERFORMANCE_TEST_SIZE; ++ndx) {
                raw_integers.emplace_back(static_cast<int>(ndx));
            }
        } // destruction happens here
        
        double raw_type_time = raw_timer.elapsed_ms();
        LOG_INFO_PRINT("raw type construction/destruction: {:.2f} ms for {} elements", 
                      raw_type_time, PERFORMANCE_TEST_SIZE);
        
        double overhead_percentage = ((strong_type_time - raw_type_time) / raw_type_time) * 100.0;
        LOG_INFO_PRINT("overhead: {:.2f}%", overhead_percentage);
        
        // assert that overhead is minimal (less than 50%)
        assert(overhead_percentage < 50.0);
    }
    
    // performance test for arithmetic operations
    void test_arithmetic_performance() {
        LOG_INFO_PRINT("testing arithmetic operations performance");
        
        // prepare test data
        std::vector<StrongTypes::Real> strong_values;
        std::vector<double> raw_values;
        
        strong_values.reserve(PERFORMANCE_TEST_SIZE);
        raw_values.reserve(PERFORMANCE_TEST_SIZE);
        
        std::random_device rd;
        std::mt19937 gen{rd()};
        std::uniform_real_distribution<double> dist{1.0, 100.0};
        
        for (size_t ndx = 0; ndx < PERFORMANCE_TEST_SIZE; ++ndx) {
            double value = dist(gen);
            strong_values.emplace_back(StrongTypes::Real{value});
            raw_values.emplace_back(value);
        }
        
        // test strong type arithmetic performance
        Timer strong_timer;
        StrongTypes::Real strong_sum{0.0};
        for (size_t ndx = 0; ndx < PERFORMANCE_TEST_SIZE - 1; ++ndx) {
            strong_sum = strong_sum + strong_values[ndx] * strong_values[ndx + 1];
        }
        double strong_arithmetic_time = strong_timer.elapsed_ms();
        
        LOG_INFO_PRINT("strong type arithmetic: {:.2f} ms, result: {}", 
                      strong_arithmetic_time, strong_sum.value());
        
        // test raw type arithmetic for comparison
        Timer raw_timer;
        double raw_sum{0.0};
        for (size_t ndx = 0; ndx < PERFORMANCE_TEST_SIZE - 1; ++ndx) {
            raw_sum = raw_sum + raw_values[ndx] * raw_values[ndx + 1];
        }
        double raw_arithmetic_time = raw_timer.elapsed_ms();
        
        LOG_INFO_PRINT("raw type arithmetic: {:.2f} ms, result: {}", raw_arithmetic_time, raw_sum);
        
        double arithmetic_overhead = ((strong_arithmetic_time - raw_arithmetic_time) / raw_arithmetic_time) * 100.0;
        LOG_INFO_PRINT("arithmetic overhead: {:.2f}%", arithmetic_overhead);
        
        // verify results are equivalent (within floating point tolerance)
        assert(std::abs(strong_sum.value() - raw_sum) < 1e-6);
        
        // assert that arithmetic overhead is minimal (less than 100%)
        assert(arithmetic_overhead < 100.0);
    }
    
    // performance test for container operations
    void test_container_performance() {
        LOG_INFO_PRINT("testing container operations performance");
        
        // prepare test data
        std::vector<StrongTypes::Integer> strong_numbers;
        std::vector<int> raw_numbers;
        
        strong_numbers.reserve(PERFORMANCE_TEST_SIZE);
        raw_numbers.reserve(PERFORMANCE_TEST_SIZE);
        
        std::random_device rd;
        std::mt19937 gen{rd()};
        std::uniform_int_distribution<int> dist{1, 1000};
        
        for (size_t ndx = 0; ndx < PERFORMANCE_TEST_SIZE; ++ndx) {
            int value = dist(gen);
            strong_numbers.emplace_back(StrongTypes::Integer{value});
            raw_numbers.emplace_back(value);
        }
        
        // test strong type sorting performance
        Timer strong_sort_timer;
        std::sort(strong_numbers.begin(), strong_numbers.end());
        double strong_sort_time = strong_sort_timer.elapsed_ms();
        
        LOG_INFO_PRINT("strong type sorting: {:.2f} ms for {} elements", 
                      strong_sort_time, PERFORMANCE_TEST_SIZE);
        
        // test raw type sorting for comparison
        Timer raw_sort_timer;
        std::sort(raw_numbers.begin(), raw_numbers.end());
        double raw_sort_time = raw_sort_timer.elapsed_ms();
        
        LOG_INFO_PRINT("raw type sorting: {:.2f} ms for {} elements", 
                      raw_sort_time, PERFORMANCE_TEST_SIZE);
        
        double sort_overhead = ((strong_sort_time - raw_sort_time) / raw_sort_time) * 100.0;
        LOG_INFO_PRINT("sorting overhead: {:.2f}%", sort_overhead);
        
        // verify both vectors are sorted correctly
        assert(std::is_sorted(strong_numbers.begin(), strong_numbers.end()));
        assert(std::is_sorted(raw_numbers.begin(), raw_numbers.end()));
        
        // assert that sorting overhead is reasonable (less than 200%)
        assert(sort_overhead < 200.0);
        
        // test accumulate performance
        Timer strong_acc_timer;
        auto strong_total = std::accumulate(strong_numbers.begin(), strong_numbers.end(), 
                                          StrongTypes::Integer{0});
        double strong_acc_time = strong_acc_timer.elapsed_ms();
        
        Timer raw_acc_timer;
        auto raw_total = std::accumulate(raw_numbers.begin(), raw_numbers.end(), 0);
        double raw_acc_time = raw_acc_timer.elapsed_ms();
        
        LOG_INFO_PRINT("strong type accumulate: {:.2f} ms, result: {}", 
                      strong_acc_time, strong_total.value());
        LOG_INFO_PRINT("raw type accumulate: {:.2f} ms, result: {}", raw_acc_time, raw_total);
        
        // verify results are equal
        assert(strong_total.value() == raw_total);
        
        double acc_overhead = ((strong_acc_time - raw_acc_time) / raw_acc_time) * 100.0;
        LOG_INFO_PRINT("accumulate overhead: {:.2f}%", acc_overhead);
    }
    
    // performance test for hash operations
    void test_hash_performance() {
        LOG_INFO_PRINT("testing hash operations performance");
        
        constexpr size_t HASH_TEST_SIZE = 100000;
        
        // prepare test data
        std::vector<StrongTypes::UserId> user_ids;
        std::vector<std::size_t> raw_ids;
        
        user_ids.reserve(HASH_TEST_SIZE);
        raw_ids.reserve(HASH_TEST_SIZE);
        
        for (size_t ndx = 0; ndx < HASH_TEST_SIZE; ++ndx) {
            user_ids.emplace_back(StrongTypes::UserId{ndx});
            raw_ids.emplace_back(ndx);
        }
        
        // test strong type hashing performance
        Timer strong_hash_timer;
        std::hash<StrongTypes::UserId> strong_hasher;
        std::size_t strong_hash_sum{0};
        for (const auto& user_id : user_ids) {
            strong_hash_sum ^= strong_hasher(user_id);
        }
        double strong_hash_time = strong_hash_timer.elapsed_ms();
        
        LOG_INFO_PRINT("strong type hashing: {:.2f} ms for {} elements, checksum: {}", 
                      strong_hash_time, HASH_TEST_SIZE, strong_hash_sum);
        
        // test raw type hashing for comparison
        Timer raw_hash_timer;
        std::hash<std::size_t> raw_hasher;
        std::size_t raw_hash_sum{0};
        for (const auto& id : raw_ids) {
            raw_hash_sum ^= raw_hasher(id);
        }
        double raw_hash_time = raw_hash_timer.elapsed_ms();
        
        LOG_INFO_PRINT("raw type hashing: {:.2f} ms for {} elements, checksum: {}", 
                      raw_hash_time, HASH_TEST_SIZE, raw_hash_sum);
        
        // verify hash consistency
        assert(strong_hash_sum == raw_hash_sum);
        
        double hash_overhead = ((strong_hash_time - raw_hash_time) / raw_hash_time) * 100.0;
        LOG_INFO_PRINT("hashing overhead: {:.2f}%", hash_overhead);
        
        // assert that hashing overhead is minimal (less than 50%)
        assert(hash_overhead < 50.0);
    }
    
    // run all performance tests
    void run_performance_tests() {
        LOG_INFO_PRINT("starting performance tests");
        LOG_INFO_PRINT("============================");
        
        test_construction_performance();
        LOG_INFO_PRINT("");
        
        test_arithmetic_performance();
        LOG_INFO_PRINT("");
        
        test_container_performance();
        LOG_INFO_PRINT("");
        
        test_hash_performance();
        LOG_INFO_PRINT("");
        
        LOG_INFO_PRINT("============================");
        LOG_INFO_PRINT("all performance tests completed successfully!");
    }
}

// uncomment to run performance tests standalone
/*
int main() {
    try {
        PerformanceTests::run_performance_tests();
        return 0;
    } catch (const std::exception& e) {
        LOG_ERROR_PRINT("exception in performance tests: {}", e.what());
        return 1;
    } catch (...) {
        LOG_ERROR_PRINT("unknown exception in performance tests");
        return 1;
    }
}
*/