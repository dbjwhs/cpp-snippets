// MIT License
// Copyright (c) 2025 dbjwhs

#include <iostream>
#include <string>
#include "../../../headers/project_utils.hpp"

template <typename DataType>
class LinkedList {
private:
    struct Node {
        DataType data;
        std::unique_ptr<Node> next;
        explicit Node(DataType value) : data(std::move(value)), next(nullptr) {}
    };

    std::unique_ptr<Node> m_head;
    Node* m_current;  // non-owning pointer for iteration
    size_t m_size;

public:
    // constructor
    LinkedList() : m_head(nullptr), m_current(nullptr), m_size(0) {}

    // destructor - unique_ptr handles cleanup automatically
    ~LinkedList() = default;

    // copy constructor
    LinkedList(const LinkedList& other) : m_head(nullptr), m_current(nullptr), m_size(0) {
        for (Node* temp = other.m_head.get(); temp != nullptr; temp = temp->next.get()) {
            add(temp->data);
        }
    }

    // move constructor
    LinkedList(LinkedList&& other) noexcept
        : m_head(std::move(other.m_head))
        , m_current(other.m_current)
        , m_size(other.m_size) {
        other.m_current = nullptr;
        other.m_size = 0;
    }

    // copy assignment
    LinkedList& operator=(const LinkedList& other) {
        if (this != &other) {
            LinkedList temp(other);  // copy-and-swap idiom
            std::swap(m_head, temp.m_head);
            std::swap(m_current, temp.m_current);
            std::swap(m_size, temp.m_size);
        }
        return *this;
    }

    // move assignment
    LinkedList& operator=(LinkedList&& other) noexcept {
        if (this != &other) {
            m_head = std::move(other.m_head);
            m_current = other.m_current;
            m_size = other.m_size;
            other.m_current = nullptr;
            other.m_size = 0;
        }
        return *this;
    }

    // add element to the end of the list
    void add(DataType value) {
        auto newNode = std::make_unique<Node>(std::move(value));
        if (!m_head) {
            m_head = std::move(newNode);
        } else {
            Node *temp = m_head.get();
            while (temp->next) {
                temp = temp->next.get();
            }
            temp->next = std::move(newNode);
        }
        m_size++;
    }

    // add element at specific position
    bool insertAt(DataType value, size_t position) {
        if (position > m_size) {
            return false;
        }
        auto newNode = std::make_unique<Node>(std::move(value));
        if (position == 0) {
            newNode->next = std::move(m_head);
            m_head = std::move(newNode);
        } else {
            Node *temp = m_head.get();
            for (size_t ndx = 0; ndx < position - 1; ndx++) {
                temp = temp->next.get();
            }
            newNode->next = std::move(temp->next);
            temp->next = std::move(newNode);
        }
        m_size++;
        return true;
    }

    // remove first occurrence of value
    bool remove(const DataType& value) {
        if (!m_head) {
            return false;
        }
        if (m_head->data == value) {
            m_head = std::move(m_head->next);
            m_size--;
            return true;
        }

        Node *temp = m_head.get();
        while (temp->next && temp->next->data != value) {
            temp = temp->next.get();
        }

        if (temp->next) {
            temp->next = std::move(temp->next->next);
            m_size--;
            return true;
        }
        return false;
    }

    // remove element at specific position
    bool removeAt(const size_t position) {
        if (position >= m_size) return false;

        if (position == 0) {
            m_head = std::move(m_head->next);
        } else {
            Node* temp = m_head.get();
            for (size_t ndx = 0; ndx < position - 1; ndx++) {
                temp = temp->next.get();
            }
            temp->next = std::move(temp->next->next);
        }
        m_size--;
        return true;
    }

    // search for a value
    [[nodiscard]] bool search(const DataType& value) const {
        for (Node* temp = m_head.get(); temp != nullptr; temp = temp->next.get()) {
            if (temp->data == value) return true;
        }
        return false;
    }

    // get value at specific position
    bool get(const size_t position, DataType& value) const {
        if (position >= m_size) return false;

        Node *temp = m_head.get();
        for (size_t ndx = 0; ndx < position; ndx++) {
            temp = temp->next.get();
        }
        value = temp->data;
        return true;
    }

    // reset iterator to beginning
    void reset() {
        m_current = m_head.get();
    }

    // get next element (for iteration)
    bool getNext(DataType& value) {
        if (!m_current) {
            return false;
        }
        value = m_current->data;
        m_current = m_current->next.get();
        return true;
    }

    // clear the entire list
    void clear() {
        // unique_ptr automatically free's memory up the chain
        m_head = nullptr;
        m_current = nullptr;
        m_size = 0;
    }

    // get size of list
    [[nodiscard]] size_t getSize() const {
        return m_size;
    }

    // check if list is empty
    [[nodiscard]] bool isEmpty() const {
        return m_size == 0;
    }
};

// test helper function to print test results
void printTestResult(const std::string& testName, const bool passed) {
    Logger logger("../custom.log");
    const LogLevel logLevel = passed ? LogLevel::INFO : LogLevel::CRITICAL;

    logger.log(logLevel, testName + (passed ? " PASSED" : " FAILED"));
}

// test suite for linkedlist
int main() {
    Logger logger("../custom.log");

    logger.log(LogLevel::INFO, "starting linkedlist test suite...");
    logger.log(LogLevel::INFO, "================================");;

    // test 1: constructor and isempty
    {
        LinkedList<int> list;
        // ReSharper disable CppDFAConstantConditions
        bool passed = list.isEmpty() && list.getSize() == 0;
        // ReSharper restore CppDFAConstantConditions
        printTestResult("constructor and isempty", passed);
    }

    // test 2: adding elements
    {
        LinkedList<int> list;
        list.add(1);
        list.add(2);
        list.add(3);
        bool passed = !list.isEmpty() && list.getSize() == 3;
        int value;
        passed &= list.get(0, value) && value == 1;
        passed &= list.get(1, value) && value == 2;
        passed &= list.get(2, value) && value == 3;
        printTestResult("adding elements", passed);
    }

    // test 3: removing elements
    {
        LinkedList<int> list;
        list.add(1);
        list.add(2);
        list.add(3);
        bool passed = list.remove(2);
        passed &= list.getSize() == 2;
        int value;
        passed &= list.get(0, value) && value == 1;
        passed &= list.get(1, value) && value == 3;
        printTestResult("removing elements", passed);
    }

    // test 4: inserting at position
    {
        LinkedList<int> list;
        list.add(1);
        list.add(3);
        bool passed = list.insertAt(2, 1);
        passed &= list.getSize() == 3;
        int value;
        passed &= list.get(0, value) && value == 1;
        passed &= list.get(1, value) && value == 2;
        passed &= list.get(2, value) && value == 3;
        printTestResult("inserting at position", passed);
    }

    // test 5: remove at position
    {
        LinkedList<int> list;
        list.add(1);
        list.add(2);
        list.add(3);
        bool passed = list.removeAt(1);
        passed &= list.getSize() == 2;
        int value;
        passed &= list.get(0, value) && value == 1;
        passed &= list.get(1, value) && value == 3;
        printTestResult("remove at position", passed);
    }

    // test 6: search
    {
        LinkedList<int> list;
        list.add(1);
        list.add(2);
        list.add(3);
        bool passed = list.search(2);
        passed &= !list.search(4);
        printTestResult("search", passed);
    }

    // test 7: iterator
    {
        LinkedList<int> list;
        list.add(1);
        list.add(2);
        list.add(3);
        list.reset();
        int value;
        bool passed = true;
        int expected[] = {1, 2, 3};
        int i = 0;
        while (list.getNext(value)) {
            passed &= value == expected[i++];
        }
        passed &= i == 3;
        printTestResult("iterator", passed);
    }

    // test 8: copy constructor
    {
        LinkedList<int> list1;
        list1.add(1);
        list1.add(2);
        list1.add(3);
        LinkedList<int> list2(list1);
        bool passed = list2.getSize() == list1.getSize();
        int value1, value2;
        for (size_t ndx = 0; ndx < list1.getSize(); ndx++) {
            list1.get(ndx, value1);
            list2.get(ndx, value2);
            passed &= value1 == value2;
        }
        printTestResult("copy constructor", passed);
    }

    // test 9: assignment operator
    {
        LinkedList<int> list1;
        list1.add(1);
        list1.add(2);
        LinkedList<int> list2;
        list2 = list1;
        bool passed = list2.getSize() == list1.getSize();
        int value1, value2;
        for (size_t ndx = 0; ndx < list1.getSize(); ndx++) {
            list1.get(ndx, value1);
            list2.get(ndx, value2);
            passed &= value1 == value2;
        }
        printTestResult("assignment operator", passed);
    }

    // test 10: clear
    {
        LinkedList<int> list;
        list.add(1);
        list.add(2);
        list.add(3);
        list.clear();
        // ReSharper disable CppDFAConstantConditions
        bool passed = list.isEmpty() && list.getSize() == 0;
        // ReSharper restore CppDFAConstantConditions
        printTestResult("clear", passed);
    }

    // test 11: edge cases
    {
        LinkedList<int> list;
        int value;
        bool passed = !list.remove(1);  // remove from empty list
        passed &= !list.removeAt(0);    // remove at from empty list
        passed &= !list.get(0, value);  // get from empty list
        passed &= !list.search(1);      // search in empty list
        list.add(1);
        passed &= !list.insertAt(2, 5); // insert at invalid position
        passed &= !list.removeAt(5);    // remove at invalid position
        passed &= !list.get(5, value);  // get at invalid position
        printTestResult("edge cases", passed);
    }

    // test 12: different data types
    {
        LinkedList<std::string> strList;
        bool passed = true;
        strList.add("Hello");
        strList.add("World");
        passed &= strList.getSize() == 2;
        std::string value;
        passed &= strList.get(0, value) && value == "Hello";
        passed &= strList.get(1, value) && value == "World";

        LinkedList<double> doubleList;
        doubleList.add(1.1);
        doubleList.add(2.2);
        passed &= doubleList.getSize() == 2;
        double dValue;
        passed &= doubleList.get(0, dValue) && dValue == 1.1;
        passed &= doubleList.get(1, dValue) && dValue == 2.2;

        printTestResult("different data types", passed);
    }

    logger.log(LogLevel::INFO, "================================");
    logger.log(LogLevel::INFO, "test suite completed.");

    return 0;
}
