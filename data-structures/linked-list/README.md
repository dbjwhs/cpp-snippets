# Modern C++ Linked List Implementation

A robust, template-based linked list implementation in modern C++ that emphasizes type safety, memory safety, and flexibility. This implementation uses smart pointers for automatic memory management and supports any data type that satisfies basic requirements.

## Features

- Template-based implementation with descriptive template parameter naming (`DataType` instead of the conventional `T`)
- Smart pointer (`std::unique_ptr`) usage for automatic memory management
- Move semantics support for efficient data transfer
- Comprehensive iterator support
- Exception-safe operations
- Copy and move construction/assignment
- Thread-safe const operations

## Usage

### Basic Operations

```cpp
// Create a list of integers
LinkedList<int> numberList;

// Add elements
numberList.add(1);
numberList.add(2);
numberList.add(3);

// Insert at specific position (zero-based index)
numberList.insertAt(42, 1);  // Inserts 42 at index 1

// Remove elements
numberList.remove(2);       // Removes first occurrence of 2
numberList.removeAt(0);     // Removes element at index 0

// Search
bool found = numberList.search(42);  // Returns true if 42 is in the list

// Access elements
int value;
if (numberList.get(0, value)) {
    std::cout << "First element: " << value << std::endl;
}
```

### Using Custom Types

The LinkedList class works with any type that:
- Is copy constructible
- Is move constructible (for optimal performance)
- Has an equality operator (for search operations)

Example with a custom type:

```cpp
struct Customer {
    std::string name;
    int id;
    
    bool operator==(const Customer& other) const {
        return id == other.id;
    }
};

LinkedList<Customer> customerList;
customerList.add({"John Doe", 1});
customerList.add({"Jane Smith", 2});
```

### Iterator Usage

```cpp
LinkedList<std::string> stringList;
stringList.add("Hello");
stringList.add("World");

// Iterate through the list
stringList.reset();  // Reset iterator to beginning
std::string value;
while (stringList.getNext(value)) {
    std::cout << value << " ";
}
```

## Memory Management

This implementation uses `std::unique_ptr` for automatic memory management, which means:
- No manual memory management required
- No memory leaks
- Exception-safe resource handling
- Clear ownership semantics

## Performance Considerations

- Adding elements at the end: O(n)
- Inserting at a position: O(n)
- Removing elements: O(n)
- Searching: O(n)
- Accessing by index: O(n)
- Memory overhead: One pointer per element

## Thread Safety

- Const operations are thread-safe
- Multiple readers are safe
- Concurrent modifications require external synchronization

## Requirements

- C++17 or later
- Standard Template Library (STL)
- Support for smart pointers

## Example Implementation Details

```cpp
template <typename DataType>
class LinkedList {
    struct Node {
        DataType data;
        std::unique_ptr<Node> next;
        explicit Node(DataType value) : data(std::move(value)), next(nullptr) {}
    };
    
    std::unique_ptr<Node> m_head;
    Node* m_current;
    size_t m_size;
    // ... implementation details
};
```

## Design Decisions

1. Template Parameter Naming
    - Uses `DataType` instead of `T` for better code readability
    - Makes the purpose and usage of the template parameter immediately clear
    - Follows the principle that code should be self-documenting

2. Smart Pointer Usage
    - `std::unique_ptr` enforces single ownership
    - Automatic cleanup prevents memory leaks
    - Modern C++ best practices

3. Member Variable Naming
    - `m_` prefix clearly identifies class members
    - Improves code readability and maintainability
    - Makes scope and ownership clear

## Best Practices

1. Always check return values for operations that might fail:
   ```cpp
   if (!list.insertAt(value, position)) {
       // Handle failure
   }
   ```

2. Use appropriate value types:
   ```cpp
   // Good: Efficient for small types
   LinkedList<int> intList;
   
   // Good: Moves large objects efficiently
   LinkedList<std::vector<int>> vectorList;
   ```

3. Reset iterator when needed:
   ```cpp
   list.reset();  // Before starting iteration
   ```

## License
This project is licensed under the MIT License - see the LICENSE file for details.

## Contributing
Contributions are welcome! Please feel free to submit pull requests with improvements or bug fixes.
