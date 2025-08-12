// MIT License
// Copyright (c) 2025 dbjwhs

#include <iostream>
#include <cassert>
#include <vector>
#include <string>
#include <format>
#include "../../../headers/project_utils.hpp"

// hoare logic, developed by sir charles antony richard hoare in 1969, is a formal system for 
// reasoning about the correctness of computer programs. it uses logical assertions to specify
// the state of a program before and after execution of code segments. the core concept is the 
// hoare triple, written as {p} c {q}, where p is a precondition, c is a command/program, and q
// is a post condition. this system allows mathematically proving that programs meet their
// specifications. hoare logic has been foundational in formal verification, compiler technology,
// and programming language design. common applications include safety-critical systems, smart 
// contracts, and security protocols where correctness is essential.

// class representing the state of variables in a program
class ProgramState {
private:
    // map to store variable names and their values
    std::unordered_map<std::string, int> m_variables;

public:
    // constructor to initialize an empty state
    ProgramState() = default;
    
    // constructor to initialize state with a set of variables
    explicit ProgramState(const std::unordered_map<std::string, int>& variables) : m_variables(variables) {}
    
    // method to set a variable's value
    void setVariable(const std::string& name, const int value) {
        m_variables[name] = value;
    }
    
    // method to get a variable's value
    [[nodiscard]] int getVariable(const std::string& name) const {
        if (const auto it = m_variables.find(name); it != m_variables.end()) {
            return it->second;
        }
        // return 0 if variable doesn't exist (for simplicity)
        return 0;
    }
    
    // method to check if a variable exists
    [[nodiscard]] bool hasVariable(const std::string& name) const {
        return m_variables.contains(name);
    }
    
    // method to get all variables
    [[nodiscard]] const std::unordered_map<std::string, int>& getVariables() const {
        return m_variables;
    }
};

// base class for assertions in hoare logic
class Assertion {
public:
    // virtual destructor for proper inheritance
    virtual ~Assertion() = default;
    
    // method to evaluate if assertion holds for a given program state
    [[nodiscard]] virtual bool evaluate(const ProgramState& state) const = 0;
    
    // method to get string representation of the assertion
    [[nodiscard]] virtual std::string toString() const = 0;
};

// class for constant assertion (always true or false)
class ConstantAssertion final : public Assertion {
private:
    // boolean value of the assertion
    bool m_value;

public:
    // constructor to set the constant value
    explicit ConstantAssertion(const bool value) : m_value(value) {}
    
    // evaluate method implementation
    [[nodiscard]] bool evaluate(const ProgramState& state) const override {
        return m_value;
    }
    
    // string representation method implementation
    [[nodiscard]] std::string toString() const override {
        return m_value ? "true" : "false";
    }
};

// class for variable equality assertion (e.g., x == 5)
class VariableEqualityAssertion final : public Assertion {
private:
    // name of the variable
    std::string m_variableName;
    
    // value to compare against
    int m_value;

public:
    // constructor to initialize with variable name and expected value
    VariableEqualityAssertion(std::string variableName, const int value)
        : m_variableName(std::move(variableName)), m_value(value) {}
    
    // evaluate method implementation
    [[nodiscard]] bool evaluate(const ProgramState& state) const override {
        return state.getVariable(m_variableName) == m_value;
    }
    
    // string representation method implementation
    [[nodiscard]] std::string toString() const override {
        return m_variableName + " == " + std::to_string(m_value);
    }
};

// class for conjunction of assertions (AND)
class ConjunctionAssertion final : public Assertion {
private:
    // left and right assertions for AND operation
    std::shared_ptr<Assertion> m_left;
    std::shared_ptr<Assertion> m_right;

public:
    // constructor to initialize with two assertions
    ConjunctionAssertion(std::shared_ptr<Assertion> left, std::shared_ptr<Assertion> right)
        : m_left(std::move(left)), m_right(std::move(right)) {}
    
    // evaluate method implementation (both must be true)
    [[nodiscard]] bool evaluate(const ProgramState& state) const override {
        return m_left->evaluate(state) && m_right->evaluate(state);
    }
    
    // string representation method implementation
    [[nodiscard]] std::string toString() const override {
        return "(" + m_left->toString() + " ∧ " + m_right->toString() + ")";
    }
};

// base class for commands in hoare logic
class Command {
public:
    // virtual destructor for proper inheritance
    virtual ~Command() = default;
    
    // method to execute the command on a program state
    [[nodiscard]] virtual ProgramState execute(const ProgramState& state) const = 0;
    
    // method to get string representation of the command
    [[nodiscard]] virtual std::string toString() const = 0;
    
    // method to compute weakest precondition for a given post-condition
    [[nodiscard]] virtual std::shared_ptr<Assertion> weakestPrecondition(std::shared_ptr<Assertion> post_condition) const = 0;
};

// class for assignment command (e.g., x = 5)
class AssignmentCommand final : public Command {
private:
    // name of the variable to assign
    std::string m_variableName;
    
    // value to assign
    int m_value;

    // flag to indicate if the value is from another variable
    bool m_isVariableValue;

    // source variable name (if m_isVariableValue is true)
    std::string m_sourceVariableName;

public:
    // constructor to initialize with variable name and constant value
    AssignmentCommand(std::string variableName, const int value)
        : m_variableName(std::move(variableName)),
          m_value(value),
          m_isVariableValue(false) {}

    // constructor to initialize with variable name and source variable
    AssignmentCommand(std::string variableName, std::string sourceVariableName)
        : m_variableName(std::move(variableName)),
          m_value(0),
          m_isVariableValue(true),
          m_sourceVariableName(std::move(sourceVariableName)) {}

    // execute method implementation
    [[nodiscard]] ProgramState execute(const ProgramState& state) const override {
        ProgramState newState = state;

        // if assigning from another variable, get its value from the state
        if (m_isVariableValue) {
            const int sourceValue = state.getVariable(m_sourceVariableName);
            newState.setVariable(m_variableName, sourceValue);
        } else {
            // otherwise use the constant value
            newState.setVariable(m_variableName, m_value);
        }

        return newState;
    }

    // string representation method implementation
    [[nodiscard]] std::string toString() const override {
        if (m_isVariableValue) {
            return m_variableName + " = " + m_sourceVariableName;
        } else {
            return m_variableName + " = " + std::to_string(m_value);
        }
    }

    // weakest precondition calculation for assignment
    [[nodiscard]] std::shared_ptr<Assertion> weakestPrecondition(std::shared_ptr<Assertion> post_condition) const override {
        // for an assignment x = e, the weakest precondition for Q is Q[x/e]
        // this is a simplified version as we only handle constant assignments
        // in a real implementation. we would need substitution of expressions
        return post_condition;
    }
};

// class for sequence of commands
class SequenceCommand final : public Command {
private:
    // first and second commands in sequence
    std::shared_ptr<Command> m_first;
    std::shared_ptr<Command> m_second;

public:
    // constructor to initialize with two commands
    SequenceCommand(std::shared_ptr<Command> first, std::shared_ptr<Command> second)
        : m_first(std::move(first)), m_second(std::move(second)) {}

    // execute method implementation (execute first, then second)
    [[nodiscard]] ProgramState execute(const ProgramState& state) const override {
        ProgramState intermediate = m_first->execute(state);
        return m_second->execute(intermediate);
    }

    // string representation method implementation
    [[nodiscard]] std::string toString() const override {
        return m_first->toString() + "; " + m_second->toString();
    }

    // weakest precondition calculation for a sequence
    [[nodiscard]] std::shared_ptr<Assertion> weakestPrecondition(std::shared_ptr<Assertion> post_condition) const override {
        // for C1;C2, the weakest precondition for Q is wp(C1, wp(C2, Q))
        const auto wp2 = m_second->weakestPrecondition(post_condition);
        return m_first->weakestPrecondition(wp2);
    }
};

// class representing a hoare triple {p} c {q}
class HoareTriple {
private:
    // precondition, command, and post-condition of the triple
    std::shared_ptr<Assertion> m_precondition;
    std::shared_ptr<Command> m_command;
    std::shared_ptr<Assertion> m_post_condition;

public:
    // constructor to initialize with precondition, command, and post-condition
    HoareTriple(std::shared_ptr<Assertion> precondition,
        std::shared_ptr<Command> command,
        std::shared_ptr<Assertion> post_condition
    ) : m_precondition(std::move(precondition)),
        m_command(std::move(command)),
        m_post_condition(std::move(post_condition)) {}

    // method to verify if the triple is valid
    [[nodiscard]] static bool verify() {
        // this is simplistic verification
        // in reality, formal verification requires proof systems
        return true; // placeholder
    }

    // method to execute the command and check if post-condition holds
    [[nodiscard]] bool executeAndVerify(const ProgramState& initialState) const {
        // check if precondition holds
        if (!m_precondition->evaluate(initialState)) {
            LOG_INFO("Precondition does not hold");
            return false;
        }

        // execute command
        const ProgramState finalState = m_command->execute(initialState);

        // check if post-condition holds
        const bool post_condition_holds = m_post_condition->evaluate(finalState);
        if (!post_condition_holds) {
            LOG_INFO("Post condition does not hold after execution");
        }

        return post_condition_holds;
    }

    // method to get string representation of the triple
    [[nodiscard]] std::string toString() const {
        return "{" + m_precondition->toString() + "} " + m_command->toString() + " " + "{" + m_post_condition->toString() + "}";
    }

    // getters for the components
    [[nodiscard]] std::shared_ptr<Assertion> getPrecondition() const { return m_precondition; }
    [[nodiscard]] std::shared_ptr<Command> getCommand() const { return m_command; }
    [[nodiscard]] std::shared_ptr<Assertion> getPostcondition() const { return m_post_condition; }
};

// function to test a simple increment example
void testIncrementExample() {
    LOG_INFO("Running increment example test");

    // create a program state with variable x = 5
    ProgramState state;
    state.setVariable("x", 5);

    // create a precondition: x == 5
    const auto precondition = std::make_shared<VariableEqualityAssertion>("x", 5);

    // create a command: x = 6
    const auto command = std::make_shared<AssignmentCommand>("x", 6);

    // create a post-condition: x == 6
    auto post_condition = std::make_shared<VariableEqualityAssertion>("x", 6);

    // create a hoare triple: {x == 5} x = 6 {x == 6}
    HoareTriple triple(precondition, command, post_condition);

    // log the triple
    LOG_INFO(std::format("Hoare Triple: {}", triple.toString()));

    // execute and verify
    bool result = triple.executeAndVerify(state);

    // check the result
    assert(result);
    LOG_INFO(std::format("Triple verification result: {}", result ? "passed" : "failed"));

    // test with a different initial state where precondition doesn't hold
    ProgramState invalidState;
    invalidState.setVariable("x", 10);

    // this should not verify correctly
    result = triple.executeAndVerify(invalidState);
    assert(!result);
    LOG_INFO(std::format("Triple verification with invalid state: {}", result ? "passed" : "failed"));
}

// function to test a more complex swap example
void testSwapExample() {
    LOG_INFO("Running swap example test");

    // create a program state with variables a = 10, b = 20
    ProgramState state;
    state.setVariable("a", 10);
    state.setVariable("b", 20);

    // create commands for swap operation using the variable-based assignment
    auto cmd1 = std::make_shared<AssignmentCommand>("temp", "a");   // temp = a
    auto cmd2 = std::make_shared<AssignmentCommand>("a", "b");      // a = b
    auto cmd3 = std::make_shared<AssignmentCommand>("b", "temp");   // b = temp

    // create sequence command: temp = a; a = b; b = temp
    auto seq1 = std::make_shared<SequenceCommand>(cmd1, cmd2);
    const auto swapCommand = std::make_shared<SequenceCommand>(seq1, cmd3);

    // create a precondition: a == 10 ∧ b == 20
    auto pre1 = std::make_shared<VariableEqualityAssertion>("a", 10);
    auto pre2 = std::make_shared<VariableEqualityAssertion>("b", 20);
    const auto precondition = std::make_shared<ConjunctionAssertion>(pre1, pre2);

    // create a post-condition: a == 20 ∧ b == 10
    auto post1 = std::make_shared<VariableEqualityAssertion>("a", 20);
    auto post2 = std::make_shared<VariableEqualityAssertion>("b", 10);
    const auto post_condition = std::make_shared<ConjunctionAssertion>(post1, post2);

    // create a hoare triple: {a == 10 ∧ b == 20} temp = a; a = b; b = temp {a == 20 ∧ b == 10}
    const HoareTriple triple(precondition, swapCommand, post_condition);

    // log the triple
    LOG_INFO(std::format("Hoare Triple: {}", triple.toString()));

    // execute and verify
    const bool result = triple.executeAndVerify(state);

    // check a result with detailed logging on failure
    if (!result) {
        LOG_INFO("Failure details:");
        LOG_INFO(std::format("  Precondition: {}", triple.getPrecondition()->toString()));
        LOG_INFO(std::format("  Command: {}", triple.getCommand()->toString()));
        LOG_INFO(std::format("  Post condition: {}", triple.getPostcondition()->toString()));

        // execute command to see the final state
        ProgramState finalState = triple.getCommand()->execute(state);
        LOG_INFO("Final state after execution:");
        for (const auto& [var, val] : finalState.getVariables()) {
            LOG_INFO(std::format("  {} = {}", var, val));
        }

        // manually check post-condition
        const bool postEval = triple.getPostcondition()->evaluate(finalState);
        LOG_INFO(std::format("Postcondition evaluation: {}", postEval ? "true" : "false"));
    }

    assert(result);
    LOG_INFO(std::format("Triple verification result: {}", result ? "passed" : "failed"));
}

// function to test a case with conjunction assertions
void testConjunctionExample() {
    LOG_INFO("Running conjunction example test");

    // create a program state with variables x = 5, y = 10
    ProgramState state;
    state.setVariable("x", 5);
    state.setVariable("y", 10);

    // create commands for operation: x = 6; y = 11
    auto cmd1 = std::make_shared<AssignmentCommand>("x", 6);
    auto cmd2 = std::make_shared<AssignmentCommand>("y", 11);
    const auto sequence = std::make_shared<SequenceCommand>(cmd1, cmd2);

    // create a precondition: x == 5 ∧ y == 10
    auto pre1 = std::make_shared<VariableEqualityAssertion>("x", 5);
    auto pre2 = std::make_shared<VariableEqualityAssertion>("y", 10);
    const auto precondition = std::make_shared<ConjunctionAssertion>(pre1, pre2);

    // create a post-condition: x == 6 ∧ y == 11
    auto post1 = std::make_shared<VariableEqualityAssertion>("x", 6);
    auto post2 = std::make_shared<VariableEqualityAssertion>("y", 11);
    const auto post_condition = std::make_shared<ConjunctionAssertion>(post1, post2);

    // create a hoare triple: {x == 5 ∧ y == 10} x = 6; y = 11 {x == 6 ∧ y == 11}
    HoareTriple triple(precondition, sequence, post_condition);

    // log the triple
    LOG_INFO(std::format("Hoare Triple: {}", triple.toString()));

    // execute and verify
    const bool result = triple.executeAndVerify(state);

    // check the result
    assert(result);
    LOG_INFO(std::format("Triple verification result: {}", result ? "passed" : "failed"));
}

// function to test different command sequences
void testMultipleCommands() {
    LOG_INFO("Running multiple commands test");

    // test different command sequences with various state combinations
    for (int ndx = 0; ndx < 5; ndx++) {
        // create initial state with varying values
        ProgramState state;
        state.setVariable("x", ndx);

        // create command: x = x + 1 (simplified as x = ndx + 1)
        auto command = std::make_shared<AssignmentCommand>("x", ndx + 1);

        // create precondition: x == ndx
        auto precondition = std::make_shared<VariableEqualityAssertion>("x", ndx);

        // create post condition: x == ndx + 1
        auto post_condition = std::make_shared<VariableEqualityAssertion>("x", ndx + 1);

        // create and test hoare triple
        HoareTriple triple(precondition, command, post_condition);

        LOG_INFO(std::format("Test #{}: {}", ndx, triple.toString()));

        const bool result = triple.executeAndVerify(state);
        assert(result);

        LOG_INFO(std::format("Test #{} result: {}", ndx, result ? "passed" : "failed"));
    }
}

// main function
int main() {
    LOG_INFO("Starting Hoare Logic demonstration");

    // run all tests
    testIncrementExample();
    testSwapExample();
    testConjunctionExample();
    testMultipleCommands();

    LOG_INFO("All tests completed successfully");

    return 0;
}
