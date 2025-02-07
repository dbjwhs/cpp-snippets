// MIT License
// Copyright (c) 2025 dbjwhs

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <stdexcept>
#include "../../../headers/project_utils.hpp"

// base class for both files and directories
class INode {
protected:
    std::string m_name;
    bool m_isDirectory;
    std::string m_content;
    time_t m_createdTime;
    time_t m_modifiedTime;

public:
    INode(std::string name, const bool isDir)
        : m_name(std::move(name)), m_isDirectory(isDir) {
        m_createdTime = time(nullptr);
        m_modifiedTime = m_createdTime;
    }

    virtual ~INode() = default;

    [[nodiscard]] std::string getName() const { return m_name; }
    [[nodiscard]] bool isDir() const { return m_isDirectory; }
    [[nodiscard]] time_t getCreatedTime() const { return m_createdTime; }
    [[nodiscard]] time_t getModifiedTime() const { return m_modifiedTime; }

    // ABC, pure virtual
    [[nodiscard]] virtual size_t getSize() const = 0;
};

// file class
class File final : public INode {
public:
    explicit File(const std::string& name) : INode(name, false) {}

    void write(const std::string& newContent) {
        m_content = newContent;
        m_modifiedTime = time(nullptr);
    }

    [[nodiscard]] std::string read() const {
        return m_content;
    }

    [[nodiscard]] size_t getSize() const override {
        return m_content.size();
    }
};

// directory class
class Directory final : public INode {
private:
    std::unordered_map<std::string, std::shared_ptr<INode>> m_children;

public:
    explicit Directory(const std::string& name) : INode(name, true) {}

    void addChild(const std::shared_ptr<INode>& child) {
        m_children[child->getName()] = child;
        m_modifiedTime = time(nullptr);
    }

    void removeChild(const std::string& name) {
        if (m_children.find(name) != m_children.end()) {
            m_children.erase(name);
            m_modifiedTime = time(nullptr);
        }
    }

    [[nodiscard]] std::shared_ptr<INode> getChild(const std::string& name) const {
        if (const auto it = m_children.find(name); it != m_children.end()) {
            return it->second;
        }
        return nullptr;
    }

    [[nodiscard]] std::vector<std::string> list() const {
        std::vector<std::string> names;

        names.reserve(m_children.size());
        for (const auto& [name, node] : m_children) { // structured bindings more readable than std::pair IMHO
            names.push_back(name + (node->isDir() ? "/" : ""));
        }
        return names;
    }

    [[nodiscard]] size_t getSize() const override {
        size_t total = 0;
        for (const auto& pair : m_children) {
            total += pair.second->getSize();
        }
        return total;
    }
};

// mockfilesystem class to manage the entire file system
class MockFileSystem {
private:
    std::shared_ptr<Directory> m_root;
    std::shared_ptr<Directory> m_currentDirectory;

    static std::vector<std::string> splitPath(const std::string& path) {
        std::vector<std::string> parts;
        size_t start = 0;
        size_t end = path.find('/');

        while (end != std::string::npos) {
            if (end - start > 0) {
                parts.push_back(path.substr(start, end - start));
            }
            start = end + 1;
            end = path.find('/', start);
        }

        if (start < path.length()) {
            parts.push_back(path.substr(start));
        }

        return parts;
    }

    [[nodiscard]] std::shared_ptr<Directory> navigateToParent(const std::string& path) const {
        const auto parts = splitPath(path);
        auto current = m_root;

        for (size_t i = 0; i < parts.size() - 1; ++i) {
            auto node = current->getChild(parts[i]);
            if (!node || !node->isDir()) {
                throw std::runtime_error("invalid path: " + path);
            }
            current = std::dynamic_pointer_cast<Directory>(node);
        }

        return current;
    }

public:
    MockFileSystem() {
        m_root = std::make_shared<Directory>("/");
        m_currentDirectory = m_root;
    }

    void createFile(const std::string& path, const std::string& content = "") const {
        const auto parentDir = navigateToParent(path);
        const auto parts = splitPath(path);
        auto fileName = parts.back();

        const auto file = std::make_shared<File>(fileName);
        file->write(content);
        parentDir->addChild(file);
    }

    void createDirectory(const std::string& path) const {
        const auto parentDir = navigateToParent(path);
        const auto parts = splitPath(path);
        auto dirName = parts.back();

        const auto newDir = std::make_shared<Directory>(dirName);
        parentDir->addChild(newDir);
    }

    void remove(const std::string& path) const {
        const auto parentDir = navigateToParent(path);
        const auto parts = splitPath(path);
        parentDir->removeChild(parts.back());
    }

    [[nodiscard]] std::string readFile(const std::string& path) const {
        const auto parentDir = navigateToParent(path);
        const auto parts = splitPath(path);
        const auto node = parentDir->getChild(parts.back());

        if (!node || node->isDir()) {
            throw std::runtime_error("not a file: " + path);
        }

        return std::dynamic_pointer_cast<File>(node)->read();
    }

    void writeFile(const std::string& path, const std::string& content) const {
        const auto parentDir = navigateToParent(path);
        const auto parts = splitPath(path);
        const auto node = parentDir->getChild(parts.back());

        if (!node || node->isDir()) {
            throw std::runtime_error("not a file: " + path);
        }

        std::dynamic_pointer_cast<File>(node)->write(content);
    }

    [[nodiscard]] std::vector<std::string> listDirectory(const std::string& path = "") const {
        if (path.empty()) {
            return m_currentDirectory->list();
        }

        const auto dir = navigateToParent(path);
        const auto parts = splitPath(path);
        const auto node = dir->getChild(parts.back());

        if (!node || !node->isDir()) {
            throw std::runtime_error("not a directory: " + path);
        }

        return std::dynamic_pointer_cast<Directory>(node)->list();
    }
};

// example usage
int main() {
    const MockFileSystem fs;
    Logger logger("../custom.log");

    // create some directories
    fs.createDirectory("home/");
    fs.createDirectory("home/user/");
    fs.createDirectory("home/user/documents/");

    // create and write to files
    fs.createFile("home/user/documents/note.txt", "Hello, World!");
    fs.createFile("home/user/test.txt", "Test file");

    // list directories
    logger.log(LogLevel::INFO, "listing /home/user/:");
    for (const auto& entry : fs.listDirectory("home/user/")) {
        logger.log(LogLevel::INFO, entry);
    }

    // read file content
    logger.log(LogLevel::INFO, "reading note.txt:");
    logger.log(LogLevel::INFO, fs.readFile("home/user/documents/note.txt"));

    // modify a file
    fs.writeFile("home/user/documents/note.txt", "Updated content!");
    logger.log(LogLevel::INFO, "reading updated note.txt:");
    logger.log(LogLevel::INFO, fs.readFile("home/user/documents/note.txt"));

    // remove a file
    fs.remove("home/user/test.txt");

    logger.log(LogLevel::INFO, "listing /home/user/ after removal:");
    for (const auto& entry : fs.listDirectory("home/user/")) {
        logger.log(LogLevel::INFO, entry);
    }

    return 0;
}
