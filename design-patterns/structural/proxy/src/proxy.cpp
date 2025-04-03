// MIT License
// Copyright (c) 2025 dbjwhs

#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include <fstream>
#include <algorithm>
#include <format>
#include "../../../../headers/project_utils.hpp"

// proxy_pattern.cpp
// implementation of the proxy design pattern
//
// the proxy pattern, introduced by the gang of four in 1994, provides a surrogate or
// placeholder for another object to control access to it. it belongs to the structural
// pattern family and is widely used in modern software development.
//
// historical context:
// - originated in early distributed systems for managing remote resources
// - gained prominence with the rise of object-oriented programming
// - evolved to handle modern concerns like lazy loading and access control
//
// real-world applications:
// 1. virtual proxy (lazy loading):
//    - loading large images in web browsers
//    - database connection pooling
//    - loading heavy documents in word processors
//
// 2. protection proxy (access control):
//    - corporate network access management
//    - cloud service authentication layers
//    - database query permissions
//
// 3. remote proxy:
//    - microservices communication
//    - distributed system interfaces
//    - rest api gateways
//
// 4. logging proxy:
//    - system activity monitoring
//    - audit trails in financial systems
//    - debug logging in production systems
//
// this implementation demonstrates a protection proxy with logging capabilities,
// commonly used in enterprise systems where access control and audit trails are
// crucial. the pattern is particularly relevant in:
// - financial transaction systems
// - healthcare record management
// - enterprise resource planning (erp) systems
// - cloud-based service interfaces
//
// ----------------
//
// class hierarchy:
//
// idataprocessor (interface)
// |-> defines interface for data processing operations
// |-> pure virtual methods: processdata(), getprocessedcount()
// |
// |---> realdataprocessor
// |     |-> concrete implementation that does actual data processing
// |     |-> maintains count of processed items
// |     |-> processes vector<string> data with detailed logging
// |     |-> member variables:
// |         |-> m_processedcount: tracks total items processed
// |
// |---> dataprocessorproxy
//       |-> controls access to realdataprocessor
//       |-> implements authentication checking
//       |-> provides additional logging and access tracking
//       |-> member variables:
//           |-> m_realprocessor: unique_ptr to realdataprocessor
//           |-> m_isauthenticated: tracks authentication state
//           |-> m_accesscount: tracks number of access attempts
//
// key relationships:
// - proxy forwards authenticated requests to real processor
// - proxy maintains sole ownership of real processor via unique_ptr
// - both derived classes implement idataprocessor interface
// - proxy adds authentication and logging without modifying real processor

// interface for both a real subject and proxy
class IDataProcessor {
public:
    virtual ~IDataProcessor() = default;
    virtual void processData(const std::vector<std::string>& data) = 0;
    [[nodiscard]] virtual std::size_t getProcessedCount() const = 0;
};

// real subject class that does the actual work
class RealDataProcessor final : public IDataProcessor {
private:
    std::size_t m_processedCount{0};  // tracks number of items processed

public:
    void processData(const std::vector<std::string>& data) override {
        m_processedCount += data.size();

        // log processing activity
        LOG_INFO(std::format("processing {} items in real processor", data.size()));

        // simulate processing
        for (const auto& item : data) {
            LOG_INFO(std::format("processing item: {}", item));
        }
    }

    [[nodiscard]] std::size_t getProcessedCount() const override {
        return m_processedCount;
    }
};

// proxy class that adds access control and logging
class DataProcessorProxy final : public IDataProcessor {
private:
    std::unique_ptr<RealDataProcessor> m_realProcessor;
    bool m_isAuthenticated{false};
    std::size_t m_accessCount{0};  // tracks number of access attempts

public:
    DataProcessorProxy() : m_realProcessor(std::make_unique<RealDataProcessor>()) {}

    void authenticate() {
        m_isAuthenticated = true;
        LOG_INFO("proxy: authentication successful");
    }

    void processData(const std::vector<std::string>& data) override {
        m_accessCount++;

        // check authentication before allowing access
        if (!m_isAuthenticated) {
            LOG_INFO("proxy: access denied - not authenticated");
            return;
        }

        // log proxy access
        LOG_INFO(std::format("proxy: forwarding {} items to real processor", data.size()));

        // forward request to real processor
        m_realProcessor->processData(data);
    }

    [[nodiscard]] std::size_t getProcessedCount() const override {
        return m_realProcessor->getProcessedCount();
    }

    [[nodiscard]] std::size_t getAccessCount() const {
        return m_accessCount;
    }
};

// test suite for vector<string> data
void runVectorTests() {
    LOG_INFO("starting vector<string> tests");

    DataProcessorProxy proxy;
    std::vector<std::string> testData{"item1", "item2", "item3"};

    // test 1: unauthenticated access
    LOG_INFO("test 1: attempting unauthenticated access");
    proxy.processData(testData);
    assert(proxy.getProcessedCount() == 0);
    assert(proxy.getAccessCount() == 1);

    // test 2: authenticated access
    LOG_INFO("test 2: attempting authenticated access");
    proxy.authenticate();
    proxy.processData(testData);
    assert(proxy.getProcessedCount() == 3);
    assert(proxy.getAccessCount() == 2);

    // test 3: multiple processing calls
    LOG_INFO("test 3: testing multiple processing calls");
    proxy.processData(testData);
    assert(proxy.getProcessedCount() == 6);
    assert(proxy.getAccessCount() == 3);

    LOG_INFO("vector<string> tests completed successfully");
}

// utility function to read a file into vector of strings (one per line)
std::vector<std::string> readFileToVector(const std::string& filename) {
    std::vector<std::string> lines;
    std::ifstream file(filename);

    if (!file.is_open()) {
        throw std::runtime_error(std::format("failed to open file: {}", filename));
    }

    std::string line;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }

    LOG_INFO(std::format("read {} lines from file: {}", lines.size(), filename));

    return lines;
}

// test suite for file data
void runFileTests(const std::string& file1, const std::string& file2) {
    LOG_INFO("starting file tests");

    DataProcessorProxy proxy;

    // test 1: read and process original file
    LOG_INFO("test 1: processing original file");
    const auto originalData = readFileToVector(file1);

    // authenticate and process
    proxy.authenticate();
    proxy.processData(originalData);

    // verify processing
    assert(proxy.getProcessedCount() == originalData.size());
    assert(proxy.getAccessCount() == 1);

    // test 2: compare processing of both files
    LOG_INFO("test 2: comparing file processing");
    const auto alteredData = readFileToVector(file2);

    // process altered file
    proxy.processData(alteredData);

    // verify cumulative processing
    assert(proxy.getProcessedCount() == originalData.size() + alteredData.size());
    assert(proxy.getAccessCount() == 2);

    // test 3: process files multiple times to ensure consistent behavior
    LOG_INFO("test 3: testing multiple file processing");
    proxy.processData(originalData);
    proxy.processData(alteredData);

    // verify final counts
    assert(proxy.getProcessedCount() == 2 * (originalData.size() + alteredData.size()));
    assert(proxy.getAccessCount() == 4);

    LOG_INFO("file tests completed successfully");
}

int main() {
    try {
        // run vector tests
        runVectorTests();

        // run file tests
        runFileTests("../../../../testing-files/frost_giants_daughter.txt",
                    "../../../../testing-files/frost_giants_daughter-altered.txt");

        LOG_INFO("all tests completed successfully");
        return 0;
    } catch (const std::exception& e) {
        LOG_INFO(std::format("error during testing: {}", e.what()));
        return 1;
    }
}
