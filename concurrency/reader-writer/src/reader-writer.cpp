// MIT License
// Copyright (c) 2025 dbjwhs

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <chrono>
#include <random>

#define DECLARE_NON_COPYABLE(ClassType) \
    ClassType(const ClassType&) = delete; \
    ClassType& operator=(const ClassType&) = delete

#define DECLARE_NON_MOVEABLE(ClassType) \
    ClassType(ClassType&) = delete; \
    ClassType& operator=(ClassType&) = delete

// simple random generator for int's
class RandomGenerator {
private:
    std::mt19937 m_gen;
    std::uniform_int_distribution<int> m_dist;  // for integers
    // or
    // std::uniform_real_distribution<double> m_dist;  // for floating point

public:
    RandomGenerator(int min, int max)
        : m_gen(std::random_device{}())
        , m_dist(min, max) {}

    int getNumber() {
        return m_dist(m_gen);
    }

    // delete copy and move operations
    DECLARE_NON_COPYABLE(RandomGenerator);
    DECLARE_NON_MOVEABLE(RandomGenerator);
};

// thread-safe singleton logger
class Logger {
private:
    static inline std::mutex m_mutex;  // C++17 inline static member
    std::ostream& m_stream;

    // private constructor for singleton
    explicit Logger() : m_stream(std::cout) {}

    // delete copy and move operations
    DECLARE_NON_COPYABLE(Logger);
    DECLARE_NON_MOVEABLE(Logger);

public:
    // get singleton instance
    static Logger& getInstance() {
        static Logger instance;
        return instance;
    }

    template<typename... Args>
    void print(Args&&... args) {
        std::lock_guard<std::mutex> lock(m_mutex);
        (m_stream << ... << std::forward<Args>(args)) << std::endl;
    }
};

class ReadersWriters {
private:
    // resource protection
    std::mutex m_mutex;
    std::condition_variable m_readCV;
    std::condition_variable m_writeCV;

    // state tracking
    int m_activeReaders;        // current number of active readers
    int m_waitingReaders;       // readers waiting to acquire the lock
    bool m_isWriting;           // flag to track if a writer is currently active
    int m_waitingWriters;       // writers waiting to acquire the lock
    int m_sharedResource;       // the actual resource being protected

public:
    ReadersWriters() :
        m_activeReaders(0),
        m_waitingReaders(0),
        m_isWriting(false),
        m_waitingWriters(0),
        m_sharedResource(0) {}

    void startRead() {
        // lock is automatically released when going out of scope
        std::unique_lock<std::mutex> lock(m_mutex);

        // increment waiting readers counter
        m_waitingReaders++;

        // wait if there's an active writer or waiting writers
        // this gives preference to writers to prevent their starvation
        m_readCV.wait(lock, [this]() {
            return !m_isWriting && m_waitingWriters == 0;
        });

        // decrement waiting readers and increment active readers
        m_waitingReaders--;
        m_activeReaders++;
    }

    void endRead() {
        std::unique_lock<std::mutex> lock(m_mutex);

        // decrement active readers count
        m_activeReaders--;

        // if this was the last reader, notify a waiting writer
        if (m_activeReaders == 0) {
            m_writeCV.notify_one();
        }
    }

    void startWrite() {
        // lock is automatically released when going out of scope
        std::unique_lock<std::mutex> lock(m_mutex);

        // increment waiting writers counter
        m_waitingWriters++;

        // wait until there are no active readers and no active writer
        m_writeCV.wait(lock, [this]() {
            return !m_isWriting && m_activeReaders == 0;
        });

        // decrement waiting writers and set writing flag
        m_waitingWriters--;
        m_isWriting = true;
    }

    void endWrite() {
        // lock is automatically released when going out of scope
        std::unique_lock<std::mutex> lock(m_mutex);

        // clear writing flag
        m_isWriting = false;

        // if there are waiting writers, give them priority
        // otherwise, wake up all waiting readers
        if (m_waitingWriters > 0) {
            m_writeCV.notify_one();
        } else {
            m_readCV.notify_all();
        }
    }

    // methods to simulate reading and writing with RAII locks
    void readResource() {
        // acquire read lock using RAII, note very clean interface now for the startRead
        // and endRead calls, upon ReadLock construction we start the read, on function
        // return we end the read on ReadLock destruction
        class ReadLock {
            ReadersWriters& m_rw;
        public:
            explicit ReadLock(ReadersWriters& rw) : m_rw(rw) {
                m_rw.startRead();
            }
            ~ReadLock() {
                m_rw.endRead();
            }

            // delete copy operations to prevent multiple releases
            DECLARE_NON_COPYABLE(ReadLock);
        } m_readLock(*this);

        Logger::getInstance().print("Thread ", std::this_thread::get_id(), " reading resource: ", m_sharedResource);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    void writeResource(int value) {
        // acquire write lock using RAII, note very clean interface now for the startWrite
        // and endWrite calls, upon WriteLock construction we start the write, on function
        // return we end the write on WriteLock destruction
        class WriteLock {
            ReadersWriters& m_rw;
        public:
            explicit WriteLock(ReadersWriters& rw) : m_rw(rw) {
                m_rw.startWrite();
            }
            ~WriteLock() {
                m_rw.endWrite();
            }

            // delete copy operations to prevent multiple releases
            DECLARE_NON_COPYABLE(WriteLock);
        } m_writeLock(*this);

        m_sharedResource = value;
        Logger::getInstance().print("Thread ", std::this_thread::get_id(), " wrote resource: ", value);
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
};

// example usage
int main() {
    // setup how many reader/write threads we want, ensure they are generated at compile time
    constexpr auto READER_THREAD_CNT = 2;
    constexpr auto WRITER_THREAD_CNT = 5;

    // reserve our know sized vector<std::thread>
    std::vector<std::thread> threads;
    threads.reserve(READER_THREAD_CNT + WRITER_THREAD_CNT);
    Logger::getInstance().print("Reserving ", READER_THREAD_CNT + WRITER_THREAD_CNT, " threads");

    // read/write class
    ReadersWriters rw;

    // we will generate random amounts of read/writes
    RandomGenerator random_rw(3, 15);

    // create reader threads
    for (int read_thrd_cnt = 0; read_thrd_cnt < READER_THREAD_CNT; ++read_thrd_cnt) {
        threads.emplace_back([&rw, read_thrd_cnt, &random_rw]() {
            Logger::getInstance().print("Started reader thread ", read_thrd_cnt);
            int read_cnt = random_rw.getNumber();
            for (int reads = 0; reads < read_cnt; ++reads) {
                rw.readResource();
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }
            Logger::getInstance().print("Finished reader thread ", read_thrd_cnt);
        });
    }

    // create writer threads
    for (int write_thrd_cnt = 0; write_thrd_cnt < WRITER_THREAD_CNT; ++write_thrd_cnt) {
        threads.emplace_back([&rw, write_thrd_cnt, &random_rw]() {
            Logger::getInstance().print("Started writer thread ", write_thrd_cnt);
            int write_cnt = random_rw.getNumber();
            for (int writes = 0; writes < write_cnt; ++writes) {
                rw.writeResource(write_thrd_cnt * 10 + writes);
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
            Logger::getInstance().print("Finished writer thread ", write_thrd_cnt);
        });
    }

    // wait for all threads to complete
    for (auto& thread : threads) {
        thread.join();
    }
    return 0;
}
