// MIT License
// Copyright (c) 2025 dbjwhs

#include "../headers/crtp.hpp"
#include "../../../../headers/project_utils.hpp"
#include <cassert>
#include <string>

using namespace crtp_pattern;

void test_basic_roundtrip();
void test_value_with_delimiters();
void test_empty_values();
void test_no_delimiter();
void test_special_characters();
void test_long_values();

int main() {
    LOG_INFO_PRINT("=== Serialization (Static Interface Enforcement) Test Suite ===");

    try {
        test_basic_roundtrip();
        test_value_with_delimiters();
        test_empty_values();
        test_no_delimiter();
        test_special_characters();
        test_long_values();

        LOG_INFO_PRINT("=== All serialization tests passed ===");
        return 0;

    } catch (const std::exception& e) {
        LOG_ERROR_PRINT("Test failed with exception: {}", e.what());
        return 1;
    }
}

void test_basic_roundtrip() {
    LOG_INFO_PRINT("\n--- Basic Round-Trip ---");

    const Config cfg("host", "localhost");
    const std::string serialized = cfg.to_string();
    assert(serialized == "host=localhost");
    LOG_INFO_PRINT("  Serialize: passed ('{}')", serialized);

    const auto restored = Config::from_string(serialized);
    assert(restored.key() == "host");
    assert(restored.value() == "localhost");
    LOG_INFO_PRINT("  Deserialize: passed");

    // verify accessors
    const Config cfg2("port", "5432");
    assert(cfg2.key() == "port");
    assert(cfg2.value() == "5432");
    assert(cfg2.to_string() == "port=5432");
    LOG_INFO_PRINT("  Accessors and serialize: passed");
}

void test_value_with_delimiters() {
    LOG_INFO_PRINT("\n--- Value Containing '=' ---");

    // only the first '=' is the delimiter
    const Config cfg("url", "http://host:8080/path?a=1&b=2");
    const std::string s = cfg.to_string();
    assert(s == "url=http://host:8080/path?a=1&b=2");
    LOG_INFO_PRINT("  Serialize with '=' in value: passed");

    const auto r = Config::from_string(s);
    assert(r.key() == "url");
    assert(r.value() == "http://host:8080/path?a=1&b=2");
    LOG_INFO_PRINT("  Deserialize preserves value: passed");

    // multiple '=' signs
    const Config cfg2("expr", "a=b=c=d");
    const auto r2 = Config::from_string(cfg2.to_string());
    assert(r2.key() == "expr");
    assert(r2.value() == "a=b=c=d");
    LOG_INFO_PRINT("  Multiple '=' in value: passed");
}

void test_empty_values() {
    LOG_INFO_PRINT("\n--- Empty Values ---");

    // empty value
    const Config cfg("empty_key", "");
    assert(cfg.to_string() == "empty_key=");
    const auto r = Config::from_string(cfg.to_string());
    assert(r.key() == "empty_key");
    assert(r.value().empty());
    LOG_INFO_PRINT("  Empty value: passed");

    // empty key
    const Config cfg2("", "some_value");
    assert(cfg2.to_string() == "=some_value");
    const auto r2 = Config::from_string(cfg2.to_string());
    assert(r2.key().empty());
    assert(r2.value() == "some_value");
    LOG_INFO_PRINT("  Empty key: passed");

    // both empty
    const Config cfg3("", "");
    assert(cfg3.to_string() == "=");
    const auto r3 = Config::from_string(cfg3.to_string());
    assert(r3.key().empty());
    assert(r3.value().empty());
    LOG_INFO_PRINT("  Both empty: passed");
}

void test_no_delimiter() {
    LOG_INFO_PRINT("\n--- No Delimiter ---");

    const auto r = Config::from_string("no_equals_here");
    assert(r.key().empty());
    assert(r.value() == "no_equals_here");
    LOG_INFO_PRINT("  No '=' present: passed");

    const auto r2 = Config::from_string("");
    assert(r2.key().empty());
    assert(r2.value().empty());
    LOG_INFO_PRINT("  Empty string: passed");
}

void test_special_characters() {
    LOG_INFO_PRINT("\n--- Special Characters ---");

    const Config cfg("path", "/usr/local/bin/program");
    const auto r = Config::from_string(cfg.to_string());
    assert(r.value() == "/usr/local/bin/program");
    LOG_INFO_PRINT("  File path: passed");

    const Config cfg2("json", R"({"key": "value"})");
    const auto r2 = Config::from_string(cfg2.to_string());
    assert(r2.value() == R"({"key": "value"})");
    LOG_INFO_PRINT("  JSON value: passed");

    const Config cfg3("spaces", "hello world foo bar");
    const auto r3 = Config::from_string(cfg3.to_string());
    assert(r3.value() == "hello world foo bar");
    LOG_INFO_PRINT("  Spaces in value: passed");

    const Config cfg4("tab", "col1\tcol2\tcol3");
    const auto r4 = Config::from_string(cfg4.to_string());
    assert(r4.value() == "col1\tcol2\tcol3");
    LOG_INFO_PRINT("  Tabs in value: passed");
}

void test_long_values() {
    LOG_INFO_PRINT("\n--- Long Values ---");

    // build a long string
    std::string long_value;
    for (int i = 0; i < 1000; ++i) {
        long_value += std::format("segment_{}_", i);
    }

    const Config cfg("long_key", long_value);
    const auto r = Config::from_string(cfg.to_string());
    assert(r.key() == "long_key");
    assert(r.value() == long_value);
    LOG_INFO_PRINT("  Long value ({} chars): passed", long_value.size());
}
