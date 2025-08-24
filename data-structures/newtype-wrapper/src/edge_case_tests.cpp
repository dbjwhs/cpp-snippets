// MIT License
// Copyright (c) 2025 dbjwhs

#include <print>
#include <cassert>
#include <limits>
#include <cmath>
#include <sstream>
#include <stdexcept>
#include <type_traits>

#include "../headers/strong_type.hpp"
#include <project_utils.hpp>

// edge case testing namespace
namespace EdgeCaseTests {
    
    // test numeric limits and boundary conditions
    void test_numeric_limits() {
        LOG_INFO_PRINT("testing numeric limits and boundary conditions");
        
        // test maximum integer values
        StrongTypes::Integer max_int{std::numeric_limits<int>::max()};
        assert(max_int.value() == std::numeric_limits<int>::max());
        LOG_INFO_PRINT("maximum integer: {}", max_int.value());
        
        // test minimum integer values
        StrongTypes::Integer min_int{std::numeric_limits<int>::min()};
        assert(min_int.value() == std::numeric_limits<int>::min());
        LOG_INFO_PRINT("minimum integer: {}", min_int.value());
        
        // test maximum double values
        StrongTypes::Real max_real{std::numeric_limits<double>::max()};
        assert(max_real.value() == std::numeric_limits<double>::max());
        LOG_INFO_PRINT("maximum double: {}", max_real.value());
        
        // test minimum positive double values
        StrongTypes::Real min_positive_real{std::numeric_limits<double>::min()};
        assert(min_positive_real.value() == std::numeric_limits<double>::min());
        LOG_INFO_PRINT("minimum positive double: {}", min_positive_real.value());
        
        // test zero values
        StrongTypes::Real zero_real{0.0};
        assert(zero_real.value() == 0.0);
        LOG_INFO_PRINT("zero double: {}", zero_real.value());
        
        // test negative zero (ieee 754)
        StrongTypes::Real neg_zero{-0.0};
        assert(neg_zero.value() == -0.0);
        assert(std::signbit(neg_zero.value()));
        LOG_INFO_PRINT("negative zero: {} (signbit: {})", neg_zero.value(), std::signbit(neg_zero.value()));
        
        LOG_INFO_PRINT("numeric limits tests passed");
    }
    
    // test special floating point values
    void test_special_float_values() {
        LOG_INFO_PRINT("testing special floating point values");
        
        // test positive infinity
        StrongTypes::Real pos_inf{std::numeric_limits<double>::infinity()};
        assert(std::isinf(pos_inf.value()) && pos_inf.value() > 0);
        LOG_INFO_PRINT("positive infinity: {}", pos_inf.value());
        
        // test negative infinity
        StrongTypes::Real neg_inf{-std::numeric_limits<double>::infinity()};
        assert(std::isinf(neg_inf.value()) && neg_inf.value() < 0);
        LOG_INFO_PRINT("negative infinity: {}", neg_inf.value());
        
        // test nan (not a number)
        StrongTypes::Real nan_val{std::numeric_limits<double>::quiet_NaN()};
        assert(std::isnan(nan_val.value()));
        LOG_INFO_PRINT("nan value: {}", nan_val.value());
        
        // test signaling nan
        StrongTypes::Real snan_val{std::numeric_limits<double>::signaling_NaN()};
        assert(std::isnan(snan_val.value()));
        LOG_INFO_PRINT("signaling nan value: {}", snan_val.value());
        
        // test arithmetic with special values
        auto inf_plus_one = pos_inf + StrongTypes::Real{1.0};
        assert(std::isinf(inf_plus_one.value()));
        LOG_INFO_PRINT("infinity + 1 = {}", inf_plus_one.value());
        
        auto inf_times_zero = pos_inf * StrongTypes::Real{0.0};
        assert(std::isnan(inf_times_zero.value()));
        LOG_INFO_PRINT("infinity * 0 = {}", inf_times_zero.value());
        
        // test nan comparisons (all comparisons with nan should be false)
        assert(!(nan_val == nan_val));
        assert(!(nan_val < StrongTypes::Real{1.0}));
        assert(!(nan_val > StrongTypes::Real{1.0}));
        LOG_INFO_PRINT("nan comparison behavior verified");
        
        LOG_INFO_PRINT("special floating point values tests passed");
    }
    
    // test empty string and edge cases for string strong types
    void test_string_edge_cases() {
        LOG_INFO_PRINT("testing string edge cases");
        
        // test empty string
        StrongTypes::SessionId empty_session{""};
        assert(empty_session.value().empty());
        assert(empty_session.value().size() == 0);
        LOG_INFO_PRINT("empty string length: {}", empty_session.value().size());
        
        // test very long string
        std::string long_str(10000, 'x');
        StrongTypes::SessionId long_session{long_str};
        assert(long_session.value().size() == 10000);
        assert(long_session.value()[0] == 'x');
        assert(long_session.value()[9999] == 'x');
        LOG_INFO_PRINT("long string length: {}", long_session.value().size());
        
        // test string with special characters
        std::string special_str{"hello\nworld\t\r\0special"};
        StrongTypes::SessionId special_session{special_str};
        assert(special_session.value() == special_str);
        LOG_INFO_PRINT("special characters string length: {}", special_session.value().size());
        
        // test unicode string
        std::string unicode_str{"hello 世界 🌍"};
        StrongTypes::SessionId unicode_session{unicode_str};
        assert(unicode_session.value() == unicode_str);
        LOG_INFO_PRINT("unicode string: '{}'", unicode_session.value());
        
        LOG_INFO_PRINT("string edge cases tests passed");
    }
    
    // test overflow and underflow scenarios
    void test_overflow_underflow() {
        LOG_INFO_PRINT("testing overflow and underflow scenarios");
        
        // test integer overflow behavior (implementation defined, but should not crash)
        StrongTypes::Integer near_max{std::numeric_limits<int>::max() - 1};
        auto overflow_result = near_max + StrongTypes::Integer{2};
        LOG_INFO_PRINT("near max + 2 = {} (overflow behavior)", overflow_result.value());
        
        // test integer underflow behavior
        StrongTypes::Integer near_min{std::numeric_limits<int>::min() + 1};
        auto underflow_result = near_min - StrongTypes::Integer{2};
        LOG_INFO_PRINT("near min - 2 = {} (underflow behavior)", underflow_result.value());
        
        // test double overflow to infinity
        StrongTypes::Real large_double{std::numeric_limits<double>::max()};
        auto double_overflow = large_double * StrongTypes::Real{2.0};
        assert(std::isinf(double_overflow.value()));
        LOG_INFO_PRINT("double overflow: {}", double_overflow.value());
        
        // test double underflow to zero
        StrongTypes::Real tiny_double{std::numeric_limits<double>::min()};
        auto double_underflow = tiny_double * StrongTypes::Real{0.5};
        LOG_INFO_PRINT("double underflow: {} (may be denormal or zero)", double_underflow.value());
        
        LOG_INFO_PRINT("overflow and underflow tests passed");
    }
    
    // test stream operations with edge cases
    void test_stream_edge_cases() {
        LOG_INFO_PRINT("testing stream operations edge cases");
        
        // test output stream with special float values
        std::ostringstream inf_oss;
        StrongTypes::Real inf_value{std::numeric_limits<double>::infinity()};
        inf_oss << inf_value;
        LOG_INFO_PRINT("infinity stream output: '{}'", inf_oss.str());
        
        std::ostringstream nan_oss;
        StrongTypes::Real nan_value{std::numeric_limits<double>::quiet_NaN()};
        nan_oss << nan_value;
        LOG_INFO_PRINT("nan stream output: '{}'", nan_oss.str());
        
        // test input stream error conditions
        std::istringstream invalid_int_iss{"not_a_number"};
        StrongTypes::Integer int_input{};
        invalid_int_iss >> int_input;
        assert(invalid_int_iss.fail());
        LOG_INFO_PRINT("invalid integer input handling: stream failed as expected");
        
        std::istringstream invalid_double_iss{"also_not_a_number"};
        StrongTypes::Real real_input{};
        invalid_double_iss >> real_input;
        assert(invalid_double_iss.fail());
        LOG_INFO_PRINT("invalid double input handling: stream failed as expected");
        
        // test empty stream input
        std::istringstream empty_iss{""};
        StrongTypes::Integer empty_int_input{42}; // start with non-default value
        empty_iss >> empty_int_input;
        assert(empty_iss.fail());
        assert(empty_int_input.value() == 42); // should remain unchanged
        LOG_INFO_PRINT("empty stream input: value unchanged ({})", empty_int_input.value());
        
        LOG_INFO_PRINT("stream edge cases tests passed");
    }
    
    // test hash collisions and distribution
    void test_hash_edge_cases() {
        LOG_INFO_PRINT("testing hash edge cases");
        
        std::hash<StrongTypes::UserId> user_hasher;
        std::hash<StrongTypes::SessionId> session_hasher;
        
        // test hash of zero
        StrongTypes::UserId zero_user{0};
        auto zero_hash = user_hasher(zero_user);
        LOG_INFO_PRINT("hash of user id 0: {}", zero_hash);
        
        // test hash of maximum value
        StrongTypes::UserId max_user{std::numeric_limits<std::size_t>::max()};
        auto max_hash = user_hasher(max_user);
        LOG_INFO_PRINT("hash of max user id: {}", max_hash);
        
        // test hash consistency - same value should always hash to same result
        StrongTypes::UserId same_user{12345};
        auto hash1 = user_hasher(same_user);
        auto hash2 = user_hasher(StrongTypes::UserId{12345});
        assert(hash1 == hash2);
        LOG_INFO_PRINT("hash consistency verified: {} == {}", hash1, hash2);
        
        // test empty string hash
        StrongTypes::SessionId empty_session{""};
        auto empty_string_hash = session_hasher(empty_session);
        LOG_INFO_PRINT("empty string hash: {}", empty_string_hash);
        
        // test very similar strings have different hashes (good distribution)
        StrongTypes::SessionId session1{"session_000001"};
        StrongTypes::SessionId session2{"session_000002"};
        auto hash_a = session_hasher(session1);
        auto hash_b = session_hasher(session2);
        assert(hash_a != hash_b); // very likely to be different
        LOG_INFO_PRINT("similar string hashes: {} vs {}", hash_a, hash_b);
        
        LOG_INFO_PRINT("hash edge cases tests passed");
    }
    
    // test copy and move semantics edge cases
    void test_copy_move_semantics() {
        LOG_INFO_PRINT("testing copy and move semantics edge cases");
        
        // test self-assignment
        StrongTypes::Integer self_assign_test{42};
        self_assign_test = self_assign_test; // self-assignment
        assert(self_assign_test.value() == 42);
        LOG_INFO_PRINT("self-assignment test passed: {}", self_assign_test.value());
        
        // test move from moved-from object (should still be valid but unspecified state)
        StrongTypes::Real move_source{3.14};
        StrongTypes::Real move_dest{std::move(move_source)};
        assert(move_dest.value() == 3.14);
        // move_source is now in valid but unspecified state
        LOG_INFO_PRINT("move construction: dest={}, source state is valid but unspecified", move_dest.value());
        
        // test assignment to moved-from object
        move_source = StrongTypes::Real{2.71};
        assert(move_source.value() == 2.71);
        LOG_INFO_PRINT("assignment to moved-from object: {}", move_source.value());
        
        // test swap with identical values
        StrongTypes::Integer swap_a{100};
        StrongTypes::Integer swap_b{100};
        swap_a.swap(swap_b);
        assert(swap_a.value() == 100);
        assert(swap_b.value() == 100);
        LOG_INFO_PRINT("swap identical values: a={}, b={}", swap_a.value(), swap_b.value());
        
        LOG_INFO_PRINT("copy and move semantics tests passed");
    }
    
    // test constexpr evaluation capabilities
    void test_constexpr_evaluation() {
        LOG_INFO_PRINT("testing constexpr evaluation capabilities");
        
        // test constexpr construction
        constexpr StrongTypes::Integer const_int{42};
        static_assert(const_int.value() == 42);
        LOG_INFO_PRINT("constexpr construction: {}", const_int.value());
        
        // test constexpr arithmetic
        constexpr StrongTypes::Integer a{10};
        constexpr StrongTypes::Integer b{5};
        constexpr auto sum = a + b;
        constexpr auto diff = a - b;
        constexpr auto product = a * b;
        
        static_assert(sum.value() == 15);
        static_assert(diff.value() == 5);
        static_assert(product.value() == 50);
        
        LOG_INFO_PRINT("constexpr arithmetic: {}+{}={}, {}-{}={}, {}*{}={}", 
                      a.value(), b.value(), sum.value(),
                      a.value(), b.value(), diff.value(),
                      a.value(), b.value(), product.value());
        
        // test constexpr comparison
        constexpr bool equal = (a == StrongTypes::Integer{10});
        constexpr bool less = (b < a);
        constexpr bool greater = (a > b);
        
        static_assert(equal);
        static_assert(less);
        static_assert(greater);
        
        LOG_INFO_PRINT("constexpr comparisons: equal={}, less={}, greater={}", equal, less, greater);
        
        LOG_INFO_PRINT("constexpr evaluation tests passed");
    }
    
    // run all edge case tests
    void run_edge_case_tests() {
        LOG_INFO_PRINT("starting edge case tests");
        LOG_INFO_PRINT("==========================");
        
        test_numeric_limits();
        LOG_INFO_PRINT("");
        
        test_special_float_values();
        LOG_INFO_PRINT("");
        
        test_string_edge_cases();
        LOG_INFO_PRINT("");
        
        test_overflow_underflow();
        LOG_INFO_PRINT("");
        
        test_stream_edge_cases();
        LOG_INFO_PRINT("");
        
        test_hash_edge_cases();
        LOG_INFO_PRINT("");
        
        test_copy_move_semantics();
        LOG_INFO_PRINT("");
        
        test_constexpr_evaluation();
        LOG_INFO_PRINT("");
        
        LOG_INFO_PRINT("==========================");
        LOG_INFO_PRINT("all edge case tests completed successfully!");
    }
}

// uncomment to run edge case tests standalone
/*
int main() {
    try {
        EdgeCaseTests::run_edge_case_tests();
        return 0;
    } catch (const std::exception& e) {
        LOG_ERROR_PRINT("exception in edge case tests: {}", e.what());
        return 1;
    } catch (...) {
        LOG_ERROR_PRINT("unknown exception in edge case tests");
        return 1;
    }
}
*/
