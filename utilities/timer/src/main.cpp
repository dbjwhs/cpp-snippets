// MIT License
// Copyright (c) 2025 dbjwhs

#include <chrono>
#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <thread>  // for sleep_for in our test

class HighResolutionTimer {
public:
    // Start the timer
    void start() {
        start_time = std::chrono::high_resolution_clock::now();
        is_running = true;
    }

    // Stop the timer
    void stop() {
        if (is_running) {
            end_time = std::chrono::high_resolution_clock::now();
            is_running = false;
        }
    }

    // Reset the timer
    void reset() {
        is_running = false;
    }

    // Get elapsed time in various units
    double elapsed_nanoseconds() const {
        return get_elapsed_duration<std::chrono::nanoseconds>();
    }

    double elapsed_microseconds() const {
        return get_elapsed_duration<std::chrono::microseconds>();
    }

    double elapsed_milliseconds() const {
        return get_elapsed_duration<std::chrono::milliseconds>();
    }

    double elapsed_seconds() const {
        return get_elapsed_duration<std::chrono::seconds>();
    }

    // Get formatted string of elapsed time with appropriate unit
    std::string elapsed_formatted() const {
        double elapsed = elapsed_nanoseconds();
        std::stringstream ss;
        ss << std::fixed << std::setprecision(3);

        if (elapsed < 1000.0) {
            ss << elapsed << " ns";
        } else if (elapsed < 1000000.0) {
            ss << (elapsed / 1000.0) << " µs";
        } else if (elapsed < 1000000000.0) {
            ss << (elapsed / 1000000.0) << " ms";
        } else {
            ss << (elapsed / 1000000000.0) << " s";
        }

        return ss.str();
    }

    // Check if timer is currently running
    bool running() const {
        return is_running;
    }

private:
    std::chrono::high_resolution_clock::time_point start_time;
    std::chrono::high_resolution_clock::time_point end_time;
    bool is_running = false;

    // Helper template function to get elapsed time in specified duration
    template<typename DurationType>
    double get_elapsed_duration() const {
        if (!is_running) {
            auto duration = std::chrono::duration_cast<DurationType>(end_time - start_time);
            return static_cast<double>(duration.count());
        } else {
            auto current_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<DurationType>(current_time - start_time);
            return static_cast<double>(duration.count());
        }
    }
};

int main() {
    HighResolutionTimer timer;

    // Test 1: Basic timing
    std::cout << "Test 1: Basic timing with microseconds delay\n";
    timer.start();
    std::this_thread::sleep_for(std::chrono::microseconds(500));
    timer.stop();
    std::cout << "Elapsed time: " << timer.elapsed_formatted() << "\n\n";

    // Test 2: Different time units
    std::cout << "Test 2: Display time in different units\n";
    timer.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    timer.stop();
    std::cout << "Nanoseconds:  " << timer.elapsed_nanoseconds() << " ns\n";
    std::cout << "Microseconds: " << timer.elapsed_microseconds() << " µs\n";
    std::cout << "Milliseconds: " << timer.elapsed_milliseconds() << " ms\n";
    std::cout << "Seconds:      " << timer.elapsed_seconds() << " s\n";
    std::cout << "Formatted:    " << timer.elapsed_formatted() << "\n\n";

    // Test 3: Timer status
    std::cout << "Test 3: Timer status checking\n";
    timer.reset();
    std::cout << "After reset, timer running: " << (timer.running() ? "yes" : "no") << "\n";
    timer.start();
    std::cout << "After start, timer running: " << (timer.running() ? "yes" : "no") << "\n";
    timer.stop();
    std::cout << "After stop, timer running: " << (timer.running() ? "yes" : "no") << "\n\n";

    // Test 4: Measuring while running
    std::cout << "Test 4: Measuring while timer is running\n";
    timer.start();
    std::cout << "Starting measurement...\n";
    for (int i = 1; i <= 3; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        std::cout << "Time at check " << i << ": " << timer.elapsed_formatted() << "\n";
    }
    timer.stop();
    std::cout << "Final time: " << timer.elapsed_formatted() << "\n\n";

    // Test 5: Multiple start/stops
    std::cout << "Test 5: Multiple start/stops\n";
    timer.reset();
    timer.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    timer.stop();
    std::cout << "First measurement: " << timer.elapsed_formatted() << "\n";

    timer.start();  // Start a new measurement
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    timer.stop();
    std::cout << "Second measurement: " << timer.elapsed_formatted() << "\n";

    return 0;
}
