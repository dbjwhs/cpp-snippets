# Iterator Design Pattern

The Iterator Pattern is a behavioral design pattern that provides a way to sequentially access elements of an aggregate
object without exposing its underlying representation. It decouples algorithms from container implementations, enabling
traversal of different data structures using a uniform interface. The pattern emerged in the 1970s with the CLU programming
language, which first introduced the concept of iterators. It was later formalized in the seminal "Design Patterns:
Elements of Reusable Object-Oriented Software" (1994) by Erich Gamma, Richard Helm, Ralph Johnson, and John Vlissides
(known as the Gang of Four). The pattern has since become a fundamental concept in modern programming languages, with many
standard libraries implementing iterators as a core feature for collection traversal.

## Problem & Solution

The Iterator Pattern addresses several common problems in software design. Without iterators, clients need intimate
knowledge of container internals to traverse elements, creating tight coupling between algorithms and data structures. When
container implementations change, client code breaks. Additionally, different traversal strategies (forward, backward,
filtered) would pollute container classes with unrelated traversal code. The Iterator Pattern solves these issues by
extracting traversal behavior into separate iterator objects with a standard interface. This enables clients to traverse
collections without knowing their internal structure, supports multiple iterators on the same collection simultaneously,
and allows for specialized traversal strategies without modifying the original containers.

## Structure & Implementation

This implementation provides a robust, RAII-compliant C++ version of the Iterator Pattern with the following components:

- `Iterator`: Abstract interface defining traversal operations (first, next, isDone, current)
- `Aggregate`: Interface for collections that can create iterators
- `ConcreteAggregate`: Concrete collection implementation
- `ConcreteIterator`: Standard forward iterator implementation
- `ReverseIterator`: Specialized iterator for backward traversal
- `FilteringIterator`: Specialized iterator that filters elements based on a predicate

```cpp
// Example usage:
auto collection = std::make_unique<ConcreteAggregate<int>>();
collection->addMany(1, 2, 3, 4, 5);

// Forward iteration
auto iterator = collection->createIterator();
for (iterator->first(); !iterator->isDone(); iterator->next()) {
    std::cout << iterator->current() << std::endl;
}

// Reverse iteration
auto reverseIterator = std::make_unique<ReverseIterator<int>>(*collection);
for (reverseIterator->first(); !reverseIterator->isDone(); reverseIterator->next()) {
    std::cout << reverseIterator->current() << std::endl;
}

// Filtered iteration (even numbers only)
auto isEven = [](const int& n) { return n % 2 == 0; };
auto filteringIterator = std::make_unique<FilteringIterator<int>>(*collection, isEven);
for (filteringIterator->first(); !filteringIterator->isDone(); filteringIterator->next()) {
    std::cout << filteringIterator->current() << std::endl;
}
```

## Advantages

- **Decoupling**: Separates traversal algorithms from container implementations
- **Single Responsibility**: Collections focus on storing data while iterators handle traversal
- **Multiple Traversals**: Supports concurrent iterations over the same collection
- **Specialized Iterations**: Enables different traversal strategies without modifying collections
- **Uniformity**: Provides a consistent interface for traversing different data structures

## Disadvantages

- **Additional Classes**: Increases the number of classes in the system
- **Complexity**: May be overkill for simple collections with straightforward traversal needs
- **Iterator Invalidation**: Changes to collections during iteration can invalidate iterators if not carefully managed
- **State Management**: Iterators must maintain position state, which can be error-prone

## Real-World Applications

- Standard Template Library (STL) containers and their iterators in C++
- Java Collections Framework iterators
- File system directory/file traversal

## Further Reading

- **Design Patterns: Elements of Reusable Object-Oriented Software** by Gamma, Helm, Johnson, Vlissides
- **Modern C++ Design: Generic Programming and Design Patterns Applied** by Andrei Alexandrescu
- **Effective STL: 50 Specific Ways to Improve Your Use of the Standard Template Library** by Scott Meyers

## License

This code is provided under the MIT License. Feel free to use, modify, and distribute as needed.

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.