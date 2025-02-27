// MIT License
// Copyright (c) 2025 dbjwhs

#include "../../../headers/project_utils.hpp"

// class representing a philosopher in the dining problem
class Philosopher {
private:
    int m_id;                    // unique identifier for each philosopher
    std::mutex *m_leftFork;      // left fork mutex pointer
    std::mutex *m_rightFork;     // right fork mutex pointer
    int m_mealsEaten;            // counter for meals eaten
    static constexpr int m_maxMeals = 3;  // maximum meals each philosopher will eat
    Logger *m_logger;

public:
    // constructor initializing philosopher with their id and adjacent forks
    Philosopher(const int id, std::mutex* leftFork, std::mutex* rightFork, Logger* logger)
        : m_id(id), m_leftFork(leftFork), m_rightFork(rightFork), m_mealsEaten(0), m_logger(logger) {}

    // main dining action for each philosopher
    void dine() {
        while (m_mealsEaten < m_maxMeals) {
            think();
            eat();
        }
    }

private:
    // simulate thinking process
    void think() const {
        RandomGenerator random(1, 100);
        m_logger->log(LogLevel::INFO, "Philosopher ", m_id, " is thinking...");
        // random thinking time between 1-3 seconds
        std::this_thread::sleep_for(std::chrono::seconds(random.getNumber() % 3 + 1));
    }

    // simulate eating process with resource management
    void eat() {
        RandomGenerator random(100, 1000);

        // try to pick up forks using resource hierarchy to prevent deadlock
        if (m_id % 2 == 0) {
            // even numbered philosophers pick up left fork first
            std::lock_guard<std::mutex> leftLock(*m_leftFork);
            m_logger->log(LogLevel::INFO, "Philosopher ", m_id, " picked up left fork");

            // delay to demonstrate deadlock prevention
            std::this_thread::sleep_for(std::chrono::milliseconds(random.getNumber()));

            std::lock_guard<std::mutex> rightLock(*m_rightFork);
            m_logger->log(LogLevel::INFO, "Philosopher ", m_id, " picked up right fork");
        } else {
            // odd numbered philosophers pick up right fork first
            std::lock_guard<std::mutex> rightLock(*m_rightFork);
            m_logger->log(LogLevel::INFO, "Philosopher ", m_id, " picked up right fork");

            // delay to demonstrate deadlock prevention
            std::this_thread::sleep_for(std::chrono::milliseconds(random.getNumber()));

            std::lock_guard<std::mutex> leftLock(*m_leftFork);
            m_logger->log(LogLevel::INFO, "Philosopher ", m_id, " picked up left fork");
        }

        // eating process
        m_logger->log(LogLevel::INFO, "Philosopher ", m_id, " is eating meal ", m_mealsEaten + 1);
        std::this_thread::sleep_for(std::chrono::seconds(random.getNumber() % 3 + 1));
        m_mealsEaten++;

        // forks are automatically released when lock_guards go out of scope
        m_logger->log(LogLevel::INFO, "Philosopher ", m_id, " finished eating and put down forks");
    }

    // delete copy and move operations
    DECLARE_NON_COPYABLE(Philosopher);
    DECLARE_NON_MOVEABLE(Philosopher);
};

int main() {
    constexpr int numPhilosophers = 5;

    // create forks (represented by mutexes)
    std::vector<std::mutex> forks(numPhilosophers);

    // create logger
    Logger custom_logger("../custom.log");

    // create philosophers first
    std::vector<std::unique_ptr<Philosopher>> philosophers;
    philosophers.reserve(numPhilosophers);
for (int ndx = 0; ndx < numPhilosophers; ++ndx) {
        philosophers.push_back(std::make_unique<Philosopher>(
            ndx,
            &forks[ndx],
            &forks[(ndx + 1) % numPhilosophers],
            &custom_logger
        ));
    }

    // create and store philosopher threads
    std::vector<std::thread> threads;
    threads.reserve(numPhilosophers);
for (int ndx = 0; ndx < numPhilosophers; ++ndx) {
        threads.emplace_back(&Philosopher::dine, philosophers[ndx].get());
    }

    // wait for all philosophers to finish dining
    for (auto& thread : threads) {
        thread.join();
    }

    custom_logger.log(LogLevel::INFO, "All philosophers have finished dining!\n");
    return 0;
}
