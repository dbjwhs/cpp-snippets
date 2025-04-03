// MIT License
// Copyright (c) 2025 dbjwhs

#include <cassert>
#include <string>
#include <format>
#include "../../../headers/project_utils.hpp"

// the diamond problem occurs in c++ when a derived class inherits from two classes that share a common base class
// without virtual inheritance, this creates two separate copies of the base class, leading to ambiguity
// example hierarchy that causes the diamond problem:
//
//     a      // base class
//    / \
//   b   c    // both inherit from a
//    \ /
//     d      // inherits from both b and c, creating two copies of a
//
// this causes issues like:
// - ambiguous member access (which a::member should d use?)
// - duplicate data members from base class a
// - ambiguous virtual function calls
// - complex construction and destruction order
//
// virtual inheritance solves this by ensuring only one copy of the base class exists
// class b : virtual public a {}; // solution using virtual inheritance

// base class remains the same
class PowerSource {
public:
    int m_power_level;
    std::string m_source_id;

public:
    PowerSource(const int power, std::string id)
        : m_power_level(power), m_source_id(std::move(id)) {
        Logger::getInstance().log(LogLevel::INFO,
            std::format("PowerSource constructed with id: {}", m_source_id));
    }

    [[nodiscard]] virtual int getPower() const {
        return m_power_level;
    }

    virtual ~PowerSource() = default;
};

// remove virtual inheritance here
class Electrical : /* add 'virtual' here to fix diamond problem */ public PowerSource {
protected:
    float m_voltage;

public:
    Electrical(int power, float voltage)
        : PowerSource(power, "ELEC"), m_voltage(voltage) {
        Logger::getInstance().log(LogLevel::INFO,
            std::format("Electrical constructed with voltage: {}", m_voltage));
    }

    [[nodiscard]] int getPower() const override {
        return static_cast<int>(m_power_level * m_voltage);
    }
};

// remove virtual inheritance here
class Mechanical : /* add 'virtual' here to fix diamond problem */ public PowerSource {
protected:
    float m_efficiency;

public:
    Mechanical(const int power, const float efficiency)
        : PowerSource(power, "MECH"), m_efficiency(efficiency) {
        Logger::getInstance().log(LogLevel::INFO,
            std::format("Mechanical constructed with efficiency: {}", m_efficiency));
    }

    [[nodiscard]] int getPower() const override {
        return static_cast<int>(m_power_level * m_efficiency);
    }
};

// hybrid class now has ambiguous base class
class HybridEngine final : public Electrical, public Mechanical {
private:
    bool m_is_active;

public:
    HybridEngine(int base_power, float voltage, float efficiency)
        // cannot initialize powersource directly anymore due to ambiguity
        : Electrical(base_power, voltage),
          Mechanical(base_power, efficiency),
          m_is_active(true) {
        Logger::getInstance().log(LogLevel::INFO,
            std::format("HybridEngine constructed with base power: {}", base_power));
    }

    // now we have ambiguity! which powersource::getpower() do we use?
    [[nodiscard]] int getPower() const override {
        // this will cause a compilation error due to ambiguous access
        // return m_power_level;  // which m_power_level? electrical or mechanical?

        // need to explicitly specify which path to use
        const int electrical_power = Electrical::getPower();
        const int mechanical_power = Mechanical::getPower();
        return m_is_active ? (electrical_power + mechanical_power) / 2 : 0;
    }
};

int main() {
    LOG_INFO("Starting diamond problem demonstration");

    HybridEngine engine(100, 1.5f, 0.8f);

    // these lines now cause ambiguity errors, a while back compilers generally did not pick
    // these up, but g++ is so that's good, uncomment out, for example of issues, and/or see
    // the above comments in mechanical and electrical classes for fix

    // powersource* base_ptr = &engine;  // error: ambiguous conversion
    // engine.m_power_level;             // error: ambiguous access
    // engine.powersource::getpower();   // error: ambiguous access

    // must explicitly specify the path:
    Logger::getInstance().log(LogLevel::INFO,
        std::format("Electrical power path: {}", engine.Electrical::getPower()));
    Logger::getInstance().log(LogLevel::INFO,
        std::format("Mechanical power path: {}", engine.Mechanical::getPower()));

    // demonstrate the two separate powersource instances
    Logger::getInstance().log(LogLevel::INFO,
        std::format("Electrical source ID: {}",
            static_cast<Electrical&>(engine).m_source_id));
    Logger::getInstance().log(LogLevel::INFO,
        std::format("Mechanical source ID: {}",
            static_cast<Mechanical&>(engine).m_source_id));

    // this will now create two separate powersource objects
    assert(&static_cast<Electrical*>(&engine)->PowerSource::m_power_level !=
           &static_cast<Mechanical*>(&engine)->PowerSource::m_power_level);
    LOG_INFO("Verified separate PowerSource instances");

    return 0;
}
