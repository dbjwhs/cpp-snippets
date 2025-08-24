// MIT License
// Copyright (c) 2025 dbjwhs

#ifndef THREAD_AFFINITY_HPP
#define THREAD_AFFINITY_HPP

//
// Thread Affinity Cross-Platform Abstraction
//
// Thread affinity allows binding threads to specific CPU cores to optimize cache locality,
// reduce context switching overhead, and achieve more predictable performance characteristics.
// This pattern emerged from high-performance computing and real-time systems where precise
// control over thread placement is crucial for achieving deterministic timing.
//
// The concept became mainstream with multi-core processors in the mid-2000s. Modern applications
// use thread affinity for:
// - Scientific computing: NUMA-aware thread placement
// - Real-time systems: Isolating critical threads from OS scheduler interference
// - Benchmarking: Eliminating measurement variability from thread migration
// - Gaming: Dedicating cores to specific subsystems (audio, rendering, simulation)
//
// Platform Support:
// - Linux: Full support via pthread_setaffinity_np() and cpu_set_t
// - Windows: Full support via SetThreadAffinityMask()
// - macOS: Limited/restricted - Apple prioritizes automatic scheduling optimization
//
// On macOS, this implementation provides:
// - Thread priority adjustment as a partial substitute
// - Quality of Service (QoS) classes for system-aware scheduling hints
// - Graceful degradation with clear diagnostics
// - Cache-conscious thread placement simulation for educational purposes
//

#include <thread>
#include <vector>
#include <optional>
#include <expected>
#include <string>
#include <chrono>
#include <atomic>

// Platform detection
#ifdef __linux__
    #define THREAD_AFFINITY_LINUX 1
    #include <pthread.h>
    #include <sched.h>
    #include <unistd.h>
#elif defined(_WIN32) || defined(_WIN64)
    #define THREAD_AFFINITY_WINDOWS 1
    #include <windows.h>
    #include <processthreadsapi.h>
#elif defined(__APPLE__) && defined(__MACH__)
    #define THREAD_AFFINITY_MACOS 1
    #include <pthread.h>
    #include <sys/types.h>
    #include <sys/sysctl.h>
    #include <mach/thread_policy.h>
    #include <mach/thread_act.h>
    #include <dispatch/dispatch.h>
#else
    #define THREAD_AFFINITY_UNSUPPORTED 1
#endif

namespace thread_affinity {

enum class AffinityResult {
    Success,
    UnsupportedPlatform,
    InvalidCoreId,
    SystemError,
    PermissionDenied,
    FeatureRestricted
};

struct AffinityInfo {
    std::vector<int> available_cores;
    int logical_core_count;
    int physical_core_count;
    bool supports_affinity;
    std::string platform_info;
    std::string limitation_reason;
};

class ThreadAffinityManager {
public:
    static std::expected<AffinityInfo, std::string> getSystemInfo();
    
    static std::expected<void, std::string> setThreadAffinity(
        std::thread::native_handle_type thread_handle, 
        int core_id
    );
    
    static std::expected<void, std::string> setCurrentThreadAffinity(int core_id);
    
    static std::expected<std::vector<int>, std::string> getCurrentThreadAffinity(
        std::thread::native_handle_type thread_handle
    );
    
    static std::expected<void, std::string> setThreadPriority(
        std::thread::native_handle_type thread_handle,
        int priority
    );

private:
    static std::expected<AffinityInfo, std::string> getLinuxSystemInfo();
    static std::expected<AffinityInfo, std::string> getWindowsSystemInfo();
    static std::expected<AffinityInfo, std::string> getMacOSSystemInfo();
    
    static std::expected<void, std::string> setLinuxThreadAffinity(
        std::thread::native_handle_type thread_handle, 
        int core_id
    );
    
    static std::expected<void, std::string> setWindowsThreadAffinity(
        std::thread::native_handle_type thread_handle, 
        int core_id
    );
    
    static std::expected<void, std::string> setMacOSThreadHints(
        std::thread::native_handle_type thread_handle, 
        int core_preference
    );
};

class BenchmarkHelper {
public:
    struct CacheTestResult {
        std::chrono::nanoseconds with_affinity;
        std::chrono::nanoseconds without_affinity;
        double cache_hit_ratio_with_affinity;
        double cache_hit_ratio_without_affinity;
        bool affinity_actually_set;
        std::string notes;
    };
    
    static CacheTestResult runCacheLocalityTest(int iterations = 10000);
    static void demonstrateAffinityBenefits();
    
private:
    static std::chrono::nanoseconds runMemoryIntensiveTask(size_t data_size);
    static double estimateCacheHitRatio(std::chrono::nanoseconds timing, size_t data_size);
};

class QualityOfServiceManager {
public:
    enum class QoSClass {
        UserInteractive,    // Highest priority - UI responsiveness
        UserInitiated,      // User-initiated work with visible progress
        Utility,           // Background work with user awareness
        Background         // Lowest priority - maintenance work
    };
    
    static std::expected<void, std::string> setThreadQoS(
        std::thread::native_handle_type thread_handle,
        QoSClass qos_class
    );
    
    static std::string getQoSDescription(QoSClass qos_class);

private:
    static std::expected<void, std::string> setMacOSQoS(
        std::thread::native_handle_type thread_handle,
        QoSClass qos_class
    );
};

class ThreadPlacementSimulator {
public:
    struct CPUTopology {
        int physical_cores;
        int logical_cores;
        bool has_hyperthreading;
        std::vector<std::vector<int>> core_groups; // NUMA nodes or core clusters
    };
    
    static CPUTopology detectCPUTopology();
    static std::vector<int> recommendCoreAssignment(int thread_count, const CPUTopology& topology);
    static void visualizeCoreUsage(const std::vector<int>& core_assignments, const CPUTopology& topology);
    
private:
    static bool isAppleSilicon();
    static CPUTopology getAppleSiliconTopology();
    static CPUTopology getIntelTopology();
};

// RAII helper for thread affinity management
class AffinityGuard {
public:
    explicit AffinityGuard(int target_core);
    explicit AffinityGuard(std::thread& thread, int target_core);
    ~AffinityGuard();
    
    bool was_successful() const { return success_; }
    const std::string& get_error() const { return error_message_; }
    
private:
    std::thread::native_handle_type thread_handle_;
    std::vector<int> original_affinity_;
    bool success_;
    std::string error_message_;
    bool restore_on_destructor_;
};

// Educational demonstration classes
class AffinityDemoScenarios {
public:
    static void demonstrateProducerConsumerAffinity();
    static void demonstrateNUMAAwareProcessing();
    static void demonstrateBenchmarkConsistency();
    static void demonstrateRealTimeAudioSimulation();
    
private:
    static void runProducerConsumerWithAffinity(bool use_affinity);
    static void runNUMAWorkload(const std::vector<int>& core_assignments);
    static void runConsistencyBenchmark(bool use_affinity, int iterations);
    static void runAudioThreadSimulation(bool use_dedicated_core);
};

// Platform-specific utilities
namespace platform_utils {
    std::string getPlatformName();
    std::string getCPUInfo();
    bool isRunningInVirtualMachine();
    std::optional<std::string> getSchedulerInfo();
}

} // namespace thread_affinity

#endif // THREAD_AFFINITY_HPP
