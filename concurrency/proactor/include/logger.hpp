// MIT License
// Copyright (c) 2025 dbjwhs

#ifndef PROACTOR_LOGGER_HPP
#define PROACTOR_LOGGER_HPP

#include <string>
#include <fstream>
#include <mutex>

namespace proactor {

enum class LogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR
};

class Logger {
public:
    // Singleton access
    static Logger& getInstance();

    // Log a message with specified level
    void log(LogLevel level, const std::string& message);

    // Set log file
    void setLogFile(const std::string& filepath);
    
    // Destructor
    ~Logger();

private:
    // Private constructor for singleton
    Logger();
    
    // Disable copy operations
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    // Mutex for thread safety
    std::mutex m_mutex;
    
    // Log file stream
    std::ofstream m_logFile;
    
    // Log to console flag
    bool m_logToConsole;
};

} // namespace proactor

#endif // PROACTOR_LOGGER_HPP
