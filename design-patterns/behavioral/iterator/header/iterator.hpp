// MIT License
// Copyright (c) 2025 dbjwhs

// mit license
// copyright (c) 2025 dbjwhs

#ifndef ITERATOR_PATTERN_HPP
#define ITERATOR_PATTERN_HPP

#include "../../../../headers/project_utils.hpp"

// forward declarations
template <typename ElementType>
class Iterator;

// abstract aggregate interface
template <typename AggrType>
class Aggregate {
public:
    virtual ~Aggregate() = default;
    virtual std::unique_ptr<Iterator<AggrType>> createIterator() = 0;
    virtual void add(const AggrType& item) = 0;
    [[nodiscard]] virtual size_t size() const = 0;
    virtual AggrType& at(size_t index) = 0;
    virtual const AggrType& at(size_t index) const = 0;
};

// abstract iterator interface
template <typename IterType>
class Iterator {
public:
    virtual ~Iterator() = default;

    // positions iterator to the first element
    virtual void first() = 0;

    // advances iterator to the next element
    virtual void next() = 0;

    // checks if traversal is complete
    [[nodiscard]] virtual bool isDone() const = 0;

    // returns the current element
    virtual IterType& current() = 0;
};

// concrete aggregate implementation
template <typename AggregateType>
class ConcreteAggregate final : public Aggregate<AggregateType> {
private:
    // store the collection elements
    std::vector<AggregateType> m_items;

public:
    // default constructor for empty collections
    ConcreteAggregate() = default;

    // factory method for creating with initializer list
    static std::unique_ptr<ConcreteAggregate<AggregateType>> create(std::initializer_list<AggregateType> items) {
        auto aggregate = std::make_unique<ConcreteAggregate<AggregateType>>();
        for (const auto& item : items) {
            aggregate->add(item);
        }
        return aggregate;
    }

    // creates an iterator for traversing this aggregate
    std::unique_ptr<Iterator<AggregateType>> createIterator() override;

    // adds an item to the aggregate
    void add(const AggregateType& item) override {
        m_items.push_back(item);
    }

    // adds multiple elements at once (variadic template)
    template<typename... Args>
    void addMany(Args&&... args) {
        (this->add(std::forward<Args>(args)), ...);
    }

    // returns the number of elements in the aggregate
    [[nodiscard]] size_t size() const override {
        return m_items.size();
    }

    // accesses element at specified index
    AggregateType& at(size_t index) override {
        return m_items.at(index);
    }

    // const access to an element at specified index
    const AggregateType& at(size_t index) const override {
        return m_items.at(index);
    }
};

// concrete iterator implementation
template <typename IteratorType>
class ConcreteIterator final : public Iterator<IteratorType> {
private:
    // reference to the aggregate being traversed
    std::reference_wrapper<ConcreteAggregate<IteratorType>> m_aggregate;
    // current position in the traversal
    size_t m_current_index;

public:
    // constructor that takes the aggregate to iterate over
    explicit ConcreteIterator(ConcreteAggregate<IteratorType>& aggregate)
        : m_aggregate(aggregate), m_current_index(0) {}

    // positions iterator to the first element
    void first() override {
        m_current_index = 0;
    }

    // advances iterator to the next element
    void next() override {
        ++m_current_index;
    }

    // checks if traversal is complete
    [[nodiscard]] bool isDone() const override {
        return m_current_index >= m_aggregate.get().size();
    }

    // returns the current element
    IteratorType& current() override {
        if (this->isDone()) {
            throw std::out_of_range("Iterator out of bounds");
        }
        return m_aggregate.get().at(m_current_index);
    }
};

// implementation of createiterator for the concreteaggregate
template <typename AggregateType>
std::unique_ptr<Iterator<AggregateType>> ConcreteAggregate<AggregateType>::createIterator() {
    return std::make_unique<ConcreteIterator<AggregateType>>(*this);
}

// specialized reverse iterator implementation
template <typename ElementType>
class ReverseIterator final : public Iterator<ElementType> {
private:
    std::reference_wrapper<ConcreteAggregate<ElementType>> m_aggregate; // reference to the aggregate being traversed
    size_t m_current_index{}; // current position in the traversal

public:
    // constructor that takes the aggregate to iterate over
    explicit ReverseIterator(ConcreteAggregate<ElementType>& aggregate) : m_aggregate(aggregate) {
        // initialize position
        this->first();
    }

    // positions iterator to the last element (first in reverse order)
    void first() override {
        if (m_aggregate.get().size() > 0) {
            m_current_index = m_aggregate.get().size() - 1;
        } else {
            // empty collection case
            m_current_index = 0;
        }
    }

    // advances iterator to the previous element (next in reverse order)
    void next() override {
        if (m_current_index > 0) {
            --m_current_index;
        } else {
            // set to done state when we've passed the first element
            m_current_index = m_aggregate.get().size();
        }
    }

    // checks if traversal is complete
    [[nodiscard]] bool isDone() const override {
        return m_aggregate.get().size() == 0 || m_current_index >= m_aggregate.get().size();
    }

    // returns the current element
    ElementType& current() override {
        if (this->isDone()) {
            throw std::out_of_range("Reverse iterator out of bounds");
        }
        return m_aggregate.get().at(m_current_index);
    }
};

// specialized filtering iterator implementation
template <typename ElementType>
class FilteringIterator final : public Iterator<ElementType> {
private:
    // reference to the aggregate being traversed
    std::reference_wrapper<ConcreteAggregate<ElementType>> m_aggregate;
    // current position in the traversal
    size_t m_current_index;
    // filter predicate function
    std::function<bool(const ElementType&)> m_predicate;

    // finds the next element satisfying the predicate
    void findNextValid() {
        while (m_current_index < m_aggregate.get().size() && !m_predicate(m_aggregate.get().at(m_current_index))) {
            ++m_current_index;
        }
    }

public:
    // constructor that takes the aggregate and filter predicate
    FilteringIterator(ConcreteAggregate<ElementType>& aggregate, std::function<bool(const ElementType&)> predicate)
        : m_aggregate(aggregate), m_current_index(0), m_predicate(std::move(predicate)) {
        // initialize to the first valid element
        this->findNextValid();
    }

    // positions iterator to the first element that satisfies the predicate
    void first() override {
        m_current_index = 0;
        this->findNextValid();
    }

    // advances iterator to the next element that satisfies the predicate
    void next() override {
        ++m_current_index;
        this->findNextValid();
    }

    // checks if traversal is complete
    [[nodiscard]] bool isDone() const override {
        return m_current_index >= m_aggregate.get().size();
    }

    // returns the current element
    ElementType& current() override {
        if (this->isDone()) {
            throw std::out_of_range("Filtering iterator out of bounds");
        }
        return m_aggregate.get().at(m_current_index);
    }
};

#endif // iterator_pattern_hpp
