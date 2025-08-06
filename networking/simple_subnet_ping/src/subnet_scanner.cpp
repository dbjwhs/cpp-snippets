// MIT License
// Copyright (c) 2025 dbjwhs

#include "../headers/subnet_scanner.hpp"
#include "../headers/ping_manager.hpp"
#include "../../../headers/project_utils.hpp"
#include <regex>
#include <sstream>
#include <future>
#include <algorithm>

namespace NetworkScanner {
    
    SubnetScanner::SubnetScanner(boost::asio::io_context& io_context)
        : m_io_context{io_context} {
        LOG_INFO_PRINT("subnet scanner initialized with io_context");
    }
    
    std::expected<std::string, ScanError> SubnetScanner::get_local_ip() const {
        try {
            // create resolver to find local interfaces
            boost::asio::ip::tcp::resolver resolver{m_io_context};
            
            // get local hostname
            const std::string hostname = boost::asio::ip::host_name();
            LOG_INFO_PRINT("detected hostname: {}", hostname);
            
            // resolve hostname to get ip addresses
            // to iterate through resolved addresses to find ipv4
            for (const auto endpoints = resolver.resolve(hostname, ""); const auto& endpoint : endpoints) {
                // check if it's ipv4 and not loopback
                if (const auto address = endpoint.endpoint().address(); address.is_v4() && !address.is_loopback()) {
                    const std::string ip_str = address.to_string();
                    
                    // skip common non-routable addresses ### dbj little bit of a hack but hey...
                    if (ip_str.starts_with("169.254.") || ip_str.starts_with("127.")) {
                        continue;
                    }
                    
                    LOG_INFO_PRINT("found local ip address: {}", ip_str);
                    return ip_str;
                }
            }
            
            LOG_ERROR_PRINT("no valid ipv4 address found");
            return std::unexpected{ScanError::network_interface_error};
            
        } catch (const std::exception& e) {
            LOG_ERROR_PRINT("exception in get_local_ip: {}", e.what());
            return std::unexpected{ScanError::system_error};
        }
    }
    
    std::expected<std::string, ScanError> SubnetScanner::extract_subnet(const std::string& ip_address) const {
        // validate ip address format
        if (!is_valid_ip(ip_address)) {
            LOG_ERROR_PRINT("invalid ip address format: {}", ip_address);
            return std::unexpected{ScanError::invalid_subnet};
        }
        
        // find last dot to extract subnet base (assumes /24 network)
        const auto last_dot = ip_address.find_last_of('.');
        if (last_dot == std::string::npos) {
            LOG_ERROR_PRINT("malformed ip address: {}", ip_address);
            return std::unexpected{ScanError::invalid_subnet};
        }
        
        const std::string subnet_base = ip_address.substr(0, last_dot);
        LOG_INFO_PRINT("extracted subnet base: {}", subnet_base);
        return subnet_base;
    }
    
    std::expected<std::vector<std::string>, ScanError> SubnetScanner::generate_subnet_ips(const std::string& subnet_base) const {
        // validate a subnet base format (should be three octets)
        if (const std::regex subnet_regex{R"(^\d{1,3}\.\d{1,3}\.\d{1,3}$)"}; !std::regex_match(subnet_base, subnet_regex)) {
            LOG_ERROR_PRINT("invalid subnet base format: {}", subnet_base);
            return std::unexpected{ScanError::invalid_subnet};
        }
        
        std::vector<std::string> ip_addresses{};
        
        // reserve space for all possible addresses (1-254, excluding 0 and 255)
        ip_addresses.reserve(254);
        
        // generate all possible host addresses in subnet
        for (int host = 1; host <= 254; ++host) {
            std::ostringstream ip_stream;
            ip_stream << subnet_base << "." << host;
            ip_addresses.emplace_back(ip_stream.str());
        }
        
        LOG_INFO_PRINT("generated {} ip addresses for subnet {}", ip_addresses.size(), subnet_base);
        return ip_addresses;
    }
    
    std::expected<std::vector<DeviceInfo>, ScanError> SubnetScanner::scan_subnet(const std::string& subnet_base) const {
        // generate a list of ip addresses to scan
        auto ip_list_result = generate_subnet_ips(subnet_base);
        if (!ip_list_result) {
            return std::unexpected{ip_list_result.error()};
        }
        
        const auto& ip_addresses = ip_list_result.value();
        std::vector<DeviceInfo> discovered_devices{};
        
        // reserve space for potential discoveries
        discovered_devices.reserve(ip_addresses.size());
        
        LOG_INFO_PRINT("starting subnet scan for {} addresses", ip_addresses.size());
        
        // create ping manager for this scan
        PingManager ping_manager{m_io_context};
        ping_manager.set_timeout(m_timeout);
        
        // use futures for concurrent ping operations
        std::vector<std::future<std::expected<PingResult, ScanError>>> ping_futures{};
        ping_futures.reserve(std::min(m_max_concurrent, ip_addresses.size()));
        
        std::size_t completed_pings{0};
        
        // process addresses in batches to limit concurrency
        for (std::size_t batch_start = 0; batch_start < ip_addresses.size(); batch_start += m_max_concurrent) {
            const std::size_t batch_end = std::min(batch_start + m_max_concurrent, ip_addresses.size());
            ping_futures.clear();
            
            // launch ping operations for current batch
            for (std::size_t ndx = batch_start; ndx < batch_end; ++ndx) {
                const std::string& target_ip = ip_addresses[ndx];
                
                ping_futures.emplace_back(std::async(std::launch::async, [&ping_manager, target_ip]() {
                    return ping_manager.ping(target_ip);
                }));
            }
            
            // collect results from current batch
            for (std::size_t ndx = 0; ndx < ping_futures.size(); ++ndx) {
                auto result = ping_futures[ndx].get();
                completed_pings++;
                
                if (result && result.value().m_success) {
                    const auto& ping_result = result.value();
                    
                    DeviceInfo device{};
                    device.m_ip_address = ping_result.m_target_ip;
                    device.m_is_active = true;
                    device.m_response_time_ms = ping_result.m_response_time_ms;
                    
                    // attempt to resolve hostname
                    if (auto hostname_result = resolve_hostname(device.m_ip_address)) {
                        device.m_hostname = hostname_result.value();
                    } else {
                        device.m_hostname = "unknown";
                    }
                    
                    discovered_devices.emplace_back(std::move(device));
                    LOG_INFO_PRINT("discovered active device: {} ({}) - {:.2f}ms", 
                                  device.m_ip_address, device.m_hostname, device.m_response_time_ms);
                }
                
                // log progress every 50 completions
                if (completed_pings % 50 == 0) {
                    LOG_INFO_PRINT("ping progress: {}/{} completed", completed_pings, ip_addresses.size());
                }
            }
        }
        
        LOG_INFO_PRINT("subnet scan completed. found {} active devices out of {} addresses", 
                      discovered_devices.size(), ip_addresses.size());
        
        return discovered_devices;
    }
    
    void SubnetScanner::set_timeout(std::chrono::milliseconds timeout) {
        m_timeout = timeout;
        LOG_INFO_PRINT("ping timeout set to {}ms", timeout.count());
    }
    
    void SubnetScanner::set_max_concurrent(std::size_t max_concurrent) {
        m_max_concurrent = max_concurrent;
        LOG_INFO_PRINT("maximum concurrent pings set to {}", max_concurrent);
    }
    
    bool SubnetScanner::is_valid_ip(const std::string& ip) const {
        // basic ipv4 format validation using regex
        const std::regex ip_regex{R"(^(?:[0-9]{1,3}\.){3}[0-9]{1,3}$)"};
        
        if (!std::regex_match(ip, ip_regex)) {
            return false;
        }
        
        // validate each octet is in range 0-255
        std::istringstream ip_stream{ip};
        std::string octet;
        
        while (std::getline(ip_stream, octet, '.')) {
            const int value = std::stoi(octet);
            if (value < 0 || value > 255) {
                return false;
            }
        }
        
        return true;
    }
    
    std::expected<std::string, ScanError> SubnetScanner::resolve_hostname(const std::string& ip_address) const {
        try {
            // create resolver for hostname lookup
            boost::asio::ip::tcp::resolver resolver{m_io_context};
            
            // convert ip string to an address object
            const boost::asio::ip::address addr = boost::asio::ip::make_address(ip_address);
            
            // create endpoint from address
            const boost::asio::ip::tcp::endpoint endpoint{addr, 0};
            
            // perform reverse dns lookup
            if (const auto results = resolver.resolve(endpoint); !results.empty()) {
                return results.begin()->host_name();
            }
            
            return std::unexpected{ScanError::system_error};
            
        } catch (const std::exception& e) {
            LOG_WARNING_PRINT("hostname resolution failed for {}: {}", ip_address, e.what());
            return std::unexpected{ScanError::system_error};
        }
    }
    
} // namespace NetworkScanner