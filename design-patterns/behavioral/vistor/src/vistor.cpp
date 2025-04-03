// MIT License
// Copyright (c) 2025 dbjwhs

#ifndef VISITOR_PATTERN_HPP
#define VISITOR_PATTERN_HPP

#include <cmath>    // M_PI
#include <cassert>
#include <format>
#include "../../../../headers/project_utils.hpp"

// visitor design pattern
//
// history and overview:
// the visitor pattern is a behavioral design pattern that was first described in the "gang of four" (gof) book
// "design patterns: elements of reusable object-oriented software" published in 1994 by erich gamma, richard helm,
// ralph johnson, and john vlissides. it allows adding new operations to existing object structures without modifying them.
//
// how it works:
// - defines a separate visitor object that encapsulates an operation to be performed on elements of an object structure
// - allows defining new operations without changing the classes of the elements on which they operate
// - implements double dispatch, meaning the operation executed depends on both the type of visitor and the type of element
//
// common usages:
// 1. when a complex object structure contains many different object types with differing interfaces
// 2. when new operations need to be added frequently to the object structure without changing its classes
// 3. when operations on the object structure need to be decoupled from the structure itself
// 4. when applying operations across a diverse set of unrelated classes
// 5. in compilers (for traversing abstract syntax trees)
// 6. in document object models (for traversing and operating on xml/html elements)
//
// advantages:
// - open/closed principle: adds new operations without modifying existing classes
// - single responsibility principle: separates algorithms from the objects they operate on
// - collects related operations in one class and separates unrelated ones
//
// disadvantages:
// - breaks encapsulation as visitors must access element internals
// - difficult to add new element types as it requires updating all visitors
// - can lead to a complex design if overused

// forward declarations
class Circle;
class Square;
class Triangle;

// visitor interface
class ShapeVisitor {
public:
    // virtual destructor for proper cleanup in derived classes
    virtual ~ShapeVisitor() = default;
    
    // visit methods for each concrete element type
    virtual void visit(const Circle& circle) = 0;
    virtual void visit(const Square& square) = 0;
    virtual void visit(const Triangle& triangle) = 0;
};

// element interface
class Shape {
public:
    // virtual destructor for proper cleanup in derived classes
    virtual ~Shape() = default;
    
    // accept method that takes a visitor
    virtual void accept(ShapeVisitor& visitor) const = 0;
    
    // common interface for all shapes
    [[nodiscard]] virtual std::string getName() const = 0;
};

// concrete element: circle
class Circle final : public Shape {
private:
    // member variable for radius with m_ prefix as required
    double m_radius;

public:
    // constructor that initializes the radius
    explicit Circle(const double radius) : m_radius(radius) {}
    
    // implementation of accept method that calls the appropriate visit method
    void accept(ShapeVisitor& visitor) const override {
        visitor.visit(*this);
    }
    
    // getter for radius
    [[nodiscard]] double getRadius() const {
        return m_radius;
    }
    
    // implementation of getName method
    [[nodiscard]] std::string getName() const override {
        return "Circle";
    }
};

// concrete element: square
class Square final : public Shape {
private:
    // member variable for side length with m_ prefix as required
    double m_side;

public:
    // constructor that initializes the side length
    explicit Square(const double side) : m_side(side) {}
    
    // implementation of accept method that calls the appropriate visit method
    void accept(ShapeVisitor& visitor) const override {
        visitor.visit(*this);
    }
    
    // getter for side length
    [[nodiscard]] double getSide() const {
        return m_side;
    }
    
    // implementation of getName method
    [[nodiscard]] std::string getName() const override {
        return "Square";
    }
};

// concrete element: triangle
class Triangle final : public Shape {
private:
    // member variables for triangle sides with m_ prefix as required
    double m_a;
    double m_b;
    double m_c;

public:
    // constructor that initializes all three sides
    Triangle(const double a, const double b, const double c) : m_a(a), m_b(b), m_c(c) {}
    
    // implementation of accept method that calls the appropriate visit method
    void accept(ShapeVisitor& visitor) const override {
        visitor.visit(*this);
    }
    
    // getters for sides
    [[nodiscard]] double getA() const {
        return m_a;
    }
    [[nodiscard]] double getB() const {
        return m_b;
    }
    [[nodiscard]] double getC() const {
        return m_c;
    }
    
    // implementation of getName method
    [[nodiscard]] std::string getName() const override {
        return "Triangle";
    }
};

// concrete visitor: area calculator
class AreaVisitor final : public ShapeVisitor {
private:
    // member variable to store the calculated area with m_ prefix as required
    mutable double m_area = 0.0;

public:
    // reset area to 0
    void reset() const {
        m_area = 0.0;
    }
    
    // get the calculated area
    double getArea() const {
        return m_area;
    }
    
    // implementation for Circle
    void visit(const Circle& circle) override {
        m_area = M_PI * circle.getRadius() * circle.getRadius();
        Logger::getInstance().log(LogLevel::INFO, std::format("Calculated area of {} with radius {}: {}", 
                                  circle.getName(), circle.getRadius(), m_area));
    }
    
    // implementation for Square
    void visit(const Square& square) override {
        m_area = square.getSide() * square.getSide();
        Logger::getInstance().log(LogLevel::INFO, std::format("Calculated area of {} with side {}: {}", 
                                  square.getName(), square.getSide(), m_area));
    }
    
    // implementation for Triangle (using Heron's formula)
    void visit(const Triangle& triangle) override {
        // calculate semi-perimeter
        const double semi_perimeter = (triangle.getA() + triangle.getB() + triangle.getC()) / 2.0;
        
        // calculate area using heron's formula
        m_area = std::sqrt(semi_perimeter * (semi_perimeter - triangle.getA())
            * (semi_perimeter - triangle.getB())
            * (semi_perimeter - triangle.getC()));
        
        Logger::getInstance().log(LogLevel::INFO, 
                                  std::format("Calculated area of {} with sides {}, {}, {}: {}", 
                                  triangle.getName(), triangle.getA(), triangle.getB(), triangle.getC(), m_area));
    }
};

// concrete visitor: perimeter calculator
class PerimeterVisitor final : public ShapeVisitor {
private:
    // member variable to store the calculated perimeter with m_ prefix as required
    mutable double m_perimeter = 0.0;

public:
    // reset perimeter to 0
    void reset() const {
        m_perimeter = 0.0;
    }
    
    // get the calculated perimeter
    double getPerimeter() const {
        return m_perimeter;
    }
    
    // implementation for Circle
    void visit(const Circle& circle) override {
        m_perimeter = 2.0 * M_PI * circle.getRadius();
        Logger::getInstance().log(LogLevel::INFO, std::format("Calculated perimeter of {} with radius {}: {}", 
                                  circle.getName(), circle.getRadius(), m_perimeter));
    }
    
    // implementation for Square
    void visit(const Square& square) override {
        m_perimeter = 4.0 * square.getSide();
        Logger::getInstance().log(LogLevel::INFO, std::format("Calculated perimeter of {} with side {}: {}", 
                                  square.getName(), square.getSide(), m_perimeter));
    }
    
    // implementation for Triangle
    void visit(const Triangle& triangle) override {
        m_perimeter = triangle.getA() + triangle.getB() + triangle.getC();
        Logger::getInstance().log(LogLevel::INFO, 
                                  std::format("Calculated perimeter of {} with sides {}, {}, {}: {}", 
                                  triangle.getName(), triangle.getA(), triangle.getB(), triangle.getC(), m_perimeter));
    }
};

// concrete visitor: shape description generator
class DescriptionVisitor : public ShapeVisitor {
private:
    // member variable to store the generated description with m_ prefix as required
    mutable std::string m_description;

public:
    // reset description to empty string
    void reset() const {
        m_description.clear();
    }
    
    // get the generated description
    std::string getDescription() const {
        return m_description;
    }
    
    // implementation for Circle
    void visit(const Circle& circle) override {
        m_description = std::format("A circle with radius {}", circle.getRadius());
        LOG_INFO(std::format("Generated description: {}", m_description));
    }
    
    // implementation for Square
    void visit(const Square& square) override {
        m_description = std::format("A square with side length {}", square.getSide());
        LOG_INFO(std::format("Generated description: {}", m_description));
    }
    
    // implementation for Triangle
    void visit(const Triangle& triangle) override {
        m_description = std::format("A triangle with sides {}, {}, and {}", 
                                   triangle.getA(), triangle.getB(), triangle.getC());
        LOG_INFO(std::format("Generated description: {}", m_description));
    }
};

// main function with comprehensive testing
int main() {
    LOG_INFO("Starting Visitor Pattern Test");
    
    // create shapes
    std::vector<std::unique_ptr<Shape>> shapes;
    shapes.push_back(std::make_unique<Circle>(5.0));                // [0]
    shapes.push_back(std::make_unique<Square>(4.0));                // [1]
    shapes.push_back(std::make_unique<Triangle>(3.0, 4.0, 5.0));    // [2]
    
    LOG_INFO(std::format("Created {} shapes for testing", shapes.size()));
    
    // create visitors
    AreaVisitor areaVisitor;
    PerimeterVisitor perimeterVisitor;
    DescriptionVisitor descriptionVisitor;
    
    LOG_INFO("Created visitors: AreaVisitor, PerimeterVisitor, DescriptionVisitor");
    
    // test area calculations
    LOG_INFO("Testing area calculations...");
    
    // expected values for testing
    constexpr double expectedCircleArea = M_PI * 5.0 * 5.0;
    constexpr double expectedSquareArea = 4.0 * 4.0;
    constexpr double expectedTriangleArea = 6.0; // 3-4-5 triangle has area 6
    
    // test circle area
    shapes[0]->accept(areaVisitor);
    double circleArea = areaVisitor.getArea();
    LOG_INFO(std::format("Circle area: {}, Expected: {}", circleArea, expectedCircleArea));
    assert(std::abs(circleArea - expectedCircleArea) < 0.0001);
    
    // test square area
    shapes[1]->accept(areaVisitor);
    double squareArea = areaVisitor.getArea();
    LOG_INFO(std::format("Square area: {}, Expected: {}", squareArea, expectedSquareArea));
    assert(std::abs(squareArea - expectedSquareArea) < 0.0001);
    
    // test triangle area
    shapes[2]->accept(areaVisitor);
    double triangleArea = areaVisitor.getArea();
    LOG_INFO(std::format("Triangle area: {}, Expected: {}", triangleArea, expectedTriangleArea));
    assert(std::abs(triangleArea - expectedTriangleArea) < 0.0001);
    
    // test perimeter calculations
    LOG_INFO("Testing perimeter calculations...");
    
    // expected values for testing
    constexpr double expectedCirclePerimeter = 2.0 * M_PI * 5.0;
    constexpr double expectedSquarePerimeter = 4.0 * 4.0;
    constexpr double expectedTrianglePerimeter = 3.0 + 4.0 + 5.0;
    
    // test circle perimeter
    shapes[0]->accept(perimeterVisitor);
    double circlePerimeter = perimeterVisitor.getPerimeter();
    LOG_INFO(std::format("Circle perimeter: {}, Expected: {}", circlePerimeter, expectedCirclePerimeter));
    assert(std::abs(circlePerimeter - expectedCirclePerimeter) < 0.0001);
    
    // test square perimeter
    shapes[1]->accept(perimeterVisitor);
    double squarePerimeter = perimeterVisitor.getPerimeter();
    LOG_INFO(std::format("Square perimeter: {}, Expected: {}", squarePerimeter, expectedSquarePerimeter));
    assert(std::abs(squarePerimeter - expectedSquarePerimeter) < 0.0001);
    
    // test triangle perimeter
    shapes[2]->accept(perimeterVisitor);
    double trianglePerimeter = perimeterVisitor.getPerimeter();
    LOG_INFO(std::format("Triangle perimeter: {}, Expected: {}", trianglePerimeter, expectedTrianglePerimeter));
    assert(std::abs(trianglePerimeter - expectedTrianglePerimeter) < 0.0001);
    
    // test description generation
    LOG_INFO("Testing description generation...");
    
    // expected descriptions
    const std::string expectedCircleDesc = "A circle with radius 5";
    const std::string expectedSquareDesc = "A square with side length 4";
    const std::string expectedTriangleDesc = "A triangle with sides 3, 4, and 5";
    
    // test circle description
    shapes[0]->accept(descriptionVisitor);
    std::string circleDesc = descriptionVisitor.getDescription();
    LOG_INFO(std::format("Circle description: {}", circleDesc));
    assert(circleDesc == expectedCircleDesc);
    
    // test square description
    shapes[1]->accept(descriptionVisitor);
    std::string squareDesc = descriptionVisitor.getDescription();
    LOG_INFO(std::format("Square description: {}", squareDesc));
    assert(squareDesc == expectedSquareDesc);
    
    // test triangle description
    shapes[2]->accept(descriptionVisitor);
    std::string triangleDesc = descriptionVisitor.getDescription();
    LOG_INFO(std::format("Triangle description: {}", triangleDesc));
    assert(triangleDesc == expectedTriangleDesc);
    
    // demonstrate polymorphic behavior through a shape collection
    LOG_INFO("Demonstrating polymorphic behavior through shape collection...");
    
    // iterate through all shapes and apply all visitors
    for (const auto& shape : shapes) {
        LOG_INFO(std::format("Processing shape: {}", shape->getName()));
        
        // reset visitors
        areaVisitor.reset();
        perimeterVisitor.reset();
        descriptionVisitor.reset();
        
        // apply all visitors to current shape
        shape->accept(areaVisitor);
        shape->accept(perimeterVisitor);
        shape->accept(descriptionVisitor);
        
        // log results
        Logger::getInstance().log(LogLevel::INFO, std::format("Results for {}: Area = {}, Perimeter = {}, Description = '{}'",
                                  shape->getName(), areaVisitor.getArea(), perimeterVisitor.getPerimeter(), 
                                  descriptionVisitor.getDescription()));
    }
    
    LOG_INFO("All tests passed successfully!");
    return 0;
}

#endif // VISITOR_PATTERN_HPP
