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

        // return our unique thread name
        return (ThreadUtility::threadGroupNameCache[std::this_thread::get_id()]);
    }

    static int GetThreadCounter() {
        std::unique_lock<std::mutex> autoLock(ThreadUtilityMutex);
        return (ThreadUtility::threadGroupCounter);
    }
};

int ThreadUtility::threadGroupCounter = 0;
std::mutex ThreadUtility::ThreadUtilityMutex;
std::thread::id ThreadUtility::mainThreadID = std::this_thread::get_id();
std::map<std::thread::id, std::string> ThreadUtility::threadGroupNameCache;
std::map<std::thread::id, std::string> ThreadUtility::threadGroupLogCache;
#define threadGroupLogCacheAt  ThreadUtility::threadGroupLogCache[std::this_thread::get_id()]
#define threadGroupLogCacheDel ThreadUtility::threadGroupLogCache.erase(std::this_thread::get_id())



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
        threadGroupLogCacheAt += "ThreadGroup::threadGroupPromiseMethod: (group id:" + std::to_string(thisThreadGroupUUID) + ")";
        
        // Set value at thread exit - this will be called even if exception occurs
        promiseObj.set_value_at_thread_exit(threadGroupLogCacheAt);
        threadGroupLogCacheDel;
        
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
    ThreadUtility::AddThreadName();                         // will add main thread to our thread name list

    // test ThreadGroupContainer
    ThreadGroupContainer ThreadGroupContainer;
    for (int thrdex = 0; thrdex < 1000; ++thrdex)
        ThreadGroupContainer.Add(driverMethod);

    ThreadGroupContainer.Start();
    ThreadGroupContainer.Join();

    return 0;
}
