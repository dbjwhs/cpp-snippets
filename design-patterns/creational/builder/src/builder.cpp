// MIT License
// Copyright (c) 2025 dbjwhs

#include <string>
#include <cassert>
#include <stdexcept>
#include <format>
#include "../../../../headers/project_utils.hpp"

// the builder pattern: history, problems solved, and real-world applications
//
// history:
// - introduced by gang of four (gof) in their 1994 design patterns book
// - evolved from common problems in object-oriented programming where objects required complex initialization
// - became increasingly important as systems grew more complex and object configuration became more sophisticated
//
// as an example, the year is ~2008 and i open a file in the vmware source code, only to uncover a c++ constructor
// that was, i kid you not, 20+ pages long. at least that is what i recall, it was scary.
//
// core problems solved:
// 1. telescoping constructor problem:
//    - eliminates the need for multiple constructors with different parameter combinations
//    - example: computer(cpu) vs computer(cpu, ram) vs computer(cpu, ram, gpu, storage...)
//    - becomes unmanageable with many optional parameters
//
// 2. object consistency:
//    - ensures objects are always created in a valid state
//    - prevents partially initialized objects
//    - validates all required parameters before object creation
//    - maintains invariants throughout the construction process
//
// 3. immutability:
//    - supports creation of immutable objects (thread-safe)
//    - all validation happens before the object is available
//    - object cannot be modified after construction
//
// 4. separation of concerns:
//    - separates the complex construction process from the object's representation
//    - construction steps can be varied independently from the core business logic
//    - different directors can create different representations using the same builder
//
// real-world applications:
// 1. configuration builders:
//    - database connection configurations
//    - network request builders (e.g., apache httpclient)
//    - ui component builders
//    - test data builders for unit testing
//
// 2. document generation:
//    - pdf document builders
//    - html document construction
//    - complex report generators
//    - sql query builders
//
// advantages in modern c++:
// - works well with move semantics
// - supports perfect forwarding
// - can leverage smart pointers for memory management
// - enables compile-time validation using concepts (c++20)
// - integrates well with std::optional for truly optional parameters
//
// best practices:
// - use fluent interface (method chaining) for readable construction
// - validate parameters early
// - provide clear error messages for invalid configurations
// - use custom exceptions for validation failures
// - consider making the builder reusable for multiple objects
// - use const correctness for immutable results
// - leverage modern c++ features for type safety
//
// when to use:
// - object requires complex initialization
// - need to enforce specific construction sequences
// - want to prevent invalid object states
// - need different representations of the same construction process
// - construction process must be isolated from the object representation
//
// when not to use:
// - object construction is simple
// - no validation required
// - all parameters are mandatory
// - no complex configuration options
// - performance is critical and object construction is in a hot path

// custom exception for builder validation
class BuilderValidationError final : public std::runtime_error {
public:
    explicit BuilderValidationError(const std::string& message)
        : std::runtime_error(message) {}
};

// product class that will be built using the builder pattern
class Computer {
private:
    std::string m_cpu;
    std::string m_motherboard;
    int m_ram{0};
    bool m_hasGPU{false};
    std::string m_storage;

    // make both constructor and builder friend to allow proper construction
    friend class ComputerBuilder;
    friend std::unique_ptr<Computer> std::make_unique<Computer>();

public:
    // getters for testing and validation
    [[nodiscard]] std::string getCPU() const { return m_cpu; }
    [[nodiscard]] std::string getMotherboard() const { return m_motherboard; }
    [[nodiscard]] int getRAM() const { return m_ram; }
    [[nodiscard]] bool hasGPU() const { return m_hasGPU; }
    [[nodiscard]] std::string getStorage() const { return m_storage; }
};

// builder class with fluent interface
class ComputerBuilder {
private:
    // use unique_ptr to manage computer object lifetime
    std::unique_ptr<Computer> m_computer;

    // validation helper method
    void validateComputer() const {
        if (m_computer->m_cpu.empty()) {
            throw BuilderValidationError("CPU must be specified");
        }
        if (m_computer->m_motherboard.empty()) {
            throw BuilderValidationError("Motherboard must be specified");
        }
        if (m_computer->m_ram <= 0) {
            throw BuilderValidationError("RAM must be greater than 0");
        }
    }

public:
    ComputerBuilder() : m_computer(std::make_unique<Computer>()) {}

    // builder methods with method chaining
    ComputerBuilder& setCPU(const std::string& cpu) {
        m_computer->m_cpu = cpu;
        return *this;
    }

    ComputerBuilder& setMotherboard(const std::string& motherboard) {
        m_computer->m_motherboard = motherboard;
        return *this;
    }

    ComputerBuilder& setRAM(const int ram) {
        m_computer->m_ram = ram;
        return *this;
    }

    ComputerBuilder& setGPU(const bool hasGPU) {
        m_computer->m_hasGPU = hasGPU;
        return *this;
    }

    ComputerBuilder& setStorage(const std::string& storage) {
        m_computer->m_storage = storage;
        return *this;
    }

    // build method returns the constructed computer
    [[nodiscard]] std::unique_ptr<Computer> build() {
        validateComputer();
        return std::move(m_computer);
    }
};

// director class to demonstrate different computer configurations; note
// the stacked calls to class methods during the construction, this is called
// method chaining; behind the scenes
//
// what looks like methods being called during construction:
//    computerbuilder()
//        .setcpu("amd ryzen 9")
//        .setmotherboard("asus rog")
//        .build();
//
// is actually executing as separate steps:
// 1. computerbuilder temp;                   // temporary object created
// 2. temp.setcpu("amd ryzen 9");             // returns reference to temp
// 3. temp.setmotherboard("asus rog");        // called on same object
// 4. temp.build();                           // final build step
//
// this works because each setter returns computerbuilder&:
//    computerbuilder& setcpu(const std::string& cpu) {
//        m_computer->m_cpu = cpu;
//        return *this;    // returns reference to self
//    }
//
// advantages:
// - more readable than multiple separate lines
// - clearly shows the build sequence
// - reduces need for temporary variables
// - maintains object context through the chain
//
// limitations:
// - all chained methods must return reference to the builder
// - harder to debug intermediate states
// - can mask performance implications
// - error handling can be less clear
//
// this pattern is often called the "fluent interface" and has been
// available since early c++ and makes sequential calls look more elegant
//
class ComputerDirector {
public:
    [[nodiscard]] static std::unique_ptr<Computer> buildGamingPC() {
        return ComputerBuilder()
            .setCPU("AMD Ryzen 9")
            .setMotherboard("ASUS ROG")
            .setRAM(32)
            .setGPU(true)
            .setStorage("2TB NVMe SSD")
            .build();
    }

    [[nodiscard]] static std::unique_ptr<Computer> buildOfficePC() {
        return ComputerBuilder()
            .setCPU("Intel i5")
            .setMotherboard("ASRock B660")
            .setRAM(16)
            .setGPU(false)
            .setStorage("512GB SSD")
            .build();
    }
};

// comprehensive test function
void runTests() {
    // test case 1: build gaming pc and verify all components
    Logger::getInstance().log(LogLevel::INFO, std::format("Running test case 1: Gaming PC build"));
    const auto gamingPC = ComputerDirector::buildGamingPC();
    assert(gamingPC->getCPU() == "AMD Ryzen 9" && "gaming pc cpu mismatch");
    assert(gamingPC->getRAM() == 32 && "gaming pc ram mismatch");
    assert(gamingPC->hasGPU() && "gaming pc should have gpu");
    Logger::getInstance().log(LogLevel::INFO, std::format("Test case 1 passed"));

    // test case 2: build office pc and verify all components
    Logger::getInstance().log(LogLevel::INFO, std::format("Running test case 2: Office PC build"));
    const auto officePC = ComputerDirector::buildOfficePC();
    assert(officePC->getCPU() == "Intel i5" && "office pc cpu mismatch");
    assert(officePC->getRAM() == 16 && "office pc ram mismatch");
    assert(!officePC->hasGPU() && "office pc should not have gpu");
    Logger::getInstance().log(LogLevel::INFO, std::format("Test case 2 passed"));

    // test case 3: custom build with manual validation
    Logger::getInstance().log(LogLevel::INFO, std::format("Running test case 3: Custom build validation"));
    const auto customPC = ComputerBuilder()
        .setCPU("Intel i7")
        .setMotherboard("MSI PRO")
        .setRAM(64)
        .setGPU(true)
        .setStorage("4TB NVMe RAID")
        .build();

    assert(customPC->getStorage() == "4TB NVMe RAID" && "custom pc storage mismatch");
    assert(customPC->getRAM() == 64 && "custom pc ram mismatch");
    Logger::getInstance().log(LogLevel::INFO, std::format("Test case 3 passed"));

    // test case 4: verify builder validation
    Logger::getInstance().log(LogLevel::INFO, std::format("Running test case 4: Builder validation"));
    bool validationCaught = false;
    try {
        auto invalidPC = ComputerBuilder()
            .setRAM(32)  // missing required cpu and motherboard
            .build();
    } catch (const BuilderValidationError& e) {
        validationCaught = true;
        Logger::getInstance().log(LogLevel::INFO, std::format("Caught expected validation error: {}", e.what()));
    }
    assert(validationCaught && "validation error should have been caught");
    Logger::getInstance().log(LogLevel::INFO, std::format("Test case 4 passed"));
}

int main() {
    try {
        Logger::getInstance().log(LogLevel::INFO, std::format("Starting Builder Pattern tests"));
        runTests();
        Logger::getInstance().log(LogLevel::INFO, std::format("All tests completed successfully"));
        return 0;
    } catch (const std::exception& e) {
        Logger::getInstance().log(LogLevel::ERROR, std::format("Test failure: {}", e.what()));
        return 1;
    }
}
