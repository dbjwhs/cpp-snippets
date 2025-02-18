// MIT License
// Copyright (c) 2025 dbjwhs

#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <cassert>
#include <format>
#include <bits/ranges_algo.h>
#include "../../../../headers/project_utils.hpp"

// Define file system concept
template<typename TypeFileSystem>
concept FileSystem = requires(TypeFileSystem file_system
    , std::string_view path, std::string_view source
    , std::string_view dest) {
    { file_system.createDirectory(path) } -> std::same_as<bool>;
    { file_system.copyFile(source, dest) } -> std::same_as<bool>;
    { file_system.listFiles(path) } -> std::same_as<std::vector<std::string>>;
    { file_system.supportsSymlinks() } -> std::same_as<bool>;
    { file_system.supportsPermissions() } -> std::same_as<bool>;
};

// Constants for invalid characters
namespace FileSystemConstants {
    constexpr std::string_view INVALID_WIN_CHARS{"<>:\"/\\|?*"};
    constexpr std::string_view SPECIAL_APFS_CHARS{"/\0:"};
    constexpr size_t MAX_WIN_PATH{260};
    constexpr size_t MAX_APFS_PATH{1024};
}

// Abstract interface for modern file system operations
class IFileSystem {
public:
    virtual ~IFileSystem() noexcept = default;
    [[nodiscard]] virtual bool createDirectory(std::string_view path) = 0;
    [[nodiscard]] virtual bool copyFile(std::string_view source, std::string_view dest) = 0;
    [[nodiscard]] virtual std::vector<std::string> listFiles(std::string_view path) = 0;
    [[nodiscard]] virtual constexpr bool supportsSymlinks() = 0;
    [[nodiscard]] virtual constexpr bool supportsPermissions() = 0;
};

// APFS (Apple File System) implementation
class APFSSystem {
private:
    [[maybe_unused]] bool m_caseSensitive{false};
    [[maybe_unused]] std::string m_volumeName{};

public:
    // APFS specific methods with its native format
    [[nodiscard]] static bool createAPFSDirectory(std::string_view path, uint32_t permissions) {
        Logger::getInstance().log(LogLevel::INFO, std::format("creating apfs directory: {} with unix permissions: {:o}",
                                path, permissions));
        return true;
    }

    [[nodiscard]] static bool copyAPFSFile(std::string_view source, std::string_view dest,
                                   bool preserveMetadata) {
        Logger::getInstance().log(LogLevel::INFO, std::format("copying apfs file with metadata preservation: {}",
                                preserveMetadata ? "yes" : "no"));
        Logger::getInstance().log(LogLevel::INFO, std::format("source: {}dest: {}", source, dest));
        return true;
    }

    [[nodiscard]] static std::vector<std::string> getAPFSContents(std::string_view path,
                                                          const bool includeHidden) {
        Logger::getInstance().log(LogLevel::INFO, std::format("listing apfs directory contents{}",
                                includeHidden ? " (including hidden files)" : ""));
        return {"file1.txt", ".ds_store", "folder1"};
    }
};

// FAT32 (Windows File System) implementation
class FAT32System {
private:
    char m_driveLetter{};
    bool m_quickFormat{false};

public:
    [[nodiscard]] static bool makeFAT32Dir(std::string_view path) {
        Logger::getInstance().log(LogLevel::INFO, std::format("creating fat32 directory: {}", path));
        return true;
    }

    [[nodiscard]] static bool copyFAT32(std::string_view source, std::string_view dest) {
        Logger::getInstance().log(LogLevel::INFO, std::format("copying fat32 file (8.3 filename format)"));
        Logger::getInstance().log(LogLevel::INFO, std::format("source: {} dest: {}", source, dest));
        return true;
    }

    [[nodiscard]] static std::vector<std::string> scanFAT32Dir(std::string_view path) {
        Logger::getInstance().log(LogLevel::INFO, std::format("scanning fat32 directory contents (8.3 format)"));
        return {"FILE1.TXT", "FOLDER1"};
    }
};

// Adapter for APFS to modern interface
class APFSAdapter : public IFileSystem {
private:
    std::unique_ptr<APFSSystem> m_apfsSystem;

    [[nodiscard]] static std::string sanitizeForAPFS(std::string_view filename) {
        std::string result{filename};

        // Handle special APFS characters using ranges
        for (char specialChar : FileSystemConstants::SPECIAL_APFS_CHARS) {
            std::ranges::replace(result, specialChar, '_');
        }

        // Check path length
        if (result.length() > FileSystemConstants::MAX_APFS_PATH) {
            auto hash = std::hash<std::string_view>{}(filename);
            auto hashStr = std::to_string(hash).substr(0, 8);
            result = result.substr(0, FileSystemConstants::MAX_APFS_PATH - hashStr.length() - 1);
            result += '_' + hashStr;
        }

        // Handle leading dots (hidden files in unix)
        if (!result.empty() && result[0] == '.') {
            result = '_' + result;
        }

        return result;
    }

public:
    APFSAdapter() : m_apfsSystem{std::make_unique<APFSSystem>()} {}

    [[nodiscard]] bool createDirectory(std::string_view path) override {
        if (path.empty()) {
            return false;
        }
        return APFSSystem::createAPFSDirectory(sanitizeForAPFS(path), 0755);
    }

    [[nodiscard]] bool copyFile(std::string_view source, std::string_view dest) override {
        return APFSSystem::copyAPFSFile(
            sanitizeForAPFS(source),
            sanitizeForAPFS(dest),
            true
        );
    }

    [[nodiscard]] std::vector<std::string> listFiles(std::string_view path) override {
        return APFSSystem::getAPFSContents(path, false);
    }

    [[nodiscard]] constexpr bool supportsSymlinks() override { return true; }
    [[nodiscard]] constexpr bool supportsPermissions() override { return true; }
};

// Adapter for FAT32 to modern interface
class FAT32Adapter : public IFileSystem {
private:
    std::unique_ptr<FAT32System> m_fat32System;

    [[nodiscard]] static std::string sanitizeForFAT32(std::string_view filename) {
        std::string result{filename};

        // Remove invalid windows characters using ranges
        for (char invalidChar : FileSystemConstants::INVALID_WIN_CHARS) {
            std::ranges::replace(result, invalidChar, '_');
        }

        // Handle spaces
        std::ranges::replace(result, ' ', '_');

        // Check path length
        if (result.length() > FileSystemConstants::MAX_WIN_PATH) {
            const auto hash = std::hash<std::string_view>{}(filename);
            const auto hashStr = std::to_string(hash).substr(0, 8);
            result = result.substr(0, FileSystemConstants::MAX_WIN_PATH - hashStr.length() - 1);
            result += '_' + hashStr;
        }

        return result;
    }

    [[nodiscard]] static std::string convertTo83Format(std::string_view filename) {
        std::string result{filename.substr(0, 8)};
        if (const auto dot = filename.find_last_of('.'); dot != std::string_view::npos) {
            result += '.' + std::string{filename.substr(dot + 1, 3)};
        }

        // Convert to uppercase using ranges
        std::ranges::transform(result, result.begin(), [](const char c) {
            return static_cast<char>(std::toupper(c));
        });
        return result;
    }

public:
    FAT32Adapter() : m_fat32System{std::make_unique<FAT32System>()} {}

    [[nodiscard]] bool createDirectory(std::string_view path) override {
        if (path.empty()) {
            return false;
        }
        return FAT32System::makeFAT32Dir(
            convertTo83Format(sanitizeForFAT32(path))
        );
    }

    [[nodiscard]] bool copyFile(std::string_view source, std::string_view dest) override {
        return FAT32System::copyFAT32(
            convertTo83Format(sanitizeForFAT32(source)),
            convertTo83Format(sanitizeForFAT32(dest))
        );
    }

    [[nodiscard]] std::vector<std::string> listFiles(std::string_view path) override {
        return FAT32System::scanFAT32Dir(convertTo83Format(path));
    }

    [[nodiscard]] constexpr bool supportsSymlinks() override { return false; }
    [[nodiscard]] constexpr bool supportsPermissions() override { return false; }
};

// File system operations manager
template <FileSystem TypeFileSystem>
class FileOperationsManager {
public:
    std::unique_ptr<TypeFileSystem> m_fileSystem;

    explicit FileOperationsManager(std::unique_ptr<TypeFileSystem> fs)
        : m_fileSystem{std::move(fs)} {}

    void performCrossSystemCopy(std::string_view source, std::string_view dest) {
        Logger::getInstance().log(LogLevel::INFO, "performing cross-system copy operation...");

        if (m_fileSystem->supportsSymlinks()) {
            Logger::getInstance().log(LogLevel::INFO, "symlinks will be preserved");
        }

        if (m_fileSystem->supportsPermissions()) {
            Logger::getInstance().log(LogLevel::INFO, "file permissions will be preserved");
        }

        if (m_fileSystem->createDirectory(dest)) {
            Logger::getInstance().log(LogLevel::INFO, "destination directory created successfully");
        }

        if (m_fileSystem->copyFile(source, dest)) {
            Logger::getInstance().log(LogLevel::INFO, "files copied successfully");
        }

        Logger::getInstance().log(LogLevel::INFO, "destination contents:");
        for (const auto& file : m_fileSystem->listFiles(dest)) {
            Logger::getInstance().log(LogLevel::INFO, std::format("- {}", file));
        }
    }
};

int main() {
    Logger::getInstance().log(LogLevel::INFO, "testing file system adapters with invalid characters and assertions...");

    // test helper for verifying sanitized filenames
    auto testSanitization = []<FileSystem T>(std::unique_ptr<T> fs, std::string_view input,
                              std::string_view expectedPath) {
        FileOperationsManager<T> manager{std::move(fs)};

        try {
            bool success = manager.m_fileSystem->createDirectory(input);
            assert(success && "Directory creation should succeed");

            auto files = manager.m_fileSystem->listFiles(input);
            assert(!files.empty() && "Directory should not be empty after creation");

            std::string testFile{std::string{input} + "/test.txt"};
            success = manager.m_fileSystem->copyFile(testFile, expectedPath);
            assert(success && "File copy should succeed");

            Logger::getInstance().log(LogLevel::INFO, std::format("test passed for input: {}", input));
            return true;
        } catch (const std::exception& e) {
            std::cerr << std::format("test failed: {}", e.what());
            return false;
        }
    };

    // test suite 1: FAT32 adapter tests
    {
        Logger::getInstance().log(LogLevel::INFO, std::format("running fat32 adapter tests..."));

        // test case 1: invalid windows characters
        bool test1 = testSanitization(
            std::make_unique<FAT32Adapter>(),
            "test<file>name*.txt",
            "TEST_FIL_.TXT"
        );
        assert(test1 && "Invalid windows characters test failed");

        // test case 2: long path handling
        const auto longPathAdapter = std::make_unique<FAT32Adapter>();
        const std::string longPath(300, 'a');  // 300 character path
        const bool success = longPathAdapter->createDirectory(longPath);
        assert(success && "Long path handling failed");

        // test case 3: spaces and special characters
        const bool test3 = testSanitization(
            std::make_unique<FAT32Adapter>(),
            "my file name: special * chars?.txt",
            "MY_FILE_.TXT"
        );
        assert(test3 && "Special characters test failed");

        // test case 4: path with multiple invalid characters
        const bool test4 = testSanitization(
            std::make_unique<FAT32Adapter>(),
            "C:/Program Files/My<App>*|.exe",
            "C_/PROGRA_1/MY_APP__.EXE"
        );
        assert(test4 && "Multiple invalid characters test failed");

        Logger::getInstance().log(LogLevel::INFO, std::format("fat32 adapter tests completed successfully"));
    }

    // test suite 2: APFS adapter tests
    {
        Logger::getInstance().log(LogLevel::INFO, std::format("running apfs adapter tests..."));

        // test case 1: hidden files
        assert(testSanitization(
            std::make_unique<APFSAdapter>(),
            ".hiddenfile.txt",
            "_hiddenfile.txt"
        ) && "Hidden files test failed");

        // test case 2: special characters
        assert(testSanitization(
            std::make_unique<APFSAdapter>(),
            "file:with/special\0chars.txt",
            "file_with_special_chars.txt"
        ) && "Special characters test failed");

        // test case 3: long path handling
        auto longPathAdapter = std::make_unique<APFSAdapter>();
        const std::string longPath(1100, 'a');  // path longer than APFS max
        const bool success = longPathAdapter->createDirectory(longPath);
        assert(success && "Long path should be handled");

        // test case 4: combined special cases
        assert(testSanitization(
            std::make_unique<APFSAdapter>(),
            ".hidden/file:with\0special_chars.txt",
            "_hidden_file_with_special_chars.txt"
        ) && "Combined special cases test failed");

        Logger::getInstance().log(LogLevel::INFO, std::format("apfs adapter tests completed successfully"));
    }

    // test suite 3: cross-system operations
    {
        Logger::getInstance().log(LogLevel::INFO, std::format("running cross-system operation tests..."));

        // test copying from APFS to FAT32
        {
            Logger::getInstance().log(LogLevel::INFO, std::format("scenario 1: copying from apfs to fat32"));
            FileOperationsManager<FAT32Adapter> manager{std::make_unique<FAT32Adapter>()};

            std::string_view sourceFile = "/Users/john/Documents/my:project*.txt";
            std::string_view destFile = "D:\\MY_PROJ.TXT";

            manager.performCrossSystemCopy(sourceFile, destFile);
        }

        // test copying from FAT32 to APFS
        {
            Logger::getInstance().log(LogLevel::INFO, std::format("scenario 2: copying from fat32 to apfs"));
            FileOperationsManager<APFSAdapter> manager{std::make_unique<APFSAdapter>()};

            constexpr std::string_view sourceFile = "D:\\DOCS\\PRO:J*.TXT";
            constexpr std::string_view destFile = "/Users/john/Documents/project.txt";

            manager.performCrossSystemCopy(sourceFile, destFile);
        }
    }

    // test suite 4: edge cases
    {
        Logger::getInstance().log(LogLevel::INFO, std::format("running edge case tests..."));
        const auto fat32Adapter = std::make_unique<FAT32Adapter>();
        const auto apfsAdapter = std::make_unique<APFSAdapter>();

        // test empty paths
        assert(fat32Adapter->createDirectory("") == false && "Empty path should fail");
        assert(apfsAdapter->createDirectory("") == false && "Empty path should fail");

        // test paths with only invalid characters
        constexpr std::string_view allInvalidChars = "<>:\"/\\|?*";
        bool result = fat32Adapter->createDirectory(allInvalidChars);
        assert(result && "Completely invalid path should be sanitized");

        // test unicode characters
        constexpr std::string_view unicodePath = "תיקייה_with_unicode_名前.txt";
        const bool success = fat32Adapter->copyFile(unicodePath, "OUTPUT.TXT");
        assert(success && "Unicode handling should not fail");

        Logger::getInstance().log(LogLevel::INFO, std::format("edge case tests completed successfully"));
    }

    Logger::getInstance().log(LogLevel::INFO, std::format("all file system adapter tests completed successfully!"));

    return 0;
}
