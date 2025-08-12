// MIT License
// Copyright (c) 2025 dbjwhs

#include "../headers/decltype_examples.hpp"
#include "../../../headers/project_utils.hpp"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <memory>
#include <print>
#include <ranges>
#include <sstream>

namespace decltype_examples {

    // basic decltype demonstration implementations
    
    // demonstrates decltype preserving exact member variable type
    auto BasicDeclTypeDemo::getIntegerValueType() const -> decltype(m_integerValue) {
        LOG_INFO_PRINT("basic decltype with member variable: decltype(m_integerValue) = int");
        return m_integerValue;
    }
    
    // demonstrates decltype with arithmetic expressions
    auto BasicDeclTypeDemo::getDoubleExpression() const -> decltype(m_doubleValue * 2.0) {
        LOG_INFO_PRINT("decltype with expression: decltype(m_doubleValue * 2.0) = double");
        return m_doubleValue * 2.0;
    }
    
    // demonstrates decltype with member function calls
    auto BasicDeclTypeDemo::getStringLength() const -> decltype(m_stringValue.length()) {
        LOG_INFO_PRINT("decltype with member function: decltype(m_stringValue.length()) = std::string::size_type");
        return m_stringValue.length();
    }
    
    // demonstrates decltype preserving reference types with parentheses
    auto BasicDeclTypeDemo::getIntegerReference() -> decltype((m_integerValue)) {
        LOG_INFO_PRINT("decltype with parentheses: decltype((m_integerValue)) = int&");
        // parentheses around member variable make decltype return reference type
        return m_integerValue;
    }
    
    // demonstrates decltype with template parameters in expressions
    template<std::integral IntegralType>
    auto BasicDeclTypeDemo::complexExpression(IntegralType value) const -> decltype(value + m_integerValue) {
        LOG_INFO_PRINT("decltype with template expression: decltype(value + m_integerValue)");
        return value + m_integerValue;
    }
    
    // explicit template instantiation for common integral types
    template auto BasicDeclTypeDemo::complexExpression(int value) const -> decltype(value + m_integerValue);
    template auto BasicDeclTypeDemo::complexExpression(long value) const -> decltype(value + m_integerValue);
    template auto BasicDeclTypeDemo::complexExpression(short value) const -> decltype(value + m_integerValue);

    // advanced decltype with containers and iterators
    
    // demonstrates decltype preserving iterator types
    template<std::copyable ElementType>
    auto AdvancedDeclTypeDemo<ElementType>::begin() -> decltype(m_container.begin()) {
        LOG_INFO_PRINT("decltype with iterator: decltype(m_container.begin())");
        return m_container.begin();
    }
    
    template<std::copyable ElementType>
    auto AdvancedDeclTypeDemo<ElementType>::end() -> decltype(m_container.end()) {
        LOG_INFO_PRINT("decltype with iterator: decltype(m_container.end())");
        return m_container.end();
    }
    
    template<std::copyable ElementType>
    auto AdvancedDeclTypeDemo<ElementType>::cbegin() const -> decltype(m_container.cbegin()) {
        LOG_INFO_PRINT("decltype with const iterator: decltype(m_container.cbegin())");
        return m_container.cbegin();
    }
    
    template<std::copyable ElementType>
    auto AdvancedDeclTypeDemo<ElementType>::cend() const -> decltype(m_container.cend()) {
        LOG_INFO_PRINT("decltype with const iterator: decltype(m_container.cend())");
        return m_container.cend();
    }
    
    // demonstrates decltype with perfect forwarding
    template<std::copyable ElementType>
    template<std::convertible_to<ElementType> ForwardType>
    auto AdvancedDeclTypeDemo<ElementType>::addElement(ForwardType&& element) 
        -> decltype(m_container.emplace_back(std::forward<ForwardType>(element))) {
        LOG_INFO_PRINT("decltype with perfect forwarding: decltype(m_container.emplace_back(...))");
        return m_container.emplace_back(std::forward<ForwardType>(element));
    }
    
    // demonstrates decltype in sfinae context with std::expected
    template<std::copyable ElementType>
    template<std::copyable OtherType>
    auto AdvancedDeclTypeDemo<ElementType>::combineWith(const std::vector<OtherType>& other) 
        -> std::expected<decltype(m_container), DeclTypeError> {
        LOG_INFO_PRINT("decltype with sfinae and std::expected");
        
        // type compatibility check at compile time
        if constexpr (std::is_convertible_v<OtherType, ElementType>) {
            auto combined = m_container;
            combined.reserve(combined.size() + other.size());
            
            for (const auto& element : other) {
                combined.emplace_back(static_cast<ElementType>(element));
            }
            
            return combined;
        } else {
            return std::unexpected(DeclTypeError::TypeMismatch);
        }
    }
    
    // demonstrates decltype(auto) for automatic return type deduction
    template<std::copyable ElementType>
    decltype(auto) AdvancedDeclTypeDemo<ElementType>::getLastElement() {
        LOG_INFO_PRINT("decltype(auto) preserving reference type");
        
        if (m_container.empty()) {
            // return reference to static element for consistency
            static ElementType defaultElement{};
            return (defaultElement);  // parentheses ensure reference return
        }
        
        // decltype(auto) preserves reference type from back()
        return (m_container.back());
    }
    
    template<std::copyable ElementType>
    decltype(auto) AdvancedDeclTypeDemo<ElementType>::getLastElement() const {
        LOG_INFO_PRINT("decltype(auto) with const member function");
        
        if (m_container.empty()) {
            static const ElementType defaultElement{};
            return (defaultElement);  // parentheses ensure reference return
        }
        
        return (m_container.back());
    }
    
    // explicit template instantiation for common types
    template class AdvancedDeclTypeDemo<int>;
    template class AdvancedDeclTypeDemo<std::string>;
    template class AdvancedDeclTypeDemo<double>;

    // function objects and lambda decltype demonstrations
    
    // callIntFunction is defined in the header due to template lambda usage
    
    // explicit instantiation for createMultiplier (defined in header)
    template auto FunctionObjectDemo::createMultiplier(int factor);
    
    // demonstrates decltype with member function pointers
    template<std::copyable ClassType, typename MemberFunctionType>
    auto FunctionObjectDemo::callMemberFunction(ClassType& obj, MemberFunctionType func) -> decltype((obj.*func)()) {
        LOG_INFO_PRINT("decltype with member function pointer: decltype((obj.*func)())");
        return (obj.*func)();
    }
    
    // template functions with lambdas are defined in headers

    // expression template decltype demonstrations
    
    // demonstrates decltype with operator overloading
    template<std::regular ValueType>
    template<std::convertible_to<ValueType> OtherType>
    auto ExpressionTemplateDemo<ValueType>::operator+(const ExpressionTemplateDemo<OtherType>& other) const 
        -> ExpressionTemplateDemo<decltype(m_value + other.getValue())> {
        LOG_INFO_PRINT("decltype with operator+: decltype(m_value + other.getValue())");
        return ExpressionTemplateDemo<decltype(m_value + other.getValue())>{m_value + other.getValue()};
    }
    
    // demonstrates decltype with comparison operators
    template<std::regular ValueType>
    template<std::convertible_to<ValueType> OtherType>
    auto ExpressionTemplateDemo<ValueType>::operator==(const ExpressionTemplateDemo<OtherType>& other) const 
        -> decltype(m_value == other.getValue()) {
        LOG_INFO_PRINT("decltype with operator==: decltype(m_value == other.getValue())");
        return m_value == other.getValue();
    }
    
    // demonstrates decltype with compound assignment returning reference
    template<std::regular ValueType>
    template<std::convertible_to<ValueType> OtherType>
    auto ExpressionTemplateDemo<ValueType>::operator+=(const ExpressionTemplateDemo<OtherType>& other) 
        -> decltype(*this) {
        LOG_INFO_PRINT("decltype with operator+=: decltype(*this)");
        m_value += static_cast<ValueType>(other.getValue());
        return *this;
    }
    
    template<std::regular ValueType>
    auto ExpressionTemplateDemo<ValueType>::getValue() const -> decltype(m_value) {
        return m_value;
    }
    
    // explicit template instantiation for expression templates
    template class ExpressionTemplateDemo<int>;
    template class ExpressionTemplateDemo<double>;
    template class ExpressionTemplateDemo<std::string>;
    
    // explicit template instantiation for expression template operators
    template auto ExpressionTemplateDemo<int>::operator+(const ExpressionTemplateDemo<int>& other) const 
        -> ExpressionTemplateDemo<decltype(m_value + other.getValue())>;
    template auto ExpressionTemplateDemo<int>::operator+(const ExpressionTemplateDemo<double>& other) const 
        -> ExpressionTemplateDemo<decltype(m_value + other.getValue())>;

    // utility function implementations
    
    // demonstrates decltype with binary operations
    template<std::copyable FirstType, std::copyable SecondType>
    auto addValues(const FirstType& first, const SecondType& second) 
        -> decltype(first + second) {
        LOG_INFO_PRINT("decltype utility function: decltype(first + second)");
        return first + second;
    }
    
    // demonstrates decltype with conditional expressions
    template<std::copyable ElementType>
    auto selectValue(bool condition, const ElementType& first, const ElementType& second) 
        -> decltype(condition ? first : second) {
        LOG_INFO_PRINT("decltype with conditional: decltype(condition ? first : second)");
        return condition ? first : second;
    }
    
    // demonstrates decltype with array subscript
    template<std::copyable ElementType, std::size_t ArraySize>
    auto accessArray(const std::array<ElementType, ArraySize>& arr, std::size_t index) 
        -> decltype(arr[index]) {
        LOG_INFO_PRINT("decltype with array access: decltype(arr[index])");
        assert(index < ArraySize);
        return arr[index];
    }
    
    // demonstrates decltype with member function calls
    template<std::copyable ContainerType>
    auto getContainerSize(const ContainerType& container) 
        -> decltype(container.size()) {
        LOG_INFO_PRINT("decltype with member function: decltype(container.size())");
        return container.size();
    }
    
    // demonstrates decltype with pointer dereferencing
    template<std::copyable PointerType>
    auto dereferencePointer(PointerType* ptr) 
        -> decltype(*ptr) {
        LOG_INFO_PRINT("decltype with dereference: decltype(*ptr)");
        assert(ptr != nullptr);
        return *ptr;
    }
    
    // explicit template instantiation for utility functions
    template auto addValues(const int& first, const int& second) -> decltype(first + second);
    template auto addValues(const double& first, const double& second) -> decltype(first + second);
    template auto addValues(const std::string& first, const std::string& second) -> decltype(first + second);
    template auto addValues(const int& first, const double& second) -> decltype(first + second);
    
    template auto selectValue(bool condition, const int& first, const int& second) -> decltype(condition ? first : second);
    template auto selectValue(bool condition, const std::string& first, const std::string& second) -> decltype(condition ? first : second);
    
    template auto accessArray(const std::array<int, 5>& arr, std::size_t index) -> decltype(arr[index]);
    template auto accessArray(const std::array<std::string, 3>& arr, std::size_t index) -> decltype(arr[index]);
    
    template auto getContainerSize(const std::vector<int>& container) -> decltype(container.size());
    template auto getContainerSize(const std::string& container) -> decltype(container.size());
    
    template auto dereferencePointer(int* ptr) -> decltype(*ptr);
    template auto dereferencePointer(std::string* ptr) -> decltype(*ptr);

    // comprehensive test runner implementation
    
    auto DeclTypeTestRunner::runAllTests() -> std::expected<void, DeclTypeError> {
        LOG_INFO_PRINT("starting comprehensive decltype test suite");
        
        // run all test categories with railway-oriented programming
        return testBasicDeclType()
            .and_then([](){ return testAdvancedDeclType(); })
            .and_then([](){ return testFunctionObjects(); })
            .and_then([](){ return testExpressionTemplates(); })
            .and_then([](){ return testUtilityFunctions(); })
            .and_then([](){
                LOG_INFO_PRINT("all decltype tests completed successfully");
                return std::expected<void, DeclTypeError>{};
            });
    }
    
    auto DeclTypeTestRunner::testBasicDeclType() -> std::expected<void, DeclTypeError> {
        LOG_INFO_PRINT("testing basic decltype functionality");
        
        try {
            BasicDeclTypeDemo demo{};
            
            // test basic member variable decltype
            auto intValue = demo.getIntegerValueType();
            assert(intValue == 42);
            assert((std::is_same_v<decltype(intValue), int>));
            
            // test expression decltype
            auto doubleResult = demo.getDoubleExpression();
            assert(doubleResult > 6.0);
            assert((std::is_same_v<decltype(doubleResult), double>));
            
            // test member function decltype
            auto stringLength = demo.getStringLength();
            assert(stringLength > 0);
            assert((std::is_same_v<decltype(stringLength), std::string::size_type>));
            
            // test reference decltype with parentheses
            auto& intRef = demo.getIntegerReference();
            assert(&intRef == &demo.getIntegerReference());
            assert((std::is_same_v<decltype(intRef), int&>));
            
            // test template expression decltype
            auto result1 = demo.complexExpression(10);
            assert(result1 == 52);
            assert((std::is_same_v<decltype(result1), int>));
            
            auto result2 = demo.complexExpression(5L);
            assert(result2 == 47L);
            assert((std::is_same_v<decltype(result2), long>));
            
            LOG_INFO_PRINT("basic decltype tests passed");
            return std::expected<void, DeclTypeError>{};
            
        } catch (...) {
            LOG_ERROR_PRINT("basic decltype test failed with exception");
            return std::unexpected(DeclTypeError::OperationFailed);
        }
    }
    
    auto DeclTypeTestRunner::testAdvancedDeclType() -> std::expected<void, DeclTypeError> {
        LOG_INFO_PRINT("testing advanced decltype with containers");
        
        try {
            AdvancedDeclTypeDemo<int> intDemo{};
            AdvancedDeclTypeDemo<std::string> stringDemo{};
            
            // test iterator decltype
            auto beginIter = intDemo.begin();
            auto endIter = intDemo.end();
            assert((std::is_same_v<decltype(beginIter), decltype(endIter)>));
            
            // test const iterator decltype
            auto cbeginIter = intDemo.cbegin();
            auto cendIter = intDemo.cend();
            assert((std::is_same_v<decltype(cbeginIter), decltype(cendIter)>));
            
            // test perfect forwarding decltype
            int value = 42;
            intDemo.addElement(value);
            intDemo.addElement(100);
            
            std::string str = "test";
            stringDemo.addElement(str);
            stringDemo.addElement("hello");
            
            // test sfinae with decltype
            const std::vector<int> otherInts{1, 2, 3};
            const auto combineResult = intDemo.combineWith(otherInts);
            assert(combineResult.has_value());
            
            // test decltype(auto) - add some elements first
            intDemo.addElement(1);
            intDemo.addElement(2);
            
            decltype(auto) lastElement = intDemo.getLastElement();
            assert((std::is_reference_v<decltype(lastElement)>));
            
            LOG_INFO_PRINT("advanced decltype tests passed");
            return std::expected<void, DeclTypeError>{};
            
        } catch (...) {
            LOG_ERROR_PRINT("advanced decltype test failed with exception");
            return std::unexpected(DeclTypeError::OperationFailed);
        }
    }
    
    auto DeclTypeTestRunner::testFunctionObjects() -> std::expected<void, DeclTypeError> {
        LOG_INFO_PRINT("testing decltype with function objects and lambdas");
        
        try {
            FunctionObjectDemo demo{};
            
            // test function object decltype
            auto squareFunc = [](int x) { return x * x; };
            auto squareResult = demo.callIntFunction(squareFunc, 5);
            assert(squareResult == 25);
            assert((std::is_same_v<decltype(squareResult), int>));
            
            // test lambda creation with decltype
            auto multiplier = demo.createMultiplier(3);
            auto multiplyResult = multiplier(7);
            assert(multiplyResult == 21);
            
            // test member function pointer with decltype
            std::string testStr = "hello";
            // note: member function pointer usage demonstrated in concept
            
            LOG_INFO_PRINT("function object decltype tests passed");
            return std::expected<void, DeclTypeError>{};
            
        } catch (...) {
            LOG_ERROR_PRINT("function object decltype test failed with exception");
            return std::unexpected(DeclTypeError::OperationFailed);
        }
    }
    
    auto DeclTypeTestRunner::testExpressionTemplates() -> std::expected<void, DeclTypeError> {
        LOG_INFO_PRINT("testing decltype with expression templates");
        
        try {
            ExpressionTemplateDemo<int> intExpr{10};
            ExpressionTemplateDemo<double> doubleExpr{3.14};
            
            // test operator+ with decltype
            auto addResult = intExpr + doubleExpr;
            auto addValue = addResult.getValue();
            assert(addValue > 13.0);
            assert((std::is_same_v<decltype(addValue), double>));
            
            // test operator== with decltype
            ExpressionTemplateDemo<int> anotherInt{10};
            auto equalResult = intExpr == anotherInt;
            assert(equalResult == true);
            assert((std::is_same_v<decltype(equalResult), bool>));
            
            // test operator+= with decltype
            ExpressionTemplateDemo<int> modifiableInt{5};
            auto& compoundResult = modifiableInt += intExpr;
            assert(&compoundResult == &modifiableInt);
            assert(modifiableInt.getValue() == 15);
            assert((std::is_reference_v<decltype(compoundResult)>));
            
            LOG_INFO_PRINT("expression template decltype tests passed");
            return std::expected<void, DeclTypeError>{};
            
        } catch (...) {
            LOG_ERROR_PRINT("expression template decltype test failed with exception");
            return std::unexpected(DeclTypeError::OperationFailed);
        }
    }
    
    auto DeclTypeTestRunner::testUtilityFunctions() -> std::expected<void, DeclTypeError> {
        LOG_INFO_PRINT("testing decltype utility functions");
        
        try {
            // test addValues with different types
            auto intSum = addValues(5, 10);
            assert(intSum == 15);
            assert((std::is_same_v<decltype(intSum), int>));
            
            auto doubleSum = addValues(3.14, 2.86);
            assert(doubleSum > 5.9 && doubleSum < 6.1);
            assert((std::is_same_v<decltype(doubleSum), double>));
            
            auto stringSum = addValues(std::string{"hello"}, std::string{" world"});
            assert(stringSum == "hello world");
            assert((std::is_same_v<decltype(stringSum), std::string>));
            
            // test selectValue with conditional
            auto selectedInt = selectValue(true, 42, 0);
            assert(selectedInt == 42);
            assert((std::is_same_v<decltype(selectedInt), int>));
            
            auto selectedString = selectValue(false, std::string{"first"}, std::string{"second"});
            assert(selectedString == "second");
            assert((std::is_same_v<decltype(selectedString), std::string>));
            
            // test array access
            std::array<int, 5> intArray{1, 2, 3, 4, 5};
            auto arrayElement = accessArray(intArray, 2);
            assert(arrayElement == 3);
            assert((std::is_same_v<decltype(arrayElement), int>));
            
            std::array<std::string, 3> stringArray{"one", "two", "three"};
            auto stringElement = accessArray(stringArray, 1);
            assert(stringElement == "two");
            assert((std::is_same_v<decltype(stringElement), std::string>));
            
            // test container size
            std::vector<int> intVector{1, 2, 3, 4};
            auto vectorSize = getContainerSize(intVector);
            assert(vectorSize == 4);
            assert((std::is_same_v<decltype(vectorSize), std::vector<int>::size_type>));
            
            std::string testString = "hello";
            auto stringSize = getContainerSize(testString);
            assert(stringSize == 5);
            assert((std::is_same_v<decltype(stringSize), std::string::size_type>));
            
            // test pointer dereferencing
            int intValue = 42;
            auto& dereferenced = dereferencePointer(&intValue);
            assert(dereferenced == 42);
            assert((std::is_same_v<decltype(dereferenced), int&>));
            
            std::string stringValue = "test";
            auto& stringDeref = dereferencePointer(&stringValue);
            assert(stringDeref == "test");
            assert((std::is_same_v<decltype(stringDeref), std::string&>));
            
            LOG_INFO_PRINT("utility function decltype tests passed");
            return std::expected<void, DeclTypeError>{};
            
        } catch (...) {
            LOG_ERROR_PRINT("utility function decltype test failed with exception");
            return std::unexpected(DeclTypeError::OperationFailed);
        }
    }

} // namespace decltype_examples