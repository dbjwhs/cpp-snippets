# C++ Factory Pattern Implementation

This project demonstrates two implementations of the Factory Pattern in C++: a static factory and a configurable instance-based factory.

## Overview

The project includes:
- An abstract `Animal` base class
- Concrete animal implementations (Dog, Cat, Bird)
- Two factory implementations:
    - Static `AnimalFactory`
    - Instance-based `ConfigurableAnimalFactory`

## Key Features

### Static Factory
- Provides a static creation method
- No instance state
- Simple to use
- Thread-safe for creation

### Configurable Factory
- Instance-based with configuration
- Supports different creation behaviors
- Demonstrates when to use non-static factories
- Maintains instance state

## Code Examples

### Static Factory Usage
```cpp
auto dog = AnimalFactory::createAnimal("dog");
dog->makeSound();
dog->move();
```

### Configurable Factory Usage
```cpp
// Create a noisy factory
ConfigurableAnimalFactory noisyFactory(true);
auto dog = noisyFactory.createAnimal("dog");  // Makes sound on creation

// Create a quiet factory
ConfigurableAnimalFactory quietFactory(false);
auto cat = quietFactory.createAnimal("cat");  // Silent creation
```

## Important Implementation Notes

### [[nodiscard]] Usage
The factory methods are marked with `[[nodiscard]]` to prevent accidental resource leaks:

```cpp
// Static factory
[[nodiscard]] static std::unique_ptr<Animal> createAnimal(const std::string& animalType);

// Configurable factory
[[nodiscard]] std::unique_ptr<Animal> createAnimal(const std::string& animalType) const;
```

This prevents code like:
```cpp
factory.createAnimal("dog");  // Warning: discarding return value
```

Correct usage:
```cpp
auto dog = factory.createAnimal("dog");  // Properly capturing the return value
```

### Const Correctness
The `ConfigurableAnimalFactory::createAnimal` method is marked as `const` because:
- It doesn't modify the factory's internal state
- Allows the method to be called on const objects
- Expresses design intent that this is a query operation

## Dependencies

Required headers:
- `<memory>` for std::unique_ptr
- `<string>` for std::string
- `<stdexcept>` for std::runtime_error
- `<iostream>` for output operations

## Error Handling

Both factories throw `std::runtime_error` for unknown animal types:
```cpp
try {
    auto unknown = AnimalFactory::createAnimal("fish");
} catch (const std::runtime_error& e) {
    std::cout << "Error: " << e.what() << std::endl;
}
```

## Final Classes

Concrete animal classes are marked as `final` to:
- Prevent unintended inheritance
- Make the design intent clear (these classes are not meant to be base classes)
- Enable potential compiler optimizations
- Maintain the factory pattern's encapsulation

Example:
```cpp
class Dog final : public Animal {
    // Implementation
};
```

## Best Practices Demonstrated

1. RAII through std::unique_ptr
2. Const correctness
3. [[nodiscard]] for resource management
4. Strong exception guarantees
5. Virtual destructor in base class
6. Override keyword for virtual functions
7. Final classes for concrete implementations

## When to Use Each Factory

### Use Static Factory When:
- No instance state is needed
- Single creation pathway is sufficient
- Global access is desired

### Use Configurable Factory When:
- Creation behavior needs configuration
- Factory needs to maintain state
- Different factory instances need different behaviors

## License

This code is provided under the MIT License. Feel free to use, modify, and distribute as needed.

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.
