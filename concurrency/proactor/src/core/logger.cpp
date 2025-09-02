#include "../../include/logger.hpp"
#include <iostream>
#include <chrono>
#include <format>
#include <thread>
#include <iomanip>

namespace proactor {

// Initialize the singleton instance
Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

// Constructor
Logger::Logger() : m_logToConsole(true) {
    setLogFile("../custom.log");
}

// Destructor
Logger::~Logger() {
    if (m_logFile.is_open()) {
        m_logFile.close();
    }
}

// Set the log file
void Logger::setLogFile(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_logFile.is_open()) {
        m_logFile.close();
    }
    m_logFile.open(filepath, std::ios::app);
}

// Log a message with specified level
void Logger::log(LogLevel level, const std::string& message) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    // Get current time
    auto now = std::chrono::system_clock::now();
    auto now_time_t = std::chrono::system_clock::to_time_t(now);
    auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    
    std::tm now_tm = *std::gmtime(&now_time_t);
    
    // Format time
    std::string timeStr = std::format("{:04d}-{:02d}-{:02d} {:02d}:{:02d}:{:02d}.{:03d} UTC",
                                     now_tm.tm_year + 1900,
                                     now_tm.tm_mon + 1,
                                     now_tm.tm_mday,
                                     now_tm.tm_hour,
                                     now_tm.tm_min,
                                     now_tm.tm_sec,
                                     now_ms.count());
    
    // Get log level string
    std::string levelStr;
    switch (level) {
        case LogLevel::DEBUG:
            levelStr = "DEBUG";
            break;
        case LogLevel::INFO:
            levelStr = "INFO";
            break;
        case LogLevel::WARNING:
            levelStr = "WARNING";
            break;
        case LogLevel::ERROR:
            levelStr = "ERROR";
            break;
    }
    
    // Get thread ID
    std::stringstream ss;
    ss << std::hex << std::this_thread::get_id();
    std::string threadId = ss.str();
    
    // Format log message
    std::string logMessage = std::format("{} [{}] [Thread:0x{}] {}", 
                                        timeStr, 
                                        levelStr, 
                                        threadId,
                                        message);
    
    // Log to file
    if (m_logFile.is_open()) {
        m_logFile << logMessage << std::endl;
        m_logFile.flush();
    }
    
    // Log to console
    if (m_logToConsole) {
        std::cout << logMessage << std::endl;
    }
}

} // namespace proactor
