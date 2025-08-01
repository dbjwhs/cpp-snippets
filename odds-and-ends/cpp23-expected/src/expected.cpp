// MIT License
// Copyright (c) 2025 dbjwhs

#include <expected>
#include <string>
#include <vector>
#include <iostream>
#include <cassert>
#include <format>
#include <optional>
#include <fstream>
#include <cmath>
#include "../../../headers/project_utils.hpp"

// std::expected Pattern Overview and History
//
// The std::expected template class was introduced in C++23 as a standardized way to handle
// operations that may fail without using exceptions. This pattern originated from functional
// programming languages like Haskell's Either type and Rust's Result type. The concept was
// heavily discussed in the C++ standardization committee and was inspired by the boost::outcome
// library and various third-party implementations.
//
// std::expected<T, E> represents a value that is either a successful result of type T or an
// error of type E. This provides a more explicit and composable way to handle errors compared
// to traditional C++ error handling mechanisms like exceptions, error codes, or optional types
// with separate error reporting.
//
// The pattern encourages railway-oriented programming where operations can be chained together,
// and errors automatically propagate through the chain without explicit checking at each step.
// This leads to more robust and maintainable code by making error handling explicit in the
// type system while reducing boilerplate code for error propagation.
//
// Common usage patterns include file I/O operations, network operations, parsing operations,
// mathematical computations that may fail, and any operation where failure is a normal part
// of the business logic rather than an exceptional circumstance.

// custom error types for demonstration
enum class FileError {
    NOT_FOUND,
    PERMISSION_DENIED,
    CORRUPTED,
    TOO_LARGE
};

enum class ParseError {
    INVALID_FORMAT,
    OUT_OF_RANGE,
    EMPTY_INPUT,
    INVALID_CHARACTER
};

// helper class to demonstrate std::expected with member variables
class DataProcessor {
private:
    // current file path being processed
    std::string m_current_file_path{};

    // maximum allowed file size in bytes
    size_t m_max_file_size{1024 * 1024}; // 1MB default

    // processed data cache
    std::vector<int> m_processed_data{};

public:
    // constructor with braced initialization
    explicit DataProcessor(std::string file_path, size_t max_size = 1024 * 1024)
        : m_current_file_path{std::move(file_path)}, m_max_file_size{max_size} {
        LOG_INFO_PRINT("initializing data processor for file: {}", m_current_file_path);
    }

    // simulated file reading operation that may fail
    [[nodiscard]] std::expected<std::string, FileError> read_file() const {
        LOG_INFO_PRINT("attempting to read file: {}", m_current_file_path);

        // simulate various failure conditions
        if (m_current_file_path.empty()) {
            LOG_ERROR("file path is empty");
            return std::unexpected{FileError::NOT_FOUND};
        }

        if (m_current_file_path.find(".txt") == std::string::npos) {
            LOG_ERROR_PRINT("file {} does not have .txt extension", m_current_file_path);
            return std::unexpected{FileError::PERMISSION_DENIED};
        }

        if (m_current_file_path.find("large") != std::string::npos) {
            LOG_ERROR_PRINT("file {} is too large", m_current_file_path);
            return std::unexpected{FileError::TOO_LARGE};
        }

        if (m_current_file_path.find("corrupt") != std::string::npos) {
            LOG_ERROR_PRINT("file {} appears to be corrupted", m_current_file_path);
            return std::unexpected{FileError::CORRUPTED};
        }

        // simulate successful file reading
        std::string content = "123,456,789,42,100";
        LOG_INFO_PRINT("successfully read file content: {}", content);
        return content;
    }

    // parsing operation that may fail
    std::expected<std::vector<int>, ParseError> parse_integers(const std::string& data) {
        LOG_INFO_PRINT("parsing integer data from string: {}", data);

        if (data.empty()) {
            LOG_ERROR("cannot parse empty string");
            return std::unexpected{ParseError::EMPTY_INPUT};
        }

        std::vector<int> result{};
        // reserve space if we can estimate size
        result.reserve(10);

        std::string current_number{};

        for (char c : data) {
            if (c == ',') {
                if (current_number.empty()) {
                    LOG_ERROR("found empty number in data");
                    return std::unexpected{ParseError::INVALID_FORMAT};
                }

                try {
                    int value = std::stoi(current_number);
                    result.emplace_back(value);
                    current_number.clear();
                } catch (const std::invalid_argument&) {
                    LOG_ERROR_PRINT("invalid number format: {}", current_number);
                    return std::unexpected{ParseError::INVALID_FORMAT};
                } catch (const std::out_of_range&) {
                    LOG_ERROR_PRINT("number out of range: {}", current_number);
                    return std::unexpected{ParseError::OUT_OF_RANGE};
                }
            } else if (std::isdigit(c) || c == '-') {
                current_number += c;
            } else {
                LOG_ERROR_PRINT("invalid character found: {}", c);
                return std::unexpected{ParseError::INVALID_CHARACTER};
            }
        }

        // handle the last number
        if (!current_number.empty()) {
            try {
                int value = std::stoi(current_number);
                result.emplace_back(value);
            } catch (const std::invalid_argument&) {
                LOG_ERROR_PRINT("invalid number format: {}", current_number);
                return std::unexpected{ParseError::INVALID_FORMAT};
            } catch (const std::out_of_range&) {
                LOG_ERROR_PRINT("number out of range: {}", current_number);
                return std::unexpected{ParseError::OUT_OF_RANGE};
            }
        }

        LOG_INFO_PRINT("successfully parsed {} integers", result.size());
        return result;
    }

    // chained operation demonstrating monadic operations
    std::expected<int, std::string> process_file_and_sum() {
        LOG_INFO("starting chained file processing operation");

        // first convert file result to a common error type, then chain operations
        auto file_result = read_file();
        if (!file_result) {
            std::string error_msg = format_file_error(file_result.error());
            LOG_ERROR_PRINT("file operation failed: {}", error_msg);
            return std::unexpected{error_msg};
        }

        // now we can chain operations with consistent error types
        auto parse_result = parse_integers(file_result.value());
        if (!parse_result) {
            std::string error_msg = format_parse_error(parse_result.error());
            LOG_ERROR_PRINT("parse operation failed: {}", error_msg);
            return std::unexpected{error_msg};
        }

        const auto& numbers = parse_result.value();
        if (numbers.empty()) {
            return std::unexpected{"no numbers to sum"};
        }

        int sum{0};
        for (int num : numbers) {
            sum += num;
        }

        LOG_INFO_PRINT("calculated sum: {}", sum);
        return sum;
    }

    // demonstration of proper monadic chaining with uniform error types
    std::expected<int, std::string> process_file_monadic() {
        LOG_INFO("starting monadic file processing operation");

        // convert file operation to a uniform error type first
        auto file_to_string = [this]() -> std::expected<std::string, std::string> {
            auto result = read_file();
            if (!result) {
                return std::unexpected{format_file_error(result.error())};
            }
            return result.value();
        };

        // convert parse operation to a uniform error type
        auto parse_to_string = [this](const std::string& content) -> std::expected<std::vector<int>, std::string> {
            auto result = parse_integers(content);
            if (!result) {
                return std::unexpected{format_parse_error(result.error())};
            }
            return result.value();
        };

        // now we can chain operations properly
        return file_to_string()
            .and_then(parse_to_string)
            .and_then([](const std::vector<int>& numbers) -> std::expected<int, std::string> {
                if (numbers.empty()) {
                    return std::unexpected{"no numbers to sum"};
                }

                int sum{0};
                for (int num : numbers) {
                    sum += num;
                }

                LOG_INFO_PRINT("monadic chain calculated sum: {}", sum);
                return sum;
            });
    }

    // getter for processed data
    [[nodiscard]] const std::vector<int>& get_processed_data() const {
        return m_processed_data;
    }

private:
    // helper function to format file errors
    static std::string format_file_error(FileError error) {
        switch (error) {
            case FileError::NOT_FOUND:
                return "file not found";
            case FileError::PERMISSION_DENIED:
                return "permission denied";
            case FileError::CORRUPTED:
                return "file corrupted";
            case FileError::TOO_LARGE:
                return "file too large";
            default:
                return "unknown file error";
        }
    }

    // helper function to format parse errors
    static std::string format_parse_error(ParseError error) {
        switch (error) {
            case ParseError::INVALID_FORMAT:
                return "invalid format";
            case ParseError::OUT_OF_RANGE:
                return "value out of range";
            case ParseError::EMPTY_INPUT:
                return "empty input";
            case ParseError::INVALID_CHARACTER:
                return "invalid character";
            default:
                return "unknown parse error";
        }
    }
};

// demonstration function for basic std::expected usage
std::expected<double, std::string> safe_divide(double numerator, double denominator) {
    LOG_INFO_PRINT("attempting division: {} / {}", numerator, denominator);

    if (denominator == 0.0) {
        LOG_ERROR("division by zero attempted");
        return std::unexpected{"division by zero"};
    }

    double result = numerator / denominator;
    LOG_INFO_PRINT("division result: {}", result);
    return result;
}

// demonstration function for transforming expected values
std::expected<int, std::string> safe_sqrt_and_round(double value) {
    LOG_INFO_PRINT("calculating square root and rounding for value: {}", value);

    if (value < 0) {
        LOG_ERROR("cannot calculate square root of negative number");
        return std::unexpected{"negative input for square root"};
    }

    double sqrt_result = std::sqrt(value);
    int rounded_result = static_cast<int>(std::round(sqrt_result));

    LOG_INFO_PRINT("square root and round result: {}", rounded_result);
    return rounded_result;
}

// comprehensive testing function
void run_comprehensive_tests() {
    LOG_INFO("starting comprehensive std::expected tests");

    // test 1: successful operations
    LOG_INFO("test 1: successful file processing");
    {
        DataProcessor processor{"valid_data.txt"};
        auto result = processor.process_file_and_sum();

        assert(result.has_value());
        assert(result.value() == 1510); // 123+456+789+42+100
        LOG_INFO("test 1 passed: successful processing");
    }

    // test 1b: successful monadic operations
    LOG_INFO("test 1b: successful monadic file processing");
    {
        DataProcessor processor{"valid_data.txt"};
        auto result = processor.process_file_monadic();

        assert(result.has_value());
        assert(result.value() == 1510); // 123+456+789+42+100
        LOG_INFO("test 1b passed: successful monadic processing");
    }

    // test 2: file not found error
    LOG_INFO("test 2: file not found error");
    {
        Logger::StderrSuppressionGuard stderr_guard;
        DataProcessor processor{""};
        auto result = processor.process_file_and_sum();

        assert(!result.has_value());
        assert(result.error() == "file not found");
        LOG_INFO("test 2 passed: file not found handled correctly");
    }

    // test 3: permission denied error
    LOG_INFO("test 3: permission denied error");
    {
        Logger::StderrSuppressionGuard stderr_guard;
        DataProcessor processor{"data.pdf"};
        auto result = processor.process_file_and_sum();

        assert(!result.has_value());
        assert(result.error() == "permission denied");
        LOG_INFO("test 3 passed: permission denied handled correctly");
    }

    // test 4: file too large error
    LOG_INFO("test 4: file too large error");
    {
        Logger::StderrSuppressionGuard stderr_guard;
        DataProcessor processor{"large_data.txt"};
        auto result = processor.process_file_and_sum();

        assert(!result.has_value());
        assert(result.error() == "file too large");
        LOG_INFO("test 4 passed: file too large handled correctly");
    }

    // test 5: corrupted file error
    LOG_INFO("test 5: corrupted file error");
    {
        Logger::StderrSuppressionGuard stderr_guard;
        DataProcessor processor{"corrupt_data.txt"};
        auto result = processor.process_file_and_sum();

        assert(!result.has_value());
        assert(result.error() == "file corrupted");
        LOG_INFO("test 5 passed: corrupted file handled correctly");
    }

    // test 6: successful division
    LOG_INFO("test 6: successful division");
    {
        auto result = safe_divide(10.0, 2.0);
        assert(result.has_value());
        assert(result.value() == 5.0);
        LOG_INFO("test 6 passed: successful division");
    }

    // test 7: division by zero
    LOG_INFO("test 7: division by zero");
    {
        Logger::StderrSuppressionGuard stderr_guard;
        auto result = safe_divide(10.0, 0.0);
        assert(!result.has_value());
        assert(result.error() == "division by zero");
        LOG_INFO("test 7 passed: division by zero handled correctly");
    }

    // test 8: successful sqrt and round
    LOG_INFO("test 8: successful square root and round");
    {
        auto result = safe_sqrt_and_round(16.0);
        assert(result.has_value());
        assert(result.value() == 4);
        LOG_INFO("test 8 passed: successful sqrt and round");
    }

    // test 9: negative sqrt error
    LOG_INFO("test 9: negative square root error");
    {
        Logger::StderrSuppressionGuard stderr_guard;
        auto result = safe_sqrt_and_round(-4.0);
        assert(!result.has_value());
        assert(result.error() == "negative input for square root");
        LOG_INFO("test 9 passed: negative sqrt handled correctly");
    }

    // test 10: chaining operations with transform
    LOG_INFO("test 10: chaining operations with transform");
    {
        // create wrapper functions with consistent error types
        auto safe_divide_string = [](double a, double b) -> std::expected<double, std::string> {
            auto result = safe_divide(a, b);
            return result ? std::expected<double, std::string>{result.value()}
                         : std::unexpected{result.error()};
        };

        auto safe_sqrt_string = [](double val) -> std::expected<int, std::string> {
            auto result = safe_sqrt_and_round(val);
            return result ? std::expected<int, std::string>{result.value()}
                         : std::unexpected{result.error()};
        };

        auto result = safe_divide_string(100.0, 4.0)
            .and_then(safe_sqrt_string);

        assert(result.has_value());
        assert(result.value() == 5); // sqrt(25) = 5
        LOG_INFO("test 10 passed: successful operation chaining");
    }

    // test 11: chaining operations with error propagation
    LOG_INFO("test 11: chaining with error propagation");
    {
        Logger::StderrSuppressionGuard stderr_guard;
        // create wrapper functions with consistent error types
        auto safe_divide_string = [](double a, double b) -> std::expected<double, std::string> {
            auto result = safe_divide(a, b);
            return result ? std::expected<double, std::string>{result.value()}
                         : std::unexpected{result.error()};
        };

        auto safe_sqrt_string = [](double val) -> std::expected<int, std::string> {
            auto result = safe_sqrt_and_round(val);
            return result ? std::expected<int, std::string>{result.value()}
                         : std::unexpected{result.error()};
        };

        auto result = safe_divide_string(100.0, 0.0)
            .and_then(safe_sqrt_string);

        assert(!result.has_value());
        assert(result.error() == "division by zero");
        LOG_INFO("test 11 passed: error propagation in chain");
    }

    // test 12: using value_or for default values
    LOG_INFO("test 12: value_or default handling");
    {
        Logger::StderrSuppressionGuard stderr_guard;
        auto success_result = safe_divide(10.0, 2.0);
        auto failure_result = safe_divide(10.0, 0.0);

        double successful_value = success_result.value_or(-1.0);
        double failed_value = failure_result.value_or(-1.0);

        assert(successful_value == 5.0);
        assert(failed_value == -1.0);
        LOG_INFO("test 12 passed: value_or defaults work correctly");
    }

    LOG_INFO("all comprehensive tests passed successfully");
}

// demonstration of different std::expected usage patterns
void demonstrate_usage_patterns() {
    LOG_INFO("demonstrating various std::expected usage patterns");

    // pattern 1: basic success/failure checking
    LOG_INFO("pattern 1: basic success/failure checking");
    {
        if (auto result = safe_divide(20.0, 4.0)) {
            LOG_INFO_PRINT("division successful: {}", *result);
        } else {
            LOG_ERROR_PRINT("division failed: {}", result.error());
        }
    }

    // pattern 2: value extraction with has_value()
    LOG_INFO("pattern 2: explicit has_value() checking");
    {
        auto result = safe_divide(15.0, 3.0);
        if (result.has_value()) {
            LOG_INFO_PRINT("extracted value: {}", result.value());
        }
    }

    // pattern 3: error handling with error()
    LOG_INFO("pattern 3: error extraction");
    {
        Logger::StderrSuppressionGuard stderr_guard;
        auto result = safe_divide(10.0, 0.0);
        if (!result.has_value()) {
            LOG_WARNING_PRINT("operation failed with error: {}", result.error());
        }
    }

    // pattern 4: monadic operations with and_then
    LOG_INFO("pattern 4: monadic chaining with and_then");
    {
        // create wrapper functions with consistent error types for proper chaining
        auto safe_divide_string = [](const double a, const double b) -> std::expected<double, std::string> {
            auto result = safe_divide(a, b);
            if (!result) {
                return std::unexpected{result.error()};
            }
            return result.value();
        };

        auto safe_sqrt_string = [](double val) -> std::expected<int, std::string> {
            auto result = safe_sqrt_and_round(val);
            if (!result) {
                return std::unexpected{result.error()};
            }
            return result.value();
        };

        auto final_result = safe_divide_string(144.0, 4.0)
            .and_then(safe_sqrt_string);

        if (final_result) {
            LOG_INFO_PRINT("chained operation result: {}", *final_result);
        }
    }

    // pattern 5: error recovery with or_else
    LOG_INFO("pattern 5: error recovery with or_else");
    {
        Logger::StderrSuppressionGuard stderr_guard;
        auto result = safe_divide(10.0, 0.0)
            .or_else([](const std::string& error) -> std::expected<double, std::string> {
                LOG_WARNING_PRINT("recovering from error: {}", error);
                return 0.0; // provide default value
            });

        LOG_INFO_PRINT("recovered value: {}", result.value());
    }

    // pattern 6: transforming values with transform
    LOG_INFO("pattern 6: value transformation");
    {
        auto result = safe_divide(10.0, 2.0)
            .transform([](double val) { return val * 2.0; });

        if (result) {
            LOG_INFO_PRINT("transformed value: {}", *result);
        }
    }

    LOG_INFO("usage pattern demonstrations completed");
}

int main() {
    LOG_INFO("starting std::expected demonstration program");

    try {
        // run comprehensive test suite
        run_comprehensive_tests();

        // demonstrate various usage patterns
        demonstrate_usage_patterns();

        LOG_INFO("std::expected demonstration completed successfully");
        return 0;

    } catch (const std::exception& e) {
        LOG_ERROR_PRINT("unexpected exception occurred: {}", e.what());
        return 1;
    } catch (...) {
        LOG_ERROR("unknown exception occurred");
        return 1;
    }
}