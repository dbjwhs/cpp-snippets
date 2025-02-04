// MIT License
// Copyright (c) 2025 dbjwhs

#include <thread>
#include <condition_variable>
#include <vector>
#include "../../../headers/project_utils.hpp"

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
    void readResource(Logger& logger) {
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

        logger.log(LogLevel::INFO, "Thread " + threadIdToString() + " reading resource: " + std::to_string(m_sharedResource));
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    void writeResource(int value, Logger& logger) {
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
        logger.log(LogLevel::INFO, "Thread " + threadIdToString() + " wrote resource: " + std::to_string(value));
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
};

// example usage
int main() {
    Logger logger("../custom.log");

    // setup how many reader/write threads we want, ensure they are generated at compile time
    constexpr auto READER_THREAD_CNT = 2;
    constexpr auto WRITER_THREAD_CNT = 5;

    // reserve our know sized vector<std::thread>
    std::vector<std::thread> threads;
    threads.reserve(READER_THREAD_CNT + WRITER_THREAD_CNT);
    logger.log(LogLevel::INFO, "Reserving " + std::to_string(READER_THREAD_CNT + WRITER_THREAD_CNT) + " threads");

    // read/write class
    ReadersWriters rw;

    // we will generate random amounts of read/writes
    RandomGenerator random_rw(3, 15);

    // create reader threads
    for (int read_thrd_cnt = 0; read_thrd_cnt < READER_THREAD_CNT; ++read_thrd_cnt) {
        threads.emplace_back([&rw, read_thrd_cnt, &random_rw, &logger]() {
            logger.log(LogLevel::INFO, "Started reader thread " + std::to_string(read_thrd_cnt));
            int read_cnt = random_rw.getNumber();
            for (int reads = 0; reads < read_cnt; ++reads) {
                rw.readResource(logger);
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }
            logger.log(LogLevel::INFO, "Finished reader thread " + std::to_string(read_thrd_cnt));
        });
    }

    // create writer threads
    for (int write_thrd_cnt = 0; write_thrd_cnt < WRITER_THREAD_CNT; ++write_thrd_cnt) {
        threads.emplace_back([&rw, write_thrd_cnt, &random_rw, &logger]() {
            logger.log(LogLevel::INFO, "Started writer thread " + std::to_string(write_thrd_cnt));
            const int write_cnt = random_rw.getNumber();
            for (int writes = 0; writes < write_cnt; ++writes) {
                rw.writeResource(write_thrd_cnt * 10 + writes, logger);
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
            logger.log(LogLevel::INFO, "Finished writer thread " + std::to_string(write_thrd_cnt));
        });
    }

    // wait for all threads to complete
    for (auto& thread : threads) {
        thread.join();
    }
    return 0;
}
