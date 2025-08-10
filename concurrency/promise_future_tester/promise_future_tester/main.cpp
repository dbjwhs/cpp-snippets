#include "../../../headers/project_utils.hpp"
#include <iostream>
#include <functional>                                       // std::ref
#include <thread>
#include <future>                                           // std::promise, std::future
#include <chrono>
#include <vector>
#include <map>
#include <sstream>
#include <ctime>
#include <string>
#include <atomic>

class ThreadUtility {
private:
    static bool IsMainThread() {
        // private, no lock needed, if lock needed for future please be aware of race from AddThreadName()
        return (ThreadUtility::mainThreadID == std::this_thread::get_id());
    }
public:
    static int threadGroupCounter;
    static std::mutex ThreadUtilityMutex;
    static std::thread::id mainThreadID;
    static std::map<std::thread::id, std::string> threadGroupNameCache;
    static std::map<std::thread::id, std::string> threadGroupLogCache;

    static void AddThreadName() {
        std::unique_lock<std::mutex> autoLock(ThreadUtilityMutex);
        ThreadUtility::threadGroupCounter++;
        std::stringstream threadIDStrm;
        threadIDStrm << std::this_thread::get_id();
        std::string threadIDStr = threadIDStrm.str();

        std::stringstream threadNameStrm;
        threadNameStrm << (ThreadUtility::IsMainThread() ? "Main   " : "Thread ") << ThreadUtility::threadGroupCounter << " | " << threadIDStr;
        std::string threadNameStr = threadNameStrm.str();

        // each thread of each thread group will have a unique name in our local threadGroupNameCache in
        // the form of...
        // Main    1
        // Thread <n>
        // ... were <n> is the next monotonically increasing value starting at 1 which will always be the main thread
        ThreadUtility::threadGroupNameCache[std::this_thread::get_id()] = threadNameStr;
    }

    static std::string ThreadIDStr() {
        std::unique_lock<std::mutex> autoLock(ThreadUtilityMutex);

        // Find the thread name safely without creating entries
        auto currentThreadId = std::this_thread::get_id();
        auto it = ThreadUtility::threadGroupNameCache.find(currentThreadId);
        
        if (it != ThreadUtility::threadGroupNameCache.end()) {
            return it->second;
        } else {
            // Thread not found in cache - this shouldn't happen if AddThreadName was called
            std::stringstream fallbackName;
            fallbackName << "Unknown Thread | " << currentThreadId;
            std::string fallbackStr = fallbackName.str();
            LOG_WARNING("Thread not found in name cache, using fallback: ", fallbackStr);
            return fallbackStr;
        }
    }

    static int GetThreadCounter() {
        std::unique_lock<std::mutex> autoLock(ThreadUtilityMutex);
        return (ThreadUtility::threadGroupCounter);
    }
    
    // Clean up thread-specific resources (call when thread is done)
    static void CleanupThread() {
        std::unique_lock<std::mutex> autoLock(ThreadUtilityMutex);
        auto currentThreadId = std::this_thread::get_id();
        threadGroupNameCache.erase(currentThreadId);
        threadGroupLogCache.erase(currentThreadId);
    }
    
    // Clean up all resources (call at program shutdown)
    static void CleanupAll() {
        std::unique_lock<std::mutex> autoLock(ThreadUtilityMutex);
        threadGroupNameCache.clear();
        threadGroupLogCache.clear();
        threadGroupCounter = 0;
    }
};

int ThreadUtility::threadGroupCounter = 0;
std::mutex ThreadUtility::ThreadUtilityMutex;
std::thread::id ThreadUtility::mainThreadID = std::this_thread::get_id();
std::map<std::thread::id, std::string> ThreadUtility::threadGroupNameCache;
std::map<std::thread::id, std::string> ThreadUtility::threadGroupLogCache;
// RAII-based thread log cache manager for safe automatic cleanup
class ThreadLogCacheManager {
private:
    std::thread::id threadId;
    std::string logData;
    
public:
    ThreadLogCacheManager() : threadId(std::this_thread::get_id()) {
        // Initialize with empty log data
    }
    
    ~ThreadLogCacheManager() {
        // Automatic cleanup using RAII - always executes
        std::unique_lock<std::mutex> autoLock(ThreadUtility::ThreadUtilityMutex);
        ThreadUtility::threadGroupLogCache.erase(threadId);
    }
    
    // Non-copyable, non-movable for safety
    ThreadLogCacheManager(const ThreadLogCacheManager&) = delete;
    ThreadLogCacheManager& operator=(const ThreadLogCacheManager&) = delete;
    ThreadLogCacheManager(ThreadLogCacheManager&&) = delete;
    ThreadLogCacheManager& operator=(ThreadLogCacheManager&&) = delete;
    
    // Thread-safe append to log data
    void appendLog(const std::string& data) {
        std::unique_lock<std::mutex> autoLock(ThreadUtility::ThreadUtilityMutex);
        logData += data;
        ThreadUtility::threadGroupLogCache[threadId] = logData;
    }
    
    // Thread-safe get log data
    std::string getLog() const {
        std::unique_lock<std::mutex> autoLock(ThreadUtility::ThreadUtilityMutex);
        return logData;
    }
};



class ThreadGroup
{
private:
    // our external driver methods where all BAT work for this thread group will happen
    void (*externalDriverMethod)(void);

    // thread group local object
    int thisThreadGroupUUID;                                // local to this class, will contain unique ID for each ThreadGroup class
    std::mutex threadGroupMutex;                            // class mutex
    std::vector<std::thread> threadGroupThreadPair;         // this objects thread pair, one to do work, one to wait for completion and report/log/etc
    std::promise<std::string> threadGroupPromise;           // this objects thread promise
    std::future<std::string> threadGroupFuture;             // this objects thread future

    ThreadGroup() {}                                        // private, must construct with driver method
public:
    ThreadGroup(void (*driverMethod)());
    virtual ~ThreadGroup() {}
    void threadGroupPromiseMethod(std::promise<std::string> promiseObj);
    void threadGroupFutureMethod(std::future<std::string> futureObj);
    void Start();
    void Join();

    static int GetThreadGroupUUID() {
        static std::atomic<int> threadGroupUUID{0};         // thread-safe static; global to all classes
        return ++threadGroupUUID;
    }
};

ThreadGroup::ThreadGroup(void (*driverMethod)()) {
    externalDriverMethod = driverMethod;
    thisThreadGroupUUID = ThreadGroup::GetThreadGroupUUID();
}

void
ThreadGroup::threadGroupPromiseMethod(std::promise<std::string> promiseObj) {
    try {
        ThreadUtility::AddThreadName();
        // RAII-based log management - automatically cleaned up on scope exit
        ThreadLogCacheManager logManager;
        logManager.appendLog("ThreadGroup::threadGroupPromiseMethod: (group id:" + std::to_string(thisThreadGroupUUID) + ")");
        
        // Set value at thread exit - this will be called even if exception occurs
        promiseObj.set_value_at_thread_exit(logManager.getLog());
        // No manual cleanup needed - RAII handles it automatically
        
        // Call external driver method with exception handling
        if (this->externalDriverMethod) {
            this->externalDriverMethod();
        } else {
            LOG_ERROR("External driver method is null");
        }
        
        LOG_INFO("ThreadGroup::threadGroupPromiseMethod: end");
    }
    catch (const std::exception& e) {
        LOG_ERROR("Exception in threadGroupPromiseMethod: ", e.what());
        // Promise will be set at thread exit regardless
    }
    catch (...) {
        LOG_ERROR("Unknown exception in threadGroupPromiseMethod");
        // Promise will be set at thread exit regardless
    }
}

void
ThreadGroup::threadGroupFutureMethod(std::future<std::string> futureObj) {
    try {
        ThreadUtility::AddThreadName();
        
        // Validate future before waiting
        if (!futureObj.valid()) {
            LOG_ERROR("Invalid future in threadGroupFutureMethod");
            return;
        }
        
        // Wait for promise to be fulfilled
        futureObj.wait();
        LOG_INFO("ThreadGroup::threadGroupFutureMethod: trigger (group id:", thisThreadGroupUUID, ")");
        
        // Get the result - this can throw if promise was set with exception
        std::string result = futureObj.get();
        LOG_INFO(result);
    }
    catch (const std::future_error& e) {
        LOG_ERROR("Future error in threadGroupFutureMethod: ", e.what());
    }
    catch (const std::exception& e) {
        LOG_ERROR("Exception in threadGroupFutureMethod: ", e.what());
    }
    catch (...) {
        LOG_ERROR("Unknown exception in threadGroupFutureMethod");
    }
}


void
ThreadGroup::Start() {
    try {
        // Ensure we haven't already started
        if (!threadGroupThreadPair.empty()) {
            LOG_ERROR("ThreadGroup already started");
            return;
        }

        // Get future before moving promise - this validates the promise
        threadGroupFuture = threadGroupPromise.get_future();
        
        // Validate future is valid before proceeding
        if (!threadGroupFuture.valid()) {
            LOG_ERROR("Invalid future obtained from promise");
            return;
        }

        // Create threads with moved objects - order is important
        threadGroupThreadPair.push_back(
            std::thread(&ThreadGroup::threadGroupFutureMethod, this, std::move(threadGroupFuture))
        );
        threadGroupThreadPair.push_back(
            std::thread(&ThreadGroup::threadGroupPromiseMethod, this, std::move(threadGroupPromise))
        );
    }
    catch (const std::exception& e) {
        LOG_ERROR("Exception in ThreadGroup::Start: ", e.what());
        // Clean up any created threads
        for (auto& thread : threadGroupThreadPair) {
            if (thread.joinable()) {
                thread.join();
            }
        }
        threadGroupThreadPair.clear();
        throw; // Re-throw for caller to handle
    }
}

void
ThreadGroup::Join() {
    try {
        for (auto& nextThread : threadGroupThreadPair) {
            if (nextThread.joinable()) {
                nextThread.join();
            }
        }
        // Clean up thread resources after successful join
        // Note: Each thread cleans up its own resources via RAII
    }
    catch (const std::exception& e) {
        LOG_ERROR("Exception in ThreadGroup::Join: ", e.what());
        // Still try to join remaining threads
        for (auto& nextThread : threadGroupThreadPair) {
            if (nextThread.joinable()) {
                try {
                    nextThread.join();
                }
                catch (...) {
                    // Suppress exceptions during cleanup
                }
            }
        }
        throw; // Re-throw original exception
    }
}

class ThreadGroupContainer
{
private:
    std::vector<std::unique_ptr<ThreadGroup>> threadGroups;
public:
    ThreadGroupContainer() { this->Reset(); };
    virtual ~ThreadGroupContainer() {}
    void Add(void (*driverMethod)());
    void Start();
    void Join();
    void Reset() { threadGroups.clear(); }
};


void
ThreadGroupContainer::Add(void (*driverMethod)()) {
    LOG_INFO("ThreadGroupContainer::Add: thread group");
    threadGroups.push_back(std::make_unique<ThreadGroup>(driverMethod));
}


void
ThreadGroupContainer::Start() {
    LOG_INFO("ThreadGroupContainer::Start: starting thread groups");
    for (auto& nextThreadGroup : threadGroups)
        nextThreadGroup->Start();

}


void
ThreadGroupContainer::Join() {
    LOG_INFO("ThreadGroupContainer::Join: joining thread groups");
    for (auto& nextThreadGroup : threadGroups)
        nextThreadGroup->Join();

}


void driverMethod() {
    static std::atomic<int> driverCnt{0};                   // thread-safe counter for multiple calls to this method
    int currentCount = ++driverCnt;
    std::string msg = "hi from driver method " + std::to_string(currentCount);

     // ### test code to delay some threads to show correct asynchronous behavior
    switch(currentCount) {
        case 3:
            std::this_thread::sleep_for(std::chrono::seconds(15));
            break;
        case 5:
            std::this_thread::sleep_for(std::chrono::seconds(5));
            break;
        case 15:
            std::this_thread::sleep_for(std::chrono::seconds(10));
            break;
        case 30:
            std::this_thread::sleep_for(std::chrono::seconds(4));
            break;
        case 55:
            std::this_thread::sleep_for(std::chrono::seconds(11));
            break;
        case 180:
            std::this_thread::sleep_for(std::chrono::seconds(12));
            break;
        case 750:
            std::this_thread::sleep_for(std::chrono::seconds(12));
            break;
    }

    LOG_INFO(msg);
}


int main() {
    try {
        ThreadUtility::AddThreadName();                         // will add main thread to our thread name list

        // test ThreadGroupContainer
        ThreadGroupContainer threadGroupContainer;
        for (int thrdex = 0; thrdex < 1000; ++thrdex)
            threadGroupContainer.Add(driverMethod);

        threadGroupContainer.Start();
        threadGroupContainer.Join();
        
        // Clean up all ThreadUtility resources before program exit
        ThreadUtility::CleanupAll();
        
        LOG_INFO("All thread groups completed successfully");
        return 0;
    }
    catch (const std::exception& e) {
        LOG_ERROR("Fatal exception in main: ", e.what());
        // Attempt cleanup even on failure
        try {
            ThreadUtility::CleanupAll();
        }
        catch (...) {
            // Suppress cleanup exceptions
        }
        return 1;
    }
    catch (...) {
        LOG_ERROR("Unknown fatal exception in main");
        // Attempt cleanup even on failure
        try {
            ThreadUtility::CleanupAll();
        }
        catch (...) {
            // Suppress cleanup exceptions
        }
        return 1;
    }
}
