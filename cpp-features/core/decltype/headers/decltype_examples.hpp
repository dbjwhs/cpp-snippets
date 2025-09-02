// MIT License
// Copyright (c) 2025 dbjwhs

#pragma once

#include <concepts>
#include <expected>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>
#include <functional>
#include <array>
#include <map>

//
// comprehensive deep dive into c++ decltype - a complete exploration of type deduction
//
// decltype is a fundamental type deduction mechanism introduced in c++11 that determines
// the type of an expression at compile time without evaluating it. unlike auto which
// deduces types from initialization, decltype preserves the exact type and value category
// of expressions, making it essential for template metaprogramming and perfect forwarding.
//
// history and evolution:
// - c++11: initial introduction with basic expression type deduction
// - c++14: introduced decltype(auto) for automatic return type deduction
// - c++17: enhanced with structured bindings and improved sfinae support  
// - c++20: concepts integration and improved template argument deduction
// - c++23: further refinements and standardization improvements
//
// the decltype mechanism operates through sophisticated rules that consider expression
// categories (lvalue, rvalue, xvalue), cv-qualifiers, and reference types. it forms
// the foundation of modern c++ metaprogramming alongside concepts and requires expressions.
//
// common usage patterns include:
// 1. template return type deduction
// 2. sfinae (substitution failure is not an error) techniques
// 3. perfect forwarding implementations
// 4. compile-time type introspection
// 5. expression template libraries
// 6. generic programming with preserved type information
//

namespace decltype_examples {

    // error type for railway-oriented programming
    enum class DeclTypeError {
        InvalidInput,
        TypeMismatch,
        OperationFailed
    };

    // forward declarations for complex examples
    template<std::copyable ElementType>
    class TypeContainer;

    template<std::regular ValueType>
    class ValueWrapper;

    // basic decltype demonstration class
    class BasicDeclTypeDemo {
    private:
        int m_integerValue{42};
        double m_doubleValue{3.14159};
        std::string m_stringValue{"hello decltype"};
        
    public:
        // demonstrate basic decltype usage with member variables
        auto getIntegerValueType() const -> decltype(m_integerValue);
        
        // demonstrate decltype with expressions
        auto getDoubleExpression() const -> decltype(m_doubleValue * 2.0);
        
        // demonstrate decltype with function calls
        auto getStringLength() const -> decltype(m_stringValue.length());
        
        // demonstrate decltype preserving reference types
        auto getIntegerReference() -> decltype((m_integerValue));
        
        // demonstrate decltype with complex expressions
        template<std::integral IntegralType>
        auto complexExpression(IntegralType value) const -> decltype(value + m_integerValue);
    };

    // advanced decltype usage with template metaprogramming
    template<std::copyable ElementType>
    class AdvancedDeclTypeDemo {
    private:
        std::vector<ElementType> m_container{};
        
    public:
        // demonstrate decltype with iterators
        auto begin() -> decltype(m_container.begin());
        auto end() -> decltype(m_container.end());
        auto cbegin() const -> decltype(m_container.cbegin());
        auto cend() const -> decltype(m_container.cend());
        
        // demonstrate decltype with perfect forwarding
        template<std::convertible_to<ElementType> ForwardType>
        auto addElement(ForwardType&& element) -> decltype(m_container.emplace_back(std::forward<ForwardType>(element)));
        
        // demonstrate decltype with sfinae
        template<std::copyable OtherType>
        auto combineWith(const std::vector<OtherType>& other) 
            -> std::expected<decltype(m_container), DeclTypeError>;
        
        // demonstrate decltype(auto) for return type deduction
        decltype(auto) getLastElement();
        decltype(auto) getLastElement() const;
    };

    // decltype with function objects and lambdas
    class FunctionObjectDemo {
    private:
        std::function<int(int)> m_intFunction{[](int x) { return x * x; }};
        std::function<std::string(const std::string&)> m_stringFunction{[](const std::string& s) { return s + "!"; }};
        
    public:
        // demonstrate decltype with function object calls
        template<std::invocable<int> FunctionType>
        auto callIntFunction(FunctionType&& func, int value) -> decltype(func(value)) {
            return std::forward<FunctionType>(func)(value);
        }
        
        // demonstrate decltype with lambda expressions
        template<typename FactorType>
        auto createMultiplier(FactorType factor) {
            return [factor](int x) { return x * factor; };
        }
        
        // demonstrate decltype with member function pointers
        template<std::copyable ClassType, typename MemberFunctionType>
        auto callMemberFunction(ClassType& obj, MemberFunctionType func) -> decltype((obj.*func)());
    };

    // decltype with expression templates and operators
    template<std::regular ValueType>
    class ExpressionTemplateDemo {
    private:
        ValueType m_value{};
        
    public:
        explicit ExpressionTemplateDemo(ValueType value) : m_value{std::move(value)} {}
        
        // demonstrate decltype with arithmetic operators
        template<std::convertible_to<ValueType> OtherType>
        auto operator+(const ExpressionTemplateDemo<OtherType>& other) const 
            -> ExpressionTemplateDemo<decltype(m_value + other.getValue())>;
        
        // demonstrate decltype with comparison operators
        template<std::convertible_to<ValueType> OtherType>
        auto operator==(const ExpressionTemplateDemo<OtherType>& other) const 
            -> decltype(m_value == other.getValue());
        
        // demonstrate decltype with compound assignment
        template<std::convertible_to<ValueType> OtherType>
        auto operator+=(const ExpressionTemplateDemo<OtherType>& other) 
            -> decltype(*this);
        
        // getter with decltype
        auto getValue() const -> decltype(m_value);
    };

    // utility functions for comprehensive testing
    
    // demonstrate decltype with trailing return types
    template<std::copyable FirstType, std::copyable SecondType>
    auto addValues(const FirstType& first, const SecondType& second) 
        -> decltype(first + second);
    
    // demonstrate decltype with conditional expressions
    template<std::copyable ElementType>
    auto selectValue(bool condition, const ElementType& first, const ElementType& second) 
        -> decltype(condition ? first : second);
    
    // demonstrate decltype with array access
    template<std::copyable ElementType, std::size_t ArraySize>
    auto accessArray(const std::array<ElementType, ArraySize>& arr, std::size_t index) 
        -> decltype(arr[index]);
    
    // demonstrate decltype with member access
    template<std::copyable ContainerType>
    auto getContainerSize(const ContainerType& container) 
        -> decltype(container.size());
    
    // demonstrate decltype with pointer dereferencing
    template<std::copyable PointerType>
    auto dereferencePointer(PointerType* ptr) 
        -> decltype(*ptr);
    
    // demonstrate decltype in requires expressions (c++20 concepts)
    template<typename ExpressionType>
    concept HasDeclTypeExpression = requires(ExpressionType expr) {
        { expr };  // just check that the expression is valid
    };
    
    // comprehensive test runner
    class DeclTypeTestRunner {
    public:
        static auto runAllTests() -> std::expected<void, DeclTypeError>;
        
    private:
        static auto testBasicDeclType() -> std::expected<void, DeclTypeError>;
        static auto testAdvancedDeclType() -> std::expected<void, DeclTypeError>;
        static auto testFunctionObjects() -> std::expected<void, DeclTypeError>;
        static auto testExpressionTemplates() -> std::expected<void, DeclTypeError>;
        static auto testUtilityFunctions() -> std::expected<void, DeclTypeError>;
    };

} // namespace decltype_examples
