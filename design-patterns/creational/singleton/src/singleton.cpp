// MIT License
// Copyright (c) 2025 dbjwhs

#include <iostream>
#include <string>
#include <cassert>
#include "../../../../headers/project_utils.hpp"

// generic singleton template class
template<typename TypeName>
class Singleton {
private:
    // private instance pointer
    static TypeName* m_instance;

protected:
    // protected constructor to allow inheritance while preventing direct instantiation
    Singleton() = default;

    // delete copy constructor and assignment operator
    DECLARE_NON_COPYABLE(Singleton);

public:
    // public method to get instance
    static TypeName* getInstance() {
        if (m_instance == nullptr) {
            m_instance = new TypeName();
        }
        return m_instance;
    }

    // public method to destroy instance
    static void destroyInstance() {
        delete m_instance;
        m_instance = nullptr;
    }
};

// initialize static member
template<typename TypeName>
TypeName* Singleton<TypeName>::m_instance = nullptr;

// example class to test a singleton pattern
class Configuration : public Singleton<Configuration> {
    friend class Singleton<Configuration>;
private:
    std::string m_appName;
    int m_maxConnections;

    // private constructor
    Configuration() : m_appName("default"), m_maxConnections(10) {}

public:
    // getter and setter methods
    void setAppName(const std::string& name) { m_appName = name; }
    [[nodiscard]] std::string getAppName() const { return m_appName; }

    void setMaxConnections(int max) { m_maxConnections = max; }
    [[nodiscard]] int getMaxConnections() const { return m_maxConnections; }
};

// another example class to test a singleton pattern
class LogObject : public Singleton<LogObject> {
    friend class Singleton<LogObject>;
private:
    bool m_debugMode;

    // private constructor
    LogObject() : m_debugMode(false) {}

public:
    void setDebugMode(const bool mode) { m_debugMode = mode; }
    [[nodiscard]] bool getDebugMode() const { return m_debugMode; }

    void log(const std::string& message) const {
        if (m_debugMode) {
            std::cout << "debug: " << message << std::endl;
        } else {
            std::cout << "info: " << message << std::endl;
        }
    }
};

int main() {
    // test configuration singleton
    Configuration* config1 = Configuration::getInstance();
    const Configuration* config2 = Configuration::getInstance();

    // note to old C programmers regarding C++ assert messages:
    // the ability to add a custom message using && with a string literal is a C++ feature
    // that takes advantage of:
    // 1. string literals being convertible to bool (evaluating to true)
    // 2. the short-circuit behavior of &&
    // 3. the C++ assertion system's ability to capture both parts as the message
    // this is one of those subtle but useful c++ features that isn't well known
    // verify both pointers point to the same instance
    assert(config1 == config2 && "singleton instances should be identical");
    std::cout << "singleton instance was identical" << std::endl;

    // test configuration functionality
    config1->setAppName("test application");
    config1->setMaxConnections(20);

    // verify changes are reflected in both references
    assert(config2->getAppName() == "test application" && "app name should match");
    std::cout << "app name matched" << std::endl;

    assert(config2->getMaxConnections() == 20 && "max connections should match");
    std::cout << "max connections matched" << std::endl;

    // test logger singleton
    LogObject* logger1 = LogObject::getInstance();
    LogObject* logger2 = LogObject::getInstance();

    // verify logger instances are identical
    assert(logger1 == logger2 && "logger instances should be identical");
    std::cout << "logger was identical" << std::endl;

    // test logger functionality
    logger1->setDebugMode(true);
    assert(logger2->getDebugMode() == true && "debug mode should match");
    std::cout << "debug mode matched" << std::endl;

    // test logging
    logger1->log("testing singleton logger");
    logger2->log("testing with second reference");

    // cleanup
    Configuration::destroyInstance();
    LogObject::destroyInstance();

    std::cout << "all tests passed successfully!" << std::endl;
    return 0;
}
