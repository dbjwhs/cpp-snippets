// MIT License
// Copyright (c) 2025 dbjwhs

#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>
#include <cassert>
#include <chrono>
#include <format>
#include <atomic>
#include <future>
#include <map>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <cerrno>
#include <algorithm>
#include <fstream>
#include "../../../headers/project_utils.hpp"

// Function to run tests, implemented in src/testing/tests.cpp
void runTests();

int main() {
    try {
        Logger::getInstance().log(LogLevel::INFO, "proactor design pattern example");

        // run tests
        runTests();

        Logger::getInstance().log(LogLevel::INFO, "proactor design pattern example completed successfully");
        return 0;
    }
    catch (const std::exception& e) {
        Logger::getInstance().log(LogLevel::ERROR, std::format("exception: {}", e.what()));
        return 1;
    }
}
