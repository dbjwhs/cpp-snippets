// MIT License
// Copyright (c) 2025 dbjwhs

#ifndef DATABASE_CONNECTION_HPP
#define DATABASE_CONNECTION_HPP

#include <string>
#include <chrono>
#include <memory>
#include <atomic>
#include <utility>
#include "../../../../headers/project_utils.hpp"

namespace multiton_pattern {

// simulated database connection class to demonstrate multiton pattern
class DatabaseConnection {
private:
    std::string m_connection_string{};
    std::string m_database_name{};
    std::string m_instance_guid{};
    std::chrono::steady_clock::time_point m_creation_time{};
    std::atomic<bool> m_is_connected{false};
    std::atomic<std::size_t> m_query_count{0};

public:
    // default constructor
    DatabaseConnection() 
        : m_connection_string{"default://localhost:5432"}
        , m_database_name{"default_db"}
        , m_instance_guid{utils::generate_guid()}
        , m_creation_time{std::chrono::steady_clock::now()}
        , m_is_connected{true} {
        LOG_INFO_PRINT("DatabaseConnection created with default configuration [{}]", m_instance_guid);
    }

    // parameterized constructor
    explicit DatabaseConnection(std::string  connection_string, std::string  database_name = "default_db")
        : m_connection_string{std::move(connection_string)}
        , m_database_name{std::move(database_name)}
        , m_instance_guid{utils::generate_guid()}
        , m_creation_time{std::chrono::steady_clock::now()}
        , m_is_connected{true} {
        LOG_INFO_PRINT("DatabaseConnection created for database: {} with connection: {} [{}]"
            , m_database_name, m_connection_string, m_instance_guid);
    }

    // destructor
    ~DatabaseConnection() {
        if (m_is_connected.load()) {
            disconnect();
        }
        LOG_INFO_PRINT("DatabaseConnection destroyed for database: {} [{}]", m_database_name, m_instance_guid);
    }

    // delete copy operations to ensure proper resource management
    DatabaseConnection(const DatabaseConnection&) = delete;
    DatabaseConnection& operator=(const DatabaseConnection&) = delete;

    // allow move operations
    DatabaseConnection(DatabaseConnection&& other) noexcept
        : m_connection_string{std::move(other.m_connection_string)}
        , m_database_name{std::move(other.m_database_name)}
        , m_instance_guid{std::move(other.m_instance_guid)}
        , m_creation_time{other.m_creation_time}
        , m_is_connected{other.m_is_connected.load()}
        , m_query_count{other.m_query_count.load()} {
        other.m_is_connected = false;
        other.m_query_count = 0;
        LOG_INFO_PRINT("DatabaseConnection moved for database: {} [{}]", m_database_name, m_instance_guid);
    }

    DatabaseConnection& operator=(DatabaseConnection&& other) noexcept {
        if (this != &other) {
            if (m_is_connected.load()) {
                disconnect();
            }
            
            m_connection_string = std::move(other.m_connection_string);
            m_database_name = std::move(other.m_database_name);
            m_instance_guid = std::move(other.m_instance_guid);
            m_creation_time = other.m_creation_time;
            m_is_connected = other.m_is_connected.load();
            m_query_count = other.m_query_count.load();
            
            other.m_is_connected = false;
            other.m_query_count = 0;
            LOG_INFO_PRINT("DatabaseConnection move-assigned for database: {} [{}]", m_database_name, m_instance_guid);
        }
        return *this;
    }

    // simulate database query execution
    void execute_query(const std::string& query) {
        if (!m_is_connected.load()) {
            LOG_ERROR_PRINT("Cannot execute query on disconnected database: {}", m_database_name);
            return;
        }

        m_query_count.fetch_add(1);
        LOG_INFO_PRINT("Executing query #{} on database {} [{}]: {}", m_query_count.load(), m_database_name, m_instance_guid, query);
        
        // simulate query execution time
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    // get connection information
    const std::string& get_connection_string() const noexcept {
        return m_connection_string;
    }

    const std::string& get_database_name() const noexcept {
        return m_database_name;
    }

    bool is_connected() const noexcept {
        return m_is_connected.load();
    }

    std::size_t get_query_count() const noexcept {
        return m_query_count.load();
    }

    // get uptime in milliseconds
    std::chrono::milliseconds get_uptime() const {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - m_creation_time
        );
    }

    // disconnect from database
    void disconnect() noexcept {
        if (m_is_connected.exchange(false)) {
            LOG_INFO_PRINT("Disconnected from database: {} [{}]", m_database_name, m_instance_guid);
        }
    }

    // reconnect to database
    void reconnect() noexcept {
        if (!m_is_connected.exchange(true)) {
            LOG_INFO_PRINT("Reconnected to database: {} [{}]", m_database_name, m_instance_guid);
        }
    }

    // get status string for debugging
    std::string get_status() const {
        return std::format("Database: {}, Connection: {}, Connected: {}, Queries: {}, Uptime: {}ms [{}]",
                          m_database_name, m_connection_string, m_is_connected.load(), 
                          m_query_count.load(), get_uptime().count(), m_instance_guid);
    }

    // get the instance guid
    const std::string& get_instance_guid() const noexcept {
        return m_instance_guid;
    }
};

} // namespace multiton_pattern

#endif // DATABASE_CONNECTION_HPP