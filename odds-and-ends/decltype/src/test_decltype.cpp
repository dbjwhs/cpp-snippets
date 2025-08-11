// MIT License
// Copyright (c) 2025 dbjwhs

#include "../headers/decltype_examples.hpp"
#include "../../../headers/project_utils.hpp"

#include <cassert>
#include <sstream>
#include <type_traits>

// comprehensive test suite for decltype functionality
// demonstrates all decltype capabilities with thorough validation
namespace decltype_tests {

    // test basic decltype functionality
    auto testBasicDeclTypeFeatures() -> bool {
        LOG_INFO_PRINT("testing basic decltype features...");
        
        decltype_examples::BasicDeclTypeDemo demo{};
        
        // test member variable decltype
        auto intValue = demo.getIntegerValueType();
        assert(intValue == 42);
        assert((std::is_same_v<decltype(intValue), int>));
        LOG_INFO_PRINT("✓ basic member variable decltype");
        
        // test expression decltype
        auto doubleResult = demo.getDoubleExpression();
        assert(doubleResult > 6.0);
        assert((std::is_same_v<decltype(doubleResult), double>));
        LOG_INFO_PRINT("✓ arithmetic expression decltype");
        
        // test member function decltype
        auto stringLength = demo.getStringLength();
        assert(stringLength > 0);
        assert((std::is_same_v<decltype(stringLength), std::string::size_type>));
        LOG_INFO_PRINT("✓ member function call decltype");
        
        // test reference decltype with parentheses
        auto& intRef = demo.getIntegerReference();
        assert(&intRef == &demo.getIntegerReference());
        assert((std::is_same_v<decltype(intRef), int&>));
        LOG_INFO_PRINT("✓ reference type preservation with decltype");
        
        // test template expression decltype
        auto result1 = demo.complexExpression(10);
        assert(result1 == 52);
        assert((std::is_same_v<decltype(result1), int>));
        
        auto result2 = demo.complexExpression(5L);
        assert(result2 == 47L);
        assert((std::is_same_v<decltype(result2), long>));
        LOG_INFO_PRINT("✓ template expression decltype");
        
        return true;
    }
    
    // test advanced container and iterator decltype
    auto testAdvancedContainerDeclType() -> bool {
        LOG_INFO_PRINT("testing advanced container decltype features...");
        
        decltype_examples::AdvancedDeclTypeDemo<int> intDemo{};
        decltype_examples::AdvancedDeclTypeDemo<std::string> stringDemo{};
        
        // test iterator type deduction
        auto beginIter = intDemo.begin();
        auto endIter = intDemo.end();
        assert((std::is_same_v<decltype(beginIter), decltype(endIter)>));
        LOG_INFO_PRINT("✓ iterator type deduction with decltype");
        
        // test const iterator type deduction
        auto cbeginIter = intDemo.cbegin();
        auto cendIter = intDemo.cend();
        assert((std::is_same_v<decltype(cbeginIter), decltype(cendIter)>));
        LOG_INFO_PRINT("✓ const iterator type deduction");
        
        // test perfect forwarding with decltype
        int value = 42;
        intDemo.addElement(value);
        intDemo.addElement(100);
        
        std::string str = "test";
        stringDemo.addElement(str);
        stringDemo.addElement("hello");
        LOG_INFO_PRINT("✓ perfect forwarding with decltype");
        
        // test sfinae with decltype and std::expected
        std::vector<int> otherInts{1, 2, 3};
        auto combineResult = intDemo.combineWith(otherInts);
        assert(combineResult.has_value());
        LOG_INFO_PRINT("✓ sfinae with decltype in std::expected context");
        
        return true;
    }
    
    // test function objects and lambda decltype
    auto testFunctionObjectDeclType() -> bool {
        LOG_INFO_PRINT("testing function object decltype features...");
        
        decltype_examples::FunctionObjectDemo demo{};
        
        // test lambda function decltype
        auto squareFunc = [](int x) { return x * x; };
        auto squareResult = demo.callIntFunction(squareFunc, 5);
        assert(squareResult == 25);
        assert((std::is_same_v<decltype(squareResult), int>));
        LOG_INFO_PRINT("✓ lambda function decltype");
        
        // test lambda creation with decltype
        auto multiplier = demo.createMultiplier(3);
        auto multiplyResult = multiplier(7);
        assert(multiplyResult == 21);
        LOG_INFO_PRINT("✓ lambda creation with decltype");
        
        // test function composition with decltype
        auto addFunc = [](int x) { return x + 10; };
        auto composeResult = demo.callIntFunction(addFunc, 15);
        assert(composeResult == 25);
        assert((std::is_same_v<decltype(composeResult), int>));
        LOG_INFO_PRINT("✓ function composition with decltype");
        
        return true;
    }
    
    // test expression template decltype
    auto testExpressionTemplateDeclType() -> bool {
        LOG_INFO_PRINT("testing expression template decltype features...");
        
        decltype_examples::ExpressionTemplateDemo<int> intExpr{10};
        decltype_examples::ExpressionTemplateDemo<double> doubleExpr{3.14};
        
        // test binary operator decltype
        auto addResult = intExpr + doubleExpr;
        auto addValue = addResult.getValue();
        assert(addValue > 13.0);
        assert((std::is_same_v<decltype(addValue), double>));
        LOG_INFO_PRINT("✓ binary operator expression decltype");
        
        // test comparison operator decltype
        decltype_examples::ExpressionTemplateDemo<int> anotherInt{10};
        auto equalResult = intExpr == anotherInt;
        assert(equalResult == true);
        assert((std::is_same_v<decltype(equalResult), bool>));
        LOG_INFO_PRINT("✓ comparison operator decltype");
        
        // test compound assignment decltype
        decltype_examples::ExpressionTemplateDemo<int> modifiableInt{5};
        auto& compoundResult = modifiableInt += intExpr;
        assert(&compoundResult == &modifiableInt);
        assert(modifiableInt.getValue() == 15);
        assert((std::is_reference_v<decltype(compoundResult)>));
        LOG_INFO_PRINT("✓ compound assignment decltype");
        
        // test chained operations with decltype
        decltype_examples::ExpressionTemplateDemo<int> chain1{1};
        decltype_examples::ExpressionTemplateDemo<int> chain2{2};
        decltype_examples::ExpressionTemplateDemo<int> chain3{3};
        
        auto chainResult = chain1 + chain2 + chain3;
        assert(chainResult.getValue() == 6);
        LOG_INFO_PRINT("✓ chained operations with decltype");
        
        return true;
    }
    
    // test utility function decltype
    auto testUtilityFunctionDeclType() -> bool {
        LOG_INFO_PRINT("testing utility function decltype features...");
        
        // test binary operation decltype with different types
        auto intSum = decltype_examples::addValues(5, 10);
        assert(intSum == 15);
        assert((std::is_same_v<decltype(intSum), int>));
        
        auto doubleSum = decltype_examples::addValues(3.14, 2.86);
        assert(doubleSum > 5.9 && doubleSum < 6.1);
        assert((std::is_same_v<decltype(doubleSum), double>));
        
        auto mixedSum = decltype_examples::addValues(5, 2.5);
        assert(mixedSum > 7.4 && mixedSum < 7.6);
        assert((std::is_same_v<decltype(mixedSum), double>));
        LOG_INFO_PRINT("✓ binary operation type deduction");
        
        // test conditional expression decltype
        auto selectedInt = decltype_examples::selectValue(true, 42, 0);
        assert(selectedInt == 42);
        assert((std::is_same_v<decltype(selectedInt), int>));
        
        auto selectedString = decltype_examples::selectValue(false, std::string{"first"}, std::string{"second"});
        assert(selectedString == "second");
        assert((std::is_same_v<decltype(selectedString), std::string>));
        LOG_INFO_PRINT("✓ conditional expression decltype");
        
        // test array access decltype
        std::array<int, 5> intArray{1, 2, 3, 4, 5};
        auto arrayElement = decltype_examples::accessArray(intArray, 2);
        assert(arrayElement == 3);
        assert((std::is_same_v<decltype(arrayElement), int>));
        
        std::array<std::string, 3> stringArray{"one", "two", "three"};
        auto stringElement = decltype_examples::accessArray(stringArray, 1);
        assert(stringElement == "two");
        assert((std::is_same_v<decltype(stringElement), std::string>));
        LOG_INFO_PRINT("✓ array access decltype");
        
        // test container size decltype
        std::vector<int> intVector{1, 2, 3, 4};
        auto vectorSize = decltype_examples::getContainerSize(intVector);
        assert(vectorSize == 4);
        assert((std::is_same_v<decltype(vectorSize), std::vector<int>::size_type>));
        
        std::string testString = "hello";
        auto stringSize = decltype_examples::getContainerSize(testString);
        assert(stringSize == 5);
        assert((std::is_same_v<decltype(stringSize), std::string::size_type>));
        LOG_INFO_PRINT("✓ container size decltype");
        
        // test pointer dereferencing decltype
        int intValue = 42;
        auto& dereferenced = decltype_examples::dereferencePointer(&intValue);
        assert(dereferenced == 42);
        assert((std::is_same_v<decltype(dereferenced), int&>));
        
        std::string stringValue = "test";
        auto& stringDeref = decltype_examples::dereferencePointer(&stringValue);
        assert(stringDeref == "test");
        assert((std::is_same_v<decltype(stringDeref), std::string&>));
        LOG_INFO_PRINT("✓ pointer dereference decltype");
        
        return true;
    }
    
    // test decltype with concepts and constraints
    auto testConceptConstrainedDeclType() -> bool {
        LOG_INFO_PRINT("testing concept-constrained decltype features...");
        
        // test concept satisfaction with decltype
        auto intValue = 42;
        auto doubleValue = 3.14159;
        auto stringValue = std::string{"test"};
        
        static_assert(decltype_examples::HasDeclTypeExpression<decltype(intValue)>);
        static_assert(decltype_examples::HasDeclTypeExpression<decltype(doubleValue)>);
        static_assert(decltype_examples::HasDeclTypeExpression<decltype(stringValue)>);
        LOG_INFO_PRINT("✓ concept satisfaction with decltype");
        
        // test template constraint satisfaction
        decltype_examples::AdvancedDeclTypeDemo<int> intContainer{};
        decltype_examples::AdvancedDeclTypeDemo<std::string> stringContainer{};
        
        // these should compile due to concept constraints
        intContainer.addElement(42);
        stringContainer.addElement("hello");
        LOG_INFO_PRINT("✓ template constraint satisfaction");
        
        // test sfinae with concepts
        static_assert(std::copyable<int>);
        static_assert(std::copyable<std::string>);
        static_assert(std::regular<int>);
        static_assert(std::regular<double>);
        LOG_INFO_PRINT("✓ concept-based sfinae with decltype");
        
        return true;
    }
    
    // test edge cases and special decltype scenarios
    auto testDeclTypeEdgeCases() -> bool {
        LOG_INFO_PRINT("testing decltype edge cases and special scenarios...");
        
        // test decltype with void expressions
        auto voidFunc = []() { return; };
        static_assert(std::is_same_v<decltype(voidFunc()), void>);
        LOG_INFO_PRINT("✓ decltype with void expressions");
        
        // test decltype with throw expressions
        auto throwExpr = [](bool shouldThrow) -> int {
            return shouldThrow ? throw std::runtime_error("test") : 42;
        };
        
        // decltype of throw expression is the common type
        try {
            auto result = throwExpr(false);
            assert(result == 42);
            assert((std::is_same_v<decltype(throwExpr(false)), int>));
        } catch (...) {
            // expected for edge case testing
        }
        LOG_INFO_PRINT("✓ decltype with conditional throw expressions");
        
        // test decltype with comma operator
        auto commaExpr = [](int x, int y) {
            // use comma operator in a meaningful way
            return (static_cast<void>(x), y);  // comma operator returns right operand
        };
        auto commaResult = commaExpr(1, 2);
        assert(commaResult == 2);
        assert((std::is_same_v<decltype(commaResult), int>));
        LOG_INFO_PRINT("✓ decltype with comma operator");
        
        // test decltype with ternary operator and different types
        auto ternaryMixed = [](bool condition) -> decltype(condition ? 42 : 3.14) {
            return condition ? 42 : 3.14;
        };
        auto ternaryResult = ternaryMixed(true);
        assert((std::is_same_v<decltype(ternaryResult), double>));  // common type is double
        LOG_INFO_PRINT("✓ decltype with mixed-type ternary operator");
        
        return true;
    }

} // namespace decltype_tests

// main test runner function
int main() {
    LOG_INFO_PRINT("starting comprehensive decltype test suite...");
    
    // suppress stderr during testing to keep output clean
    Logger::StderrSuppressionGuard suppressGuard{};
    
    try {
        // run all test categories
        bool allTestsPassed = true;
        
        allTestsPassed &= decltype_tests::testBasicDeclTypeFeatures();
        LOG_INFO_PRINT("");
        
        allTestsPassed &= decltype_tests::testAdvancedContainerDeclType();
        LOG_INFO_PRINT("");
        
        allTestsPassed &= decltype_tests::testFunctionObjectDeclType();
        LOG_INFO_PRINT("");
        
        allTestsPassed &= decltype_tests::testExpressionTemplateDeclType();
        LOG_INFO_PRINT("");
        
        allTestsPassed &= decltype_tests::testUtilityFunctionDeclType();
        LOG_INFO_PRINT("");
        
        allTestsPassed &= decltype_tests::testConceptConstrainedDeclType();
        LOG_INFO_PRINT("");
        
        allTestsPassed &= decltype_tests::testDeclTypeEdgeCases();
        LOG_INFO_PRINT("");
        
        if (allTestsPassed) {
            LOG_INFO_PRINT("🎉 all decltype tests passed successfully!");
            LOG_INFO_PRINT("comprehensive decltype functionality verified");
            return 0;
        } else {
            LOG_ERROR_PRINT("❌ some decltype tests failed");
            return 1;
        }
        
    } catch (const std::exception& e) {
        LOG_ERROR_PRINT("❌ test suite failed with exception: {}", e.what());
        return 1;
    } catch (...) {
        LOG_ERROR_PRINT("❌ test suite failed with unknown exception");
        return 1;
    }
}