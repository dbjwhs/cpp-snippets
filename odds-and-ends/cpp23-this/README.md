# C++23 Explicit Object Parameter (Deducing this)

The explicit object parameter is a significant language feature added to C++23 that allows developers to explicitly
declare the implicit 'this' pointer in member function definitions. This feature has been a part of other programming
languages like Python (where it's commonly known as 'self') for a long time, but C++ had historically treated the 'this'
pointer as an implicit parameter that could not be directly manipulated in the function parameter list. The feature was
proposed in paper P0847R7 "Deducing this" by Gašper Ažman, Simon Brand, Ben Deane, and Barry Revzin, and went through
several design iterations before being incorporated into the C++23 standard.

## Use Cases and Problems Solved

The explicit object parameter feature addresses several long-standing C++ design challenges:

1. **Code Duplication for Const/Non-const Methods**: Previously, developers often had to write nearly identical functions
   twice to handle both const and non-const contexts. With explicit object parameter, a single function can handle both cases.

2. **CRTP (Curiously Recurring Template Pattern) Simplification**: The explicit object parameter makes CRTP implementations
   cleaner and less error-prone by allowing direct access to the derived type from the base template. *more information about this to follow later in documet*

3. **Reference Qualification**: Handling different reference qualifiers (`&`, `&&`) traditionally required multiple function
   overloads. The explicit object parameter allows a single function to adapt to the calling context.

4. **Overload Pattern Implementation**: The explicit object parameter enables more flexible implementations of the Overload
   Pattern, allowing functions to be customized based on both the object type and the parameters.

5. **Perfect Forwarding in Member Functions**: It simplifies perfect forwarding scenarios, reducing the complexity of
   template metaprogramming in class designs.

## Basic Usage

```cpp
class Example {
private:
    int m_value{0};

public:
    // Traditional member function
    void traditional_method() {
        m_value++;
    }

    // Explicit object parameter version
    void explicit_method(this Example& self) {
        self.m_value++;
    }
};
```

## CRTP Example

```cpp
template<typename Derived>
class Base {
public:
    void method(this Base<Derived>& self) {
        // Access derived implementation directly
        static_cast<Derived&>(self).implementation();
    }
};

class Derived : public Base<Derived> {
public:
    void implementation() {
        // Implementation details here
    }
};
```

## Avoiding Duplication with Const and Non-const Contexts

```cpp
class DataContainer {
private:
    std::vector<int> m_data;

public:
    // Single function works for both const and non-const contexts
    auto& get_data(this auto&& self) {
        return self.m_data;
    }
};
```

## Overload Pattern Example

```cpp
class Formatter {
public:
    void format(this auto&& self, auto&& output) {
        // Implementation adapts to both self type and output type
        output(self.get_formatted_data());
    }
};
```

## Good Practices

- Use explicit object parameter when you need to avoid duplication between const and non-const member functions
- Use it to simplify CRTP implementations
- Use it when you need to handle different reference qualifiers in a unified way
- Consider it for complex template metaprogramming scenarios involving member functions

## Potential Drawbacks

- May be confusing to developers not familiar with the feature
- Can make simple code more verbose if used unnecessarily
- Requires C++23 or later, limiting portability to older codebases

## Understanding CRTP with C++23's "deducing this"

## What is CRTP?

The Curiously Recurring Template Pattern (CRTP) is a C++ idiom where a class derives from a template class using itself as a template parameter. This pattern enables static polymorphism, allowing polymorphic behavior to be resolved at compile-time rather than runtime.

## Basic CRTP Structure

The traditional CRTP pattern looks like this:

```cpp
template <typename Derived>
class Base {
public:
    void interface() {
        // Access the derived class implementation
        static_cast<Derived*>(this)->implementation();
    }
    
    // Optional default implementation
    void implementation() {
        std::cout << "Base implementation\n";
    }
};

class Derived : public Base<Derived> {
public:
    void implementation() {
        std::cout << "Derived implementation\n";
    }
};
```

## The Problem with Traditional CRTP

While powerful, traditional CRTP has several drawbacks:

1. Requires explicit `static_cast` operations that are verbose and error-prone
2. Type safety issues if the cast is incorrect
3. Boilerplate code that clutters the implementation

## C++23's "deducing this" Solution

C++23 introduces "explicit object parameter" syntax (commonly called "deducing this"), which significantly improves CRTP implementations. This feature allows member functions to deduce the type of their object parameter.

### New Syntax

```cpp
template <typename Derived>
class Base {
public:
    void interface(this auto&& self) {
        // Direct access to derived implementation without casting
        self.implementation();
    }
    
    void implementation(this auto&& self) {
        std::cout << "Base implementation\n";
    }
};

class Derived : public Base<Derived> {
public:
    void implementation(this auto&& self) {
        std::cout << "Derived implementation\n";
    }
};
```

## Benefits of C++23 Approach

1. **Type Safety**: The compiler automatically deduces the correct type
2. **No Casts**: Eliminates error-prone static casts
3. **Cleaner Code**: More readable and maintainable
4. **Better Compiler Diagnostics**: Errors are caught at the declaration site

## Complete Example with Testing

Here's a complete example demonstrating CRTP with C++23's "deducing this" feature, including testing:

```cpp
#include <iostream>
#include <cassert>
#include <string>
#include <vector>

// Base CRTP class using C++23's "deducing this"
template <typename Derived>
class Shape {
public:
    // Calculate area using derived implementation
    double area(this auto&& self) const {
        return self.calculateArea();
    }
    
    // Default implementation
    double calculateArea() const {
        return 0.0; // Default implementation
    }
    
    // Get shape name using CRTP
    std::string name(this auto&& self) const {
        return self.getShapeName();
    }
    
    std::string getShapeName() const {
        return "Unknown Shape";
    }
};

// Rectangle implementation
class Rectangle : public Shape<Rectangle> {
private:
    double width_;
    double height_;
    
public:
    Rectangle(double width, double height) : width_(width), height_(height) {}
    
    double calculateArea() const {
        return width_ * height_;
    }
    
    std::string getShapeName() const {
        return "Rectangle";
    }
    
    double width() const { return width_; }
    double height() const { return height_; }
};

// Circle implementation
class Circle : public Shape<Circle> {
private:
    double radius_;
    
public:
    explicit Circle(double radius) : radius_(radius) {}
    
    double calculateArea() const {
        return 3.14159 * radius_ * radius_;
    }
    
    std::string getShapeName() const {
        return "Circle";
    }
    
    double radius() const { return radius_; }
};

// Test suite
void runTests() {
    // Test Rectangle
    Rectangle rect(5.0, 4.0);
    assert(rect.area() == 20.0);
    assert(rect.name() == "Rectangle");
    
    // Test Circle
    Circle circle(3.0);
    assert(std::abs(circle.area() - 28.27431) < 0.0001);
    assert(circle.name() == "Circle");
    
    // Using a collection of shapes
    std::vector<Shape<Rectangle>*> rectangles;
    rectangles.push_back(new Rectangle(2.0, 3.0));
    rectangles.push_back(new Rectangle(4.0, 5.0));
    
    double totalArea = 0.0;
    for (auto* rect : rectangles) {
        totalArea += rect->area();
    }
    
    assert(std::abs(totalArea - 26.0) < 0.0001);
    
    // Cleanup
    for (auto* rect : rectangles) {
        delete rect;
    }
    
    std::cout << "All tests passed!\n";
}

int main() {
    runTests();
    
    // Example usage
    Rectangle rect(10.0, 5.0);
    Circle circle(4.0);
    
    std::cout << "Shape: " << rect.name() << ", Area: " << rect.area() << std::endl;
    std::cout << "Shape: " << circle.name() << ", Area: " << circle.area() << std::endl;
    
    return 0;
}
```

## Performance Considerations

CRTP's primary advantage is that it provides polymorphic behavior without virtual function overhead:

1. **Compile-time Resolution**: Method calls are resolved at compile time
2. **No vtable**: Avoids the memory and indirection overhead of virtual methods
3. **Inlining**: Compiler can inline method calls, improving performance

## When to Use CRTP with C++23

CRTP with C++23's "deducing this" is particularly useful for:

1. **High-performance Systems**: When virtual function overhead is a concern
2. **Static Interface Enforcement**: Ensuring derived classes implement specific methods
3. **Mixins and Reusable Components**: Creating composable behavior
4. **Template Method Pattern**: When you need a fixed algorithm with customizable steps
5. **Expression Templates**: Building complex expression systems

## Limitations

Even with C++23 improvements, CRTP has some limitations:

1. **Increased Compilation Time**: Template-heavy code can slow compilation
2. **Code Bloat**: Templates can generate multiple instantiations of similar code
3. **Complexity**: Can be harder to understand than traditional inheritance
4. **Debugging Difficulty**: Template errors can be cryptic

## Conclusion

C++23's "deducing this" feature significantly improves the CRTP idiom by making it more type-safe, reducing boilerplate code, and eliminating error-prone casts. This modernization preserves all the performance benefits of traditional CRTP while addressing many of its ergonomic shortcomings.

By leveraging this new syntax, C++ developers can write cleaner, safer static polymorphism code that performs as well as traditional CRTP but with improved maintainability.

## Books and Resources

- "C++ Templates: The Complete Guide" (2nd Edition) by David Vandevoorde, Nicolai M. Josuttis, and Douglas Gregor
- "Effective Modern C++" by Scott Meyers
- "C++23 - The Complete Guide" by Nicolai M. Josuttis
- C++ Reference: [Explicit object parameter](https://en.cppreference.com/w/cpp/language/member_functions#Explicit_object_parameter)
- Original Proposal: [P0847R7 - Deducing this](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2021/p0847r7.html)

## License
This code is provided under the MIT License. Feel free to use, modify, and distribute as needed.

## Contributing
Contributions are welcome! Please feel free to submit a Pull Request.
