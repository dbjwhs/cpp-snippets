// MIT License
// Copyright (c) 2025 dbjwhs

#pragma once

#include <string>
#include <vector>
#include <expected>
#include <boost/asio.hpp>

//
// subnet scanner class provides functionality to discover active devices on a local network subnet
// this pattern emerged from the need to map network topology and discover devices without relying
// on external network scanning tools like nmap. the approach uses icmp ping to test connectivity
// to each possible ip address within a subnet range, commonly used in network administration,
// iot device discovery, and security auditing. historical implementations date back to early
// unix network utilities, with modern c++ implementations leveraging boost.asio for
// cross-platform asynchronous networking operations.
//

namespace NetworkScanner {
    
    // error types for railway-oriented programming
    enum class ScanError {
        network_interface_error,
        invalid_subnet,
        permission_denied,
        timeout_exceeded,
        system_error
    };
    
    // result structure for discovered devices
    struct DeviceInfo {
        std::string m_ip_address{};
        std::string m_hostname{};
        double m_response_time_ms{};
        bool m_is_active{false};
    };
    
    class SubnetScanner {
    public:
        // constructor with io_context reference for async operations
        explicit SubnetScanner(boost::asio::io_context& io_context);
        
        // destructor
        ~SubnetScanner() = default;
        
        // copy operations deleted to prevent resource conflicts
        SubnetScanner(const SubnetScanner&) = delete;
        SubnetScanner& operator=(const SubnetScanner&) = delete;
        
        // move operations deleted due to reference member
        SubnetScanner(SubnetScanner&&) = delete;
        SubnetScanner& operator=(SubnetScanner&&) = delete;
        
        // get the local machine's ip address on primary interface
        [[nodiscard]] std::expected<std::string, ScanError> get_local_ip() const;
        
        // extract subnet from ip address (assumes /24 network)
        [[nodiscard]] std::expected<std::string, ScanError> extract_subnet(const std::string& ip_address) const;
        
        // generate all possible ip addresses in subnet range
        [[nodiscard]] std::expected<std::vector<std::string>, ScanError> generate_subnet_ips(const std::string& subnet_base) const;
        
        // scan all devices in the given subnet
        [[nodiscard]] std::expected<std::vector<DeviceInfo>, ScanError> scan_subnet(const std::string& subnet_base);
        
        // set timeout for ping operations (default 1000ms)
        void set_timeout(std::chrono::milliseconds timeout);
        
        // set maximum concurrent ping operations (default 50)
        void set_max_concurrent(std::size_t max_concurrent);
        
    private:
        // reference to boost asio io context for async operations
        boost::asio::io_context& m_io_context;
        
        // timeout for individual ping operations
        std::chrono::milliseconds m_timeout{1000};
        
        // maximum number of concurrent ping operations
        std::size_t m_max_concurrent{50};
        
        // helper method to validate ip address format
        [[nodiscard]] bool is_valid_ip(const std::string& ip) const;
        
        // helper method to resolve hostname from ip address
        [[nodiscard]] std::expected<std::string, ScanError> resolve_hostname(const std::string& ip_address) const;
    };
    
} // namespace NetworkScanner