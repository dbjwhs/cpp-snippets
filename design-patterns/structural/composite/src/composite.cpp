// MIT License
// Copyright (c) 2025 dbjwhs

#include <memory>
#include <vector>
#include <string>
#include <cassert>
#include <format>
#include <algorithm>
#include "../../../../headers/project_utils.hpp"

// composite pattern
// -----------------
// history:
// - introduced in 1994 as one of the original "gang of four" design patterns
// - evolved from early work in smalltalk and other object-oriented systems
// - gained widespread adoption in gui frameworks and document structures
//
// purpose:
// - allows composition of objects into tree structures to represent part-whole hierarchies
// - enables clients to treat individual objects and compositions uniformly
// - implements the principle "compose objects into tree structures to represent part-whole hierarchies"
//
// best used when:
// - you need to represent hierarchical tree structures of objects
// - you want clients to be able to ignore differences between compositions of objects and individual objects
// - the structure might be built dynamically at runtime
//
// common applications:
// - file system structures (folders containing files and other folders)
// - gui frameworks (containers holding other ui elements)
// - document object models (dom in html/xml)
// - organizational structures (departments containing sub-departments and employees)
// - graphics systems (groups of shapes)
//
// advantages:
// - simplifies client code by allowing uniform treatment of objects
// - makes it easier to add new types of components
// - provides flexibility in building complex tree structures
//
// disadvantages:
// - can make the design overly general when you only need part of the functionality
// - can make it harder to restrict what can be added to the composite
//
// implementation notes:
// - this implementation uses modern c++ features and smart pointers for memory management
// - includes comprehensive testing to demonstrate typical usage patterns
// - uses a logging system to track structural changes
// - throws exceptions for unsupported operations on leaf nodes

// forward declarations
class Component;
using ComponentPtr = std::shared_ptr<Component>;

// abstract component class representing both leaf and composite objects
class Component {
protected:
    std::string m_name;

public:
    explicit Component(std::string name) : m_name(std::move(name)) {}
    virtual ~Component() = default;

    // core operations that both leaf and composite must implement
    virtual void add(const ComponentPtr&) {
        throw std::runtime_error("Operation not supported");
    }
    virtual void remove(const ComponentPtr&) {
        throw std::runtime_error("Operation not supported");
    }
    [[nodiscard]] virtual int getCount() const = 0;  // returns total number of components
    [[nodiscard]] virtual std::string getName() const { return m_name; }
};

// leaf class representing end nodes with no children
class Leaf final : public Component {
public:
    explicit Leaf(const std::string& name) : Component(name) {}

    // leaf nodes have a count of 1
    [[nodiscard]] int getCount() const override { return 1; }
};

// composite class that can contain other components (both leafs and composites)
class Composite final : public Component {
private:
    std::vector<ComponentPtr> m_children;

public:
    explicit Composite(const std::string& name) : Component(name) {}

    // add a child component
    void add(const ComponentPtr& component) override {
        m_children.push_back(component);
        Logger::getInstance().log(LogLevel::INFO, 
            std::format("Added component {} to {}", component->getName(), m_name));
    }

    // remove a child component
    void remove(const ComponentPtr& component) override {
        if (const auto it = std::find(m_children.begin(), m_children.end(), component); it != m_children.end()) {
            m_children.erase(it);
            Logger::getInstance().log(LogLevel::INFO, 
                std::format("Removed component {} from {}", component->getName(), m_name));
        }
    }

    // recursively count all components in the hierarchy
    [[nodiscard]] int getCount() const override {
        int total = 1;  // count self
        for (const auto& child : m_children) {
            total += child->getCount();
        }
        return total;
    }
};

// comprehensive test function to verify the implementation
void test_composite_pattern() {
    // test case 1: create and test leaf nodes
    const auto leaf1 = std::make_shared<Leaf>("Leaf1");
    const auto leaf2 = std::make_shared<Leaf>("Leaf2");
    
    assert(leaf1->getCount() == 1);
    assert(leaf2->getCount() == 1);
    assert(leaf1->getName() == "Leaf1");
    assert(leaf2->getName() == "Leaf2");
    
    // test case 2: verify leaf operations throw exceptions
    bool exception_thrown = false;
    try {
        leaf1->add(leaf2);
    } catch (const std::runtime_error&) {
        LOG_INFO("leaf-add() threw as expected");
        exception_thrown = true;
    }
    assert(exception_thrown);
    
    // test case 3: create and test composite node
    const auto composite1 = std::make_shared<Composite>("Composite1");
    composite1->add(leaf1);
    composite1->add(leaf2);

    assert(composite1->getCount() == 3);  // composite + 2 leaves
    LOG_INFO("composite1->getCount() == 3, successful");
    
    // test case 4: test nested composites
    const auto composite2 = std::make_shared<Composite>("Composite2");
    const auto leaf3 = std::make_shared<Leaf>("Leaf3");
    
    composite2->add(leaf3);
    composite1->add(composite2);
    
    assert(composite1->getCount() == 5);  // composite1 + 2 leaves + composite2 + 1 leaf
    LOG_INFO("composite1 + 2 leaves + composite2 + 1 leaf == 5, successful");
    
    // test case 5: test remove operation
    composite1->remove(leaf1);
    assert(composite1->getCount() == 4);
    LOG_INFO("composite1->getCount() == 4, successful");
    
    LOG_INFO("All composite pattern tests passed successfully");
}

int main() {
    try {
        test_composite_pattern();
        return 0;
    } catch (const std::exception& e) {
        Logger::getInstance().log(LogLevel::ERROR, 
            std::format("Test failed with error: {}", e.what()));
        return 1;
    }
}
