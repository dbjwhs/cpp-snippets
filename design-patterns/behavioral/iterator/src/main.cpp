// MIT License
// Copyright (c) 2025 dbjwhs

#include <vector>
#include <cassert>
#include <format>
#include <functional>
#include "../header/iterator.hpp"

int main() {
    // create a test collection using two-step approach (more reliable with make_unique)
    const auto numbers = std::make_unique<ConcreteAggregate<int>>();
    numbers->addMany(1, 2, 3, 4, 5);

    Logger::getInstance().log(LogLevel::INFO, std::format("Created aggregate with {} elements", numbers->size()));

    // test 1: standard forward iteration
    Logger::getInstance().log(LogLevel::INFO, "Test 1: Forward iteration");
    const auto iterator = numbers->createIterator();

    // verify iterator initialization
    assert(!iterator->isDone() && "New iterator should not be in done state");
    assert(iterator->current() == 1 && "First element should be 1");

    // collect elements using the iterator
    std::vector<int> collected;
    for (iterator->first(); !iterator->isDone(); iterator->next()) {
        collected.push_back(iterator->current());
        Logger::getInstance().log(LogLevel::INFO, std::format("  Visited element: {}", iterator->current()));
    }

    // verify all elements were visited in correct order
    assert(collected.size() == 5 && "Should visit 5 elements");
    assert(collected[0] == 1 && collected[1] == 2 && collected[2] == 3 &&
           collected[3] == 4 && collected[4] == 5 && "Elements should be in order");

    // test 2: verify is done behavior
    Logger::getInstance().log(LogLevel::INFO, "Test 2: Testing isDone and boundary conditions");
    assert(iterator->isDone() && "Iterator should be done after traversal");

    // test for exception when accessing after completion
    bool exceptionThrown = false;
    try {
        iterator->current();
    } catch (const std::out_of_range& e) {
        exceptionThrown = true;
        Logger::getInstance().log(LogLevel::INFO, std::format("  Exception correctly thrown: {}", e.what()));
    }
    assert(exceptionThrown && "Exception should be thrown when accessing past end");

    // test 3: reverse iteration
    Logger::getInstance().log(LogLevel::INFO, "Test 3: Reverse iteration");

    // pass a reference instead of a pointer to the reverse iterator constructor
    auto reverseIterator = std::make_unique<ReverseIterator<int>>(*numbers);

    // verify reverse iterator initialization
    assert(!reverseIterator->isDone() && "New reverse iterator should not be in done state");
    assert(reverseIterator->current() == 5 && "First element in reverse should be 5");

    // collect elements using the reverse iterator
    std::vector<int> reversedElements;
    for (reverseIterator->first(); !reverseIterator->isDone(); reverseIterator->next()) {
        reversedElements.push_back(reverseIterator->current());
        Logger::getInstance().log(LogLevel::INFO, std::format("  Visited element: {}", reverseIterator->current()));
    }

    // verify all elements were visited in reverse order
    assert(reversedElements.size() == 5 && "Should visit 5 elements");
    assert(reversedElements[0] == 5 && reversedElements[1] == 4 && reversedElements[2] == 3 &&
           reversedElements[3] == 2 && reversedElements[4] == 1 && "Elements should be in reverse order");

    // test 4: filtering iteration (even numbers only)
    Logger::getInstance().log(LogLevel::INFO, "Test 4: Filtering iteration (even numbers only)");
    auto isEven = [](const int& n) -> bool { return n % 2 == 0; };

    // pass a reference instead of a pointer to the filtering iterator constructor
    const auto filteringIterator = std::make_unique<FilteringIterator<int>>(*numbers, isEven);

    // collect elements using the filtering iterator
    std::vector<int> evenNumbers;
    for (filteringIterator->first(); !filteringIterator->isDone(); filteringIterator->next()) {
        evenNumbers.push_back(filteringIterator->current());
        Logger::getInstance().log(LogLevel::INFO, std::format("  Visited element: {}", filteringIterator->current()));
    }

    // verify only even numbers were visited
    assert(evenNumbers.size() == 2 && "Should visit 2 even elements");
    assert(evenNumbers[0] == 2 && evenNumbers[1] == 4 && "Should only visit even numbers");

    // test 5: empty collection
    Logger::getInstance().log(LogLevel::INFO, "Test 5: Empty collection behavior");
    const auto emptyCollection = std::make_unique<ConcreteAggregate<int>>();

    // test iterator on an empty collection
    const auto emptyIterator = emptyCollection->createIterator();
    assert(emptyIterator->isDone() && "Iterator on empty collection should be immediately done");

    // test reverse iterator on empty collection
    // use reference instead of pointer
    const auto emptyReverseIterator = std::make_unique<ReverseIterator<int>>(*emptyCollection);
    assert(emptyReverseIterator->isDone() && "Reverse iterator on empty collection should be immediately done");

    // verify exception on empty collection access
    bool emptyExceptionThrown = false;
    try {
        emptyIterator->current();
    } catch (const std::out_of_range& e) {
        emptyExceptionThrown = true;
        Logger::getInstance().log(LogLevel::INFO, std::format("  Exception correctly thrown on empty collection: {}", e.what()));
    }
    assert(emptyExceptionThrown && "Exception should be thrown when accessing empty collection");

    // test 6: using the variadic template method in a different way
    Logger::getInstance().log(LogLevel::INFO, "Test 6: Using variadic addMany method");
    const auto moreNumbers = std::make_unique<ConcreteAggregate<int>>();
    moreNumbers->addMany(10, 20, 30, 40, 50);

    const auto moreIterator = moreNumbers->createIterator();
    std::vector<int> moreCollected;

    for (moreIterator->first(); !moreIterator->isDone(); moreIterator->next()) {
        moreCollected.push_back(moreIterator->current());
        Logger::getInstance().log(LogLevel::INFO, std::format("  Added element: {}", moreIterator->current()));
    }

    assert(moreCollected.size() == 5 && "Should have 5 elements from addMany");
    assert(moreCollected[0] == 10 && moreCollected[4] == 50 && "Elements should match added values");

    Logger::getInstance().log(LogLevel::INFO, "All iterator tests completed successfully");

    return 0;
}
