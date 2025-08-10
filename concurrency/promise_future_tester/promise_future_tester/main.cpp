#include "../../../headers/project_utils.hpp"
#include <iostream>
#include <functional>                                       // std::ref
#include <thread>
#include <future>                                           // std::promise, std::future
#include <chrono>
#include <vector>
#include <map>
#include <ctime>
#include <string>
#include <atomic>
#include <format>                                           // C++20 std::format for efficient string operations

// Configuration constants - replace hardcoded magic numbers
namespace Config {
    // Thread configuration
    constexpr int DEFAULT_THREAD_COUNT = 1000;
    
    // Sleep duration configuration (in seconds)
    constexpr int SLEEP_DURATION_SHORT = 4;
    constexpr int SLEEP_DURATION_MEDIUM = 5;
    constexpr int SLEEP_DURATION_LONG = 10;
    constexpr int SLEEP_DURATION_VERY_LONG = 11;
    constexpr int SLEEP_DURATION_EXTENDED = 12;
    constexpr int SLEEP_DURATION_MAX = 15;
    
    // Thread case configuration - specific thread numbers for delayed execution
    constexpr int DELAY_CASE_1 = 3;      // Thread 3 gets maximum delay
    constexpr int DELAY_CASE_2 = 5;      // Thread 5 gets medium delay
    constexpr int DELAY_CASE_3 = 15;     // Thread 15 gets long delay
    constexpr int DELAY_CASE_4 = 30;     // Thread 30 gets short delay
    constexpr int DELAY_CASE_5 = 55;     // Thread 55 gets very long delay
    constexpr int DELAY_CASE_6 = 180;    // Thread 180 gets extended delay
    constexpr int DELAY_CASE_7 = 750;    // Thread 750 gets extended delay
}

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

        // single-allocation string formatting using C++20 std::format
        std::string threadName = std::format("{} {} | {}",
                                             ThreadUtility::IsMainThread() ? "Main  " : "Thread",
                                             ThreadUtility::threadGroupCounter,
                                             std::this_thread::get_id()
        );

        // each thread of each thread group will have a unique name in our local threadGroupNameCache in
        // the form of...
        // Main    1
        // Thread <n>
        // ... were <n> is the next monotonically increasing value starting at 1 which will always be the main thread
        ThreadUtility::threadGroupNameCache[std::this_thread::get_id()] = std::move(threadName);
    }

    [[nodiscard]] static std::string ThreadIDStr() {
        std::unique_lock<std::mutex> autoLock(ThreadUtilityMutex);

        // Find the thread name safely without creating entries
        auto currentThreadId = std::this_thread::get_id();
        auto it = ThreadUtility::threadGroupNameCache.find(currentThreadId);

        if (it != ThreadUtility::threadGroupNameCache.end()) {
            return it->second;
        } else {
            // Thread not found in cache - this shouldn't happen if AddThreadName was called
            std::string fallbackStr = std::format("Unknown Thread | {}", currentThreadId);
            LOG_WARNING("Thread not found in name cache, using fallback: ", fallbackStr);
            return fallbackStr;
        }
    }

    [[nodiscard]] static int GetThreadCounter() {
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

    // Get current memory usage statistics for monitoring
    [[nodiscard]] static std::pair<size_t, size_t> GetMemoryUsage() {
        std::unique_lock<std::mutex> autoLock(ThreadUtilityMutex);
        return {threadGroupNameCache.size(), threadGroupLogCache.size()};
    }

    // Clean up stale entries (for long-running applications) - IMPLEMENTATION MOVED AFTER ThreadLifecycleManager
    [[nodiscard]] static size_t CleanupStaleEntries();
};

int ThreadUtility::threadGroupCounter = 0;

std::mutex ThreadUtility::ThreadUtilityMutex;

std::thread::id ThreadUtility::mainThreadID = std::this_thread::get_id();

std::map<std::thread::id, std::string> ThreadUtility::threadGroupNameCache;
std::map<std::thread::id, std::string> ThreadUtility::threadGroupLogCache;
// Thread lifecycle states for tracking
enum class ThreadLifeCycleState {
    CREATED,    // Thread object created but not started
    RUNNING,    // Thread is actively executing
    COMPLETED,  // Thread completed successfully
    FAILED,     // Thread completed with error
    JOINING     // Thread is being joined
};

// RAII-based thread lifecycle manager for complete thread resource management
class ThreadLifeCycleManager {
private:
    std::thread::id threadId;
    std::string logData;
    ThreadLifeCycleState state;
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point endTime;
    
    // Static tracking for all thread lifecycles
    static std::map<std::thread::id, ThreadLifeCycleState> threadStates;
    static std::map<std::thread::id, std::chrono::steady_clock::time_point> threadStartTimes;
    
public:
    ThreadLifeCycleManager() : threadId(std::this_thread::get_id()), state(ThreadLifeCycleState::CREATED) {
        startTime = std::chrono::steady_clock::now();
        
        // Register thread lifecycle start
        std::unique_lock<std::mutex> autoLock(ThreadUtility::ThreadUtilityMutex);
        threadStates[threadId] = ThreadLifeCycleState::RUNNING;
        threadStartTimes[threadId] = startTime;
        
        LOG_INFO(std::format("Thread lifecycle started for thread {}", threadId));
    }
    
    ~ThreadLifeCycleManager() {
        endTime = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        try {
            // Complete thread lifecycle cleanup
            std::unique_lock<std::mutex> autoLock(ThreadUtility::ThreadUtilityMutex);
            
            // Update state to completed or failed based on whether we're in an exception context
            state = std::uncaught_exceptions() > 0 ? ThreadLifeCycleState::FAILED : ThreadLifeCycleState::COMPLETED;
            threadStates[threadId] = state;
            
            // Clean up all thread-specific resources from both ThreadUtility and ThreadLifecycleManager
            ThreadUtility::threadGroupLogCache.erase(threadId);
            ThreadUtility::threadGroupNameCache.erase(threadId);
            
            // Remove from lifecycle tracking
            threadStates.erase(threadId);
            threadStartTimes.erase(threadId);
            
            const char* stateStr = (state == ThreadLifeCycleState::COMPLETED) ? "COMPLETED" : "FAILED";
            LOG_INFO(std::format("Thread lifecycle ended for thread {} - State: {} - Duration: {}ms", 
                threadId, stateStr, duration.count()));
                
        } catch (...) {
            // Ensure cleanup continues even if logging fails - comprehensive fallback cleanup
            try {
                std::unique_lock<std::mutex> autoLock(ThreadUtility::ThreadUtilityMutex);
                // Clean up all possible thread-related resources
                ThreadUtility::threadGroupLogCache.erase(threadId);
                ThreadUtility::threadGroupNameCache.erase(threadId);
                threadStates.erase(threadId);
                threadStartTimes.erase(threadId);
            } catch (...) {
                // Final fallback - suppress all exceptions during cleanup
            }
        }
    }
    
    // Non-copyable, non-movable for safety
    ThreadLifeCycleManager(const ThreadLifeCycleManager&) = delete;
    ThreadLifeCycleManager& operator=(const ThreadLifeCycleManager&) = delete;
    ThreadLifeCycleManager(ThreadLifeCycleManager&&) = delete;
    ThreadLifeCycleManager& operator=(ThreadLifeCycleManager&&) = delete;
    
    // Thread-safe append to log data
    void appendLog(const std::string& data) {
        std::unique_lock<std::mutex> autoLock(ThreadUtility::ThreadUtilityMutex);
        logData += data;
        ThreadUtility::threadGroupLogCache[threadId] = logData;
    }
    
    // Thread-safe get log data
    [[nodiscard]] std::string getLog() const {
        std::unique_lock<std::mutex> autoLock(ThreadUtility::ThreadUtilityMutex);
        return logData;
    }
    
    // Get current thread state
    [[nodiscard]] ThreadLifeCycleState getState() const {
        return state;
    }
    
    // Static methods for thread lifecycle monitoring
    [[nodiscard]] static std::map<std::thread::id, ThreadLifeCycleState> getAllThreadStates() {
        std::unique_lock<std::mutex> autoLock(ThreadUtility::ThreadUtilityMutex);
        return threadStates;
    }
    
    [[nodiscard]] static size_t getActiveThreadCount() {
        std::unique_lock<std::mutex> autoLock(ThreadUtility::ThreadUtilityMutex);
        return threadStates.size();
    }
    
    [[nodiscard]] static std::chrono::milliseconds getThreadRuntime(std::thread::id id) {
        std::unique_lock<std::mutex> autoLock(ThreadUtility::ThreadUtilityMutex);
        auto it = threadStartTimes.find(id);
        if (it != threadStartTimes.end()) {
            auto now = std::chrono::steady_clock::now();
            return std::chrono::duration_cast<std::chrono::milliseconds>(now - it->second);
        }
        return std::chrono::milliseconds{0};
    }
};

// Static member definitions for ThreadLifeCycleManager
std::map<std::thread::id, ThreadLifeCycleState> ThreadLifeCycleManager::threadStates;
std::map<std::thread::id, std::chrono::steady_clock::time_point> ThreadLifeCycleManager::threadStartTimes;

// Implementation of ThreadUtility::CleanupStaleEntries (moved here due to dependency on ThreadLifecycleManager)
size_t ThreadUtility::CleanupStaleEntries() {
    std::unique_lock<std::mutex> autoLock(ThreadUtilityMutex);
    
    // Get currently active thread IDs from lifecycle manager
    auto activeThreads = ThreadLifeCycleManager::getAllThreadStates();
    
    size_t cleanedCount = 0;
    
    // Clean up name cache for threads not in active tracking
    for (auto it = threadGroupNameCache.begin(); it != threadGroupNameCache.end();) {
        if (!activeThreads.contains(it->first)) {
            it = threadGroupNameCache.erase(it);
            ++cleanedCount;
        } else {
            ++it;
        }
    }
    
    // Clean up log cache for threads not in active tracking
    for (auto it = threadGroupLogCache.begin(); it != threadGroupLogCache.end();) {
        if (!activeThreads.contains(it->first)) {
            it = threadGroupLogCache.erase(it);
            ++cleanedCount;
        } else {
            ++it;
        }
    }
    
    if (cleanedCount > 0) {
        LOG_INFO(std::format("Cleaned up {} stale thread entries", cleanedCount));
    }
    
    return cleanedCount;
}



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

    [[nodiscard]] static int GetThreadGroupUUID() {
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
        // RAII-based lifecycle management - automatically tracks thread lifecycle
        ThreadLifeCycleManager lifecycleManager;
        lifecycleManager.appendLog(std::format("ThreadGroup::threadGroupPromiseMethod: (group id:{})", thisThreadGroupUUID));
        
        // Set value at thread exit - this will be called even if exception occurs
        promiseObj.set_value_at_thread_exit(lifecycleManager.getLog());
        // No manual cleanup needed - RAII handles it automatically
        
        // Call external driver method with exception handling
        if (this->externalDriverMethod) {
            this->externalDriverMethod();
        } else {
            // This is a programming error - null driver method
            const std::string errorMsg = std::format("External driver method is null for ThreadGroup {}", thisThreadGroupUUID);
            LOG_ERROR(errorMsg);
            // Don't throw from thread method - promise will be set at thread exit
        }
        
        LOG_INFO("ThreadGroup::threadGroupPromiseMethod: end");
    }
    catch (const std::exception& e) {
        // General exceptions should be logged with context
        const std::string errorMsg = std::format("Exception in threadGroupPromiseMethod (ID {}): {}", thisThreadGroupUUID, e.what());
        LOG_ERROR(errorMsg);
        // Promise will be set at thread exit regardless - don't re-throw from thread methods
    }
    catch (...) {
        // Unknown exceptions should be logged with context
        const std::string errorMsg = std::format("Unknown exception in threadGroupPromiseMethod (ID {})", thisThreadGroupUUID);
        LOG_ERROR(errorMsg);
        // Promise will be set at thread exit regardless - don't re-throw from thread methods
    }
}

void
ThreadGroup::threadGroupFutureMethod(std::future<std::string> futureObj) {
    try {
        ThreadUtility::AddThreadName();
        // RAII-based lifecycle management - automatically tracks thread lifecycle
        ThreadLifeCycleManager lifecycleManager;
        
        // Validate future before waiting - this is a programming error if invalid
        if (!futureObj.valid()) {
            const std::string errorMsg = std::format("Invalid future in threadGroupFutureMethod for ThreadGroup {}", thisThreadGroupUUID);
            LOG_ERROR(errorMsg);
            return; // Cannot continue without valid future, but don't throw from thread method
        }
        
        // Wait for promise to be fulfilled
        futureObj.wait();
        LOG_INFO("ThreadGroup::threadGroupFutureMethod: trigger (group id:", thisThreadGroupUUID, ")");
        
        // Get the result - this can throw if promise was set with exception
        std::string result = futureObj.get();
        LOG_INFO(result);
    }
    catch (const std::future_error& e) {
        // Future errors are expected when promises are broken or invalid
        const std::string errorMsg = std::format("Future error in threadGroupFutureMethod (ID {}): {}", thisThreadGroupUUID, e.what());
        LOG_ERROR(errorMsg);
        // Don't re-throw from thread methods - log and continue
    }
    catch (const std::exception& e) {
        // General exceptions should be logged with context
        const std::string errorMsg = std::format("Exception in threadGroupFutureMethod (ID {}): {}", thisThreadGroupUUID, e.what());
        LOG_ERROR(errorMsg);
        // Don't re-throw from thread methods - log and continue
    }
    catch (...) {
        // Unknown exceptions should be logged with context
        const std::string errorMsg = std::format("Unknown exception in threadGroupFutureMethod (ID {})", thisThreadGroupUUID);
        LOG_ERROR(errorMsg);
        // Don't re-throw from thread methods - log and continue
    }
}


void
ThreadGroup::Start() {
    try {
        // Ensure we haven't already started - this is a programming error
        if (!threadGroupThreadPair.empty()) {
            const std::string errorMsg = std::format("ThreadGroup {} already started", thisThreadGroupUUID);
            LOG_ERROR(errorMsg);
            throw std::logic_error(errorMsg);
        }

        // Get future before moving promise - this validates the promise
        threadGroupFuture = threadGroupPromise.get_future();
        
        // Validate future is valid before proceeding - this is a resource issue
        if (!threadGroupFuture.valid()) {
            const std::string errorMsg = std::format("Invalid future obtained from promise in ThreadGroup {}", thisThreadGroupUUID);
            LOG_ERROR(errorMsg);
            throw std::runtime_error(errorMsg);
        }

        // Create threads with moved objects - order is important
        threadGroupThreadPair.push_back(
            std::thread(&ThreadGroup::threadGroupFutureMethod, this, std::move(threadGroupFuture))
        );
        threadGroupThreadPair.push_back(
            std::thread(&ThreadGroup::threadGroupPromiseMethod, this, std::move(threadGroupPromise))
        );
    }
    catch (const std::logic_error&) {
        // Re-throw logic errors (programming errors) without modification
        throw;
    }
    catch (const std::runtime_error&) {
        // Re-throw runtime errors (resource issues) without modification  
        throw;
    }
    catch (const std::exception& e) {
        // Wrap unexpected exceptions with context
        const std::string errorMsg = std::format("Unexpected exception in ThreadGroup::Start (ID {}): {}", thisThreadGroupUUID, e.what());
        LOG_ERROR(errorMsg);
        
        // Clean up any created threads
        for (auto& thread : threadGroupThreadPair) {
            if (thread.joinable()) {
                thread.join();
            }
        }
        threadGroupThreadPair.clear();
        throw std::runtime_error(errorMsg);
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
    bool started = false;
    bool joined = false;
public:
    ThreadGroupContainer() { this->Reset(); };
    
    // Proper resource management - ensure all threads are joined before destruction
    ~ThreadGroupContainer() {
        try {
            // If threads were started but not joined, attempt to join them
            if (started && !joined) {
                LOG_WARNING("ThreadGroupContainer destructor: threads not properly joined, attempting cleanup");
                Join();
            }
        }
        catch (const std::exception& e) {
            LOG_ERROR("Exception during ThreadGroupContainer cleanup: ", e.what());
            // Continue with destruction even if join fails
        }
        catch (...) {
            LOG_ERROR("Unknown exception during ThreadGroupContainer cleanup");
            // Continue with destruction even if join fails
        }
    }
    
    void Add(void (*driverMethod)());
    void Start();
    void Join();
    void Reset() { 
        // Ensure threads are properly joined before clearing
        if (started && !joined) {
            try {
                Join();
            }
            catch (...) {
                LOG_ERROR("Failed to join threads during Reset()");
            }
        }
        threadGroups.clear(); 
        started = false;
        joined = false;
    }
};


void
ThreadGroupContainer::Add(void (*driverMethod)()) {
    LOG_INFO("ThreadGroupContainer::Add: thread group");
    threadGroups.push_back(std::make_unique<ThreadGroup>(driverMethod));
}


void
ThreadGroupContainer::Start() {
    if (started) {
        LOG_WARNING("ThreadGroupContainer::Start: already started");
        return;
    }
    
    LOG_INFO("ThreadGroupContainer::Start: starting thread groups");
    
    std::vector<std::unique_ptr<ThreadGroup>*> startedGroups;
    try {
        // Start all thread groups with exception safety
        for (auto& nextThreadGroup : threadGroups) {
            nextThreadGroup->Start();
            startedGroups.push_back(&nextThreadGroup);
        }
        started = true;
    }
    catch (const std::exception& e) {
        LOG_ERROR("Exception during ThreadGroupContainer::Start: ", e.what());
        
        // Clean up any thread groups that were started before the failure
        LOG_INFO("Attempting to join already started thread groups...");
        for (auto* startedGroup : startedGroups) {
            try {
                (*startedGroup)->Join();
            }
            catch (...) {
                // Suppress exceptions during cleanup
            }
        }
        throw; // Re-throw original exception
    }
}


void
ThreadGroupContainer::Join() {
    if (!started) {
        LOG_WARNING("ThreadGroupContainer::Join: threads not started yet");
        return;
    }
    
    if (joined) {
        LOG_WARNING("ThreadGroupContainer::Join: already joined");
        return;
    }
    
    LOG_INFO("ThreadGroupContainer::Join: joining thread groups");
    
    // Track any exceptions but continue trying to join all threads
    std::vector<std::exception_ptr> exceptions;
    
    for (auto& nextThreadGroup : threadGroups) {
        try {
            nextThreadGroup->Join();
        }
        catch (...) {
            // Store the exception but continue joining other thread groups
            exceptions.push_back(std::current_exception());
        }
    }
    
    joined = true;
    
    // If there were any exceptions during joining, rethrow the first one
    if (!exceptions.empty()) {
        LOG_ERROR("Exceptions occurred during thread joining");
        std::rethrow_exception(exceptions.front());
    }
}


void driverMethod() {
    static std::atomic<int> driverCnt{0};                   // thread-safe counter for multiple calls to this method
    int currentCount = ++driverCnt;
    std::string msg = std::format("in driver method {}", currentCount);

     // Test code to delay specific threads to demonstrate asynchronous behavior using configurable constants
    switch(currentCount) {
        case Config::DELAY_CASE_1:
            std::this_thread::sleep_for(std::chrono::seconds(Config::SLEEP_DURATION_MAX));
            break;
        case Config::DELAY_CASE_2:
            std::this_thread::sleep_for(std::chrono::seconds(Config::SLEEP_DURATION_MEDIUM));
            break;
        case Config::DELAY_CASE_3:
            std::this_thread::sleep_for(std::chrono::seconds(Config::SLEEP_DURATION_LONG));
            break;
        case Config::DELAY_CASE_4:
            std::this_thread::sleep_for(std::chrono::seconds(Config::SLEEP_DURATION_SHORT));
            break;
        case Config::DELAY_CASE_5:
            std::this_thread::sleep_for(std::chrono::seconds(Config::SLEEP_DURATION_VERY_LONG));
            break;
        case Config::DELAY_CASE_6:
            std::this_thread::sleep_for(std::chrono::seconds(Config::SLEEP_DURATION_EXTENDED));
            break;
        case Config::DELAY_CASE_7:
            std::this_thread::sleep_for(std::chrono::seconds(Config::SLEEP_DURATION_EXTENDED));
            break;
        default:
            // No delay for other thread numbers
            break;
    }

    LOG_INFO(msg);
}


int main() {
    try {
        ThreadUtility::AddThreadName();                         // will add main thread to our thread name list

        // test ThreadGroupContainer using configurable thread count
        ThreadGroupContainer threadGroupContainer;
        for (int thrdex = 0; thrdex < Config::DEFAULT_THREAD_COUNT; ++thrdex)
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
