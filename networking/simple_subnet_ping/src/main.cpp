// MIT License
// Copyright (c) 2025 dbjwhs

#include <iostream>
#include <chrono>
#include <iomanip>
#include <algorithm>
#include <boost/asio.hpp>

#include "../headers/subnet_scanner.hpp"
#include "../headers/ping_manager.hpp"
#include "../../../headers/project_utils.hpp"

using namespace NetworkScanner;

void print_banner() {
    std::cout << "\n═══════════════════════════════════════════════════════════════════════════════\n";
    std::cout << "                        Simple Subnet Ping Scanner v1.0                        \n";
    std::cout << "                               MIT License 2025                                \n";
    std::cout << "═══════════════════════════════════════════════════════════════════════════════\n\n";
}

void print_scan_results(const std::vector<DeviceInfo>& devices) {
    if (devices.empty()) {
        std::cout << "❌ No active devices found on the subnet.\n\n";
        return;
    }
    
    std::cout << "🎯 Found " << devices.size() << " active device(s):\n\n";
    
    // table header
    std::cout << std::left << std::setw(16) << "IP Address"
              << std::setw(25) << "Hostname" 
              << std::setw(12) << "Response"
              << "Status\n";
    std::cout << std::string(65, '-') << "\n";
    
    // sort devices by ip address for consistent output
    auto sorted_devices = devices;
    std::sort(sorted_devices.begin(), sorted_devices.end(), 
              [](const DeviceInfo& a, const DeviceInfo& b) {
                  return a.m_ip_address < b.m_ip_address;
              });
    
    for (const auto& device : sorted_devices) {
        std::cout << std::left << std::setw(16) << device.m_ip_address
                  << std::setw(25) << (device.m_hostname.empty() ? "unknown" : device.m_hostname)
                  << std::setw(12) << std::fixed << std::setprecision(2) 
                  << device.m_response_time_ms << "ms"
                  << (device.m_is_active ? "🟢 Active" : "🔴 Inactive") << "\n";
    }
    std::cout << "\n";
}

void print_error_message(ScanError error) {
    std::cout << "❌ Scan failed: ";
    switch (error) {
        case ScanError::network_interface_error:
            std::cout << "Network interface error - could not determine local IP address\n";
            std::cout << "   💡 Check your network connection and try again\n";
            break;
        case ScanError::invalid_subnet:
            std::cout << "Invalid subnet configuration\n";
            std::cout << "   💡 Ensure you have a valid IP address assigned\n";
            break;
        case ScanError::permission_denied:
            std::cout << "Permission denied - ICMP requires elevated privileges\n";
            std::cout << "   💡 Try running with: sudo ./simple_subnet_ping\n";
            break;
        case ScanError::timeout_exceeded:
            std::cout << "Network timeout exceeded\n";
            std::cout << "   💡 Network may be slow or unreachable\n";
            break;
        case ScanError::system_error:
            std::cout << "System error occurred\n";
            std::cout << "   💡 Check system logs for more details\n";
            break;
    }
    std::cout << "\n";
}

#define TESTING_MODE
#ifdef TESTING_MODE
int run_comprehensive_tests() {
    print_banner();
    std::cout << "🧪 Running comprehensive test suite...\n\n";
    
    // initialize boost asio context
    boost::asio::io_context io_context;
    
    // test 1: subnet scanner initialization
    std::cout << "Test 1: SubnetScanner initialization... ";
    try {
        SubnetScanner scanner{io_context};
        std::cout << "✅ PASSED\n";
    } catch (const std::exception& e) {
        std::cout << "❌ FAILED: " << e.what() << "\n";
        return 1;
    }
    
    // test 2: ping manager initialization
    std::cout << "Test 2: PingManager initialization... ";
    try {
        PingManager ping_manager{io_context};
        std::cout << "✅ PASSED\n";
    } catch (const std::exception& e) {
        std::cout << "❌ FAILED: " << e.what() << "\n";
        return 1;
    }
    
    // test 3: local ip detection
    std::cout << "Test 3: Local IP detection... ";
    try {
        SubnetScanner scanner{io_context};
        auto local_ip_result = scanner.get_local_ip();
        if (local_ip_result) {
            std::cout << "✅ PASSED (detected: " << local_ip_result.value() << ")\n";
        } else {
            std::cout << "⚠️  WARNING: Could not detect local IP\n";
        }
    } catch (const std::exception& e) {
        std::cout << "❌ FAILED: " << e.what() << "\n";
    }
    
    // test 4: subnet extraction
    std::cout << "Test 4: Subnet extraction... ";
    try {
        SubnetScanner scanner{io_context};
        auto subnet_result = scanner.extract_subnet("192.168.1.100");
        if (subnet_result && subnet_result.value() == "192.168.1") {
            std::cout << "✅ PASSED\n";
        } else {
            std::cout << "❌ FAILED: Expected '192.168.1', got '" 
                      << (subnet_result ? subnet_result.value() : "error") << "'\n";
        }
    } catch (const std::exception& e) {
        std::cout << "❌ FAILED: " << e.what() << "\n";
    }
    
    // test 5: ip generation
    std::cout << "Test 5: IP address generation... ";
    try {
        SubnetScanner scanner{io_context};
        auto ip_list_result = scanner.generate_subnet_ips("192.168.1");
        if (ip_list_result && ip_list_result.value().size() == 254) {
            std::cout << "✅ PASSED (generated 254 addresses)\n";
        } else {
            std::cout << "❌ FAILED: Expected 254 addresses, got " 
                      << (ip_list_result ? ip_list_result.value().size() : 0) << "\n";
        }
    } catch (const std::exception& e) {
        std::cout << "❌ FAILED: " << e.what() << "\n";
    }
    
    // test 6: loopback ping test
    std::cout << "Test 6: Loopback ping test... ";
    try {
        PingManager ping_manager{io_context};
        ping_manager.set_timeout(std::chrono::milliseconds{2000});
        
        auto ping_result = ping_manager.ping("127.0.0.1");
        if (ping_result && ping_result.value().m_success) {
            std::cout << "✅ PASSED (" << std::fixed << std::setprecision(2) 
                      << ping_result.value().m_response_time_ms << "ms)\n";
        } else {
            std::cout << "⚠️  WARNING: Loopback ping failed (may require permissions)\n";
        }
    } catch (const std::exception& e) {
        std::cout << "❌ FAILED: " << e.what() << "\n";
    }
    
    std::cout << "\n🎉 Test suite completed!\n";
    std::cout << "💡 Note: Some network tests may fail without proper permissions or network access\n\n";
    
    return 0;
}
#endif

int main(int argc, char* argv[]) {
#ifdef TESTING_MODE
    return run_comprehensive_tests();
#else
    
    print_banner();
    
    // initialize logging system
    try {
        Logger::getInstance("subnet_ping.log");
        LOG_INFO_PRINT("simple subnet ping scanner started");
    } catch (const std::exception& e) {
        std::cout << "⚠️  Warning: Could not initialize logging: " << e.what() << "\n";
        std::cout << "Continuing without file logging...\n\n";
    }
    
    // initialize boost asio io_context
    boost::asio::io_context io_context;
    
    try {
        // create subnet scanner instance
        SubnetScanner scanner{io_context};
        
        // set scanning parameters
        scanner.set_timeout(std::chrono::milliseconds{1000});
        scanner.set_max_concurrent(25); // reduce concurrent pings for better reliability
        
        std::cout << "🔍 Detecting local network configuration...\n";
        
        // get local ip address
        auto local_ip_result = scanner.get_local_ip();
        if (!local_ip_result) {
            print_error_message(local_ip_result.error());
            return static_cast<int>(local_ip_result.error());
        }
        
        const std::string local_ip = local_ip_result.value();
        std::cout << "✅ Local IP detected: " << local_ip << "\n";
        
        // extract subnet base
        auto subnet_result = scanner.extract_subnet(local_ip);
        if (!subnet_result) {
            print_error_message(subnet_result.error());
            return static_cast<int>(subnet_result.error());
        }
        
        const std::string subnet_base = subnet_result.value();
        std::cout << "📡 Scanning subnet: " << subnet_base << ".1-254\n";
        std::cout << "⏱️  This may take 30-60 seconds depending on network conditions...\n\n";
        
        // start timing the scan
        const auto scan_start = std::chrono::steady_clock::now();
        
        // perform subnet scan
        auto scan_result = scanner.scan_subnet(subnet_base);
        
        const auto scan_end = std::chrono::steady_clock::now();
        const auto scan_duration = std::chrono::duration_cast<std::chrono::milliseconds>(
            scan_end - scan_start);
        
        if (!scan_result) {
            print_error_message(scan_result.error());
            return static_cast<int>(scan_result.error());
        }
        
        // display results
        const auto& discovered_devices = scan_result.value();
        print_scan_results(discovered_devices);
        
        // print scan summary
        std::cout << "📊 Scan Summary:\n";
        std::cout << "   • Subnet: " << subnet_base << ".0/24\n";
        std::cout << "   • Active devices: " << discovered_devices.size() << "/254\n";
        std::cout << "   • Scan duration: " << scan_duration.count() << "ms\n";
        std::cout << "   • Log file: subnet_ping.log\n\n";
        
        LOG_INFO_PRINT("scan completed successfully - found {} devices in {}ms",
                      discovered_devices.size(), scan_duration.count());
        
    } catch (const std::exception& e) {
        std::cout << "💥 Unexpected error: " << e.what() << "\n\n";
        LOG_ERROR_PRINT("unexpected exception: {}", e.what());
        return 1;
    }
    
    std::cout << "✨ Scan completed successfully!\n";
    return 0;
    
#endif
}