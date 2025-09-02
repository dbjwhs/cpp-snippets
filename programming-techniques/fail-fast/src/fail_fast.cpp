// MIT License
// Copyright (c) 2025 dbjwhs

#include <string>
#include <stdexcept>
#include <cassert>
#include <format>
#include "../../../../headers/project_utils.hpp"

// fail-fast pattern emerged in the 1970s alongside defensive programming practices
// and gained prominence in the development of reliable systems at institutions
// like bell labs and ibm. the pattern became widely recognized during the rise of
// object-oriented programming in the 1990s, particularly in mission-critical
// applications where early error detection was crucial.
//
// purpose:
// - immediately reports and handles invalid states or operations
// - prevents cascading failures by failing at the first sign of error
// - enhances debugging by providing clear point-of-failure identification
//
// implementation characteristics:
// - validates preconditions before executing operations
// - checks invariants consistently throughout object lifecycle
// - throws exceptions immediately upon detecting invalid states
// - provides detailed error information through logging and exceptions
//
// use cases:
// - financial systems requiring strict data validity
// - safety-critical systems where early error detection is essential
// - complex systems where debugging needs to be streamlined
// - applications requiring high reliability and data integrity
//
// note: this implementation demonstrates the pattern in a banking context
// with comprehensive state validation and immediate error reporting

// fail-fast pattern implementation that validates state immediately
// throws exceptions for invalid states to prevent cascading failures
class FailFastAccount {
private:
    // account balance in dollars
    double m_balance{0.0};
    
    // account holder's name, non-empty string required
    std::string m_accountHolder;
    
    // minimum allowed balance
    const double m_minimumBalance{-1000.0};
    
    // account status flag
    bool m_isActive{true};

    // validates if the account is in a valid state for operations
    void validateState() const {
        if (!m_isActive) {
            // make this info since we have negative tests, but should be error/critical
            LOG_INFO(std::format("Account validation failed: Account is inactive"));
            throw std::runtime_error("Account is inactive");
        }
        if (m_accountHolder.empty()) {
            LOG_ERROR(std::format("Account validation failed: Invalid account holder"));
            throw std::runtime_error("Invalid account holder");
        }
    }

public:
    // constructor enforcing initial valid state
    explicit FailFastAccount(const std::string& accountHolder) : m_accountHolder(accountHolder) {
        if (accountHolder.empty()) {
            // make this info since we have negative tests, but should be error/critical
            LOG_INFO(std::format("Failed to create account: Empty account holder name"));
            throw std::invalid_argument("Account holder name cannot be empty");
        }
        LOG_INFO(std::format("Account created for: {}", accountHolder));
    }

    // deposit money into account, fails fast on invalid amount
    void deposit(double amount) {
        validateState();
        if (amount <= 0) {
            // make this info since we have negative tests, but should be error/critical
            LOG_INFO(std::format("Invalid deposit amount: {}", amount));
            throw std::invalid_argument("Deposit amount must be positive");
        }
        m_balance += amount;
        LOG_INFO(std::format("Deposited ${:.2f}, new balance: ${:.2f}", amount, m_balance));
    }

    // withdraw money from an account, fails fast on invalid amount or insufficient funds
    void withdraw(double amount) {
        validateState();
        if (amount <= 0) {
            LOG_ERROR(std::format("Invalid withdrawal amount: {}", amount));
            throw std::invalid_argument("Withdrawal amount must be positive");
        }
        if ((m_balance - amount) < m_minimumBalance) {
            // make this info since we have negative tests, but should be error/critical
            LOG_INFO(std::format("Insufficient funds: balance=${:.2f}, withdrawal=${:.2f}, minimum=${:.2f}", 
                    m_balance, amount, m_minimumBalance));
            throw std::runtime_error("Insufficient funds");
        }
        m_balance -= amount;
        LOG_INFO(std::format("Withdrawn ${:.2f}, new balance: ${:.2f}", amount, m_balance));
    }

    // close account, fails fast if already closed
    void closeAccount() {
        if (!m_isActive) {
            // make this info since we have negative tests, but should be error/critical
            LOG_INFO(std::format("Cannot close already inactive account"));
            throw std::runtime_error("Account already inactive");
        }
        m_isActive = false;
        LOG_INFO(std::format("Account closed for: {}", m_accountHolder));
    }

    // getters
    [[nodiscard]] double getBalance() const { validateState(); return m_balance; }
    [[nodiscard]] std::string getAccountHolder() const { validateState(); return m_accountHolder; }
    [[nodiscard]] bool isActive() const { return m_isActive; }
};

int main() {
    try {
        LOG_INFO("Starting Fail-Fast Pattern tests");

        // test 1: valid account creation
        FailFastAccount account("John Doe");
        assert(account.getAccountHolder() == "John Doe");
        assert(account.getBalance() == 0.0);
        assert(account.isActive());

        // test 2: invalid account creation
        try {
            FailFastAccount invalidAccount("");
            assert(false && "Should have thrown exception");
        } catch (const std::invalid_argument& e) {
            LOG_INFO("Test passed: Empty account holder name rejected", e.what());
        }

        // test 3: valid deposit
        account.deposit(1000.0);
        assert(account.getBalance() == 1000.0);

        // test 4: invalid deposit
        try {
            account.deposit(-100.0);
            assert(false && "Should have thrown exception");
        } catch (const std::invalid_argument& e) {
            LOG_INFO("Test passed: Negative deposit rejected", e.what());
        }

        // test 5: valid withdrawal
        account.withdraw(500.0);
        assert(account.getBalance() == 500.0);

        // test 6: withdrawal exceeding minimum balance
        try {
            account.withdraw(2000.0);
            assert(false && "Should have thrown exception");
        } catch (const std::runtime_error& e) {
            LOG_INFO("Test passed: Excessive withdrawal rejected", e.what());
        }

        // test 7: account closure
        account.closeAccount();
        assert(!account.isActive());

        // test 8: operations on closed account
        try {
            account.deposit(100.0);
            assert(false && "Should have thrown exception");
        } catch (const std::runtime_error& e) {
            LOG_INFO("Test passed: Operation on closed account rejected", e.what());
        }

        // test 9: double closure
        try {
            account.closeAccount();
            assert(false && "Should have thrown exception");
        } catch (const std::runtime_error& e) {
            LOG_INFO("Test passed: Double closure rejected", e.what());
        }

        LOG_INFO("All tests completed successfully");
        
    } catch (const std::exception& e) {
        LOG_ERROR(std::format("Unexpected error: {}", e.what()));
        return 1;
    }

    return 0;
}
