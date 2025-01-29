// MIT License
// Copyright (c) 2025 dbjwhs

// dir_scanner.cpp
#include "dir_scanner.hpp"
#include <iostream>
#include <iomanip>
#include <ctime>
#include <sstream>

#ifdef __unix__
#include <pwd.h>
#include <grp.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

// implementation of permission string formatter
std::string FileInfo::get_permission_string() const {
    std::string result;
    auto obj_permissions = static_cast<std::underlying_type_t<fs::perms>>(m_permissions);

    // Format: rwxrwxrwx
    result.reserve(9);
    for (int i = 8; i >= 0; --i) {
        if (i % 3 == 2) result += (obj_permissions & (1 << i)) ? 'r' : '-';
        if (i % 3 == 1) result += (obj_permissions & (1 << i)) ? 'w' : '-';
        if (i % 3 == 0) result += (obj_permissions & (1 << i)) ? 'x' : '-';
    }
    return result;
}

DirectoryScanner::DirectoryScanner(const fs::path& root_path) : m_root(root_path) {
    if (!fs::exists(root_path)) {
        throw std::runtime_error("Directory does not exist: " + root_path.string());
    }
    if (!fs::is_directory(root_path)) {
        throw std::runtime_error("Path is not a directory: " + root_path.string());
    }
}

// helper function to format filesystem time
std::string DirectoryScanner::format_file_time(const fs::file_time_type& time) {
    const auto duration = time.time_since_epoch();
    const auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);
    const std::time_t time_value = seconds.count();
    std::stringstream s_stream;
    s_stream << std::put_time(std::localtime(&time_value), "%Y-%m-%d %H:%M:%S");
    return s_stream.str();
}

// helper function to get POSIX ownership information
void DirectoryScanner::get_posix_ownership(const fs::path& path,
                                         std::string& owner,
                                         std::string& group) {
#ifdef __unix__
    // todo: untested
    struct stat st;
    if (stat(path.c_str(), &st) == 0) {
        struct passwd* pw = getpwuid(st.st_uid);
        struct group* gr = getgrgid(st.st_gid);
        owner = pw ? pw->pw_name : std::to_string(st.st_uid);
        group = gr ? gr->gr_name : std::to_string(st.st_gid);
    }
#else
    // on non-POSIX systems, leave empty
    owner = "";
    group = "";
#endif
}

// helper function to gather detailed file information
FileInfo DirectoryScanner::get_file_info(const fs::directory_entry& entry) {
    FileInfo info;
    std::error_code ec; // For error handling

    // basic path information
    info.m_path = entry.path();
    info.m_is_directory = entry.is_directory(ec);
    info.m_is_symlink = entry.is_symlink(ec);

    // file size (only for regular files)
    if (entry.is_regular_file(ec)) {
        info.m_file_size = entry.file_size(ec);
    }

    // permission information
    info.m_permissions = entry.status(ec).permissions();

    // last write time
    info.m_last_write_time = format_file_time(entry.last_write_time(ec));

    // get POSIX ownership information
    get_posix_ownership(entry.path(), info.m_owner, info.m_group);

    return info;
}

std::vector<FileInfo> DirectoryScanner::scan_detailed() const {
    std::vector<FileInfo> scanned_results;
    std::queue<fs::path> dirs_to_scan;
    dirs_to_scan.push(m_root);

    while (!dirs_to_scan.empty()) {
        const fs::path current_dir = dirs_to_scan.front();
        dirs_to_scan.pop();

        try {
            for (const auto& entry : fs::directory_iterator(current_dir)) {
                scanned_results.push_back(get_file_info(entry));

                if (entry.is_directory()) {
                    dirs_to_scan.push(entry.path());
                }
            }
        } catch (const fs::filesystem_error& e) {
            // skip directories we can't access
            continue;
        }
    }
    return scanned_results;
}

std::vector<FileInfo> DirectoryScanner::scan_with_filter(
    const std::function<bool(const FileInfo&)>& filter) const {

    std::vector<FileInfo> scanned_results;
    std::queue<fs::path> dirs_to_scan;
    dirs_to_scan.push(m_root);

    while (!dirs_to_scan.empty()) {
        const fs::path current_dir = dirs_to_scan.front();
        dirs_to_scan.pop();

        try {
            for (const auto& entry : fs::directory_iterator(current_dir)) {
                FileInfo info = get_file_info(entry);
                if (filter(info)) {
                    scanned_results.push_back(std::move(info));
                }
                if (entry.is_directory()) {
                    dirs_to_scan.push(entry.path());
                }
            }
        } catch (const fs::filesystem_error& e) {
            continue;
        }
    }
    return scanned_results;
}

std::vector<FileInfo> DirectoryScanner::scan_files_detailed() const {
    return scan_with_filter([](const FileInfo& info) {
        return !info.m_is_directory;
    });
}

std::vector<FileInfo> DirectoryScanner::scan_directories_detailed() const {
    return scan_with_filter([](const FileInfo& info) {
        return info.m_is_directory;
    });
}

std::vector<FileInfo> DirectoryScanner::scan_by_pattern_detailed(
    const std::string& pattern) const {

    // convert glob pattern to regex
    std::string regex_pattern = pattern;
    const std::regex special_chars{R"([\.\[\]\(\)\\\*\?\{\}\|\+])"};
    regex_pattern = std::regex_replace(pattern, special_chars, R"(\$&)");
    regex_pattern = std::regex_replace(regex_pattern, std::regex(R"(\\\*)"), ".*");
    regex_pattern = std::regex_replace(regex_pattern, std::regex(R"(\\\?)"), ".");
    std::regex pattern_regex(regex_pattern);

    return scan_with_filter([&pattern_regex](const FileInfo& info) {
        return !info.m_is_directory &&
               std::regex_match(info.m_path.filename().string(), pattern_regex);
    });
}

// example usage
int main() {
    try {
        const DirectoryScanner scanner(".");

        std::cout << "Detailed file information:\n";
        for (const auto& info : scanner.scan_detailed()) {
            std::cout << "\nPath: " << info.m_path.string() << "\n";
            std::cout << "Type: " << (info.m_is_directory ? "Directory" : "File") << "\n";
            if (!info.m_is_directory) {
                std::cout << "Size: " << info.m_file_size << " bytes\n";
            }
            std::cout << "Permissions: " << info.get_permission_string() << "\n";
            std::cout << "Last modified: " << info.m_last_write_time << "\n";

            // print ownership information on POSIX systems
            if (!info.m_owner.empty()) {
                std::cout << "Owner: " << info.m_owner << "\n";
                std::cout << "Group: " << info.m_group << "\n";
            }

            if (info.m_is_symlink) {
                std::cout << "Type: Symbolic Link\n";
            }
        }

        // example of filtering for large files (> 1MB)
        std::cout << "\nLarge files (>1MB):\n";
        const auto large_files = scanner.scan_with_filter([](const FileInfo& info) {
            return !info.m_is_directory && info.m_file_size > 1024 * 1024;
        });

        for (const auto& file : large_files) {
            std::cout << file.m_path.filename().string() << ": "
                     << (file.m_file_size / 1024.0 / 1024.0) << " MB\n";
        }

        // Test pattern matching for specific file types
        std::cout << "\nC++ source files (*.cpp):\n";
        auto cpp_files = scanner.scan_by_pattern_detailed("*.cpp");
        for (const auto& file : cpp_files) {
            std::cout << file.m_path.filename().string() << "\n";
        }

        std::cout << "\nNinja files (*.ninja):\n";
        auto ninja_files = scanner.scan_by_pattern_detailed("*.ninja*");
        for (const auto& file : ninja_files) {
            std::cout << file.m_path.filename().string() << "\n";
        }

        std::cout << "\nText files (*.txt):\n";
        auto text_files = scanner.scan_by_pattern_detailed("*.txt");
        for (const auto& file : text_files) {
            std::cout << file.m_path.filename().string() << "\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}