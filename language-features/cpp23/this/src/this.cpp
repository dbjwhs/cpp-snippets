// MIT License
// Copyright (c) 2025 dbjwhs

#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <cassert>
#include <format>
#include "../../../../headers/project_utils.hpp"

// Explicit Object Parameter (Deducing this) - History and Overview
// ---------------------------------------------------------------
// The explicit object parameter is a C++23 feature that allows making the implicit 'this' pointer explicit
// in member function definitions. Prior to C++23, member functions had an implicit 'this' pointer which couldn't
// be explicitly declared or customized in the function parameter list.
//
// This pattern has been common in Python, where the 'self' parameter is explicit. In C++, the feature went through
// several design revisions before finally being included in the C++23 standard as part of paper P0847R7
// by Gašper Ažman, Simon Brand, Ben Deane, and Barry Revzin.
//
// The explicit object parameter provides several benefits:
// 1. Simplifies advanced techniques such as CRTP (Curiously Recurring Template Pattern)
// 2. Enables the Overload Pattern to be implemented more cleanly
// 3. Avoids code duplication for const and non-const member function variants
// 4. Allows for more flexible member function qualifiers (&, &&, const, volatile)
// 5. Simplifies forwarding patterns
//
// Common use cases include polymorphic chaining, perfect forwarding in member functions,
// and providing a single function definition for multiple reference qualifiers.

// CRTP (Curiously Recurring Template Pattern) example; see README.md -> What is CRTP?
template <typename Derived>
class Base {
public:
    bool interface(this auto&& self) {
        // Direct access to derived implementation without casting
        return self.implementation();
    }

    bool implementation(this auto&& self) {
        LOG_INFO_PRINT("Base implementation");
        return false;
    }
};

class Derived : public Base<Derived> {
public:
    bool implementation(this auto&& self) {
        LOG_INFO_PRINT("Derived implementation");
        return true;
    }
};

// Basic example of explicit object parameter
class BasicExample {
private:
    // member variable with m_ prefix
    int m_value{0};

public:
    // constructor
    explicit BasicExample(int value) : m_value{value} {}

    // traditional member function
    void traditional_increment() {
        m_value++;
        LOG_INFO_PRINT("traditional_increment: value = {}", m_value);
    }

    // explicit object parameter version - notice the 'this' parameter
    void explicit_increment(this BasicExample& self) {
        self.m_value++;
        LOG_INFO_PRINT("explicit_increment: value = {}", self.m_value);
    }

    // const qualified version
    int get_value(this const BasicExample& self) {
        return self.m_value;
    }
};

// Example of using explicit object parameter to avoid duplication
// between const and non-const member functions
class DuplicationAvoidance {
private:
    // member variable representing a collection
    std::vector<int> m_data{};

public:
    // constructor with an initializer list
    DuplicationAvoidance(std::initializer_list<int> init) : m_data{init} {}

    // a single function definition that works for both const and non-const contexts,
    // the auto&& deduces the reference type based on the calling context
    auto& get_data(this auto&& self) {
        LOG_INFO_PRINT("get_data called");
        return self.m_data;
    }
};

// CRTP example using explicit object parameter
// base class template
template<typename Derived>
class CRTPBase {
public:
    // method using explicit object parameter with CRTP
    void double_value(this CRTPBase<Derived>& self) {
        // cast to a derived type using static_cast
        static_cast<Derived&>(self).double_impl();
        LOG_INFO_PRINT("double_value called from base");
    }

    // method to demonstrate chaining with CRTP
    Derived& chain_call(this CRTPBase<Derived>& self, int value) {
        LOG_INFO_PRINT("chain_call with value: {}", value);
        return static_cast<Derived&>(self);
    }
};

// derived class for CRTP
class CRTPDerived : public CRTPBase<CRTPDerived> {
private:
    // member variable with m_ prefix
    int m_value{10};

public:
    // implementation required by CRTP pattern
    void double_impl() {
        m_value *= 2;
        LOG_INFO_PRINT("double_impl: value now = {}", m_value);
    }

    // getter for the value
    int get_value(this const CRTPDerived& self) {
        return self.m_value;
    }
};

// overload pattern example using explicit object parameter
class Printable {
private:
    // member variables
    std::string m_name{};
    int m_id{0};

public:
    // constructor
    Printable(std::string name, const int id) : m_name{std::move(name)}, m_id{id} {}

    // generic print function with explicit object parameter
    void print(this auto&& self, auto&& output) {
        output(std::format("Name: {}, ID: {}", self.m_name, self.m_id));
    }
};

// helper class for reference qualification demonstration
class RefQualifier {
private:
    // member variable
    std::string m_data{"default"};

public:
    // the function that deduces reference qualifier
    // shows how && and & qualifiers can be handled with one function
    std::string get_status(this auto&& self) {
        if constexpr (std::is_lvalue_reference_v<decltype(self)>) {
            LOG_INFO_PRINT("Called on lvalue");
            return "lvalue: " + self.m_data;
        } else {
            LOG_INFO_PRINT("Called on rvalue");
            return "rvalue: " + self.m_data;
        }
    }
};

// function for testing the reference qualifier behavior
void test_ref_qualifier() {
    LOG_INFO_PRINT("Testing reference qualification");

    // create an lvalue
    RefQualifier lvalue;
    // call on lvalue
    std::string lvalue_result = lvalue.get_status();
    LOG_INFO_PRINT("Result: {}", lvalue_result);

    // call on rvalue
    std::string rvalue_result = RefQualifier{}.get_status();
    LOG_INFO_PRINT("Result: {}", rvalue_result);

    // assert that the results are different
    assert(lvalue_result != rvalue_result);
    assert(lvalue_result.find("lvalue") != std::string::npos);
    assert(rvalue_result.find("rvalue") != std::string::npos);
}

// function for testing basic functionality
void test_basic_example() {
    LOG_INFO_PRINT("Testing basic explicit object parameter");

    // create an instance
    BasicExample obj{5};

    // call traditional method
    obj.traditional_increment();
    assert(obj.get_value() == 6);

    // call explicit this method
    obj.explicit_increment();
    assert(obj.get_value() == 7);

    LOG_INFO_PRINT("Basic test completed successfully");
}

// function for testing duplication avoidance
void test_duplication_avoidance() {
    LOG_INFO_PRINT("Testing duplication avoidance");

    // create an instance
    DuplicationAvoidance obj{1, 2, 3, 4, 5};

    // non-const reference
    auto& data = obj.get_data();
    LOG_INFO_PRINT("Size of data: {}", data.size());

    // modify the data
    data.push_back(6);

    // const reference
    const DuplicationAvoidance& const_obj = obj;
    const auto& const_data = const_obj.get_data();
    LOG_INFO_PRINT("Size of const data: {}", const_data.size());

    // verify that both refer to the same data
    assert(data.size() == const_data.size());
    assert(data.size() == 6);

    LOG_INFO_PRINT("Duplication avoidance test completed successfully");
}

// function for testing CRTP
void test_crtp() {
    LOG_INFO_PRINT("Testing CRTP with explicit object parameter");

    // create an instance
    CRTPDerived derived;

    // initial value
    int initial_value = derived.get_value();
    LOG_INFO_PRINT("Initial value: {}", initial_value);

    // call base class method which will invoke derived class implementation
    derived.double_value();

    // verify result
    int new_value = derived.get_value();
    LOG_INFO_PRINT("New value: {}", new_value);
    assert(new_value == initial_value * 2);

    // test method chaining
    //
    // method chaining: enables fluent interfaces where multiple operations chain together in a single
    // expression. benefits include:
    //  1) improved readability through sequential operations without temporary variables
    //  2) reduced code verbosity by eliminating repetitive object references
    //  3) creation of intuitive apis that read naturally like sentences
    //  4) better encapsulation by keeping implementation details hidden behind a clean interface
    //  5) support for functional programming patterns with immutable objects.
    //
    //  explicit object parameter in
    //  c++23 greatly simplifies implementing chaining by making return type handling more straightforward,
    //  particularly in inheritance hierarchies.
    derived.chain_call(42).chain_call(43).double_value();

    // verify final result
    int final_value = derived.get_value();
    LOG_INFO_PRINT("Final value: {}", final_value);
    assert(final_value == new_value * 2);

    LOG_INFO_PRINT("CRTP test completed successfully");
}

// function for a testing overload pattern
void test_overload_pattern() {
    LOG_INFO_PRINT("Testing overload pattern");

    // create an instance
    Printable obj{"Example", 12345};

    // using lambda as the output mechanism
    bool lambda_called = false;
    obj.print([&lambda_called](const std::string& output) {
        LOG_INFO_PRINT("Lambda output: {}", output);
        lambda_called = true;
    });

    // verify lambda was called
    assert(lambda_called);

    // using a function object as the output mechanism
    struct OutputFunctor {
        bool called{false};
        void operator()(const std::string& output) {
            LOG_INFO_PRINT("Functor output: {}", output);
            called = true;
        }
    };

    OutputFunctor functor;
    obj.print(functor);

    // verify functor was called
    assert(functor.called);

    LOG_INFO_PRINT("Overload pattern test completed successfully");
}

void test_basic_crtp() {
    Derived derived;

    assert(derived.interface());
}

// main function
int main() {
    LOG_INFO_PRINT("Starting explicit object parameter examples");

    // run all tests
    test_basic_example();
    test_duplication_avoidance();
    test_basic_crtp();
    test_crtp();
    test_overload_pattern();
    test_ref_qualifier();


    LOG_INFO_PRINT("All tests completed successfully");
    return 0;
}
