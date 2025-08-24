// MIT License
// Copyright (c) 2025 dbjwhs

#include <print>
#include <cassert>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <expected>
#include <functional>

#include "../headers/strong_type.hpp"
#include <project_utils.hpp>

// demonstrate custom business domain types
namespace BusinessTypes {
    // employee identification strong type
    DEFINE_STRONG_TYPE(EmployeeId, std::size_t, EqualityComparable, OrderedComparable, Streamable, Hashable);
    
    // salary type with arithmetic operations
    DEFINE_STRONG_TYPE(Salary, double, EqualityComparable, OrderedComparable, Arithmetic, Streamable);
    
    // age type with increment/decrement support
    DEFINE_STRONG_TYPE(Age, int, EqualityComparable, OrderedComparable, Arithmetic, Incrementable, Streamable);
    
    // email type with basic operations
    DEFINE_STRONG_TYPE(Email, std::string, EqualityComparable, Streamable, Hashable);
    
    // temperature with arithmetic
    DEFINE_STRONG_TYPE(Temperature, double, EqualityComparable, OrderedComparable, Arithmetic, Streamable);
    
    // distance with units preserved
    DEFINE_STRONG_TYPE(Distance, double, EqualityComparable, OrderedComparable, Arithmetic, Streamable);
}

// test basic construction and value access
void test_basic_construction() {
    LOG_INFO_PRINT("testing basic construction and value access");
    
    // test default construction
    StrongTypes::Integer default_int{};
    assert(default_int.value() == 0);
    LOG_INFO_PRINT("default construction: {}", default_int.value());
    
    // test explicit construction
    StrongTypes::Integer explicit_int{42};
    assert(explicit_int.value() == 42);
    LOG_INFO_PRINT("explicit construction: {}", explicit_int.value());
    
    // test in-place construction
    StrongTypes::SessionId session_id{std::in_place, "session_12345"};
    assert(session_id.value() == "session_12345");
    LOG_INFO_PRINT("in-place construction: {}", session_id.value());
    
    // test copy construction
    StrongTypes::Real original{3.14};
    StrongTypes::Real copy{original};
    assert(copy.value() == original.value());
    LOG_INFO_PRINT("copy construction: {} -> {}", original.value(), copy.value());
    
    // test move construction
    StrongTypes::Real moved{std::move(copy)};
    assert(moved.value() == 3.14);
    LOG_INFO_PRINT("move construction result: {}", moved.value());
    
    LOG_INFO_PRINT("basic construction tests passed");
}

// test equality and comparison operations
void test_equality_and_comparison() {
    LOG_INFO_PRINT("testing equality and comparison operations");
    
    StrongTypes::Integer int1{10};
    StrongTypes::Integer int2{20};
    StrongTypes::Integer int3{10};
    
    // test equality
    assert(int1 == int3);
    assert(!(int1 == int2));
    LOG_INFO_PRINT("equality: {} == {} is {}", int1.value(), int3.value(), int1 == int3);
    LOG_INFO_PRINT("equality: {} == {} is {}", int1.value(), int2.value(), int1 == int2);
    
    // test inequality
    assert(int1 != int2);
    assert(!(int1 != int3));
    LOG_INFO_PRINT("inequality: {} != {} is {}", int1.value(), int2.value(), int1 != int2);
    
    // test three-way comparison
    assert(int1 < int2);
    assert(int2 > int1);
    assert(int1 <= int3);
    assert(int1 >= int3);
    LOG_INFO_PRINT("comparison: {} < {} is {}", int1.value(), int2.value(), int1 < int2);
    LOG_INFO_PRINT("comparison: {} >= {} is {}", int1.value(), int3.value(), int1 >= int3);
    
    LOG_INFO_PRINT("equality and comparison tests passed");
}

// test arithmetic operations
void test_arithmetic_operations() {
    LOG_INFO_PRINT("testing arithmetic operations");
    
    StrongTypes::Real val1{10.5};
    StrongTypes::Real val2{2.5};
    
    // test addition
    auto sum = val1 + val2;
    assert(sum.value() == 13.0);
    LOG_INFO_PRINT("addition: {} + {} = {}", val1.value(), val2.value(), sum.value());
    
    // test subtraction
    auto diff = val1 - val2;
    assert(diff.value() == 8.0);
    LOG_INFO_PRINT("subtraction: {} - {} = {}", val1.value(), val2.value(), diff.value());
    
    // test multiplication
    auto product = val1 * val2;
    assert(product.value() == 26.25);
    LOG_INFO_PRINT("multiplication: {} * {} = {}", val1.value(), val2.value(), product.value());
    
    // test safe division
    auto division_result = val1 / val2;
    assert(division_result.has_value());
    assert(division_result.value().value() == 4.2);
    LOG_INFO_PRINT("division: {} / {} = {}", val1.value(), val2.value(), division_result.value().value());
    
    // test division by zero error handling
    StrongTypes::Real zero{0.0};
    auto division_by_zero = val1 / zero;
    assert(!division_by_zero.has_value());
    LOG_INFO_PRINT("division by zero error: {}", division_by_zero.error());
    
    // test compound assignment operations
    StrongTypes::Real compound{5.0};
    compound += StrongTypes::Real{3.0};
    assert(compound.value() == 8.0);
    LOG_INFO_PRINT("compound addition: result = {}", compound.value());
    
    compound -= StrongTypes::Real{2.0};
    assert(compound.value() == 6.0);
    LOG_INFO_PRINT("compound subtraction: result = {}", compound.value());
    
    compound *= StrongTypes::Real{2.0};
    assert(compound.value() == 12.0);
    LOG_INFO_PRINT("compound multiplication: result = {}", compound.value());
    
    // test compound division with error handling
    auto divide_result = compound.divide_assign(StrongTypes::Real{3.0});
    assert(divide_result.has_value());
    assert(compound.value() == 4.0);
    LOG_INFO_PRINT("compound division: result = {}", compound.value());
    
    LOG_INFO_PRINT("arithmetic operations tests passed");
}

// test increment and decrement operations
void test_increment_decrement() {
    LOG_INFO_PRINT("testing increment and decrement operations");
    
    StrongTypes::Integer counter{10};
    
    // test pre-increment
    auto pre_inc = ++counter;
    assert(counter.value() == 11);
    assert(pre_inc.value() == 11);
    LOG_INFO_PRINT("pre-increment: counter = {}, returned = {}", counter.value(), pre_inc.value());
    
    // test post-increment
    auto post_inc = counter++;
    assert(counter.value() == 12);
    assert(post_inc.value() == 11);
    LOG_INFO_PRINT("post-increment: counter = {}, returned = {}", counter.value(), post_inc.value());
    
    // test pre-decrement
    auto pre_dec = --counter;
    assert(counter.value() == 11);
    assert(pre_dec.value() == 11);
    LOG_INFO_PRINT("pre-decrement: counter = {}, returned = {}", counter.value(), pre_dec.value());
    
    // test post-decrement
    auto post_dec = counter--;
    assert(counter.value() == 10);
    assert(post_dec.value() == 11);
    LOG_INFO_PRINT("post-decrement: counter = {}, returned = {}", counter.value(), post_dec.value());
    
    LOG_INFO_PRINT("increment and decrement tests passed");
}

// test stream operations
void test_stream_operations() {
    LOG_INFO_PRINT("testing stream input/output operations");
    
    // test output stream
    std::ostringstream oss;
    StrongTypes::Real value{42.5};
    oss << value;
    assert(oss.str() == "42.5");
    LOG_INFO_PRINT("output stream: value {} -> string '{}'", value.value(), oss.str());
    
    // test input stream
    std::istringstream iss{"123.45"};
    StrongTypes::Real input_value{};
    iss >> input_value;
    assert(input_value.value() == 123.45);
    LOG_INFO_PRINT("input stream: string '123.45' -> value {}", input_value.value());
    
    // test input stream with string type
    std::istringstream string_iss{"hello_world"};
    StrongTypes::SessionId session_input{};
    string_iss >> session_input;
    assert(session_input.value() == "hello_world");
    LOG_INFO_PRINT("input stream string: '{}' -> value '{}'", "hello_world", session_input.value());
    
    LOG_INFO_PRINT("stream operations tests passed");
}

// test hash support for containers
void test_hash_support() {
    LOG_INFO_PRINT("testing hash support for standard containers");
    
    // test unordered_map with strong type key
    std::unordered_map<StrongTypes::UserId, std::string> user_names;
    
    StrongTypes::UserId user1{1001};
    StrongTypes::UserId user2{1002};
    StrongTypes::UserId user3{1003};
    
    user_names[user1] = "alice";
    user_names[user2] = "bob";
    user_names[user3] = "charlie";
    
    assert(user_names[user1] == "alice");
    assert(user_names[user2] == "bob");
    assert(user_names[user3] == "charlie");
    
    LOG_INFO_PRINT("hash map: user {} -> {}", user1.value(), user_names[user1]);
    LOG_INFO_PRINT("hash map: user {} -> {}", user2.value(), user_names[user2]);
    LOG_INFO_PRINT("hash map: user {} -> {}", user3.value(), user_names[user3]);
    
    // test hash consistency
    std::hash<StrongTypes::UserId> hasher;
    auto hash1 = hasher(user1);
    auto hash1_copy = hasher(StrongTypes::UserId{1001});
    assert(hash1 == hash1_copy);
    LOG_INFO_PRINT("hash consistency: hash({}) == hash({})", user1.value(), 1001);
    
    LOG_INFO_PRINT("hash support tests passed");
}

// test business domain usage scenarios
void test_business_domain_usage() {
    LOG_INFO_PRINT("testing business domain usage scenarios");
    
    using namespace BusinessTypes;
    
    // employee data structure
    struct Employee {
        EmployeeId m_id{};
        std::string m_name{};
        Age m_age{};
        Salary m_salary{};
        Email m_email{};
        
        Employee(EmployeeId id, std::string name, Age age, Salary salary, Email email)
            : m_id{id}, m_name{std::move(name)}, m_age{age}, m_salary{salary}, m_email{std::move(email)} {}
    };
    
    // create employees with strong types
    std::vector<Employee> employees;
    employees.reserve(3);
    
    employees.emplace_back(
        EmployeeId{1001}, 
        "alice johnson", 
        Age{28}, 
        Salary{75000.0}, 
        Email{"alice@company.com"}
    );
    
    employees.emplace_back(
        EmployeeId{1002}, 
        "bob smith", 
        Age{35}, 
        Salary{85000.0}, 
        Email{"bob@company.com"}
    );
    
    employees.emplace_back(
        EmployeeId{1003}, 
        "charlie brown", 
        Age{42}, 
        Salary{95000.0}, 
        Email{"charlie@company.com"}
    );
    
    LOG_INFO_PRINT("created {} employees", employees.size());
    
    // demonstrate type safety - cannot mix employee ids with other numeric types
    // this would cause compilation error:
    // StrongTypes::UserId user_id{1001};
    // EmployeeId emp_id = user_id; // compilation error - different types
    
    // calculate total salary using strong types
    Salary total_salary{0.0};
    for (const auto& employee : employees) {
        total_salary = total_salary + employee.m_salary;
        LOG_INFO_PRINT("employee {}: {} (age: {}, salary: ${})", 
                      employee.m_id.value(), 
                      employee.m_name, 
                      employee.m_age.value(), 
                      employee.m_salary.value());
    }
    
    assert(total_salary.value() == 255000.0);
    LOG_INFO_PRINT("total salary: ${}", total_salary.value());
    
    // test age increment scenario
    Employee& alice = employees[0];
    Age old_age = alice.m_age;
    ++alice.m_age; // birthday increment
    assert(alice.m_age.value() == old_age.value() + 1);
    LOG_INFO_PRINT("alice aged from {} to {}", old_age.value(), alice.m_age.value());
    
    LOG_INFO_PRINT("business domain usage tests passed");
}

// test railway-oriented programming with transform
void test_railway_oriented_programming() {
    LOG_INFO_PRINT("testing railway-oriented programming patterns");
    
    StrongTypes::Real temperature{25.0};
    
    // transform celsius to fahrenheit
    auto fahrenheit_result = temperature.transform([](const double& celsius) -> double {
        return celsius * 9.0 / 5.0 + 32.0;
    });
    
    assert(fahrenheit_result.has_value());
    assert(fahrenheit_result.value().value() == 77.0);
    LOG_INFO_PRINT("temperature transform: {}°c -> {}°f", 
                  temperature.value(), 
                  fahrenheit_result.value().value());
    
    // test and_then chaining
    auto description = temperature.and_then([](const double& temp) -> std::string {
        if (temp < 0) {
            return "freezing";
        } else if (temp < 20) {
            return "cold";
        } else if (temp < 30) {
            return "comfortable";
        } else {
            return "hot";
        }
    });
    
    assert(description == "comfortable");
    LOG_INFO_PRINT("temperature description: {}°c is {}", temperature.value(), description);
    
    // test value_or with default
    StrongTypes::Real empty_temp{};
    auto default_temp = empty_temp.value_or(20.0);
    assert(default_temp == 20.0);
    LOG_INFO_PRINT("default value: empty temp defaulted to {}", default_temp);
    
    LOG_INFO_PRINT("railway-oriented programming tests passed");
}

// test swap operations
void test_swap_operations() {
    LOG_INFO_PRINT("testing swap operations");
    
    StrongTypes::Integer val1{100};
    StrongTypes::Integer val2{200};
    
    int original_val1 = val1.value();
    int original_val2 = val2.value();
    
    // test member swap
    val1.swap(val2);
    assert(val1.value() == original_val2);
    assert(val2.value() == original_val1);
    LOG_INFO_PRINT("member swap: {} <-> {}", val1.value(), val2.value());
    
    // test free function swap
    swap(val1, val2);
    assert(val1.value() == original_val1);
    assert(val2.value() == original_val2);
    LOG_INFO_PRINT("free function swap: {} <-> {}", val1.value(), val2.value());
    
    LOG_INFO_PRINT("swap operations tests passed");
}

// test explicit conversion
void test_explicit_conversion() {
    LOG_INFO_PRINT("testing explicit conversion operations");
    
    StrongTypes::Real value{42.5};
    
    // test explicit conversion to underlying type
    double underlying = static_cast<double>(value);
    assert(underlying == 42.5);
    LOG_INFO_PRINT("explicit conversion: strong type {} -> underlying {}", value.value(), underlying);
    
    // test that implicit conversion is not allowed (would cause compilation error)
    // double implicit = value; // this would be a compilation error
    
    LOG_INFO_PRINT("explicit conversion tests passed");
}

// test container operations with strong types
void test_container_operations() {
    LOG_INFO_PRINT("testing container operations with strong types");
    
    // vector of strong type values
    std::vector<StrongTypes::Integer> numbers;
    numbers.reserve(5);
    
    for (int ndx = 1; ndx <= 5; ++ndx) {
        numbers.emplace_back(StrongTypes::Integer{ndx * 10});
    }
    
    LOG_INFO_PRINT("created vector with {} elements", numbers.size());
    
    // use algorithms with strong types
    auto sum = std::accumulate(numbers.begin(), numbers.end(), StrongTypes::Integer{0});
    assert(sum.value() == 150);
    LOG_INFO_PRINT("accumulate sum: {}", sum.value());
    
    // find maximum element
    auto max_element = std::max_element(numbers.begin(), numbers.end());
    assert(max_element != numbers.end());
    assert(max_element->value() == 50);
    LOG_INFO_PRINT("maximum element: {}", max_element->value());
    
    // sort in descending order
    std::sort(numbers.begin(), numbers.end(), std::greater<StrongTypes::Integer>{});
    assert(numbers[0].value() == 50);
    assert(numbers[4].value() == 10);
    
    std::print("sorted descending: ");
    for (size_t ndx = 0; ndx < numbers.size(); ++ndx) {
        std::print("{}", numbers[ndx].value());
        if (ndx < numbers.size() - 1) {
            std::print(", ");
        }
    }
    std::print("\n");
    
    LOG_INFO_PRINT("container operations tests passed");
}

// test error handling with division
void test_error_handling() {
    LOG_INFO_PRINT("testing error handling scenarios");
    
    using namespace BusinessTypes;
    
    Temperature temp1{100.0};
    Temperature temp2{0.0};
    
    // test successful division
    auto division_success = temp1 / Temperature{2.0};
    assert(division_success.has_value());
    assert(division_success.value().value() == 50.0);
    LOG_INFO_PRINT("successful division: {} / {} = {}", 
                  temp1.value(), 2.0, division_success.value().value());
    
    // test division by zero with proper error handling
    {
        Logger::StderrSuppressionGuard suppression_guard;
        auto division_error = temp1 / temp2;
        assert(!division_error.has_value());
        LOG_INFO_PRINT("division error handled: {}", division_error.error());
    }
    
    // test compound division error
    Temperature compound_temp{100.0};
    {
        Logger::StderrSuppressionGuard suppression_guard;
        auto compound_error = compound_temp.divide_assign(temp2);
        assert(!compound_error.has_value());
        LOG_INFO_PRINT("compound division error handled: {}", compound_error.error());
    }
    
    LOG_INFO_PRINT("error handling tests passed");
}

// comprehensive test runner
void run_comprehensive_tests() {
    LOG_INFO_PRINT("starting comprehensive strong type wrapper tests");
    LOG_INFO_PRINT("======================================================");
    
    test_basic_construction();
    LOG_INFO_PRINT("");
    
    test_equality_and_comparison();
    LOG_INFO_PRINT("");
    
    test_arithmetic_operations();
    LOG_INFO_PRINT("");
    
    test_increment_decrement();
    LOG_INFO_PRINT("");
    
    test_stream_operations();
    LOG_INFO_PRINT("");
    
    test_hash_support();
    LOG_INFO_PRINT("");
    
    test_business_domain_usage();
    LOG_INFO_PRINT("");
    
    test_railway_oriented_programming();
    LOG_INFO_PRINT("");
    
    test_swap_operations();
    LOG_INFO_PRINT("");
    
    test_explicit_conversion();
    LOG_INFO_PRINT("");
    
    test_container_operations();
    LOG_INFO_PRINT("");
    
    test_error_handling();
    LOG_INFO_PRINT("");
    
    LOG_INFO_PRINT("======================================================");
    LOG_INFO_PRINT("all comprehensive strong type wrapper tests completed successfully!");
}

int main() {
    try {
        run_comprehensive_tests();
        return 0;
    } catch (const std::exception& e) {
        LOG_ERROR_PRINT("exception caught in main: {}", e.what());
        return 1;
    } catch (...) {
        LOG_ERROR_PRINT("unknown exception caught in main");
        return 1;
    }
}
