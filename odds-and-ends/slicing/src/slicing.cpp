// MIT License
// Copyright (c) 2025 dbjwhs

#include "../../../headers/project_utils.hpp"

class Base {
protected:
    std::string baseData;
public:
    Base() : baseData("Base data") {}
    virtual void print() {
        Logger::getInstance().log(LogLevel::INFO, "Base with: " + baseData);
    }
    virtual ~Base() = default;
};

class Derived final : public Base {
private:
    std::string derivedData;
public:
    Derived() : derivedData("Derived data") {
        baseData = "Modified base data";
    }
    void print() override {
        Logger::getInstance().log(LogLevel::INFO, "Derived with: " + baseData + " and " + derivedData);
    }
};

// case 1: by pointer - no slicing
void processByPointer(Base* obj) {
    Logger::getInstance().log(LogLevel::INFO, "Processing by pointer: ");
    obj->print();
}

// case 2: by reference - no slicing
void processByReference(Base& obj) {
    Logger::getInstance().log(LogLevel::INFO, "Processing by reference: ");
    obj.print();
}

// case 3: by value - slicing occurs!
void processByValue(Base obj) {
    Logger::getInstance().log(LogLevel::INFO, "Processing by value: ");
    obj.print();  // will only print base data, derived data is lost!
}

int main() {
    Logger::getInstance().log(LogLevel::INFO, "Creating Derived object...");
    Derived d;

    Logger::getInstance().log(LogLevel::INFO, "Original object:");
    d.print();
    Logger::getInstance().log(LogLevel::INFO, "");

    // case 1: pointer - works correctly
    processByPointer(&d);

    // case 2: reference - works correctly
    processByReference(d);

    // case 3: value - demonstrates slicing
    processByValue(d);

    return 0;
}
