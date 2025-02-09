// MIT License
// Copyright (c) 2025 dbjwhs

// dir_scanner.hpp
#ifndef DIR_SCANNER_HPP
#define DIR_SCANNER_HPP

#include <filesystem>
#include <queue>
#include <string>
#include <vector>
#include <functional>
#include <regex>
#include <chrono>
#include <system_error>

namespace fs = std::filesystem;

// structure to hold detailed file/directory information
struct FileInfo {
    fs::path m_path;                   // full path
    uintmax_t m_file_size{0};          // file size (0 for directories)
    fs::perms m_permissions;           // file permissions
    std::string m_last_write_time;     // last modification time as string
    bool m_is_directory{false};        // is this a directory?
    bool m_is_symlink{false};          // is this a symbolic link?
    std::string m_owner;               // File owner (POSIX systems only)
    std::string m_group;               // file group (POSIX systems only)

    // Format permissions as string (rwxrwxrwx format)
    [[nodiscard]] std::string get_permission_string() const;
};

class DirectoryScanner {
public:
    // explicit constructor - requires a path
    explicit DirectoryScanner(const fs::path& root_path);

    // delete default constructor and copy operations
    DirectoryScanner() = delete;
    DirectoryScanner(const DirectoryScanner&) = delete;
    DirectoryScanner& operator=(const DirectoryScanner&) = delete;

    // allow moving
    DirectoryScanner(DirectoryScanner&&) = default;
    DirectoryScanner& operator=(DirectoryScanner&&) = default;

    // default destructor is sufficient as we only manage RAII-compliant members
    ~DirectoryScanner() = default;

    // scan all files and directories, returning detailed information
    [[nodiscard]] std::vector<FileInfo> scan_detailed() const;

    // scan with a custom filter function, returning detailed information
    [[nodiscard]] std::vector<FileInfo> scan_with_filter(
        const std::function<bool(const FileInfo&)>& filter) const;

    // scan only files with detailed information
    [[nodiscard]] std::vector<FileInfo> scan_files_detailed() const;

    // scan only directories with detailed information
    [[nodiscard]] std::vector<FileInfo> scan_directories_detailed() const;

    // scan files matching a pattern with detailed information
    [[nodiscard]] std::vector<FileInfo> scan_by_pattern_detailed(const std::string& pattern) const;

    // Get the root path
    [[nodiscard]] const fs::path& get_root() const { return m_root; }

private:
    // root directory path to scan
    fs::path m_root;

    // helper function to gather detailed file information
    static FileInfo get_file_info(const fs::directory_entry& entry) ;

    // helper function to get file owner and group (POSIX systems only)
    static void get_posix_ownership(const fs::path& path, std::string& owner,
                           std::string& group) ;

    // helper function to format time
    static std::string format_file_time(const fs::file_time_type& time) ;
};

#endif // DIR_SCANNER_HPP
