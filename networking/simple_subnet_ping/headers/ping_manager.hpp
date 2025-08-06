// MIT License
// Copyright (c) 2025 dbjwhs

#pragma once

#include <string>
#include <expected>
#include <chrono>
#include <boost/asio.hpp>

//
// ping manager class handles icmp ping operations for network connectivity testing
// this pattern originated from the classic unix ping utility created by mike muuss in 1983
// for testing network reachability. the implementation uses raw sockets to send icmp echo
// requests and measure round-trip time, essential for network diagnostics and monitoring.
// modern implementations must handle platform-specific socket permissions and provide
// cross-platform compatibility through boost.asio's icmp resolver and socket abstractions.
//

namespace NetworkScanner {
    
    // forward declaration
    enum class ScanError;
    
    // ping result structure
    struct PingResult {
        std::string m_target_ip{};
        bool m_success{false};
        double m_response_time_ms{0.0};
        std::string m_error_message{};
    };
    
    class PingManager {
    public:
        // constructor with io_context reference
        explicit PingManager(boost::asio::io_context& io_context);
        
        // destructor
        ~PingManager() = default;
        
        // copy operations deleted
        PingManager(const PingManager&) = delete;
        PingManager& operator=(const PingManager&) = delete;
        
        // move operations deleted due to reference member
        PingManager(PingManager&&) = delete;
        PingManager& operator=(PingManager&&) = delete;
        
        // perform synchronous ping to target ip
        [[nodiscard]] std::expected<PingResult, ScanError> ping(const std::string& target_ip);
        
        // perform asynchronous ping with callback
        void async_ping(const std::string& target_ip, 
                       std::function<void(std::expected<PingResult, ScanError>)> callback);
        
        // set ping timeout (default 1000ms)
        void set_timeout(std::chrono::milliseconds timeout);
        
        // set number of ping attempts (default 1)
        void set_retry_count(std::size_t retry_count);
        
    private:
        // reference to boost asio io context
        boost::asio::io_context& m_io_context;
        
        // icmp socket for sending ping requests
        std::unique_ptr<boost::asio::ip::icmp::socket> m_socket;
        
        // timeout for ping operations
        std::chrono::milliseconds m_timeout{1000};
        
        // number of retry attempts
        std::size_t m_retry_count{1};
        
        // sequence number for icmp packets
        std::uint16_t m_sequence_number{0};
        
        // helper method to create icmp echo request packet
        [[nodiscard]] std::vector<std::uint8_t> create_icmp_packet();
        
        // helper method to parse icmp reply packet
        [[nodiscard]] std::expected<double, ScanError> parse_icmp_reply(
            const std::vector<std::uint8_t>& reply_data,
            std::chrono::steady_clock::time_point send_time) const;
        
        // helper method to calculate icmp checksum
        [[nodiscard]] std::uint16_t calculate_checksum(const std::vector<std::uint8_t>& data) const;
        
        // helper method to initialize socket
        [[nodiscard]] std::expected<void, ScanError> initialize_socket();
    };
    
} // namespace NetworkScanner