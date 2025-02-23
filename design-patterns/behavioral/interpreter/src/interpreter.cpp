// MIT License
// Copyright (c) 2025 dbjwhs

#include <memory>
#include <string>
#include <cassert>
#include <stdexcept>
#include <sstream>
#include <thread>
#include "../../../../headers/project_utils.hpp"

// interpreter pattern overview
//
// history:
// - introduced in 1994-95 by the "gang of four" in their design patterns book
// - based on formal grammar theory from the 1950s and 60s
// - heavily influenced by early compiler design principles
// - foundational pattern in language processing and domain-specific languages (dsls)
//
// core concept:
// - defines a grammar for a simple language
// - creates an interpreter to process sentences in that language
// - builds sentences using an abstract syntax tree (ast)
//
// pattern structure:
// - abstract expression: base class defining interpret() interface
// - terminal expression: leaf nodes that implement simple grammar rules
// - nonterminal expression: composite nodes for complex rules
// - context: stores global information and state during interpretation
// - client: builds and traverses the ast
//
// common applications:
// 1. programming language interpreters
//    - script engines
//    - macro processors
//    - configuration parsers
//
// 2. mathematical expressions
//    - formula evaluators
//    - spreadsheet calculations
//    - mathematical plotting tools
//
// 3. business rules engines
//    - validation rules
//    - workflow definitions
//    - policy enforcement
//
// 4. query languages
//    - sql interpreters
//    - search expressions
//    - filter criteria
//
// key benefits:
// - flexibility in grammar definition
// - easy to extend with new expressions
// - separation of grammar from interpretation
// - natural representation of hierarchical structures
//
// limitations:
// - complex grammars create complex class hierarchies
// - can be overkill for simple languages
// - may have performance overhead
// - maintenance complexity increases with grammar size
//
// best practices:
// - keep grammar simple and focused
// - use for domain-specific languages
// - consider alternatives for complex grammars
// - implement good error handling
// - maintain clear documentation
//
// implementation tips:
// - use visitor pattern for operations on ast
// - implement proper memory management
// - provide clear error messages
// - consider parsing performance
// - add debugging capabilities

class Context;

// abstract expression interface with debug capabilities
class Expression {
public:
    virtual ~Expression() = default;
    [[nodiscard]] virtual int interpret(const Context& context) const = 0;
    [[nodiscard]] virtual std::string toString() const = 0;
    virtual void debugPrint(const int depth = 0) const {
        Logger::getInstance().log_with_depth(LogLevel::DEBUG, depth, std::format("Expression: {}", toString()));
    }
};

// enhanced context class with operation tracking
class Context {
private:
    std::unordered_map<std::string, int> m_variables;
    mutable int m_operationCount{0};

public:
    void resetOperationCount() const {
        m_operationCount = 0;
        Logger::getInstance().log(LogLevel::DEBUG, "Context: Reset operation count");
    }

    void setVariable(const std::string& name, int value) {
        m_variables[name] = value;
        Logger::getInstance().log(LogLevel::DEBUG,
            std::format("Context: Setting variable '{}' to {}", name, value));
    }

    int getVariable(const std::string& name) const {
        auto it = m_variables.find(name);
        if (it == m_variables.end()) {
            throw std::runtime_error("Variable not found: " + name);
        }
        Logger::getInstance().log(LogLevel::DEBUG,
            std::format("Context: Retrieved variable '{}' = {}", name, it->second));
        return it->second;
    }

    void incrementOperations() const {
        m_operationCount++;
        Logger::getInstance().log(LogLevel::DEBUG,
            std::format("Context: Operation count: {}", m_operationCount));
    }

    int getOperationCount() const { return m_operationCount; }
};

// terminal expression for numbers
class NumberExpression final : public Expression {
private:
    int m_number;

public:
    explicit NumberExpression(int number) : m_number(number) {
        Logger::getInstance().log(LogLevel::DEBUG,
            std::format("Creating NumberExpression with value {}", m_number));
    }

    [[nodiscard]] int interpret(const Context& context) const override {
        context.incrementOperations();
        Logger::getInstance().log(LogLevel::DEBUG,
            std::format("NumberExpression: Interpreting constant {}", m_number));
        return m_number;
    }

    [[nodiscard]] std::string toString() const override {
        return std::to_string(m_number);
    }
};

// terminal expression for variables
class VariableExpression final : public Expression {
private:
    std::string m_name;

public:
    explicit VariableExpression(std::string name) : m_name(std::move(name)) {
        Logger::getInstance().log(LogLevel::DEBUG,
            std::format("Creating VariableExpression for '{}'", m_name));
    }

    [[nodiscard]] int interpret(const Context& context) const override {
        context.incrementOperations();
        int value = context.getVariable(m_name);
        Logger::getInstance().log(LogLevel::DEBUG,
            std::format("VariableExpression: Retrieved '{}' = {}", m_name, value));
        return value;
    }

    [[nodiscard]] std::string toString() const override {
        return m_name;
    }
};

// base class for binary operations
class BinaryExpression : public Expression {
protected:
    std::unique_ptr<Expression> m_left;
    std::unique_ptr<Expression> m_right;
    std::string m_operatorSymbol;

public:
    BinaryExpression(std::unique_ptr<Expression> left, std::unique_ptr<Expression> right, std::string op)
        : m_left(std::move(left))
        , m_right(std::move(right))
        , m_operatorSymbol(std::move(op)) {
            Logger::getInstance().log(LogLevel::DEBUG,
                std::format("Creating BinaryExpression with operator '{}'", m_operatorSymbol));
    }

    void debugPrint(const int depth = 0) const override {
        Expression::debugPrint(depth);
        m_left->debugPrint(depth + 1);
        m_right->debugPrint(depth + 1);
    }

    [[nodiscard]] std::string toString() const override {
        return "(" + m_left->toString() + " " + m_operatorSymbol + " " + m_right->toString() + ")";
    }
};

// non-terminal expression for addition
class AddExpression final : public BinaryExpression {
public:
    AddExpression(std::unique_ptr<Expression> left, std::unique_ptr<Expression> right)
        : BinaryExpression(std::move(left), std::move(right), "+") {}

    [[nodiscard]] int interpret(const Context& context) const override {
        context.incrementOperations();
        int result = m_left->interpret(context) + m_right->interpret(context);
        Logger::getInstance().log(LogLevel::DEBUG,
            std::format("AddExpression: {} = {}", this->toString(), result));
        return result;
    }
};

// non-terminal expression for subtraction
class SubtractExpression final : public BinaryExpression {
public:
    SubtractExpression(std::unique_ptr<Expression> left, std::unique_ptr<Expression> right)
        : BinaryExpression(std::move(left), std::move(right), "-") {}

    [[nodiscard]] int interpret(const Context& context) const override {
        context.incrementOperations();
        int result = m_left->interpret(context) - m_right->interpret(context);
        Logger::getInstance().log(LogLevel::DEBUG,
            std::format("SubtractExpression: {} = {}", this->toString(), result));
        return result;
    }
};

// non-terminal expression for multiplication
class MultiplyExpression final : public BinaryExpression {
public:
    MultiplyExpression(std::unique_ptr<Expression> left, std::unique_ptr<Expression> right)
        : BinaryExpression(std::move(left), std::move(right), "*") {}

    [[nodiscard]] int interpret(const Context& context) const override {
        context.incrementOperations();
        int result = m_left->interpret(context) * m_right->interpret(context);
        Logger::getInstance().log(LogLevel::DEBUG,
            std::format("MultiplyExpression: {} = {}", this->toString(), result));
        return result;
    }
};

// non-terminal expression for division
class DivideExpression final : public BinaryExpression {
public:
    DivideExpression(std::unique_ptr<Expression> left, std::unique_ptr<Expression> right)
        : BinaryExpression(std::move(left), std::move(right), "/") {}

    [[nodiscard]] int interpret(const Context& context) const override {
        context.incrementOperations();
        const int rightValue = m_right->interpret(context);
        if (rightValue == 0) {
            // ### make this INFO since we have tests for it, but this should be an ERROR
            Logger::getInstance().log(LogLevel::INFO, "DivideExpression: Division by zero");
            throw std::runtime_error("Division by zero");
        }
        int result = m_left->interpret(context) / rightValue;
        Logger::getInstance().log(LogLevel::DEBUG,
            std::format("DivideExpression: {} = {}", this->toString(), result));
        return result;
    }
};

// non-terminal expression for modulo
class ModuloExpression final : public BinaryExpression {
public:
    ModuloExpression(std::unique_ptr<Expression> left, std::unique_ptr<Expression> right)
        : BinaryExpression(std::move(left), std::move(right), "%") {}

    [[nodiscard]] int interpret(const Context& context) const override {
        context.incrementOperations();
        const int rightValue = m_right->interpret(context);
        if (rightValue == 0) {
            Logger::getInstance().log(LogLevel::ERROR, "ModuloExpression: Modulo by zero");
            throw std::runtime_error("Modulo by zero");
        }
        int result = m_left->interpret(context) % rightValue;
        Logger::getInstance().log(LogLevel::DEBUG,
            std::format("ModuloExpression: {} = {}", this->toString(), result));
        return result;
    }
};

// non-terminal expression for power operation
class PowerExpression final : public BinaryExpression {
public:
    PowerExpression(std::unique_ptr<Expression> left, std::unique_ptr<Expression> right)
        : BinaryExpression(std::move(left), std::move(right), "^") {}

    [[nodiscard]] int interpret(const Context& context) const override {
        context.incrementOperations();
        const int base = m_left->interpret(context);
        const int exponent = m_right->interpret(context);
        if (exponent < 0) {
            Logger::getInstance().log(LogLevel::ERROR, "PowerExpression: Negative exponent");
            throw std::runtime_error("Negative exponent not supported");
        }
        int result = 1;
        for (int ndx = 0; ndx < exponent; ++ndx) {
            result *= base;
        }
        Logger::getInstance().log(LogLevel::DEBUG,
            std::format("PowerExpression: {} = {}", this->toString(), result));
        return result;
    }
};

// comprehensive test suite
void runTests() {
    Logger::getInstance().log(LogLevel::INFO, "Starting comprehensive interpreter pattern tests");

    // test case 1: basic arithmetic operations
    {
        Context context;
        Logger::getInstance().log(LogLevel::INFO, "Test 1: Basic arithmetic operations");

        auto expr = std::make_unique<AddExpression>(
            std::make_unique<NumberExpression>(5),
            std::make_unique<NumberExpression>(3)
        );

        expr->debugPrint();
        assert(expr->interpret(context) == 8);
        Logger::getInstance().log(LogLevel::INFO, "Test 1a: Addition passed");

        auto expr2 = std::make_unique<MultiplyExpression>(
            std::make_unique<NumberExpression>(4),
            std::make_unique<NumberExpression>(6)
        );

        expr2->debugPrint();
        assert(expr2->interpret(context) == 24);
        Logger::getInstance().log(LogLevel::INFO, "Test 1b: Multiplication passed");
    }

    // test case 2: variable operations
    {
        Context context;
        context.setVariable("x", 10);
        context.setVariable("y", 5);
        Logger::getInstance().log(LogLevel::INFO, "Test 2: Variable operations");

        auto expr = std::make_unique<DivideExpression>(
            std::make_unique<VariableExpression>("x"),
            std::make_unique<VariableExpression>("y")
        );

        expr->debugPrint();
        assert(expr->interpret(context) == 2);
        Logger::getInstance().log(LogLevel::INFO, "Test 2: Division with variables passed");
    }

    // test case 3: complex expression tree
    {
        Context context;
        context.setVariable("a", 15);
        context.setVariable("b", 3);
        Logger::getInstance().log(LogLevel::INFO, "Test 3: Complex expression tree");

        // creates: ((a + 5) * (b - 1)) % 4
        auto expr = std::make_unique<ModuloExpression>(
            std::make_unique<MultiplyExpression>(
                std::make_unique<AddExpression>(
                    std::make_unique<VariableExpression>("a"),
                    std::make_unique<NumberExpression>(5)
                ),
                std::make_unique<SubtractExpression>(
                    std::make_unique<VariableExpression>("b"),
                    std::make_unique<NumberExpression>(1)
                )
            ),
            std::make_unique<NumberExpression>(4)
        );

        expr->debugPrint();
        assert(expr->interpret(context) == 0);
        Logger::getInstance().log(LogLevel::INFO, "Test 3: Complex expression evaluation passed");
    }

    // test case 4: power operations
    {
        Context context;
        Logger::getInstance().log(LogLevel::INFO, "Test 4: Power operations");

        auto expr = std::make_unique<PowerExpression>(
            std::make_unique<NumberExpression>(2),
            std::make_unique<NumberExpression>(3)
        );

        expr->debugPrint();
        assert(expr->interpret(context) == 8);
        Logger::getInstance().log(LogLevel::INFO, "Test 4: Power operation passed");
    }

    // test case 5: error handling
    {
        Context context;
        Logger::getInstance().log(LogLevel::INFO, "Test 5: Error handling");

        // test division by zero
        auto expr1 = std::make_unique<DivideExpression>(
            std::make_unique<NumberExpression>(10),
            std::make_unique<NumberExpression>(0)
        );

        int result = 0;
        try {
            expr1->debugPrint();
            result = expr1->interpret(context);
            assert(false && "Should have thrown division by zero exception");
        } catch (const std::runtime_error& e) {
            Logger::getInstance().log(LogLevel::INFO
                , "Test 5a: Division by zero exception caught correctly {} result {}", e.what(), result);
        }

        // test undefined variable
        auto expr2 = std::make_unique<VariableExpression>("undefined");
        try {
            expr2->debugPrint();
            result = expr2->interpret(context);
            assert(false && "Should have thrown undefined variable exception");
        } catch (const std::runtime_error& e) {
            // ### make this INFO since we have tests for it, but this should be an ERROR
            Logger::getInstance().log(LogLevel::INFO, std::format("Context: {}", e.what()));

            Logger::getInstance().log(LogLevel::INFO
                , "Test 5b: Undefined variable exception caught correctly, interpret result: {}", result);
        }
    }

    // test case 6: operation counting
    {
        Context context;
        Logger::getInstance().log(LogLevel::INFO, "Test 6: Operation counting");

        // Reset operation count before this test
        context.resetOperationCount();

        // Create expression: (2 * 3) + 4
        auto expr = std::make_unique<AddExpression>(
            std::make_unique<MultiplyExpression>(
                std::make_unique<NumberExpression>(2),
                std::make_unique<NumberExpression>(3)
            ),
            std::make_unique<NumberExpression>(4)
        );

        expr->debugPrint();
        int result = expr->interpret(context);

        // Count should be:
        // 1 for multiply
        // 1 for add
        // 3 for number expressions (2, 3, and 4)
        // Total: 5 operations
        assert(context.getOperationCount() == 5);
        Logger::getInstance().log(LogLevel::INFO,
            std::format("Test 6: Operation counting passed. Total operations: {}, interpret result: {}"
                , context.getOperationCount(), result));
    }
}

int main() {
    try {
        Logger::getInstance().log(LogLevel::INFO, "Starting interpreter pattern tests");
        runTests();
        Logger::getInstance().log(LogLevel::INFO, "All tests passed successfully");
        return 0;
    } catch (const std::exception& e) {
        Logger::getInstance().log(LogLevel::ERROR, std::format("Test failed with error: {}", e.what()));
        return 1;
    }
}
