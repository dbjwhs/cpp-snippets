// MIT License
// Copyright (c) 2025 dbjwhs

#include <bit>
#include <cstdint>
#include <cassert>
#include <vector>
#include <array>
#include <span>
#include <expected>
#include <string>
#include <type_traits>
#include "../../../headers/project_utils.hpp"

// Endianness and Memory Alignment Pattern History and Usage
//
// Endianness refers to the byte order used to represent multibyte data types in computer memory.
// The term originates from Jonathan Swift's "Gulliver's Travels" where the Lilliputians argued
// over which end of a boiled egg should be cracked first. In computing, this translates to
// whether the most significant byte (MSB) or least significant byte (LSB) is stored first.
//
// Little-endian systems store the LSB at the lowest memory address (x86, x86-64, ARM in most configs).
// Big-endian systems store the MSB at the lowest memory address (PowerPC, SPARC, network protocols).
// Mixed-endian (middle-endian) exists but is rare in modern systems.
//
// The pattern emerged from hardware design differences in the 1970s and 1980s. Intel chose
// little-endian for the 8080 processor, while Motorola chose big-endian for the 68000 series.
// Network protocols typically use big-endian (network byte order) regardless of host endianness.
//
// C++23 introduced std::byteswap to provide portable byte swapping without undefined behavior
// or compiler-specific intrinsics. This replaces platform-specific solutions like _byteswap_*
// on MSVC or __builtin_bswap* on GCC/Clang.
//
// Common usage patterns include:
// - Network protocol implementation (converting between host and network byte order)
// - File format parsing (many binary formats specify endianness)
// - Cross-platform data serialization
// - Cryptographic operations requiring specific byte arrangements
// - Embedded systems communication between different architectures

enum class EndianError {
    INVALID_SIZE,
    ALIGNMENT_ERROR,
    CONVERSION_FAILURE
};

// comprehensive endianness utility class for c++23
class EndianUtils {
private:
    // cached system endianness for performance
    static constexpr std::endian m_system_endian{std::endian::native};

public:
    // detect if the current system is little endian
    [[nodiscard]] static constexpr bool is_little_endian() noexcept {
        return m_system_endian == std::endian::little;
    }

    // detect if the current system is big endian
    [[nodiscard]] static constexpr bool is_big_endian() noexcept {
        return m_system_endian == std::endian::big;
    }

    // get string representation of system endianness
    [[nodiscard]] static constexpr const char* endian_string() noexcept {
        if constexpr (m_system_endian == std::endian::little) {
            return "Little Endian";
        } else if constexpr (m_system_endian == std::endian::big) {
            return "Big Endian";
        } else {
            return "Mixed Endian";
        }
    }

    // swap bytes for any integral type using c++23 std::byteswap
    template<std::integral T>
    [[nodiscard]] static constexpr T swap_bytes(T value) noexcept {
        return std::byteswap(value);
    }

    // convert from host to network byte order (big endian)
    template<std::integral T>
    [[nodiscard]] static constexpr T host_to_network(T value) noexcept {
        if constexpr (std::endian::native == std::endian::big) {
            return value;
        } else {
            return std::byteswap(value);
        }
    }

    // convert from network byte order (big endian) to host
    template<std::integral T>
    [[nodiscard]] static constexpr T network_to_host(T value) noexcept {
        return host_to_network(value); // symmetric operation
    }

    // convert from host to little endian
    template<std::integral T>
    [[nodiscard]] static constexpr T host_to_little(T value) noexcept {
        if constexpr (std::endian::native == std::endian::little) {
            return value;
        } else {
            return std::byteswap(value);
        }
    }

    // convert from little endian to host
    template<std::integral T>
    [[nodiscard]] static constexpr T little_to_host(T value) noexcept {
        return host_to_little(value); // symmetric operation
    }

    // safely read multibyte value from a byte array with specified endianness
    template<std::integral T>
    [[nodiscard]] static std::expected<T, EndianError>
    read_as_little_endian(std::span<const std::uint8_t> bytes) noexcept {
        if (bytes.size() != sizeof(T)) {
            return std::unexpected(EndianError::INVALID_SIZE);
        }

        T result{};
        // build value byte by byte in little endian order
        for (std::size_t ndx = 0; ndx < sizeof(T); ++ndx) {
            result |= static_cast<T>(bytes[ndx]) << (ndx * 8);
        }

        return result;
    }

    // safely read multibyte value from a byte array as big endian
    template<std::integral T>
    [[nodiscard]] static std::expected<T, EndianError>
    read_as_big_endian(std::span<const std::uint8_t> bytes) noexcept {
        if (bytes.size() != sizeof(T)) {
            return std::unexpected(EndianError::INVALID_SIZE);
        }

        T result{};
        // build value byte by byte in big endian order
        for (std::size_t ndx = 0; ndx < sizeof(T); ++ndx) {
            result |= static_cast<T>(bytes[ndx]) << ((sizeof(T) - 1 - ndx) * 8);
        }

        return result;
    }

    // write multibyte value to a byte array as little endian
    template<std::integral T>
    [[nodiscard]] static std::expected<void, EndianError>
    write_as_little_endian(T value, std::span<std::uint8_t> bytes) noexcept {
        if (bytes.size() != sizeof(T)) {
            return std::unexpected(EndianError::INVALID_SIZE);
        }

        // extract bytes in little endian order
        for (std::size_t ndx = 0; ndx < sizeof(T); ++ndx) {
            bytes[ndx] = static_cast<std::uint8_t>((value >> (ndx * 8)) & 0xFF);
        }

        return {};
    }

    // write multibyte value to a byte array as big endian
    template<std::integral T>
    [[nodiscard]] static std::expected<void, EndianError>
    write_as_big_endian(T value, std::span<std::uint8_t> bytes) noexcept {
        if (bytes.size() != sizeof(T)) {
            return std::unexpected(EndianError::INVALID_SIZE);
        }

        // extract bytes in big endian order
        for (std::size_t ndx = 0; ndx < sizeof(T); ++ndx) {
            bytes[ndx] = static_cast<std::uint8_t>((value >> ((sizeof(T) - 1 - ndx) * 8)) & 0xFF);
        }

        return {};
    }
};

// example network protocol header structure
struct NetworkPacketHeader {
    std::uint32_t m_packet_id{};
    std::uint16_t m_data_length{};
    std::uint16_t m_checksum{};

    // serialize to network byte order
    [[nodiscard]] std::array<std::uint8_t, 8> serialize() const noexcept {
        std::array<std::uint8_t, 8> buffer{};

        // convert to network byte order (big endian)
        const auto net_packet_id = EndianUtils::host_to_network(m_packet_id);
        const auto net_data_length = EndianUtils::host_to_network(m_data_length);
        const auto net_checksum = EndianUtils::host_to_network(m_checksum);

        // write to buffer
        auto result = EndianUtils::write_as_big_endian(net_packet_id,
            std::span<std::uint8_t>(buffer.data(), 4));
        assert(result.has_value());

        result = EndianUtils::write_as_big_endian(net_data_length,
            std::span<std::uint8_t>(buffer.data() + 4, 2));
        assert(result.has_value());

        result = EndianUtils::write_as_big_endian(net_checksum,
            std::span<std::uint8_t>(buffer.data() + 6, 2));
        assert(result.has_value());

        return buffer;
    }

    // deserialize from network byte order
    [[nodiscard]] static std::expected<NetworkPacketHeader, EndianError>
    deserialize(std::span<const std::uint8_t> buffer) noexcept {
        if (buffer.size() != 8) {
            return std::unexpected(EndianError::INVALID_SIZE);
        }

        NetworkPacketHeader header{};

        // read packet id
        auto packet_id_result = EndianUtils::read_as_big_endian<std::uint32_t>(
            std::span<const std::uint8_t>(buffer.data(), 4));
        if (!packet_id_result.has_value()) {
            return std::unexpected(packet_id_result.error());
        }
        header.m_packet_id = EndianUtils::network_to_host(packet_id_result.value());

        // read data length
        auto data_length_result = EndianUtils::read_as_big_endian<std::uint16_t>(
            std::span<const std::uint8_t>(buffer.data() + 4, 2));
        if (!data_length_result.has_value()) {
            return std::unexpected(data_length_result.error());
        }
        header.m_data_length = EndianUtils::network_to_host(data_length_result.value());

        // read checksum
        auto checksum_result = EndianUtils::read_as_big_endian<std::uint16_t>(
            std::span<const std::uint8_t>(buffer.data() + 6, 2));
        if (!checksum_result.has_value()) {
            return std::unexpected(checksum_result.error());
        }
        header.m_checksum = EndianUtils::network_to_host(checksum_result.value());

        return header;
    }
};

// comprehensive test suite for endianness operations
class EndianTestSuite {
public:
    // test basic system endianness detection
    static void test_endian_detection() {
        LOG_INFO_PRINT("=== Testing Endianness Detection ===");

        LOG_INFO_PRINT("System endianness: {}", EndianUtils::endian_string());
        LOG_INFO_PRINT("Is little endian: {}", EndianUtils::is_little_endian());
        LOG_INFO_PRINT("Is big endian: {}", EndianUtils::is_big_endian());

        // verify exactly one endianness is detected
        assert(EndianUtils::is_little_endian() || EndianUtils::is_big_endian());
        assert(!(EndianUtils::is_little_endian() && EndianUtils::is_big_endian()));

        LOG_INFO_PRINT("✓ Endianness detection tests passed");
    }

    // test std::byteswap functionality with various integer types
    static void test_byteswap_operations() {
        LOG_INFO_PRINT("=== Testing Byte Swap Operations ===");

        // test 16-bit swap
        constexpr std::uint16_t test16{0x1234};
        constexpr auto swapped16 = EndianUtils::swap_bytes(test16);
        assert(swapped16 == 0x3412);
        LOG_INFO_PRINT("16-bit swap: 0x{:04X} -> 0x{:04X}", test16, swapped16);

        // test 32-bit swap
        constexpr std::uint32_t test32{0x12345678};
        constexpr auto swapped32 = EndianUtils::swap_bytes(test32);
        assert(swapped32 == 0x78563412);
        LOG_INFO_PRINT("32-bit swap: 0x{:08X} -> 0x{:08X}", test32, swapped32);

        // test 64-bit swap
        constexpr std::uint64_t test64{0x123456789ABCDEF0};
        constexpr auto swapped64 = EndianUtils::swap_bytes(test64);
        assert(swapped64 == 0xF0DEBC9A78563412);
        LOG_INFO_PRINT("64-bit swap: 0x{:016X} -> 0x{:016X}", test64, swapped64);

        // test double swap returns original
        assert(EndianUtils::swap_bytes(swapped16) == test16);
        assert(EndianUtils::swap_bytes(swapped32) == test32);
        assert(EndianUtils::swap_bytes(swapped64) == test64);

        LOG_INFO_PRINT("✓ Byte swap operation tests passed");
    }

    // test host to network and network to host conversions
    static void test_network_conversions() {
        LOG_INFO_PRINT("=== Testing Network Byte Order Conversions ===");

        constexpr std::uint32_t host_value{0x12345678};
        constexpr auto network_value = EndianUtils::host_to_network(host_value);
        constexpr auto back_to_host = EndianUtils::network_to_host(network_value);

        // verify round-trip conversion
        assert(back_to_host == host_value);

        LOG_INFO_PRINT("Host value: 0x{:08X}", host_value);
        LOG_INFO_PRINT("Network value: 0x{:08X}", network_value);
        LOG_INFO_PRINT("Back to host: 0x{:08X}", back_to_host);

        // on little endian systems, network conversion should swap bytes
        if constexpr (EndianUtils::is_little_endian()) {
            assert(network_value == EndianUtils::swap_bytes(host_value));
        } else {
            assert(network_value == host_value);
        }

        LOG_INFO_PRINT("✓ Network byte order conversion tests passed");
    }

    // test little endian conversions
    static void test_little_endian_conversions() {
        LOG_INFO_PRINT("=== Testing Little Endian Conversions ===");

        constexpr std::uint32_t host_value{0x12345678};
        constexpr auto little_value = EndianUtils::host_to_little(host_value);
        constexpr auto back_to_host = EndianUtils::little_to_host(little_value);

        // verify round-trip conversion
        assert(back_to_host == host_value);

        LOG_INFO_PRINT("Host value: 0x{:08X}", host_value);
        LOG_INFO_PRINT("Little endian value: 0x{:08X}", little_value);
        LOG_INFO_PRINT("Back to host: 0x{:08X}", back_to_host);

        // on big endian systems, little endian conversion should swap bytes
        if constexpr (EndianUtils::is_big_endian()) {
            assert(little_value == EndianUtils::swap_bytes(host_value));
        } else {
            assert(little_value == host_value);
        }

        LOG_INFO_PRINT("✓ Little endian conversion tests passed");
    }

    // test byte array read/write operations
    static void test_byte_array_operations() {
        LOG_INFO_PRINT("=== Testing Byte Array Read/Write Operations ===");

        // test little endian read/write
        std::array<std::uint8_t, 4> little_buffer{0x78, 0x56, 0x34, 0x12};
        const auto little_read_result = EndianUtils::read_as_little_endian<std::uint32_t>(little_buffer);
        assert(little_read_result.has_value());
        assert(little_read_result.value() == 0x12345678);

        LOG_INFO_PRINT("Little endian read: [0x{:02X}, 0x{:02X}, 0x{:02X}, 0x{:02X}] -> 0x{:08X}",
            little_buffer[0], little_buffer[1], little_buffer[2], little_buffer[3],
            little_read_result.value());

        // test little endian write
        std::array<std::uint8_t, 4> write_buffer{};
        const auto write_result = EndianUtils::write_as_little_endian<std::uint32_t>(
            0x12345678, write_buffer);
        assert(write_result.has_value());
        assert(write_buffer == little_buffer);

        LOG_INFO_PRINT("Little endian write: 0x12345678 -> [0x{:02X}, 0x{:02X}, 0x{:02X}, 0x{:02X}]",
            write_buffer[0], write_buffer[1], write_buffer[2], write_buffer[3]);

        // test big endian read/write
        std::array<std::uint8_t, 4> big_buffer{0x12, 0x34, 0x56, 0x78};
        const auto big_read_result = EndianUtils::read_as_big_endian<std::uint32_t>(big_buffer);
        assert(big_read_result.has_value());
        assert(big_read_result.value() == 0x12345678);

        LOG_INFO_PRINT("Big endian read: [0x{:02X}, 0x{:02X}, 0x{:02X}, 0x{:02X}] -> 0x{:08X}",
            big_buffer[0], big_buffer[1], big_buffer[2], big_buffer[3],
            big_read_result.value());

        // test big endian write
        write_buffer.fill(0);
        const auto big_write_result = EndianUtils::write_as_big_endian<std::uint32_t>(
            0x12345678, write_buffer);
        assert(big_write_result.has_value());
        assert(write_buffer == big_buffer);

        LOG_INFO_PRINT("Big endian write: 0x12345678 -> [0x{:02X}, 0x{:02X}, 0x{:02X}, 0x{:02X}]",
            write_buffer[0], write_buffer[1], write_buffer[2], write_buffer[3]);

        LOG_INFO_PRINT("✓ Byte array operation tests passed");
    }

    // test error handling with invalid inputs
    static void test_error_handling() {
        LOG_INFO_PRINT("=== Testing Error Handling ===");

        // test invalid buffer size for read operations
        std::array<std::uint8_t, 3> invalid_buffer{0x12, 0x34, 0x56};
        const auto read_result = EndianUtils::read_as_little_endian<std::uint32_t>(invalid_buffer);
        assert(!read_result.has_value());
        assert(read_result.error() == EndianError::INVALID_SIZE);

        LOG_INFO_PRINT("✓ Invalid buffer size error handling works");

        // test invalid buffer size for write operations
        const auto write_result = EndianUtils::write_as_little_endian<std::uint32_t>(
            0x12345678, invalid_buffer);
        assert(!write_result.has_value());
        assert(write_result.error() == EndianError::INVALID_SIZE);

        LOG_INFO_PRINT("✓ Invalid write buffer size error handling works");
        LOG_INFO_PRINT("✓ Error handling tests passed");
    }

    // test network packet header serialization/deserialization
    static void test_network_packet_header() {
        LOG_INFO_PRINT("=== Testing Network Packet Header ===");

        // create test header
        NetworkPacketHeader original_header{};
        original_header.m_packet_id = 0x12345678;
        original_header.m_data_length = 0x1234;
        original_header.m_checksum = 0x5678;

        LOG_INFO_PRINT("Original header - ID: 0x{:08X}, Length: 0x{:04X}, Checksum: 0x{:04X}",
            original_header.m_packet_id, original_header.m_data_length, original_header.m_checksum);

        // serialize to network format
        const auto serialized = original_header.serialize();

        LOG_INFO_PRINT("Serialized bytes: [{:02X}, {:02X}, {:02X}, {:02X}, {:02X}, {:02X}, {:02X}, {:02X}]",
            serialized[0], serialized[1], serialized[2], serialized[3],
            serialized[4], serialized[5], serialized[6], serialized[7]);

        // deserialize back
        const auto deserialized_result = NetworkPacketHeader::deserialize(serialized);
        assert(deserialized_result.has_value());

        const auto&[m_packet_id, m_data_length, m_checksum] = deserialized_result.value();

        LOG_INFO_PRINT("Deserialized header - ID: 0x{:08X}, Length: 0x{:04X}, Checksum: 0x{:04X}",
            m_packet_id, m_data_length,
            m_checksum);

        // verify round-trip integrity
        assert(m_packet_id == original_header.m_packet_id);
        assert(m_data_length == original_header.m_data_length);
        assert(m_checksum == original_header.m_checksum);

        LOG_INFO_PRINT("✓ Network packet header serialization tests passed");
    }

    // comprehensive test runner
    static void run_all_tests() {
        LOG_INFO_PRINT("Starting comprehensive endianness test suite...\n");

        test_endian_detection();
        LOG_INFO_PRINT("");

        test_byteswap_operations();
        LOG_INFO_PRINT("");

        test_network_conversions();
        LOG_INFO_PRINT("");

        test_little_endian_conversions();
        LOG_INFO_PRINT("");

        test_byte_array_operations();
        LOG_INFO_PRINT("");

        test_error_handling();
        LOG_INFO_PRINT("");

        test_network_packet_header();
        LOG_INFO_PRINT("");

        LOG_INFO_PRINT("🎉 All endianness tests completed successfully!");
        LOG_INFO_PRINT("System is running on {} architecture", EndianUtils::endian_string());
    }
};

// demonstrate practical usage patterns
void demonstrate_usage_patterns() {
    LOG_INFO_PRINT("=== Practical Usage Pattern Demonstrations ===");

    // pattern 1: file format with mixed endianness requirements
    LOG_INFO_PRINT("--- Pattern 1: Binary File Format Handling ---");

    // simulate reading a file header that stores integers in big endian
    std::vector<std::uint8_t> file_data{};
    file_data.reserve(12);

    // file magic number (big endian)
    constexpr std::uint32_t magic_number{0x12345678};
    std::array<std::uint8_t, 4> magic_bytes{};
    const auto magic_result = EndianUtils::write_as_big_endian(magic_number, magic_bytes);
    assert(magic_result.has_value());

    for (const auto byte : magic_bytes) {
        file_data.emplace_back(byte);
    }

    // file version (little endian for some reason)
    constexpr std::uint16_t version{0x0102};
    std::array<std::uint8_t, 2> version_bytes{};
    const auto version_result = EndianUtils::write_as_little_endian(version, version_bytes);
    assert(version_result.has_value());

    for (const auto byte : version_bytes) {
        file_data.emplace_back(byte);
    }

    // record count (host endian)
    constexpr std::uint32_t record_count{1000};
    constexpr auto host_record_count = EndianUtils::host_to_little(record_count);
    std::array<std::uint8_t, 4> count_bytes{};
    const auto count_result = EndianUtils::write_as_little_endian(host_record_count, count_bytes);
    assert(count_result.has_value());

    for (const auto byte : count_bytes) {
        file_data.emplace_back(byte);
    }

    LOG_INFO_PRINT("Created binary file data with mixed endianness");
    LOG_INFO_PRINT("File size: {} bytes", file_data.size());

    // pattern 2: network protocol conversion
    LOG_INFO_PRINT("--- Pattern 2: Network Protocol Conversion ---");

    // simulate tcp/ip style protocol where all multibyte values are big endian
    const std::vector<std::uint32_t> host_values{0x12345678, 0x9ABCDEF0, 0x11223344, 0x55667788};
    std::vector<std::uint32_t> network_values{};
    network_values.reserve(host_values.size());

    // convert all values to network byte order
    for (const auto host_val : host_values) {
        network_values.emplace_back(EndianUtils::host_to_network(host_val));
    }

    LOG_INFO_PRINT("Host to network conversion:");
    for (std::size_t ndx = 0; ndx < host_values.size(); ++ndx) {
        LOG_INFO_PRINT("  0x{:08X} -> 0x{:08X}", host_values[ndx], network_values[ndx]);
    }

    // pattern 3: cross-platform data serialization
    LOG_INFO_PRINT("--- Pattern 3: Cross-Platform Data Structure ---");

    // simulate a data structure that needs consistent byte layout across platforms
    struct SerializableData {
        std::uint64_t m_timestamp{};
        std::uint32_t m_user_id{};
        std::uint16_t m_flags{};

        // serialize to little endian for consistent cross-platform format
        [[nodiscard]] std::array<std::uint8_t, 14> serialize() const noexcept {
            std::array<std::uint8_t, 14> buffer{};

            const auto le_timestamp = EndianUtils::host_to_little(m_timestamp);
            const auto le_user_id = EndianUtils::host_to_little(m_user_id);
            const auto le_flags = EndianUtils::host_to_little(m_flags);

            auto result = EndianUtils::write_as_little_endian(le_timestamp,
                std::span<std::uint8_t>(buffer.data(), 8));
            assert(result.has_value());

            result = EndianUtils::write_as_little_endian(le_user_id,
                std::span<std::uint8_t>(buffer.data() + 8, 4));
            assert(result.has_value());

            result = EndianUtils::write_as_little_endian(le_flags,
                std::span<std::uint8_t>(buffer.data() + 12, 2));
            assert(result.has_value());

            return buffer;
        }
    };

    SerializableData data{};
    data.m_timestamp = 0x123456789ABCDEF0;
    data.m_user_id = 0x12345678;
    data.m_flags = 0x1234;

    const auto serialized_data = data.serialize();

    LOG_INFO_PRINT("Serialized cross-platform data structure:");
    LOG_INFO_PRINT("Timestamp: 0x{:016X}", data.m_timestamp);
    LOG_INFO_PRINT("User ID: 0x{:08X}", data.m_user_id);
    LOG_INFO_PRINT("Flags: 0x{:04X}", data.m_flags);
    LOG_INFO_PRINT("Serialized bytes: [");
    for (std::size_t ndx = 0; ndx < serialized_data.size(); ++ndx) {
        LOG_INFO_PRINT("  {:02X}", serialized_data[ndx]);
        if (ndx < serialized_data.size() - 1) {
            LOG_INFO_PRINT(",");
        }
    }
    LOG_INFO_PRINT("]");

    LOG_INFO_PRINT("✓ Usage pattern demonstrations completed");
}

int main() {
    LOG_INFO_PRINT("C++23 Endianness and std::byteswap Deep Dive");
    LOG_INFO_PRINT("=============================================\n");

    // run comprehensive test suite
    EndianTestSuite::run_all_tests();

    LOG_INFO_PRINT("\n");

    // demonstrate practical usage patterns
    demonstrate_usage_patterns();

    LOG_INFO_PRINT("\n🏁 Program completed successfully!");

    return 0;
}
