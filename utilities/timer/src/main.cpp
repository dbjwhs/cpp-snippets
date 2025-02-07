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
    // start the timer
    void start() {
        m_start_time = std::chrono::high_resolution_clock::now();
        m_is_running = true;
    }

    // stop the timer
    void stop() {
        if (m_is_running) {
            m_end_time = std::chrono::high_resolution_clock::now();
            m_is_running = false;
        }
    }

    // reset the timer
    void reset() {
        m_is_running = false;
    }

    // get elapsed time in various units
    [[nodiscard]] double elapsed_nanoseconds() const {
        return get_elapsed_duration<std::chrono::nanoseconds>();
    }

    [[nodiscard]] double elapsed_microseconds() const {
        return get_elapsed_duration<std::chrono::microseconds>();
    }

    [[nodiscard]] double elapsed_milliseconds() const {
        return get_elapsed_duration<std::chrono::milliseconds>();
    }

    [[nodiscard]] double elapsed_seconds() const {
        return get_elapsed_duration<std::chrono::seconds>();
    }

    // get formatted string of elapsed time with the appropriate unit
    [[nodiscard]] std::string elapsed_formatted() const {
        const double elapsed = elapsed_nanoseconds();
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

    // check if the timer is currently running
    [[nodiscard]] bool running() const {
        return m_is_running;
    }

private:
    std::chrono::high_resolution_clock::time_point m_start_time;
    std::chrono::high_resolution_clock::time_point m_end_time;
    bool m_is_running = false;

    // Helper template function to get elapsed time in specified duration
    template<typename DurationType>
    [[nodiscard]] double get_elapsed_duration() const {
        if (!m_is_running) {
            auto duration = std::chrono::duration_cast<DurationType>(m_end_time - m_start_time);
            return static_cast<double>(duration.count());
        } else {
            const auto current_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<DurationType>(current_time - m_start_time);
            return static_cast<double>(duration.count());
        }
    }
};

int main() {
    HighResolutionTimer timer;

    // test 1: Basic timing
    std::cout << "Test 1: Basic timing with microseconds delay\n";
    timer.start();
    std::this_thread::sleep_for(std::chrono::microseconds(500));
    timer.stop();
    std::cout << "Elapsed time: " << timer.elapsed_formatted() << "\n\n";

    // test 2: Different time units
    std::cout << "Test 2: Display time in different units\n";
    timer.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    timer.stop();
    std::cout << "Nanoseconds:  " << timer.elapsed_nanoseconds() << " ns\n";
    std::cout << "Microseconds: " << timer.elapsed_microseconds() << " µs\n";
    std::cout << "Milliseconds: " << timer.elapsed_milliseconds() << " ms\n";
    std::cout << "Seconds:      " << timer.elapsed_seconds() << " s\n";
    std::cout << "Formatted:    " << timer.elapsed_formatted() << "\n\n";

    // test 3: Timer status
    std::cout << "Test 3: Timer status checking\n";
    timer.reset();
    // ReSharper disable CppDFAConstantConditions
    std::cout << "After reset, timer running: " << (timer.running() ? "yes" : "no") << "\n";
    timer.start();
    std::cout << "After start, timer running: " << (timer.running() ? "yes" : "no") << "\n";
    timer.stop();
    std::cout << "After stop, timer running: " << (timer.running() ? "yes" : "no") << "\n\n";
    // ReSharper restore CppDFAConstantConditions

    // test 4: Measuring while running
    std::cout << "Test 4: Measuring while timer is running\n";
    timer.start();
    std::cout << "Starting measurement...\n";
    for (int ndx = 1; ndx <= 3; ++ndx) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        std::cout << "Time at check " << ndx << ": " << timer.elapsed_formatted() << "\n";
    }
    timer.stop();
    std::cout << "Final time: " << timer.elapsed_formatted() << "\n\n";

    // test 5: Multiple start/stops
    std::cout << "Test 5: Multiple start/stops\n";
    timer.reset();
    timer.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    timer.stop();
    std::cout << "First measurement: " << timer.elapsed_formatted() << "\n";

    // start a new measurement
    timer.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    timer.stop();
    std::cout << "Second measurement: " << timer.elapsed_formatted() << "\n";

    return 0;
}
