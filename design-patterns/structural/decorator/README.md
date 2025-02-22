# C++ Decorator Pattern Implementation

The Decorator pattern is a structural design pattern that was first introduced in the influential "Gang of Four" (GoF) Design Patterns
book published in 1994. It allows behavior to be added to individual objects dynamically without affecting the behavior of other objects
from the same class. The pattern provides a flexible alternative to subclassing for extending functionality, following the Open-Closed
Principle where software entities should be open for extension but closed for modification. This pattern creates a set of decorator
classes that are used to wrap concrete components, enabling a more modular and maintainable approach to object enhancement.

## Use Cases and Problem Solving

The Decorator pattern effectively solves several common software design challenges. It's particularly useful when you need to add
responsibilities to objects dynamically and transparently, without affecting other objects. Common applications include building complex
user interfaces, where components need various combinations of borders, scrollbars, or behaviors; in I/O stream libraries, where
different combinations of buffering or compression might be needed; and in middleware systems, where cross-cutting concerns like logging,
transaction handling, or security can be added flexibly to core components.

## Implementation Details

The implementation consists of four key components:
- Component (TextComponent): Defines the interface for objects that can have responsibilities added
- Concrete Component (SimpleText): Defines the basic object to which responsibilities can be added
- Decorator (TextDecorator): Maintains a reference to a Component and defines an interface matching Component's interface
- Concrete Decorators (BoldDecorator, ItalicDecorator, UnderlineDecorator): Add responsibilities to the component

### Example Usage

```cpp
auto text = std::make_shared<SimpleText>("Hello");
auto decoratedText = std::make_shared<BoldDecorator>(
    std::make_shared<ItalicDecorator>(text)
);
std::cout << decoratedText->render(); // Outputs: <b><i>Hello</i></b>
```

## Advantages

1. Provides greater flexibility than static inheritance
2. Avoids feature-laden classes high up in the hierarchy
3. Supports the Single Responsibility Principle
4. Allows responsibilities to be added or removed at runtime
5. Enables combining multiple behaviors through nested wrapping

## Disadvantages

1. Can result in many small objects that complicate the system
2. Decorators can lead to order dependency when multiple decorators are used
3. Initial configuration of layers can be complex
4. Can be difficult to understand for developers unfamiliar with the pattern

## Best Practices

- Keep decorator classes lightweight
- Ensure consistent interface across all decorators
- Consider using a factory pattern for creating decorated objects
- Document the expected order of decorators when order matters
- Use meaningful names that reflect the added behavior

## Notable Books and Resources

1. "Design Patterns: Elements of Reusable Object-Oriented Software" by Gamma, Helm, Johnson, and Vlissides
    - The original source of the pattern, provides comprehensive explanation and use cases

2. "Head First Design Patterns" by Freeman, Robson, Bates, and Sierra
    - Offers a more practical and visual approach to understanding the pattern

3. "Modern C++ Design" by Andrei Alexandrescu
    - Demonstrates advanced implementations using modern C++ features

## Common Pitfalls to Avoid

1. Over-engineering: Don't use decorators when simple inheritance would suffice
2. Deep nesting: Avoid creating too many layers of decorators
3. State management: Be careful with decorators that maintain state
4. Performance considerations: Consider the overhead of multiple wrapper objects

## Real-World Examples

- Java I/O Streams (BufferedInputStream, LineNumberInputStream)
- GUI Toolkits (BorderDecorator, ScrollDecorator)
- Web Service Layers (CachingDecorator, LoggingDecorator)
- Middleware Components (TransactionDecorator, SecurityDecorator)

## License
This code is provided under the MIT License. Feel free to use, modify, and distribute as needed.

## Contributing
Contributions are welcome! Please feel free to submit a Pull Request.
