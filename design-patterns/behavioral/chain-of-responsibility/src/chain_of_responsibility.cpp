// MIT License
// Copyright (c) 2025 dbjwhs

#include <string>
#include "../../../../headers/project_utils.hpp"

// base expense handler class
class ExpenseHandler {
    Logger& m_logger = Logger::getInstance();

protected:
    std::shared_ptr<ExpenseHandler> m_next_handler;
    double m_approval_limit;
    std::string m_position_name;

public:
    ExpenseHandler(const double limit, std::string  position)
        : m_next_handler(nullptr), m_approval_limit(limit), m_position_name(std::move(position)) {}

    virtual ~ExpenseHandler() = default;

    // set the next handler in the chain and return it to allow chaining
    std::shared_ptr<ExpenseHandler> set_next(std::shared_ptr<ExpenseHandler> next) {
        m_next_handler = next;
        return next;
    }

    static std::string double_to_usd(const double amount) {
        // we want 153.56 not 153.56000000
        std::ostringstream amount_usd;
        amount_usd << std::fixed << std::setprecision(2) << amount;
        return amount_usd.str();
    }

    // pure virtual function that must be implemented by derived classes
    virtual bool process_request(const double amount, const std::string& purpose) {
        // validate input
        if (amount < 0) {
            m_logger.log(LogLevel::INFO, "Error: Invalid negative amount $" + double_to_usd(amount));
            return false;
        }

        if (purpose.empty()) {
            m_logger.log(LogLevel::INFO, "Error: Purpose cannot be empty");
            return false;
        }

        if (amount <= m_approval_limit) {
            approve_expense(amount, purpose);
            return true;
        } else if (m_next_handler) {
            // pass to next handler if amount exceeds current handler's limit
            m_logger.log(LogLevel::INFO, m_position_name + ": amount exceeds my approval limit. forwarding request...");
            return m_next_handler->process_request(amount, purpose);
        } else {
            // if no next handler and amount exceeds limit
            m_logger.log(LogLevel::INFO, "Error: expense of $" + double_to_usd(amount)
                + " cannot be approved. No handler with sufficient authority in chain.");
            return false;
        }
    }

protected:
    // virtual function for approving expenses
    virtual void approve_expense(const double amount, const std::string& purpose) {
        m_logger.log(LogLevel::INFO, m_position_name + " approved expense of $" + double_to_usd(amount)
            +" for " + purpose);

        // hook for additional approval actions
        post_approve_expense(purpose);
    }

    // Hook method for additional approval actions
    virtual void post_approve_expense(const std::string& purpose) {
        // Default implementation does nothing
    }
};

// team leader can approve small expenses
class TeamLeader final : public ExpenseHandler {
public:
    TeamLeader() : ExpenseHandler(1000.0, "team leader") {}
};

// department manager can approve medium expenses
class DepartmentManager final : public ExpenseHandler {
public:
    DepartmentManager() : ExpenseHandler(5000.0, "department manager") {}
};

// director can approve large expenses
class Director final : public ExpenseHandler {
public:
    Director() : ExpenseHandler(20000.0, "director") {}
};

// ceo can approve very large expenses
class CEO final : public ExpenseHandler {
    Logger& m_logger = Logger::getInstance();

public:
    CEO() : ExpenseHandler(100000.0, "ceo") {}

protected:
    void post_approve_expense(const std::string& purpose) override {
        m_logger.log(LogLevel::INFO, "expense will be reported in quarterly financial review");
    }
};

// crom is grim
class Crom final : public ExpenseHandler {
    Logger& m_logger = Logger::getInstance();

public:
    Crom() : ExpenseHandler(1.0, "CROM") {}
    bool process_request(const double amount, const std::string& purpose) override {
        m_logger.log(LogLevel::INFO, "I am " + m_position_name + "! By the Gods! I will not approve $" + double_to_usd(amount));
        return false;
    }

};
// expense request class to encapsulate request details
class ExpenseRequest {
    double m_amount;
    std::string m_purpose;

public:
    ExpenseRequest(const double amount, std::string  purpose)
        : m_amount(amount), m_purpose(std::move(purpose)) {}

    [[nodiscard]] double get_amount() const { return m_amount; }
    [[nodiscard]] const std::string& get_purpose() const { return m_purpose; }
};

int main() {
    Logger& logger = Logger::getInstance();

    // test case 1: create our hierarchy
    const auto team_leader = std::make_shared<TeamLeader>();
    const auto dept_manager = std::make_shared<DepartmentManager>();
    const auto director = std::make_shared<Director>();
    const auto ceo = std::make_shared<CEO>();
    const auto crom = std::make_shared<Crom>();

    // create our chain
    team_leader->set_next(dept_manager);
    dept_manager->set_next(director);
    director->set_next(ceo);
    ceo->set_next(crom);

    ExpenseRequest standard_requests[] = {
        {800.0, "office supplies"},
        {3000.0, "team building event"},
        {12000.0, "new software licenses"},
        {45000.0, "department renovation"},
        {200000.0, "new satellite office"}
    };

    constexpr std::string approved("APPROVED");
    constexpr std::string rejected("REJECTED");
    logger.log(LogLevel::INFO, "expense approval chain of responsibility - test cases");
    logger.log(LogLevel::INFO, "-------------------");
    logger.log(LogLevel::INFO, "test case 1: standard approval chain");
        logger.log(LogLevel::INFO, "-------------------");
    for (const auto& request : standard_requests) {
        logger.log(LogLevel::INFO, "expense request: $" + ExpenseHandler::double_to_usd(request.get_amount())
            +" for " + request.get_purpose());
        const bool success = team_leader->process_request(request.get_amount(), request.get_purpose());
        logger.log(LogLevel::INFO, "Request status: " + (success ? approved : rejected));
        logger.log(LogLevel::INFO, "-------------------");
    }

    // test case 2: broken chain (missing ceo)
    logger.log(LogLevel::INFO, "-------------------");
    logger.log(LogLevel::INFO, "test case 2: broken chain (missing ceo)");
    const auto leader2 = std::make_shared<TeamLeader>();
    const auto manager2 = std::make_shared<DepartmentManager>();
    const auto director2 = std::make_shared<Director>();

    leader2->set_next(manager2);
    manager2->set_next(director2);

    logger.log(LogLevel::INFO, "testing high-value request with incomplete chain:");
    const bool incomplete_chain_result = leader2->process_request(50000.0, "data center upgrade");
    logger.log(LogLevel::INFO, "Request status: " + (incomplete_chain_result ? approved : rejected));

    // test case 3: direct access to middle of chain
    logger.log(LogLevel::INFO, "-------------------");
    logger.log(LogLevel::INFO, "test case 3: direct access to middle of chain");
    logger.log(LogLevel::INFO, "bypassing team leader, starting from department manager:");
    const bool mid_chain_result = dept_manager->process_request(4000.0, "emergency repairs");
    logger.log(LogLevel::INFO, "Request status: " + (mid_chain_result ? approved : rejected));

    // test case 4: edge cases
    logger.log(LogLevel::INFO, "-------------------");
    logger.log(LogLevel::INFO, "test case 4: edge cases");

    logger.log(LogLevel::INFO, "testing zero amount request:");
    const bool zero_amount_result = team_leader->process_request(0.0, "subscription renewal");
    logger.log(LogLevel::INFO, "Request status: " + (zero_amount_result ? approved : rejected));

    logger.log(LogLevel::INFO, "testing amount at exact approval limit:");
    const bool exact_limit_result1 = team_leader->process_request(1000.0, "exactly at team leader limit");
    logger.log(LogLevel::INFO, "Team leader limit test status: " + (exact_limit_result1 ? approved : rejected));

    const bool exact_limit_result2 = dept_manager->process_request(5000.0, "exactly at department manager limit");
    logger.log(LogLevel::INFO, "Department manager limit test status: " + (exact_limit_result2 ? approved : rejected));

    logger.log(LogLevel::INFO, "testing negative amount (invalid input):");
    const bool negative_amount_result = team_leader->process_request(-500.0, "invalid negative expense");
    logger.log(LogLevel::INFO, "Request status: " + (negative_amount_result ? approved : rejected));

    // test case 5: single handler chain
    logger.log(LogLevel::INFO, "-------------------");
    logger.log(LogLevel::INFO, """test case 5: single handler chain");
    const auto solo_leader = std::make_shared<TeamLeader>();

    logger.log(LogLevel::INFO, "testing with single handler:");
    const bool solo_within_limit = solo_leader->process_request(500.0, "within solo handler limit");
    logger.log(LogLevel::INFO, "Within limit request status: " + (solo_within_limit ? approved : rejected));

    const bool solo_exceeds_limit = solo_leader->process_request(2000.0, "exceeds solo handler limit");
    logger.log(LogLevel::INFO, "Exceeds limit request status: " + (solo_exceeds_limit ? approved : rejected));

    return 0;
}
