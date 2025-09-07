# Rule of Five - Comprehensive C++ Resource Management

The Rule of Five is a fundamental C++ idiom that emerged from the evolution of resource management best practices in 
modern C++. Originally derived from the Rule of Three established in the early 1990s by Bjarne Stroustrup, this 
pattern addresses the critical need for proper resource management in classes that handle dynamic resources. The Rule 
of Three stated that if a class needs a custom destructor, copy constructor, or copy assignment operator, it likely 
needs all three. However, with the introduction of move semantics in C++11 (2011), the landscape changed dramatically. 
The Rule of Five extends the original concept by including move constructor and move assignment operator, creating a 
comprehensive framework for resource management that balances safety, performance, and correctness in modern C++.

The Rule of Five addresses several critical problems in C++ resource management including memory leaks, double 
deletion, dangling pointers, and performance inefficiencies from unnecessary deep copies. When classes manage 
resources directly (such as raw pointers, file handles, network connections, or other system resources), the 
compiler-generated default implementations of special member functions are often inadequate. The default destructor 
may not properly clean up resources, leading to leaks. Default copy operations perform shallow copies, which can 
result in multiple objects attempting to manage the same resource, causing double deletion errors. Without move 
semantics, temporary objects and function returns involve expensive deep copies even when the source object is about 
to be destroyed. The Rule of Five ensures that all five special member functions work together harmoniously to provide 
safe resource transfer, efficient performance through move semantics, and robust exception safety guarantees.

## Core Components

The Rule of Five consists of these five special member functions:

### 1. Destructor (`~Class()`)
```cpp
~ResourceManager() noexcept {
    LOG_INFO_PRINT("destructor called for resource manager {}", m_id);
    deallocate_memory();
}
```

Responsible for cleaning up all resources when an object is destroyed. Must be `noexcept` and handle all cleanup 
operations safely.

### 2. Copy Constructor (`Class(const Class&)`)
```cpp
ResourceManager(const ResourceManager& other) noexcept
    : m_size{other.m_size}
    , m_capacity{other.m_capacity}
    , m_id{utils::generate_guid()} {
    // Deep copy implementation with error handling
    if (other.m_data != nullptr && other.m_capacity > 0) {
        auto allocation_result = allocate_memory(other.m_capacity);
        if (allocation_result) {
            m_data = *allocation_result;
            copy_data_from(other);
        }
    }
}
```

Creates a new object as an independent copy of another object. Must perform deep copying to avoid resource sharing 
issues.

### 3. Copy Assignment Operator (`Class& operator=(const Class&)`)
```cpp
ResourceManager& operator=(const ResourceManager& other) noexcept {
    if (this == &other) return *this; // Self-assignment check
    
    ResourceManager temp{other};      // Copy-and-swap idiom
    swap(temp);                       // Strong exception safety
    return *this;
}
```

Assigns one existing object to another. Must handle self-assignment and provide strong exception safety guarantee.

### 4. Move Constructor (`Class(Class&&)`)
```cpp
ResourceManager(ResourceManager&& other) noexcept
    : m_data{std::exchange(other.m_data, nullptr)}
    , m_size{std::exchange(other.m_size, 0)}
    , m_capacity{std::exchange(other.m_capacity, 0)}
    , m_id{utils::generate_guid()} {
    // Efficient resource transfer, no copying required
}
```

Efficiently transfers resources from a temporary or explicitly moved object. Leaves the source in a valid but 
unspecified state.

### 5. Move Assignment Operator (`Class& operator=(Class&&)`)
```cpp
ResourceManager& operator=(ResourceManager&& other) noexcept {
    if (this == &other) return *this; // Self-assignment check
    
    deallocate_memory();              // Clean up current resources
    
    // Transfer resources from source
    m_data = std::exchange(other.m_data, nullptr);
    m_size = std::exchange(other.m_size, 0);
    m_capacity = std::exchange(other.m_capacity, 0);
    
    return *this;
}
```

Efficiently transfers resources between existing objects. Provides optimal performance for assignments from temporaries.

## Usage Examples

### Basic Usage
```cpp
#include "headers/resource_manager.hpp"

// Default construction
ResourceManager<int> rm1;

// Parameterized construction (creates capacity of 100, size starts at 0)
ResourceManager<int> rm2{100};

// Copy operations
ResourceManager<int> rm3 = rm2;  // Copy constructor
rm1 = rm2;                       // Copy assignment

// Move operations (efficient)
ResourceManager<int> rm4 = std::move(rm2);  // Move constructor
rm3 = create_temporary_resource();          // Move assignment
```

### Working with Different Types
```cpp
// String resource manager
ResourceManager<std::string> string_rm{10};
auto result = string_rm.push_back("Hello World");

// Double resource manager
ResourceManager<double> double_rm{5};
auto result2 = double_rm.push_back(3.14159);

// Custom type resource manager
ResourceManager<MyCustomClass> custom_rm{20};
```

### Error Handling with Railway-Oriented Programming
```cpp
auto rm = ResourceManager<int>{5};

// Safe element access
auto access_result = rm.at(2);
if (access_result) {
    std::print("Value: {}\n", *access_result);
} else {
    std::print("Error: {}\n", error_to_string(access_result.error()));
}

// Safe element insertion
auto push_result = rm.push_back(42);
if (!push_result) {
    std::print("Failed to add element: {}\n", error_to_string(push_result.error()));
}
```

## Best Practices

### ✅ Good Practices
- Always implement all five functions together when resource management is needed
- Use `noexcept` specification for destructors and move operations
- Implement copy-and-swap idiom for strong exception safety
- Check for self-assignment in assignment operators
- Use `std::exchange` for clean move implementations
- Prefer Railway-Oriented Programming with `std::expected` over exceptions
- Use modern C++23 concepts for template parameters

### ❌ Common Pitfalls
```cpp
// Don't do this - incomplete rule implementation
class BadResource {
    int* data;
public:
    BadResource(size_t size) : data(new int[size]) {}
    ~BadResource() { delete[] data; }
    // Missing copy/move operations leads to double-delete!
};

// Don't do this - shallow copy
BadResource(const BadResource& other) : data(other.data) {} // Dangerous!

// Don't do this - forgetting self-assignment check
BadResource& operator=(const BadResource& other) {
    delete[] data;              // Oops! What if this == &other?
    data = new int[other.size];
    return *this;
}
```

### Modern C++ Integration
```cpp
template<std::copyable ElementType>
requires std::is_default_constructible_v<ElementType>
class ModernResourceManager {
    // Implementation using concepts for better error messages
    // and self-documenting interfaces
};
```

## Performance Considerations

The Rule of Five provides significant performance benefits through move semantics:

```cpp
// Expensive copy (Rule of Three era)
ResourceManager<int> expensive = create_large_resource();  // Deep copy

// Efficient move (Rule of Five era)  
ResourceManager<int> efficient = std::move(create_large_resource());  // Resource transfer

// Function returns are automatically moved in C++11+
ResourceManager<int> automatic = factory_function();  // No explicit std::move needed
```

## Testing

The implementation includes comprehensive testing:

```bash
# Build the project
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)

# Run demonstrations
./rule_of_five_demo

# Run unit tests
./rule_of_five_tests

# Or use CTest
ctest --output-on-failure --verbose
```

## Related Patterns and Evolution

### Rule of Zero
Modern C++ advocates for the "Rule of Zero" - well-designed classes should not need custom implementations of these 
functions, instead relying on smart pointers and RAII wrappers:

```cpp
class ModernClass {
    std::unique_ptr<int[]> data;    // Automatic resource management
    std::size_t size;
    // Compiler-generated special members are sufficient
};
```

### Historical Context
- **1991**: Rule of Three established with early C++
- **2003**: Exception safety and RAII principles formalized
- **2011**: C++11 introduces move semantics, leading to Rule of Five
- **2014**: Rule of Zero becomes widely adopted
- **2020**: C++20 concepts enable better template constraints
- **2025**: Modern implementations use Railway-Oriented Programming patterns

## Books and References

Several influential books have covered the Rule of Five and its evolution:

- **"Effective Modern C++" by Scott Meyers (2014)** - Comprehensive coverage of move semantics and the Rule of Five
- **"C++ Coding Standards" by Herb Sutter and Andrei Alexandrescu (2004)** - Early formalization of resource 
  management principles
- **"Exceptional C++" by Herb Sutter (1999)** - Foundation principles that led to the Rule of Five
- **"The C++ Programming Language" by Bjarne Stroustrup (4th Edition, 2013)** - Authoritative coverage of move 
  semantics
- **"Professional C++" by Marc Gregoire (5th Edition, 2021)** - Modern practical applications
- **"C++17 - The Complete Guide" by Nicolai M. Josuttis (2017)** - Evolution of resource management in modern C++
- **"C++20 - The Complete Guide" by Nicolai M. Josuttis (2022)** - Latest standards and best practices

## Advanced Topics

### Copy-and-Swap Idiom
```cpp
ResourceManager& operator=(const ResourceManager& other) {
    ResourceManager temp(other);  // May throw
    swap(temp);                   // Never throws
    return *this;                 // Strong exception safety
}
```

### SFINAE and Concepts
```cpp
template<typename T>
requires std::is_copy_constructible_v<T> && std::is_move_constructible_v<T>
class ConceptConstrainedResource {
    // Implementation automatically gets optimal copy/move behavior
};
```

## License

This code is provided under the MIT License. Feel free to use, modify, and distribute as needed.

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.