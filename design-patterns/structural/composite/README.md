# Composite Pattern Implementation

The Composite pattern is a structural design pattern that was introduced in 1994 as part of the influential "Gang of Four" design patterns.
It enables the creation of tree-like structures where individual objects and compositions of objects can be treated uniformly. Originally
developed during the early days of object-oriented programming in Smalltalk, this pattern has become fundamental in modern software
architecture. The pattern's elegance lies in its ability to represent hierarchical structures while maintaining a consistent interface for
both simple and complex elements.

## Use Cases and Problem Solutions

The Composite pattern effectively solves several common software design challenges. In graphical user interfaces, it allows containers to
hold both simple elements (buttons, text fields) and other containers, creating complex layouts with consistent handling. File systems use
this pattern to manage directories that can contain both files and other directories. Document object models (DOM) in web browsers rely on
this pattern to represent HTML/XML structures where elements can contain both text and other elements. Business software uses it to model
organizational hierarchies where departments can contain both employees and sub-departments.

### Common Applications

- File System Navigation and Management
- GUI Component Hierarchies
- Abstract Syntax Trees in Compilers
- Organization Charts and Business Hierarchies
- Graphics Systems and Scene Graphs
- Menu Systems with Nested Submenus
- Task Management with Subtasks

## Implementation Examples

### Basic Structure
```cpp
class Component {
    virtual void operation() = 0;
    virtual void add(Component*) = 0;
    virtual void remove(Component*) = 0;
};
```

### Real-World Example
```cpp
// File System Implementation showing practical usage of the Composite Pattern
class FileSystemItem {
protected:
    std::string m_name;
    std::string m_path;
    
public:
    FileSystemItem(const std::string& name, const std::string& path) 
        : m_name(name), m_path(path) {}
    
    virtual ~FileSystemItem() = default;
    virtual size_t getSize() const = 0;
    virtual void print(int indent = 0) const = 0;
    virtual bool isDirectory() const = 0;
    
    std::string getName() const { return m_name; }
    std::string getPath() const { return m_path; }
};

// Leaf class representing a file
class File : public FileSystemItem {
private:
    size_t m_size;
    std::string m_extension;

public:
    File(const std::string& name, const std::string& path, size_t size) 
        : FileSystemItem(name, path), m_size(size) {
        auto pos = name.find_last_of('.');
        m_extension = (pos != std::string::npos) ? name.substr(pos + 1) : "";
    }

    size_t getSize() const override { return m_size; }
    bool isDirectory() const override { return false; }
    
    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') 
                  << m_name << " (Size: " << m_size << " bytes)\n";
    }
};

// Composite class representing a directory
class Directory : public FileSystemItem {
private:
    std::vector<std::shared_ptr<FileSystemItem>> m_contents;

public:
    Directory(const std::string& name, const std::string& path) 
        : FileSystemItem(name, path) {}

    void addItem(const std::shared_ptr<FileSystemItem>& item) {
        m_contents.push_back(item);
    }

    void removeItem(const std::string& name) {
        m_contents.erase(
            std::remove_if(m_contents.begin(), m_contents.end(),
                [&name](const auto& item) { return item->getName() == name; }),
            m_contents.end());
    }

    size_t getSize() const override {
        return std::accumulate(m_contents.begin(), m_contents.end(), 0ULL,
            [](size_t sum, const auto& item) { return sum + item->getSize(); });
    }

    bool isDirectory() const override { return true; }

    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << m_name << " (Directory)\n";
        for (const auto& item : m_contents) {
            item->print(indent + 2);
        }
    }
};

// Example usage:
void fileSystemExample() {
    // Create root directory
    auto root = std::make_shared<Directory>("root", "/");
    
    // Add some files to root
    root->addItem(std::make_shared<File>("config.json", "/config.json", 2048));
    root->addItem(std::make_shared<File>("readme.md", "/readme.md", 1024));
    
    // Create a subdirectory
    auto docs = std::make_shared<Directory>("docs", "/docs");
    docs->addItem(std::make_shared<File>("manual.pdf", "/docs/manual.pdf", 5120));
    docs->addItem(std::make_shared<File>("api.md", "/docs/api.md", 3072));
    
    // Add subdirectory to root
    root->addItem(docs);
    
    // Print entire directory structure
    root->print();
    
    // Calculate total size
    std::cout << "Total size: " << root->getSize() << " bytes\n";
}
```

This implementation demonstrates:
- Proper memory management using smart pointers
- Clear separation between leaf (File) and composite (Directory) classes
- Practical methods like size calculation and structure printing
- Real-world considerations like file extensions and path handling
- Recursive operations through the directory structure
- Type-safe operations with virtual methods

## Advantages

1. **Uniform Treatment**: Clients can treat complex and primitive objects identically
2. **Extensibility**: New component types can be added without changing existing code
3. **Natural Hierarchy**: Represents recursive structures in a natural, intuitive way
4. **Flexible Construction**: Hierarchies can be built dynamically at runtime

## Disadvantages

1. **Over-Generalization**: Can make design overly general when simpler structures would suffice
2. **Type Safety**: Can be challenging to restrict component types in strongly-typed languages
3. **Component Management**: Memory management can become complex in deep hierarchies
4. **Performance**: Deep hierarchical operations might impact performance

## Best Practices

1. Keep component interface simple and focused
2. Use smart pointers for memory management in C++
3. Consider implementing visitor pattern for operations across the hierarchy
4. Implement clear error handling for invalid operations
5. Document component lifecycle and ownership rules

## Additional Resources

### Books
- "Design Patterns: Elements of Reusable Object-Oriented Software" by Gamma, Helm, Johnson, Vlissides
- "Head First Design Patterns" by Freeman & Freeman
- "Pattern-Oriented Software Architecture" by Buschmann, Meunier, Rohnert, Sommerlad, Stal

### Online Resources
- [RefactoringGuru - Composite Pattern](https://refactoring.guru/design-patterns/composite)
- [SourceMaking - Composite Pattern](https://sourcemaking.com/design_patterns/composite)

## Performance Considerations

When implementing the Composite pattern, consider these performance aspects:
- Depth of recursion in large hierarchies
- Memory overhead of component containers
- Impact of virtual function calls
- Cache coherency in deep structures

## Testing Strategies

1. Unit test individual components
2. Test hierarchical operations
3. Verify memory management
4. Test edge cases and error conditions
5. Performance testing for large structures

## License
This code is provided under the MIT License. Feel free to use, modify, and distribute as needed.

## Contributing
Contributions are welcome! Please feel free to submit a Pull Request.
