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

int ThreadUtility::threadGroupCounter;
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
        static int threadGroupUUID;                         // static; global to all classes
        threadGroupUUID++;
        return (threadGroupUUID);
    }
};
int threadGroupUUID = 0;

ThreadGroup::ThreadGroup(void (*driverMethod)()) {
    externalDriverMethod = driverMethod;
    thisThreadGroupUUID = ThreadGroup::GetThreadGroupUUID();
}

void
ThreadGroup::threadGroupPromiseMethod(std::promise<std::string> promiseObj) {
    ThreadUtility::AddThreadName();
    threadGroupLogCacheAt += "ThreadGroup::threadGroupPromiseMethod: (group id:" + std::to_string(thisThreadGroupUUID) + ")";
    promiseObj.set_value_at_thread_exit(threadGroupLogCacheAt);
    threadGroupLogCacheDel;
    this->externalDriverMethod();
    LOG_INFO("ThreadGroup::threadGroupPromiseMethod: end");
}

void
ThreadGroup::threadGroupFutureMethod(std::future<std::string> futureObj) {
    ThreadUtility::AddThreadName();
    futureObj.wait();
    LOG_INFO("ThreadGroup::threadGroupFutureMethod: trigger (group id:", thisThreadGroupUUID, ")");
    LOG_INFO(futureObj.get());
}


void
ThreadGroup::Start() {
    // send future to new thread
    threadGroupFuture = threadGroupPromise.get_future();    // engage our promise with future
    threadGroupThreadPair.push_back(std::thread(&ThreadGroup::threadGroupFutureMethod, this, std::move(this->threadGroupFuture)));

    // send promise to new thread which will fullfill our promise
    threadGroupThreadPair.push_back(std::thread(&ThreadGroup::threadGroupPromiseMethod, this, std::move(this->threadGroupPromise)));
}

void
ThreadGroup::Join() {
    for (auto& nextThread : threadGroupThreadPair)
        nextThread.join();
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
    threadGroups.push_back(std::unique_ptr<ThreadGroup>(new ThreadGroup(driverMethod)));
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
    static int driverCnt;                                   // keep track of multiple calls to this method
    std::string msg = "hi from driver method " + std::to_string(++driverCnt);

     // ### test code to delay some threads to show correct asynchronous behavior
    switch(driverCnt) {
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
