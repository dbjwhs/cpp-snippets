# Binary Search Tree Implementation

##  🚧 Under Construction  🚧

### Future Improvements Required

Before proceeding note the following is, and will be added soon.

1. Add removal operations
2. Implement balancing (AVL or Red-Black)
3. Add iterator support
4. Add serialization/deserialization (*mainly for data persistence to disk*)
5. Add range queries
6. And **finally**, remove hacky logging; currently logging is shoe horned in. The Best would be to add as an **observer pattern**, an observer of tree operations.
   7. Separates logging concerns from tree operations
   8. Makes it easy to add/remove logging at runtime 
   9. Allows for multiple observers (could have logging + metrics + etc.)
   10. Keeps the core tree operations clean and focused

##  🚧 Under Construction  🚧

## What is a Binary Search Tree (BST)?

A Binary Search Tree (BST) is a hierarchical data structure composed of nodes where each node stores a value and references to two children. BSTs enforce a specific ordering property:

- For any node N, all values in its left subtree are strictly less than N's value
- For any node N, all values in its right subtree are strictly greater than N's value
- No duplicate values are allowed

This ordering makes BSTs efficient for searching, as each comparison allows you to eliminate half of the remaining tree from consideration.

Example of a valid BST:
```
        5
      /   \
     3     7    // 3 < 5 < 7
    / \   / \
   2   4 6   8  // 2 < 3 < 4, 6 < 7 < 8
```

## Performance Characteristics

Time Complexities:
- Search: O(log n) - when balanced
- Insert: O(log n) - when balanced
- Delete: O(log n) - when balanced
- Traversal: O(n) - must visit all nodes

where n is the number of nodes in the tree.

Note: These O(log n) complexities assume the tree is relatively balanced. In the worst case (completely unbalanced tree), operations can degrade to O(n).

Space Complexity:
- O(n) for storing n nodes
- O(h) for recursive operations (where h is tree height)

## Use Cases

BSTs are commonly used in:
1. Implementing Symbol Tables and Dictionaries
2. Database Indexing
3. File System Organization
4. Auto-complete and Spell Checkers
5. Priority Queues
6. Expression Parsers

## Implementation Details

### Key Features
- Template-based implementation supporting any comparable type
- Smart pointer (std::unique_ptr) memory management
- Comprehensive traversal options (inorder, preorder, postorder)
- BST property validation
- Support for any type that implements operator<

### Memory Safety
- Uses std::unique_ptr for automatic memory management
- No manual deletion required
- Exception-safe operations
- RAII compliant

### Traversal Options
Three traversal methods are provided:
1. Inorder (Left-Root-Right) - visits nodes in ascending order
2. Preorder (Root-Left-Right) - useful for copying trees
3. Postorder (Left-Right-Root) - useful for deletion

### Visitor Pattern
Traversals use the visitor pattern allowing flexible node processing:
```cpp
tree.inorderTraversal([](const auto& value) {
    // Process value here
});
```

## Usage Examples

### Basic Operations
```cpp
BinaryTree<int> tree;

// Insert values
tree.insert(5);
tree.insert(3);
tree.insert(7);

// Search
bool found = tree.search(3);  // returns true

// Find min/max
int min = tree.findMinValue();  // returns 3
int max = tree.findMaxValue();  // returns 7
```

### Working with Strings
```cpp
BinaryTree<std::string> string_tree;

string_tree.insert("hello");
string_tree.insert("world");
string_tree.insert("abc");

// Traversal with custom processing
string_tree.inorderTraversal([](const std::string& s) {
    std::cout << s << " ";  // prints: "abc hello world"
});
```

## Implementation Notes

### BST Property Validation
- Use std::optional for bound checking
- Works with any comparable type
- Validates entire tree structure recursively

### Memory Management
- Automatic cleanup with smart pointers
- Deep copying for tree duplication
- No memory leaks by design

### Generic Type Support
- Works with any type supporting operator<
- No requirement for operator==
- Consistent behavior across different types

## Best Practices
1. Check for an empty tree before operations
2. Use the appropriate traversal for your use case
3. Consider balance if performance critical
4. Use custom comparators for complex types

## Thread Safety
This implementation is not thread-safe. External synchronization is required for concurrent operations.

## Error Handling
- Throws std::runtime_error for operations on empty trees
- Returns false for failed searches
- Silently ignores duplicate insertions

## License
This project is licensed under the MIT License - see the LICENSE file for details.

## Contributing
Contributions are welcome! Please feel free to submit pull requests with improvements or bug fixes.
