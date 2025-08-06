// MIT License
// Copyright (c) 2025 dbjwhs

#include "../headers/ping_manager.hpp"
#include "../headers/subnet_scanner.hpp"
#include "../../../headers/project_utils.hpp"
#include <boost/asio/ip/icmp.hpp>
#include <boost/asio/steady_timer.hpp>
#include <chrono>
#include <random>

namespace NetworkScanner {
    
    PingManager::PingManager(boost::asio::io_context& io_context)
        : m_io_context{io_context}, m_sequence_number{1} {
        
        // attempt to initialize icmp socket
        if (const auto init_result = initialize_socket(); !init_result) {
            LOG_ERROR_PRINT("failed to initialize ping manager socket");
        } else {
            LOG_INFO_PRINT("ping manager initialized successfully");
        }
    }
    
    std::expected<PingResult, ScanError> PingManager::ping(const std::string& target_ip) {
        if (!m_socket) {
            if (auto init_result = initialize_socket(); !init_result) {
                LOG_ERROR_PRINT("cannot ping - socket initialization failed");
                return std::unexpected{init_result.error()};
            }
        }
        
        PingResult result{};
        result.m_target_ip = target_ip;
        
        try {
            // resolve target address
            boost::asio::ip::icmp::resolver resolver{m_io_context};
            auto endpoints = resolver.resolve(boost::asio::ip::icmp::v4(), target_ip, "");
            
            if (endpoints.empty()) {
                result.m_error_message = "could not resolve target address";
                LOG_WARNING_PRINT("failed to resolve target: {}", target_ip);
                return result;
            }
            
            const auto destination = *endpoints.begin();
            
            // perform ping attempts with retry logic
            for (std::size_t attempt = 0; attempt < m_retry_count; ++attempt) {
                // create icmp echo request packet
                auto packet_data = create_icmp_packet();
                
                // record send time
                const auto send_time = std::chrono::steady_clock::now();
                
                // send icmp packet
                const std::size_t bytes_sent = m_socket->send_to(
                    boost::asio::buffer(packet_data), destination);
                
                if (bytes_sent != packet_data.size()) {
                    LOG_WARNING_PRINT("partial icmp packet sent to {}", target_ip);
                    continue;
                }
                
                // prepare buffer for reply
                std::vector<std::uint8_t> reply_buffer(65536);
                boost::asio::ip::icmp::endpoint reply_endpoint;
                
                // set up timeout for receive operation
                boost::asio::steady_timer timer{m_io_context};
                timer.expires_after(m_timeout);
                
                bool reply_received = false;
                std::size_t reply_bytes = 0;
                
                // async receive with timeout
                m_socket->async_receive_from(
                    boost::asio::buffer(reply_buffer),
                    reply_endpoint,
                    [&](const boost::system::error_code& ec, std::size_t bytes_received) {
                        if (!ec) {
                            reply_received = true;
                            reply_bytes = bytes_received;
                        }
                    });
                
                timer.async_wait([&](const boost::system::error_code& ec) {
                    if (!ec) {
                        // timeout occurred
                        m_socket->cancel();
                    }
                });
                
                // run io operations until reply received or timeout
                m_io_context.restart();
                m_io_context.run();
                
                if (reply_received && reply_bytes > 0) {
                    // resize buffer to actual received data
                    reply_buffer.resize(reply_bytes);
                    
                    // parse reply to get response time
                    if (auto response_time_result = parse_icmp_reply(reply_buffer, send_time)) {
                        result.m_success = true;
                        result.m_response_time_ms = response_time_result.value();
                        LOG_INFO_PRINT("ping successful to {} - {:.2f}ms", 
                                      target_ip, result.m_response_time_ms);
                        return result;
                    }
                }
                
                LOG_WARNING_PRINT("ping attempt {} to {} failed or timed out", 
                                 attempt + 1, target_ip);
            }
            
            // all attempts failed
            result.m_error_message = "all ping attempts failed or timed out";
            LOG_WARNING_PRINT("all ping attempts to {} failed", target_ip);
            return result;
            
        } catch (const std::exception& e) {
            result.m_error_message = e.what();
            LOG_ERROR_PRINT("exception during ping to {}: {}", target_ip, e.what());
            return result;
        }
    }
    
    void PingManager::async_ping(const std::string& target_ip,
                                std::function<void(std::expected<PingResult, ScanError>)> callback) {
        
        // launch ping operation asynchronously
        std::thread([this, target_ip, callback = std::move(callback)]() {
            auto result = ping(target_ip);
            callback(std::move(result));
        }).detach();
    }
    
    void PingManager::set_timeout(std::chrono::milliseconds timeout) {
        m_timeout = timeout;
        LOG_INFO_PRINT("ping timeout set to {}ms", timeout.count());
    }
    
    void PingManager::set_retry_count(std::size_t retry_count) {
        m_retry_count = retry_count;
        LOG_INFO_PRINT("ping retry count set to {}", retry_count);
    }
    
    std::vector<std::uint8_t> PingManager::create_icmp_packet() {
        // icmp echo request packet structure
        constexpr std::size_t icmp_header_size = 8;
        constexpr std::size_t payload_size = 32;
        constexpr std::size_t total_size = icmp_header_size + payload_size;
        
        std::vector<std::uint8_t> packet(total_size, 0);
        
        // icmp type (8 = echo request)
        packet[0] = 8;
        
        // icmp code (0 for echo request)
        packet[1] = 0;
        
        // checksum (calculated later)
        packet[2] = 0;
        packet[3] = 0;
        
        // identifier (process id)
        const std::uint16_t identifier = static_cast<std::uint16_t>(::getpid());
        packet[4] = static_cast<std::uint8_t>(identifier >> 8);
        packet[5] = static_cast<std::uint8_t>(identifier & 0xff);
        
        // sequence number
        packet[6] = static_cast<std::uint8_t>(m_sequence_number >> 8);
        packet[7] = static_cast<std::uint8_t>(m_sequence_number & 0xff);
        ++m_sequence_number;
        
        // payload data (timestamp for rtt calculation)
        const auto now = std::chrono::steady_clock::now();
        const auto timestamp = now.time_since_epoch().count();
        
        std::memcpy(&packet[icmp_header_size], &timestamp, sizeof(timestamp));
        
        // fill the remaining payload with pattern
        for (std::size_t ndx = icmp_header_size + sizeof(timestamp); ndx < total_size; ++ndx) {
            packet[ndx] = static_cast<std::uint8_t>(ndx & 0xff);
        }
        
        // calculate and set checksum
        const std::uint16_t checksum = calculate_checksum(packet);
        packet[2] = static_cast<std::uint8_t>(checksum >> 8);
        packet[3] = static_cast<std::uint8_t>(checksum & 0xff);
        
        return packet;
    }
    
    std::expected<double, ScanError> PingManager::parse_icmp_reply(
        const std::vector<std::uint8_t>& reply_data,
        const std::chrono::steady_clock::time_point send_time) const {
        
        // minimum size check (ip header + icmp header)
        if (reply_data.size() < 28) {
            LOG_WARNING_PRINT("icmp reply too short: {} bytes", reply_data.size());
            return std::unexpected{ScanError::system_error};
        }
        
        // skip ip header (typically 20 bytes, but check ihl field)
        const std::uint8_t ip_header_length = (reply_data[0] & 0x0f) * 4;
        
        if (reply_data.size() < ip_header_length + 8) {
            LOG_WARNING_PRINT("invalid ip header length in icmp reply");
            return std::unexpected{ScanError::system_error};
        }
        
        // extract icmp header
        const std::uint8_t icmp_type = reply_data[ip_header_length];
        const std::uint8_t icmp_code = reply_data[ip_header_length + 1];
        
        // check for echo reply (type 0, code 0)
        if (icmp_type == 0 && icmp_code == 0) {
            // calculate response time
            const auto receive_time = std::chrono::steady_clock::now();
            const auto duration = receive_time - send_time;
            const double response_time_ms = 
                std::chrono::duration<double, std::milli>(duration).count();
            
            return response_time_ms;
        }
        
        // handle error responses
        if (icmp_type == 3) { // destination unreachable
            LOG_WARNING_PRINT("destination unreachable (code: {})", icmp_code);
            return std::unexpected{ScanError::system_error};
        }
        
        if (icmp_type == 11) { // time exceeded
            LOG_WARNING_PRINT("time exceeded (code: {})", icmp_code);
            return std::unexpected{ScanError::timeout_exceeded};
        }
        
        LOG_WARNING_PRINT("unexpected icmp type: {}, code: {}", icmp_type, icmp_code);
        return std::unexpected{ScanError::system_error};
    }
    
    std::uint16_t PingManager::calculate_checksum(const std::vector<std::uint8_t>& data) const {
        std::uint32_t sum = 0;
        
        // sum all 16-bit words
        for (std::size_t ndx = 0; ndx < data.size() - 1; ndx += 2) {
            const std::uint16_t word = (static_cast<std::uint16_t>(data[ndx]) << 8) + data[ndx + 1];
            sum += word;
        }
        
        // handle odd length
        if (data.size() % 2 == 1) {
            sum += static_cast<std::uint16_t>(data[data.size() - 1]) << 8;
        }
        
        // fold 32-bit sum to 16 bits
        while (sum >> 16) {
            sum = (sum & 0xffff) + (sum >> 16);
        }
        
        // return one's complement
        return static_cast<std::uint16_t>(~sum);
    }
    
    std::expected<void, ScanError> PingManager::initialize_socket() {
        try {
            m_socket = std::make_unique<boost::asio::ip::icmp::socket>(
                m_io_context, boost::asio::ip::icmp::v4());
            
            LOG_INFO_PRINT("icmp socket initialized successfully");
            return {};
            
        } catch (const boost::system::system_error& e) {
            LOG_ERROR_PRINT("failed to create icmp socket: {}", e.what());
            
            // check for permission issues
            if (e.code() == boost::asio::error::access_denied) {
                return std::unexpected{ScanError::permission_denied};
            }
            
            return std::unexpected{ScanError::system_error};
        } catch (const std::exception& e) {
            LOG_ERROR_PRINT("exception initializing socket: {}", e.what());
            return std::unexpected{ScanError::system_error};
        }
    }
    
} // namespace NetworkScanner