// MIT License
// Copyright (c) 2025 dbjwhs

#include <unordered_map>
#include <string>
#include <cassert>
#include "../../../../headers/project_utils.hpp"

// flyweight class that stores shared state
class TreeType {
    Logger& logger = Logger::getInstance();

private:
    std::string m_name;        // tree species name
    std::string m_color;       // leaf color
    std::string m_texture;     // bark texture

public:
    TreeType(std::string  name, std::string  color, std::string  texture)
        : m_name(std::move(name)), m_color(std::move(color)), m_texture(std::move(texture)) {}

    void render(const int x, const int y) const {
        logger.log(LogLevel::INFO, "tree of type " + m_name
                  + " with " + m_color + " leaves"
                  + " and " + m_texture + " bark"
                  + " at position (" + std::to_string(x) + "," + std::to_string(y) + ")");
    }
};

// flyweight factory class
class TreeFactory {
private:
    Logger& logger = Logger::getInstance();
    std::unordered_map<std::string, std::shared_ptr<TreeType>> m_treeTypes;

public:
    std::shared_ptr<TreeType> getTreeType(const std::string& name,
                                         const std::string& color,
                                         const std::string& texture) {
        // create key for the tree type
        std::string key = name + color + texture;

        // check if we already have this tree type
        if (const auto it = m_treeTypes.find(key); it == m_treeTypes.end()) {
            // create a new tree type if it doesn't exist
            m_treeTypes[key] = std::make_shared<TreeType>(name, color, texture);
            logger.log(LogLevel::INFO, "created new tree type: " + name);
        }
        return m_treeTypes[key];
    }

    [[nodiscard]] size_t getTotalTypes() const {
        return m_treeTypes.size();
    }
};

// context class that contains extrinsic state
class Tree {
private:
    int m_x;                                   // x-coordinate
    int m_y;                                   // y-coordinate
    std::shared_ptr<TreeType> m_treeType;     // reference to flyweight

public:
    Tree(const int x, const int y, std::shared_ptr<TreeType> treeType)
        : m_x(x), m_y(y), m_treeType(std::move(treeType)) {}

    void render() const {
        m_treeType->render(m_x, m_y);
    }
};

// main function with comprehensive testing
int main() {
    Logger& logger = Logger::getInstance();

    // create the flyweight factory
    TreeFactory factory;

    // vector to store all trees
    std::vector<Tree> forest;

    // test 1: create multiple trees of the same type
    logger.log(LogLevel::INFO, "test 1: creating multiple trees of same type");
    logger.log(LogLevel::INFO, "----------------------------------------");
    forest.reserve(3);
    for (int ndx = 0; ndx < 3; ++ndx) {
        forest.emplace_back(
            ndx * 10,                                                 // x coordinate
            ndx * 10,                                                 // y coordinate
            factory.getTreeType("oak", "green", "rough")              // tree type
        );
    }

    // should be 1
    logger.log(LogLevel::INFO, "total tree types: " + std::to_string(factory.getTotalTypes()));

    // verify only one tree type was created
    assert(factory.getTotalTypes() == 1 && "test 1 failed: expected exactly one tree type");

    // test 2: create trees of different types
    logger.log(LogLevel::INFO, "test 2: creating trees of different types");
    logger.log(LogLevel::INFO, "----------------------------------------");
    forest.emplace_back(40, 40, factory.getTreeType("pine", "dark green", "scaly"));
    forest.emplace_back(50, 50, factory.getTreeType("birch", "light green", "smooth"));

    // verify three different tree types were created
    assert(factory.getTotalTypes() == 3 && "test 2 failed: expected exactly three tree types");

    // should be 3
    logger.log(LogLevel::INFO, "total tree types: " + std::to_string(factory.getTotalTypes()));

    // test 3: create a duplicate tree type
    logger.log(LogLevel::INFO, "test 3: creating duplicate tree type");
    logger.log(LogLevel::INFO, "----------------------------------------");
    forest.emplace_back(60, 60, factory.getTreeType("oak", "green", "rough"));

    // should still be 3
    logger.log(LogLevel::INFO, "total tree types: " + std::to_string(factory.getTotalTypes()));

    // verify no new type was created for duplicate tree
    assert(factory.getTotalTypes() == 3 && "test 3 failed: duplicate tree should not create new type");

    // verify forest size is correct (6 trees total)
    assert(forest.size() == 6 && "test 3 failed: incorrect number of trees in forest");

    // test 4: render all trees
    logger.log(LogLevel::INFO, "test 4: rendering all trees");
    logger.log(LogLevel::INFO, "----------------------------------------");
    for (const auto& tree : forest) {
        tree.render();
    }

    return 0;
}
