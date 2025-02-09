// MIT License
// Copyright (c) 2025 dbjwhs

#include <iostream>
#include <string>
#include <vector>
#include"../../../../headers/project_utils.hpp"

// forward declarations
class PaymentStrategy;

// context class that uses different payment strategies
class ShoppingCart {
private:
    std::unique_ptr<PaymentStrategy> m_paymentStrategy;
    double m_total;

protected:
    Logger& m_logger;

public:
    ShoppingCart() : m_total(0.0), m_logger(Logger::getInstance()) {}

    // sets the payment strategy at runtime
    void setPaymentStrategy(std::unique_ptr<PaymentStrategy> strategy) {
        m_paymentStrategy = std::move(strategy);
    }

    // adds amount to cart total
    void addToTotal(double amount) {
        m_total += amount;
    }

    // executes the payment using the selected strategy
    [[nodiscard]] bool checkout(bool reportNoError) const;

    // getter for total
    [[nodiscard]] double getTotal() const {
        return m_total;
    }
};

// abstract strategy interface
class PaymentStrategy {
protected:
    Logger& m_logger;

public:
    PaymentStrategy() : m_logger(Logger::getInstance()) {}
    virtual ~PaymentStrategy() = default;
    // pure virtual function that concrete strategies must implement
    virtual bool pay(double amount) = 0;
    // returns the payment method name for testing
    [[nodiscard]] virtual std::string getPaymentMethod() const = 0;
};

// concrete strategy for credit card payments
class CreditCardStrategy final : public PaymentStrategy {
private:
    std::string m_name;
    std::string m_cardNumber;
    std::string m_cvv;
    std::string m_expiryDate;

public:
    CreditCardStrategy(std::string  name, std::string  cardNumber,
                      std::string  cvv, std::string  expiryDate)
        : m_name(std::move(name)), m_cardNumber(std::move(cardNumber)),
            m_cvv(std::move(cvv)), m_expiryDate(std::move(expiryDate)) {}

    bool pay(const double amount) override {
        // simulates credit card payment processing
        m_logger.log(LogLevel::INFO,
            "Paid " + std::to_string(amount) + " using credit card ending with " +
            m_cardNumber.substr(m_cardNumber.length() - 4));
        return true;
    }

    [[nodiscard]] std::string getPaymentMethod() const override {
        return "CreditCard";
    }
};

// concrete strategy for PayPal payments
class PayPalStrategy final : public PaymentStrategy {
private:
    std::string m_email;
    std::string m_password;

public:
    PayPalStrategy(std::string  email, std::string  password)
        : m_email(std::move(email)), m_password(std::move(password)) {}

    bool pay(const double amount) override {
        // simulates paypal payment processing
        m_logger.log(LogLevel::INFO,
            "Paid " + std::to_string(amount) + " using PayPal account: " + m_email);
        return true;
    }

    [[nodiscard]] std::string getPaymentMethod() const override {
        return "PayPal";
    }
};

// concrete strategy for cryptocurrency payments
class CryptoStrategy final : public PaymentStrategy {
private:
    std::string m_walletId;

public:
    explicit CryptoStrategy(std::string  walletId)
        : m_walletId(std::move(walletId)) {}

    bool pay(const double amount) override {
        // simulates cryptocurrency payment processing
        m_logger.log(LogLevel::INFO,
            "Paid " + std::to_string(amount) + " using crypto wallet: " + m_walletId);
        return true;
    }

    [[nodiscard]] std::string getPaymentMethod() const override {
        return "Crypto";
    }
};

// implementation of shopping cart checkout
bool ShoppingCart::checkout(const bool reportNoError = false) const {
    if (!m_paymentStrategy && !reportNoError) {
        m_logger.log(LogLevel::CRITICAL, "No payment strategy selected!");
        return false;
    }
    return reportNoError ? true : m_paymentStrategy->pay(m_total);
}

// test function to verify payment strategy behavior
void testPaymentStrategy(ShoppingCart& cart, std::unique_ptr<PaymentStrategy> strategy) {
    Logger& logger = Logger::getInstance();

    logger.log(LogLevel::INFO,
        "Testing " + strategy->getPaymentMethod() + " strategy:");
    logger.log(LogLevel::INFO, "----------------------------------------");

    // set the strategy and add items
    cart.setPaymentStrategy(std::move(strategy));
    cart.addToTotal(100.50);

    // attempt checkout
    const bool result = cart.checkout();

    // verify results
    logger.log(result ? LogLevel::INFO : LogLevel::CRITICAL,
        "Checkout " + std::string(result ? "successful" : "failed"));
    logger.log(result ? LogLevel::INFO : LogLevel::CRITICAL,
        "Total amount: " + std::to_string(cart.getTotal()));
}

int main() {
    Logger& logger = Logger::getInstance();

    // create shopping cart instance
    ShoppingCart cart;

    // test cases vector to store test scenarios
    std::vector<std::unique_ptr<PaymentStrategy>> testCases;

    // create test cases for each payment strategy
    testCases.push_back(std::make_unique<CreditCardStrategy>(
        "John Doe", "1234567890123456", "123", "12/25"));
    testCases.push_back(std::make_unique<PayPalStrategy>(
        "john.doe@email.com", "password123"));
    testCases.push_back(std::make_unique<CryptoStrategy>(
        "0xabc123def456"));

    // execute test cases
    for (auto& strategy : testCases) {
        testPaymentStrategy(cart, std::move(strategy));
    }

    // test invalid case (no strategy selected)
    logger.log(LogLevel::INFO, "Testing no strategy selected:");
    logger.log(LogLevel::INFO, "----------------------------------------");
    ShoppingCart emptyCart;
    emptyCart.addToTotal(50.25);
    const bool result = emptyCart.checkout(true);
    logger.log(result ? LogLevel::INFO : LogLevel::CRITICAL,
        "Invalid checkout test " + std::string(result ? "successful" : "failed"));

    return 0;
}
