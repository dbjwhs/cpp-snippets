// MIT License
// Copyright (c) 2025 dbjwhs

#include <iostream>
#include <cassert>
#include <string>
#include <vector>
#include "../../../headers/project_utils.hpp"

// designated initializers in c++20
//
// history:
// designated initializers were first introduced in c99 for c language and allowed
// initialization of specific elements within an aggregate by naming the element to be initialized.
// this feature was later included in c++20, providing a cleaner way to initialize structs and
// classes, especially those with many members. the concept originated from the need to make
// initialization more explicit and self-documenting by allowing developers to specify which
// member is being initialized.
//
// designated initializers allow initializing specific members of an aggregate type by name,
// improving code readability and maintainability. they make the initialization intent explicit
// by naming the members being initialized rather than relying on positional order.
//
// common usages include:
// - initializing configuration structures with many optional fields
// - creating more self-documenting initialization code
// - initializing only a subset of struct/class members while leaving others at their default values
// - making code more maintainable when struct definitions change order

// a configuration structure that demonstrates designated initializers
struct ServerConfig {
    // server network configuration
    std::string m_hostname = "localhost";
    int m_port = 8080;
    bool m_useHttps = false;
    
    // performance settings
    int m_maxConnections = 100;
    int m_connectionTimeout = 30;
    
    // logging configuration
    bool m_enableLogging = true;
    std::string m_logLevel = "info";
    std::string m_logPath = "/var/log";
    
    // authentication settings
    bool m_requireAuth = false;
    std::string m_authMethod = "none";
};

// a more complex structure with nested elements
struct ApplicationSettings {
    // database connection settings
    struct {
        std::string m_host = "localhost";
        int m_port = 5432;
        std::string m_username = "admin";
        std::string m_password = "password";
        std::string m_dbName = "appdb";
    } m_database;
    
    // server configuration
    ServerConfig m_serverConfig;
    
    // application metadata
    std::string m_appName = "MyApp";
    std::string m_appVersion = "1.0.0";
    
    // feature flags
    bool m_enableFeatureA = false;
    bool m_enableFeatureB = false;
    bool m_enableFeatureC = false;
};

// a function to print server config for testing and display
void printServerConfig(const ServerConfig& config) {
    LOG_INFO("Server Configuration:");
    LOG_INFO(std::format("  Hostname: {}", config.m_hostname));
    LOG_INFO(std::format("  Port: {}", config.m_port));
    LOG_INFO(std::format("  HTTPS Enabled: {}", config.m_useHttps ? "Yes" : "No"));
    LOG_INFO(std::format("  Max Connections: {}", config.m_maxConnections));
    LOG_INFO(std::format("  Connection Timeout: {} seconds", config.m_connectionTimeout));
    LOG_INFO(std::format("  Logging Enabled: {}", config.m_enableLogging ? "Yes" : "No"));
    LOG_INFO(std::format("  Log Level: {}", config.m_logLevel));
    LOG_INFO(std::format("  Log Path: {}", config.m_logPath));
    LOG_INFO(std::format("  Auth Required: {}", config.m_requireAuth ? "Yes" : "No"));
    LOG_INFO(std::format("  Auth Method: {}", config.m_authMethod));
}

// test function to validate designated initializers behavior
void runTests() {
    LOG_INFO("Running designated initializers tests...");
    
    // test case 1: basic designated initializers with a few fields
    LOG_INFO("Test Case 1: Basic designated initializers");
    ServerConfig config1{
        .m_hostname = "api.example.com",
        .m_port = 443,
        .m_useHttps = true
    };
    
    // validate that specified fields were set and unspecified fields kept defaults
    assert(config1.m_hostname == "api.example.com");
    assert(config1.m_port == 443);
    assert(config1.m_useHttps == true);
    assert(config1.m_maxConnections == 100);  // default value maintained
    assert(config1.m_logLevel == "info");     // default value maintained
    LOG_INFO("Test Case 1: Passed");
    
    // test case 2: mixing designated and non-designated initializers is not allowed in c++
    // uncommenting the following would cause a compiler error:
    /*
    ServerConfig config2{
        .m_hostname = "api.example.com",
        .m_port = 443,
        8000,  // error: cannot mix designated and non-designated initializers
        .m_useHttps = true
    };
    */
    LOG_INFO("Test Case 2: Note that mixing designated and non-designated initializers is not allowed");
    
    // test case 3: designated initializers must appear in declaration order
    // uncommenting the following would cause a compiler error:
    /*
    ServerConfig config3{
        .m_port = 443,            // error: designator order doesn't match
        .m_hostname = "example.com"  // declaration order in the struct
    };
    */
    LOG_INFO("Test Case 3: Note that designated initializers must follow declaration order");
    
    // test case 4: initializing nested structures with designated initializers
    LOG_INFO("Test Case 4: Nested structures with designated initializers");
    ApplicationSettings appSettings{
        .m_database = {
            .m_host = "db.example.com",
            .m_port = 5432,
            .m_username = "appuser",
            .m_password = "securepass",
            .m_dbName = "production"
        },
        .m_serverConfig = {
            .m_hostname = "web.example.com",
            .m_port = 8443,
            .m_useHttps = true,
            .m_maxConnections = 500
        },
        .m_appName = "ProductionApp",
        .m_enableFeatureA = true
    };
    
    // validate nested structure initialization
    assert(appSettings.m_database.m_host == "db.example.com");
    assert(appSettings.m_database.m_username == "appuser");
    assert(appSettings.m_serverConfig.m_hostname == "web.example.com");
    assert(appSettings.m_serverConfig.m_port == 8443);
    assert(appSettings.m_appName == "ProductionApp");
    assert(appSettings.m_enableFeatureA == true);
    assert(appSettings.m_enableFeatureB == false);  // default maintained
    assert(appSettings.m_appVersion == "1.0.0");    // default maintained
    LOG_INFO("Test Case 4: Passed");
    
    // test case 5: partial initialization of a structure
    LOG_INFO("Test Case 5: Partial initialization");
    ServerConfig configMinimal{
        .m_logLevel = "debug"
    };
    
    // validate that only specified field changed while others kept defaults
    assert(configMinimal.m_hostname == "localhost");  // default maintained
    assert(configMinimal.m_port == 8080);            // default maintained
    assert(configMinimal.m_logLevel == "debug");     // explicitly set
    LOG_INFO("Test Case 5: Passed");
    
    // test case 6: vector of structs with designated initializers
    LOG_INFO("Test Case 6: Vector of structs with designated initializers");
    std::vector<ServerConfig> serverConfigs = {
        {
            .m_hostname = "server1.example.com",
            .m_port = 8081
        },
        {
            .m_hostname = "server2.example.com",
            .m_port = 8082,
            .m_maxConnections = 200
        },
        {
            .m_hostname = "server3.example.com",
            .m_useHttps = true,
            .m_requireAuth = true,
            .m_authMethod = "oauth2"
        }
    };
    
    // validate vector initialization
    assert(serverConfigs.size() == 3);
    assert(serverConfigs[0].m_hostname == "server1.example.com");
    assert(serverConfigs[0].m_port == 8081);
    assert(serverConfigs[1].m_maxConnections == 200);
    assert(serverConfigs[2].m_useHttps == true);
    assert(serverConfigs[2].m_authMethod == "oauth2");
    LOG_INFO("Test Case 6: Passed");
}

int main() {
    LOG_INFO("C++ Designated Initializers Example");
    
    // basic example of designated initializers
    ServerConfig myServerConfig{
        .m_hostname = "api.mycompany.com",
        .m_port = 443,
        .m_useHttps = true,
        .m_maxConnections = 1000,
        .m_requireAuth = true,
        .m_authMethod = "jwt"
    };
    
    // display configuration values
    printServerConfig(myServerConfig);
    
    // run comprehensive tests
    runTests();
    
    LOG_INFO("All examples and tests completed successfully");
    return 0;
}