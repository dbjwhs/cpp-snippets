// MIT License
// Copyright (c) 2025 dbjwhs

#include <iostream>
#include <vector>
#include <cassert>
#include <utility>
#include <string>
#include <memory>
#include <format>
#include "../../../headers/project_utils.hpp"

/*
 * Move Semantics in C++: Historical Context and Implementation Details
 *
 * Move semantics was introduced in C++11 to address inefficiencies in handling temporary objects and resource transfers.
 * Prior to C++11, when objects were passed or returned by value, expensive deep copies were often made unnecessarily.
 * This was especially problematic for resource-managing classes (e.g., containers with dynamic memory).
 *
 * The core components of move semantics are:
 * 1. rvalue references (Type&&) - A new reference type that can bind to temporary objects (rvalues)
 * 2. std::move() - A utility function that converts lvalues to rvalues, enabling move operations
 * 3. Move constructors - Special constructors that transfer resources from a temporary object
 * 4. Move assignment operators - Special assignment operators that transfer resources from a temporary object
 *
 * Common use cases include:
 * - Transferring ownership of resources (e.g., dynamic memory, file handles) between objects
 * - Optimizing container operations (insertion, resizing)
 * - Implementing move-only types (e.g., std::unique_ptr)
 * - Returning large objects from functions efficiently
 *
 * Move semantics follows the "steal the resources and leave the source in a valid but unspecified state" principle.
 * The moved-from object should be in a state that is safe to destroy and reassign, but not necessarily usable otherwise.
 */

// a simple resource-managing class to demonstrate move semantics
class ResourceManager {
private:
    // smart pointer to dynamically allocated memory
    std::unique_ptr<int[]> m_data;

    // size of the allocated array
    size_t m_size;

    // unique identifier for this instance (for tracking purposes)
    int m_id;

    // static counter to assign unique ids
    static int s_counter;

public:
    // default constructor
    ResourceManager() : m_data(nullptr), m_size(0), m_id(++s_counter) {
        Logger::getInstance().log(LogLevel::INFO, std::format("default constructor called for id {}", m_id));
    }

    // parameterized constructor
    explicit ResourceManager(const size_t size) : m_size(size), m_id(++s_counter) {
        Logger::getInstance().log(LogLevel::INFO, std::format("parameterized constructor called for id {}", m_id));

        // allocate memory using smart pointer
        m_data = std::make_unique<int[]>(m_size);

        // initialize with some values
        for (size_t ndx = 0; ndx < m_size; ++ndx) {
            m_data[ndx] = static_cast<int>(ndx * 10);
        }
    }

    // copy constructor
    ResourceManager(const ResourceManager& other) : m_size(other.m_size), m_id(++s_counter) {
        Logger::getInstance().log(LogLevel::INFO, std::format("copy constructor called, copying from id {} to new id {}"
            , other.m_id, m_id));

        // perform deep copy
        if (other.m_data) {
            m_data = std::make_unique<int[]>(m_size);
            for (size_t ndx = 0; ndx < m_size; ++ndx) {
                m_data[ndx] = other.m_data[ndx];
            }
        } else {
            m_data.reset();  // Release existing resources
        }
    }

    // move constructor
    ResourceManager(ResourceManager&& other) noexcept : m_data(nullptr), m_size(0), m_id(++s_counter) {
        Logger::getInstance().log(LogLevel::INFO, std::format("move constructor called, moving from id {} to new id {}"
            , other.m_id, m_id));

        // steal resources from other
        m_data = std::move(other.m_data);
        m_size = other.m_size;

        // leave other in a valid but "empty" state (m_data already moved)
        other.m_size = 0;

        // note: we don't modify other.m_id because it still identifies the same logical object
    }

    // copy assignment operator
    ResourceManager& operator=(const ResourceManager& other) {
        Logger::getInstance().log(LogLevel::INFO, std::format("copy assignment called, copying from id {} to id {}"
            , other.m_id, m_id));

        // guard against self-assignment
        if (this == &other) {
            return *this;
        }

        // copy size
        m_size = other.m_size;

        // perform deep copy
        if (other.m_data) {
            m_data = std::make_unique<int[]>(m_size);
            for (size_t ndx = 0; ndx < m_size; ++ndx) {
                m_data[ndx] = other.m_data[ndx];
            }
        } else {
            m_data.reset();  // Release existing resources
        }

        return *this;
    }

    // move assignment operator
    ResourceManager& operator=(ResourceManager&& other) noexcept {
        Logger::getInstance().log(LogLevel::INFO, std::format("move assignment called, moving from id {} to id {}"
            , other.m_id, m_id));

        // guard against self-assignment
        if (this == &other) {
            return *this;
        }

        // steal resources from other
        m_data = std::move(other.m_data);
        m_size = other.m_size;

        // leave other in a valid but "empty" state
        other.m_size = 0;

        return *this;
    }

    // destructor
    ~ResourceManager() {
        Logger::getInstance().log(LogLevel::INFO, std::format("destructor called for id {}", m_id));
    }

    // utility methods

    // get the size of the managed resource
    [[nodiscard]] size_t size() const {
        return m_size;
    }

    // get the id of this instance
    [[nodiscard]] int id() const {
        return m_id;
    }

    // check if this object has valid data
    [[nodiscard]] bool hasData() const {
        return m_data != nullptr;
    }

    // get a value at a specific index
    [[nodiscard]] int at(const size_t index) const {
        if (index >= m_size || !m_data) {
            throw std::out_of_range("Index out of range or null data");
        }
        return m_data[index];
    }

    // set a value at a specific index
    void set(const size_t index, const int value) const {
        if (index >= m_size || !m_data) {
            throw std::out_of_range("Index out of range or null data");
        }
        m_data[index] = value;
    }

    // print the contents of the resource
    void print() const {
        if (!m_data) {
            Logger::getInstance().log(LogLevel::INFO, std::format("ResourceManager id {} has no data", m_id));
            return;
        }

        std::string values;
        for (size_t ndx = 0; ndx < m_size; ++ndx) {
            if (ndx > 0) values += ", ";
            values += std::to_string(m_data[ndx]);
        }
        Logger::getInstance().log(LogLevel::INFO, std::format("ResourceManager id {} contains: [{}]", m_id, values));
    }
};

// initialize static counter, note static's init as 0 per C specification
int ResourceManager::s_counter;

// function that returns by value, allowing for move semantics
ResourceManager createResourceManager(size_t size) {
    Logger::getInstance().log(LogLevel::INFO, std::format("creating ResourceManager with size {}", size));

    // local variable that will be moved from when returned
    ResourceManager resource(size);

    // when we return, the move constructor will typically be used
    // (or copy elision may occur depending on compiler optimizations)
    return resource;
}

// function that uses std::move explicitly to force a move
void processAndSwap(ResourceManager& first, ResourceManager& second) {
    Logger::getInstance().log(LogLevel::INFO, "processing and swapping resources");

    // create a temporary using moved resources from first
    ResourceManager temp(std::move(first));

    // move second into first
    first = std::move(second);

    // move temp into second
    second = std::move(temp);

    // at this point, the logical values have been swapped
    // and temp will be destroyed when we exit the function
}

// test cases for ResourceManager move semantics
void runTests() {
    Logger::getInstance().log(LogLevel::INFO, "starting move semantics tests");

    // test 1: verify move constructor
    {
        Logger::getInstance().log(LogLevel::INFO, "test 1: move constructor");

        // create a source object
        ResourceManager source(5);
        source.print();

        // get the id before moving
        int sourceId = source.id();

        // verify source has data
        assert(source.hasData());
        assert(source.size() == 5);

        // move construct a new object
        const ResourceManager destination(std::move(source));
        destination.print();

        // verify the move worked correctly
        assert(!source.hasData());  // source should be empty
        assert(source.size() == 0);
        assert(destination.hasData());
        assert(destination.size() == 5);
        assert(destination.at(2) == 20);  // check data integrity

        Logger::getInstance().log(LogLevel::INFO
            , std::format("test 1 passed: source id {} is now empty, destination id {} has the data"
            , sourceId, destination.id()));
    }

    // test 2: verify move assignment
    {
        Logger::getInstance().log(LogLevel::INFO, "test 2: move assignment");

        // create source and destination objects
        ResourceManager source(3);
        ResourceManager destination(7);

        // save ids for logging
        int sourceId = source.id();
        int destId = destination.id();

        // print initial state
        source.print();
        destination.print();

        // verify initial state
        assert(source.hasData());
        assert(source.size() == 3);
        assert(destination.hasData());
        assert(destination.size() == 7);

        // perform move assignment
        destination = std::move(source);

        // print final state
        source.print();
        destination.print();

        // verify the move worked correctly
        assert(!source.hasData());  // source should be empty
        assert(source.size() == 0);
        assert(destination.hasData());
        assert(destination.size() == 3);  // should have source's old size
        assert(destination.at(1) == 10);  // check data integrity

        Logger::getInstance().log(LogLevel::INFO
            , std::format("test 2 passed: source id {} is now empty, destination id {} has new data", sourceId, destId));
    }

    // test 3: verify returning by value (potential move)
    {
        Logger::getInstance().log(LogLevel::INFO, "test 3: return by value");

        // get a resource by value (may use move or copy elision)
        const ResourceManager resource = createResourceManager(4);

        // verify the resource is valid
        assert(resource.hasData());
        assert(resource.size() == 4);

        // check data integrity
        for (size_t i = 0; i < resource.size(); ++i) {
            assert(resource.at(i) == static_cast<int>(i * 10));
        }

        resource.print();
        Logger::getInstance().log(LogLevel::INFO
            , std::format("test 3 passed: successfully received resource with id {}", resource.id()));
    }

    // test 4: verify std::move and swap functionality
    {
        Logger::getInstance().log(LogLevel::INFO, "test 4: explicit std::move for swapping");

        // create two resources with different values
        ResourceManager first(2);
        ResourceManager second(3);

        // save ids for logging
        int firstId = first.id();
        int secondId = second.id();

        // set some specific values for testing
        first.set(0, 100);
        first.set(1, 200);
        second.set(0, 300);
        second.set(1, 400);
        second.set(2, 500);

        // print initial state
        first.print();
        second.print();

        // verify initial state
        assert(first.hasData() && first.size() == 2);
        assert(second.hasData() && second.size() == 3);
        assert(first.at(0) == 100 && first.at(1) == 200);
        assert(second.at(0) == 300 && second.at(1) == 400 && second.at(2) == 500);

        // swap using our function
        processAndSwap(first, second);

        // print final state
        first.print();
        second.print();

        // verify the swap worked correctly
        assert(first.hasData() && first.size() == 3);
        assert(second.hasData() && second.size() == 2);
        assert(first.at(0) == 300 && first.at(1) == 400 && first.at(2) == 500);
        assert(second.at(0) == 100 && second.at(1) == 200);

        Logger::getInstance().log(LogLevel::INFO
            , std::format("test 4 passed: resource id {} now has 3 elements, resource id {} now has 2 elements"
            , firstId, secondId));
    }

    // test 5: verify moved-from objects can be reused
    {
        Logger::getInstance().log(LogLevel::INFO, "test 5: reusing moved-from objects");

        // create a source object
        ResourceManager source(3);
        int sourceId = source.id();

        // move from it
        ResourceManager destination(std::move(source));

        // verify a source is in a valid but empty state
        assert(!source.hasData());
        assert(source.size() == 0);

        // reuse the moved-from object
        source = ResourceManager(6);

        // verify source is now usable again
        assert(source.hasData());
        assert(source.size() == 6);
        assert(source.at(5) == 50);

        source.print();
        Logger::getInstance().log(LogLevel::INFO
            , std::format("test 5 passed: moved-from resource id {} successfully reused", sourceId));
    }

    Logger::getInstance().log(LogLevel::INFO, "all move semantics tests passed");
}

int main() {
    Logger::getInstance().log(LogLevel::INFO, "--- starting move semantics demonstration ---");

    // run comprehensive tests
    runTests();

    // example of using std::move with standard containers
    Logger::getInstance().log(LogLevel::INFO, "demonstrating std::move with standard containers");

    // create a vector of strings
    std::vector<std::string> strings = {"hello", "world", "this", "is", "a", "test"};

    // get the first string
    std::string firstString = strings[0];
    Logger::getInstance().log(LogLevel::INFO, std::format("copied first string: '{}'", firstString));
    Logger::getInstance().log(LogLevel::INFO, std::format("original still exists in vector: '{}'", strings[0]));

    // move the second string
    std::string secondString = std::move(strings[1]);
    Logger::getInstance().log(LogLevel::INFO, std::format("moved second string: '{}'", secondString));
    Logger::getInstance().log(LogLevel::INFO, std::format("original in vector is now: '{}'", strings[1]));  // will be empty

    // create a unique_ptr (move-only type)
    std::unique_ptr<int> ptr1 = std::make_unique<int>(42);
    Logger::getInstance().log(LogLevel::INFO, std::format("created unique_ptr with value: {}", *ptr1));

    // move the unique_ptr (cannot copy)
    std::unique_ptr<int> ptr2 = std::move(ptr1);
    Logger::getInstance().log(LogLevel::INFO, std::format("moved unique_ptr to ptr2, value: {}", *ptr2));

    // ptr1 is now nullptr
    assert(ptr1 == nullptr);
    Logger::getInstance().log(LogLevel::INFO, "ptr1 is now nullptr after move");

    Logger::getInstance().log(LogLevel::INFO, "--- move semantics demonstration complete ---");
    return 0;
}
