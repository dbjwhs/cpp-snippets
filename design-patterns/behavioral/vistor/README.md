# Visitor Design Pattern Implementation in C++

The Visitor pattern is a behavioral design pattern that allows you to separate algorithms from the objects on which they
operate. It was first introduced in the influential "Gang of Four" book (Design Patterns: Elements of Reusable
Object-Oriented Software) by Erich Gamma, Richard Helm, Ralph Johnson, and John Vlissides, published in 1994. The pattern
enables adding new operations to existing object structures without modifying those structures. It represents a way to
implement "double dispatch" in languages like C++ that don't natively support it, meaning the operation performed depends
on both the type of the visitor and the type of the element being visited.

## Use Cases and Problem Solutions

The Visitor pattern is particularly useful when you have a stable hierarchy of classes and need to perform various
operations on them that don't belong in the class hierarchy itself. It addresses several common problems in
object-oriented design: how to add functionality to a class hierarchy without changing it, how to keep related operations
together, and how to perform operations that span multiple unrelated classes. This pattern is ideal when you want to
perform operations across a disparate set of objects with different interfaces, enabling these operations to be changed
independently from the objects they work on. Use cases include compilers (for traversing abstract syntax trees), document
object models (for XML/HTML processing), and complex report generation systems.

## Core Components

### Visitor Interface
```cpp
class ShapeVisitor {
public:
    virtual ~ShapeVisitor() = default;
    virtual void visit(const Circle& circle) = 0;
    virtual void visit(const Square& square) = 0;
    virtual void visit(const Triangle& triangle) = 0;
};
```

### Element Interface
```cpp
class Shape {
public:
    virtual ~Shape() = default;
    virtual void accept(ShapeVisitor& visitor) const = 0;
    virtual std::string getName() const = 0;
};
```

### Concrete Elements
```cpp
class Circle : public Shape {
    // Implementation
};

class Square : public Shape {
    // Implementation
};

class Triangle : public Shape {
    // Implementation
};
```

### Concrete Visitors
```cpp
class AreaVisitor : public ShapeVisitor {
    // Implementation
};

class PerimeterVisitor : public ShapeVisitor {
    // Implementation
};
```

## Implementation Details

This implementation showcases a geometric shape processing system. It includes:
- A shape hierarchy (Circle, Square, Triangle)
- Multiple visitors (AreaVisitor, PerimeterVisitor, DescriptionVisitor)
- Comprehensive testing in the main() function

## Heron's Formula for Triangle Area

In the AreaVisitor, we use Heron's formula to calculate the area of a triangle. Named after Heron of Alexandria, this
formula calculates the area of a triangle when the lengths of all three sides are known, without needing to calculate the
height or angles.

The formula states:
```
Area = √(s(s-a)(s-b)(s-c))
```

Where:
- a, b, c are the lengths of the sides of the triangle
- s is the semi-perimeter: s = (a + b + c)/2

This elegant formula works for any triangle and is particularly useful in the Visitor pattern context as it allows us to
compute the area using only the properties exposed by the Triangle class, demonstrating how visitors can perform complex
calculations specific to each element type.

## Advantages

1. **Open/Closed Principle**: New operations can be added without modifying the element classes
2. **Single Responsibility Principle**: Each visitor encapsulates a specific algorithm or operation
3. **Consolidation of Related Operations**: Related behaviors are kept together in visitor classes
4. **Type Safety**: The pattern provides type-safe operations across heterogeneous object collections

## Disadvantages

1. **Breaks Encapsulation**: Visitors may need access to the internal details of elements
2. **Rigidity in Element Hierarchy**: Adding new element types requires updating all visitor interfaces
3. **Complexity**: Can lead to complex designs if overused
4. **Runtime Overhead**: Double dispatch mechanism adds some overhead

## Usage Example

```cpp
// Create shapes
auto circle = std::make_unique<Circle>(5.0);
auto square = std::make_unique<Square>(4.0);

// Create visitors
AreaVisitor areaVisitor;
PerimeterVisitor perimeterVisitor;

// Apply visitors to shapes
circle->accept(areaVisitor);
square->accept(perimeterVisitor);

// Get results
double circleArea = areaVisitor.getArea();
double squarePerimeter = perimeterVisitor.getPerimeter();
```

## Related Patterns

- **Composite Pattern**: Visitor is often used with Composite to operate on complex object structures
- **Iterator Pattern**: Visitors commonly use Iterators to traverse object structures
- **Command Pattern**: Both separate operations from objects, but in different ways

## Further Reading

1. "Design Patterns: Elements of Reusable Object-Oriented Software" by Gamma, Helm, Johnson, Vlissides
2. "Modern C++ Design" by Andrei Alexandrescu
3. "C++ Templates: The Complete Guide" by David Vandevoorde and Nicolai M. Josuttis

## License

This code is provided under the MIT License. Feel free to use, modify, and distribute as needed.

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.