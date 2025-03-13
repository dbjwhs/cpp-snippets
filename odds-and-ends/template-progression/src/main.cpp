// MIT License
// Copyright (c) 2025 dbjwhs

#include <iostream>
#include <utility>
#include <vector>
#include <string>
#include <format>
#include <type_traits>
#include <memory>
#include <cassert>
#include "../../../headers/project_utils.hpp"

#define TEMPLATE_VERSION "1.0.0"
#define TEMPLATE_AUTHOR "dbjwhs"
#define TEMPLATE_DATE "2025-03-12"

// C++ templates history and usage:
// templates were introduced in c++98 as a way to create generic code that works with multiple types.
// they are a powerful feature of c++ that enables generic programming - writing code that works with any data type.
// templates are compiled at compile-time, meaning they have no runtime overhead.
// common usages include:
//   1. container classes (like std::vector, std::map)
//   2. algorithms (like std::sort, std::find)
//   3. smart pointers (like std::unique_ptr, std::shared_ptr)
//   4. numeric operations (like std::min, std::max)
//   5. metaprogramming (compile-time programming)
// templates have evolved significantly with modern c++ (11/14/17/20) to include:
//   - variadic templates (c++11)
//   - template aliases (c++11)
//   - auto return type deduction (c++14)
//   - class template argument deduction (c++17)
//   - concepts (c++20)
//
// the following is this test code I have created over the years to help understand and debug these. With the help
// of Claude, I have consolidated my personal test code and entire templates example projects to this one file. yeah.
// it should be digestible in which I have started with simple template examples working up to mor complex ones. As
// templates continue to evolve in new C++ released, I (hapefully) will stay up to date with these examples.
//
// dbjwhs 13-03-2035
//

// forward declarations
void run_all_template_tests();

int main() {
    Logger::getInstance().log(LogLevel::INFO, "starting template examples");

    run_all_template_tests();

    Logger::getInstance().log(LogLevel::INFO, "all template tests completed successfully");
    return 0;
}

// =============================================================================
// 1. basic function template
// =============================================================================

// a simple function template that swaps two values of any type
template <typename T>
void swap_values(T& a, T& b) {
    // uses a temporary variable to swap values
    T temp = a;
    a = b;
    b = temp;
}

// test function for the basic swap template
void test_basic_template() {
    Logger::getInstance().log(LogLevel::INFO, "testing basic function template");

    // test with integers
    int a = 5, b = 10;
    Logger::getInstance().log(LogLevel::INFO, std::format("before swap: a = {}, b = {}", a, b));
    swap_values(a, b);
    Logger::getInstance().log(LogLevel::INFO, std::format("after swap: a = {}, b = {}", a, b));
    assert(a == 10 && b == 5 && "Integer swap failed");

    // test with strings
    std::string s1 = "hello", s2 = "world";
    Logger::getInstance().log(LogLevel::INFO, std::format("before swap: s1 = {}, s2 = {}", s1, s2));
    swap_values(s1, s2);
    Logger::getInstance().log(LogLevel::INFO, std::format("after swap: s1 = {}, s2 = {}", s1, s2));
    assert(s1 == "world" && s2 == "hello" && "String swap failed");

    // test with doubles
    double d1 = 3.14, d2 = 2.71;
    Logger::getInstance().log(LogLevel::INFO, std::format("before swap: d1 = {}, d2 = {}", d1, d2));
    swap_values(d1, d2);
    Logger::getInstance().log(LogLevel::INFO, std::format("after swap: d1 = {}, d2 = {}", d1, d2));
    assert(d1 == 2.71 && d2 == 3.14 && "Double swap failed");

    Logger::getInstance().log(LogLevel::INFO, "basic function template test passed");
}

// =============================================================================
// 2. class template
// =============================================================================

// a simple class template for a pair of values of any type
template <typename T1, typename T2>
class Pair {
private:
    // the first value of the pair
    T1 m_first;

    // the second value of the pair
    T2 m_second;

public:
    // constructor to initialize the pair
    Pair(T1 first, T2 second) : m_first(std::move(first)), m_second(std::move(second)) {}

    // getter for the first value
    [[nodiscard]] T1 get_first() const { return m_first; }

    // getter for the second value
    [[nodiscard]] T2 get_second() const { return m_second; }

    // setter for the first value
    void set_first(const T1& first) { m_first = first; }

    // setter for the second value
    void set_second(const T2& second) { m_second = second; }

    // method to swap the pair's values with another pair
    void swap(Pair<T1, T2>& other) noexcept {
        swap_values(m_first, other.m_first);
        swap_values(m_second, other.m_second);
    }
};

// test function for the pair class template
void test_class_template() {
    Logger::getInstance().log(LogLevel::INFO, "testing class template");

    // create a pair of integer and string
    Pair<int, std::string> person(42, "Alice");
    Logger::getInstance().log(LogLevel::INFO, std::format("person: id = {}, name = {}",
        person.get_first(), person.get_second()));

    // modify the pair
    person.set_first(100);
    person.set_second("Bob");
    Logger::getInstance().log(LogLevel::INFO, std::format("modified person: id = {}, name = {}",
        person.get_first(), person.get_second()));
    assert(person.get_first() == 100 && person.get_second() == "Bob" && "Pair modification failed");

    // create another pair and swap
    Pair<int, std::string> another_person(200, "Charlie");
    person.swap(another_person);
    Logger::getInstance().log(LogLevel::INFO, std::format("after swap: person1 = ({}, {}), person2 = ({}, {})",
        person.get_first(), person.get_second(), another_person.get_first(), another_person.get_second()));
    assert(person.get_first() == 200 && person.get_second() == "Charlie" &&
           another_person.get_first() == 100 && another_person.get_second() == "Bob" &&
           "Pair swap failed");

    // different type combinations
    // Different type combinations
    Pair<double, int> measurement(98.6, 42);
    Logger::getInstance().log(LogLevel::INFO, std::format("measurement: temperature = {}, pulse = {}",
        measurement.get_first(), measurement.get_second()));

    // Modify the measurement
    measurement.set_first(99.2);
    measurement.set_second(78);
    Logger::getInstance().log(LogLevel::INFO, std::format("updated measurement: temperature = {}, pulse = {}",
        measurement.get_first(), measurement.get_second()));

    // Test with another different type pair
    Pair<std::string, std::string> full_name("John", "Doe");
    Logger::getInstance().log(LogLevel::INFO, std::format("name: first = {}, last = {}",
        full_name.get_first(), full_name.get_second()));

    // We could even swap between different instances of the same template types
    Pair<std::string, std::string> another_name("Jane", "Smith");
    full_name.swap(another_name);
    Logger::getInstance().log(LogLevel::INFO, std::format("after swap: first name = {}, last name = {}",
        full_name.get_first(), full_name.get_second()));
    Logger::getInstance().log(LogLevel::INFO, "class template test passed");
}

// =============================================================================
// 3. template specialization
// =============================================================================

// generic template for maximum value
template <typename T>
T max_value(T a, T b) {
    return (a > b) ? a : b;
}

// specialization for c-style strings to compare them correctly
template <>
const char* max_value<const char*>(const char* a, const char* b) {
    return (strcmp(a, b) > 0) ? a : b;
}

// test function for template specialization
void test_template_specialization() {
    Logger::getInstance().log(LogLevel::INFO, "testing template specialization");

    // using the generic template
    int max_int = max_value(10, 20);
    Logger::getInstance().log(LogLevel::INFO, std::format("max of 10 and 20 is {}", max_int));
    assert(max_int == 20 && "Integer max failed");

    double max_double = max_value(3.14, 2.71);
    Logger::getInstance().log(LogLevel::INFO, std::format("max of 3.14 and 2.71 is {}", max_double));
    assert(max_double == 3.14 && "Double max failed");

    // using the specialized template for c-strings
    const char* s1 = "apple";
    const char* s2 = "banana";
    const char* max_str = max_value(s1, s2);
    Logger::getInstance().log(LogLevel::INFO, std::format("max of '{}' and '{}' is '{}'", s1, s2, max_str));
    assert(strcmp(max_str, "banana") == 0 && "String max failed");

    Logger::getInstance().log(LogLevel::INFO, "template specialization test passed");
}

// =============================================================================
// 4. template with default parameters
// =============================================================================

// a dynamic array template with a default allocator type
// updated for c++20 which removed allocator::construct and allocator::destroy
template <typename T, typename Allocator = std::allocator<T>>
class DynamicArray {
private:
    // pointer to the allocated memory
    T* m_data;

    // current size of the array
    size_t m_size;

    // current capacity of the array
    size_t m_capacity;

    // allocator instance
    Allocator m_allocator;

    // allocator traits for type-safe allocator operations
    using AllocTraits = std::allocator_traits<Allocator>;

public:
    // constructor with initial capacity
    explicit DynamicArray(size_t initial_capacity = 10)
        : m_size(0), m_capacity(initial_capacity) {
        // allocate memory using the allocator
        m_data = AllocTraits::allocate(m_allocator, m_capacity);
    }

    // destructor to clean up resources
    ~DynamicArray() {
        // destroy all constructed elements
        for (size_t i = 0; i < m_size; ++i) {
            AllocTraits::destroy(m_allocator, &m_data[i]);
        }
        // deallocate the memory
        AllocTraits::deallocate(m_allocator, m_data, m_capacity);
    }

    // add an element to the end of the array
    void push_back(const T& value) {
        if (m_size >= m_capacity) {
            // grow the capacity if needed
            size_t new_capacity = m_capacity * 2;
            T* new_data = AllocTraits::allocate(m_allocator, new_capacity);

            // copy existing elements to the new memory
            for (size_t i = 0; i < m_size; ++i) {
                AllocTraits::construct(m_allocator, &new_data[i], m_data[i]);
                AllocTraits::destroy(m_allocator, &m_data[i]);
            }

            // deallocate old memory and update pointers
            AllocTraits::deallocate(m_allocator, m_data, m_capacity);
            m_data = new_data;
            m_capacity = new_capacity;
        }

        // construct the new element at the end
        AllocTraits::construct(m_allocator, &m_data[m_size], value);
        ++m_size;
    }

    // access element at a given index
    T& at(size_t index) {
        if (index >= m_size) {
            throw std::out_of_range("Index out of range");
        }
        return m_data[index];
    }

    // get the current size
    [[nodiscard]] size_t size() const { return m_size; }

    // get the current capacity
    [[nodiscard]] size_t capacity() const { return m_capacity; }
};

// test function for template with default parameters
void test_template_default_params() {
    Logger::getInstance().log(LogLevel::INFO, "testing template with default parameters");

    // create a dynamic array of integers with default allocator
    DynamicArray<int> numbers;
    Logger::getInstance().log(LogLevel::INFO, std::format("initial capacity: {}", numbers.capacity()));

    // add elements
    for (int i = 0; i < 15; ++i) {
        numbers.push_back(i * 10);
    }

    Logger::getInstance().log(LogLevel::INFO, std::format("size after adding 15 elements: {}", numbers.size()));
    Logger::getInstance().log(LogLevel::INFO, std::format("capacity after growth: {}", numbers.capacity()));
    assert(numbers.size() == 15 && "DynamicArray size incorrect");
    assert(numbers.capacity() >= 15 && "DynamicArray capacity incorrect");

    // verify elements
    for (size_t i = 0; i < numbers.size(); ++i) {
        const int expected = static_cast<int>(i) * 10;
        int actual = numbers.at(i);
        Logger::getInstance().log(LogLevel::INFO, std::format("Element at index {}: {}", i, actual));
        assert(actual == expected && "DynamicArray element incorrect");
    }

    // test exception handling for out-of-bounds access
    try {
        numbers.at(100);
        assert(false && "Expected exception was not thrown");
    } catch (const std::out_of_range& e) {
        Logger::getInstance().log(LogLevel::INFO, std::format("Caught expected exception: {}", e.what()));
    }

    Logger::getInstance().log(LogLevel::INFO, "template with default parameters test passed");
}

// =============================================================================
// 5. variadic templates (c++11)
// =============================================================================

// recursive base case for variadic template
void print_values() {
    Logger::getInstance().log(LogLevel::INFO, "");  // end of recursion, print newline
}

// variadic template function to print multiple values of different types
template <typename T, typename... Args>
void print_values(T first, Args... rest) {
    // process the first argument
    Logger::getInstance().log(LogLevel::INFO, std::format("{}", first));

    // recursively process the remaining arguments
    print_values(rest...);
}

// variadic template for summing values of the same type
template <typename T>
T sum_values(T value) {
    return value;
}

template <typename T, typename... Args>
T sum_values(T first, Args... rest) {
    return first + sum_values<T>(rest...);
}

// test function for variadic templates
void test_variadic_templates() {
    Logger::getInstance().log(LogLevel::INFO, "testing variadic templates");

    // print values of different types
    Logger::getInstance().log(LogLevel::INFO, "printing multiple values:");
    Logger::getInstance().log(LogLevel::INFO, "Mixed types: ");
    print_values(42, "hello", 3.14, 'a');

    // sum integers
    int sum_int = sum_values(1, 2, 3, 4, 5);
    Logger::getInstance().log(LogLevel::INFO, std::format("sum of integers: {}", sum_int));
    assert(sum_int == 15 && "Integer sum failed");

    // sum doubles
    double sum_double = sum_values(1.1, 2.2, 3.3, 4.4);
    Logger::getInstance().log(LogLevel::INFO, std::format("sum of doubles: {}", sum_double));
    assert(std::abs(sum_double - 11.0) < 0.001 && "Double sum failed");

    Logger::getInstance().log(LogLevel::INFO, "variadic templates test passed");
}

// =============================================================================
// 6. template metaprogramming
// =============================================================================

// compile-time factorial calculation using template metaprogramming
template <unsigned int N>
struct Factorial {
    static constexpr unsigned int value = N * Factorial<N-1>::value;
};

// base case specialization for factorial of 0
template <>
struct Factorial<0> {
    static constexpr unsigned int value = 1;
};

// compile-time fibonacci calculation
template <unsigned int N>
struct Fibonacci {
    static constexpr unsigned int value = Fibonacci<N-1>::value + Fibonacci<N-2>::value;
};

// base case specializations for fibonacci
template <>
struct Fibonacci<0> {
    static constexpr unsigned int value = 0;
};

template <>
struct Fibonacci<1> {
    static constexpr unsigned int value = 1;
};

// type traits example: check if a type is arithmetic
template <typename T>
struct is_arithmetic_custom {
    static constexpr bool value = false;
};

// specializations for arithmetic types
template <>
struct is_arithmetic_custom<int> {
    static constexpr bool value = true;
};

template <>
struct is_arithmetic_custom<float> {
    static constexpr bool value = true;
};

template <>
struct is_arithmetic_custom<double> {
    static constexpr bool value = true;
};

// test function for template metaprogramming
void test_template_metaprogramming() {
    Logger::getInstance().log(LogLevel::INFO, "testing template metaprogramming");

    // compile-time factorial calculations
    constexpr unsigned int fact5 = Factorial<5>::value;
    Logger::getInstance().log(LogLevel::INFO, std::format("factorial of 5 = {}", fact5));
    assert(fact5 == 120 && "Factorial calculation failed");

    constexpr unsigned int fact10 = Factorial<10>::value;
    Logger::getInstance().log(LogLevel::INFO, std::format("factorial of 10 = {}", fact10));
    assert(fact10 == 3628800 && "Factorial calculation failed");

    // compile-time fibonacci calculations
    constexpr unsigned int fib10 = Fibonacci<10>::value;
    Logger::getInstance().log(LogLevel::INFO, std::format("fibonacci(10) = {}", fib10));
    assert(fib10 == 55 && "Fibonacci calculation failed");

    // type traits example
    bool int_is_arithmetic = is_arithmetic_custom<int>::value;
    bool string_is_arithmetic = is_arithmetic_custom<std::string>::value;

    Logger::getInstance().log(LogLevel::INFO, std::format("is_arithmetic_custom<int> = {}", int_is_arithmetic));
    Logger::getInstance().log(LogLevel::INFO, std::format("is_arithmetic_custom<std::string> = {}", string_is_arithmetic));

    assert(int_is_arithmetic && "int should be arithmetic");
    assert(!string_is_arithmetic && "std::string should not be arithmetic");

    // can also use standard type traits from <type_traits>
    bool is_int_arithmetic = std::is_arithmetic_v<int>;
    Logger::getInstance().log(LogLevel::INFO, std::format("std::is_arithmetic<int> = {}", is_int_arithmetic));
    assert(is_int_arithmetic && "std::is_arithmetic<int> should be true");

    Logger::getInstance().log(LogLevel::INFO, "template metaprogramming test passed");
}

// =============================================================================
// 7. sfinae (substitution failure is not an error)
// =============================================================================

// helper functions to detect whether a type has a size() method
template <typename T>
struct has_size_method {
private:
    // Test for size() method existence (true case)
    template <typename C>
    static constexpr auto test(int) -> decltype(std::declval<C>().size(), std::true_type{}) {
        return std::true_type{};  // Added implementation
    }

    // Fallback (false case)
    template <typename C>
    static constexpr std::false_type test(...) {
        return std::false_type{};  // Added implementation
    }

public:
    // Value will be true if T has a size() method
    static constexpr bool value = decltype(test<T>(0))::value;
};

// function that uses sfinae to call size() only if it exists
template <typename T,
    std::enable_if_t<has_size_method<T>::value, int> = 0>
size_t get_container_size(const T& container) {
    Logger::getInstance().log(LogLevel::INFO, "using container's size() method");
    return container.size();
}

// overload for types without a size() method
template <typename T,
    std::enable_if_t<!has_size_method<T>::value, int> = 0>
size_t get_container_size(const T& /* container */) {
    Logger::getInstance().log(LogLevel::INFO, "container has no size() method, returning 0");
    return 0;
}

// a class with size() method for testing
class SizeableClass {
private:
    // internal data
    std::vector<int> m_data;

public:
    // constructor
    SizeableClass() : m_data{1, 2, 3, 4, 5} {}

    // size method
    [[nodiscard]] size_t size() const { return m_data.size(); }
};

// a class without size() method for testing
class NonSizeableClass {
private:
    // some data
    int m_value;

public:
    // constructor
    NonSizeableClass() : m_value(42) {}

    // getter
    [[nodiscard]] int get_value() const { return m_value; }
};

// test function for sfinae
void test_sfinae() {
    Logger::getInstance().log(LogLevel::INFO, "testing sfinae");

    // test with a standard container
    std::vector<int> vec = {1, 2, 3, 4, 5};
    size_t vec_size = get_container_size(vec);
    Logger::getInstance().log(LogLevel::INFO, std::format("vector size: {}", vec_size));
    assert(vec_size == 5 && "Vector size incorrect");

    // test with a custom class that has size()
    SizeableClass sizeable;
    size_t sizeable_size = get_container_size(sizeable);
    Logger::getInstance().log(LogLevel::INFO, std::format("sizeable class size: {}", sizeable_size));
    assert(sizeable_size == 5 && "SizeableClass size incorrect");

    // test with a custom class that doesn't have size()
    NonSizeableClass non_sizeable;
    size_t non_sizeable_size = get_container_size(non_sizeable);
    Logger::getInstance().log(LogLevel::INFO, std::format("non-sizeable class size: {}", non_sizeable_size));
    assert(non_sizeable_size == 0 && "NonSizeableClass size should be 0");

    // test with a primitive type
    int primitive = 42;
    size_t primitive_size = get_container_size(primitive);
    Logger::getInstance().log(LogLevel::INFO, std::format("primitive type size: {}", primitive_size));
    assert(primitive_size == 0 && "Primitive size should be 0");

    Logger::getInstance().log(LogLevel::INFO, "sfinae test passed");
}

// =============================================================================
// 8. template template parameters (advanced)
// =============================================================================

// a template class that takes another template as a parameter
template <
    typename T,
    template <typename, typename = std::allocator<T>> class Container
>
class Stack {
private:
    // the actual container instance used to store elements
    Container<T> m_container;

public:
    // add an element to the top of the stack
    void push(const T& value) {
        m_container.push_back(value);
    }

    // remove and return the top element
    T pop() {
        if (empty()) {
            throw std::out_of_range("Stack is empty");
        }

        T value = m_container.back();
        m_container.pop_back();
        return value;
    }

    // check if stack is empty
    [[nodiscard]] bool empty() const {
        return m_container.empty();
    }

    // get the number of elements
    [[nodiscard]] size_t size() const {
        return m_container.size();
    }

    // peek at the top element without removing it
    T& top() {
        if (empty()) {
            throw std::out_of_range("Stack is empty");
        }
        return m_container.back();
    }
};

// test function for template parameters
void test_template_template_params() {
    Logger::getInstance().log(LogLevel::INFO, "testing template template parameters");

    // create a stack using std::vector as the container
    Stack<int, std::vector> int_stack;

    // push elements
    int_stack.push(10);
    int_stack.push(20);
    int_stack.push(30);

    Logger::getInstance().log(LogLevel::INFO, std::format("stack size: {}", int_stack.size()));
    assert(int_stack.size() == 3 && "Stack size incorrect");

    // peek at a top element
    int top_value = int_stack.top();
    Logger::getInstance().log(LogLevel::INFO, std::format("top element: {}", top_value));
    assert(top_value == 30 && "Stack top incorrect");

    // pop elements
    Logger::getInstance().log(LogLevel::INFO, "popping elements:");
    while (!int_stack.empty()) {
        int value = int_stack.pop();
        Logger::getInstance().log(LogLevel::INFO, std::format("popped: {}", value));
    }

    // check that stack is empty
    assert(int_stack.empty() && "Stack should be empty");

    // test with a string stack
    Stack<std::string, std::vector> string_stack;
    string_stack.push("hello");
    string_stack.push("world");

    std::string s1 = string_stack.pop();
    std::string s2 = string_stack.pop();

    Logger::getInstance().log(LogLevel::INFO, std::format("popped strings: {}, {}", s1, s2));
    assert(s1 == "world" && s2 == "hello" && "String stack incorrect");

    Logger::getInstance().log(LogLevel::INFO, "template template parameters test passed");
}

// =============================================================================
// 9. crtp (curiously recurring template pattern)
// =============================================================================

// base class template that will be inherited from using crtp
template <typename Derived>
class Shape {
public:
    // interface method that calls the derived implementation
    [[nodiscard]] double area() const {
        // cast this pointer to the derived type and call its implementation
        return static_cast<const Derived*>(this)->area_impl();
    }

    // interface method for perimeter
    [[nodiscard]] double perimeter() const {
        return static_cast<const Derived*>(this)->perimeter_impl();
    }

    // default implementation for displaying shape info
    void display() const {
        const auto* derived = static_cast<const Derived*>(this);
        Logger::getInstance().log(LogLevel::INFO, std::format("{}: area = {:.2f}, perimeter = {:.2f}",
            derived->name(), derived->area(), derived->perimeter()));
    }
};

// rectangle class using crtp
class Rectangle : public Shape<Rectangle> {
private:
    // width of the rectangle
    double m_width;

    // height of the rectangle
    double m_height;

public:
    // constructor
    Rectangle(double width, double height)
        : m_width(width), m_height(height) {}

    // implementation for area calculation
    [[nodiscard]] double area_impl() const {
        return m_width * m_height;
    }

    // implementation for perimeter calculation
    [[nodiscard]] double perimeter_impl() const {
        return 2 * (m_width + m_height);
    }

    // get the shape name
    [[nodiscard]] static std::string name() {
        return "Rectangle";
    }
};

// circle class using crtp
class Circle : public Shape<Circle> {
private:
    // radius of the circle
    double m_radius;

public:
    // constructor
    explicit Circle(double radius) : m_radius(radius) {}

    // implementation for area calculation
    [[nodiscard]] double area_impl() const {
        return M_PI * m_radius * m_radius;
    }

    // implementation for perimeter calculation
    [[nodiscard]] double perimeter_impl() const {
        return 2 * M_PI * m_radius;
    }

    // get the shape name
    [[nodiscard]] static std::string name() {
        return "Circle";
    }
};

// test function for crtp
void test_crtp() {
    Logger::getInstance().log(LogLevel::INFO, "testing crtp (curiously recurring template pattern)");

    // create shapes
    Rectangle rect(5.0, 3.0);
    Circle circle(2.5);

    // polymorphic behavior without virtual functions
    rect.display();
    circle.display();

    // verify calculations
    double rect_area = rect.area();
    double rect_perimeter = rect.perimeter();
    assert(std::abs(rect_area - 15.0) < 0.001 && "Rectangle area incorrect");
    assert(std::abs(rect_perimeter - 16.0) < 0.001 && "Rectangle perimeter incorrect");

    double circle_area = circle.area();
    double circle_perimeter = circle.perimeter();
    assert(std::abs(circle_area - (M_PI * 2.5 * 2.5)) < 0.001 && "Circle area incorrect");
    assert(std::abs(circle_perimeter - (2 * M_PI * 2.5)) < 0.001 && "Circle perimeter incorrect");

    // we can store shapes in a vector, but we need to use pointers for polymorphism
    std::vector<Shape<Rectangle>*> rectangles;
    rectangles.push_back(&rect);

    Logger::getInstance().log(LogLevel::INFO, "crtp test passed");
}

// =============================================================================
// 10. fold expressions (c++17)
// =============================================================================

// variadic template function using fold expressions to sum values
template <typename... Ts>
auto sum_fold(Ts... values) {
    // uses fold expression to add all parameters
    return (values + ...);
}

// variadic template to check if all values satisfy a predicate
template <typename Predicate, typename... Ts>
bool all_of_fold(Predicate pred, Ts... values) {
    // fold expression with logical and
    return (pred(values) && ...);
}

// variadic template to check if any value satisfies a predicate
template <typename Predicate, typename... Ts>
bool any_of_fold(Predicate pred, Ts... values) {
    // fold expression with logical or
    return (pred(values) || ...);
}

// variadic template to print all values with a separator
template <typename... Ts>
void print_fold(Ts... values) {
    // Create a string stream to collect all values
    std::stringstream ss;
    ss << "Values: ";

    // Use fold expression to add each value to the string stream
    ((ss << values << ", "), ...);

    // Get the resulting string and log it
    std::string result = ss.str();

    // Remove the trailing comma and space if there were any values
    if (sizeof...(Ts) > 0) {
        result = result.substr(0, result.length() - 2);
    }

    // Log the final string
    Logger::getInstance().log(LogLevel::INFO, result);
}

// test function for fold expressions
void test_fold_expressions() {
    Logger::getInstance().log(LogLevel::INFO, "testing fold expressions (c++17)");

    // sum using fold expression
    int sum = sum_fold(1, 2, 3, 4, 5);
    Logger::getInstance().log(LogLevel::INFO, std::format("sum using fold: {}", sum));
    assert(sum == 15 && "Fold sum incorrect");

    // all_of with a predicate
    auto is_positive = [](int x) { return x > 0; };
    bool all_positive = all_of_fold(is_positive, 1, 2, 3, 4, 5);
    Logger::getInstance().log(LogLevel::INFO, std::format("all positive: {}", all_positive));
    assert(all_positive && "all_of_fold with positive numbers failed");

    bool not_all_positive = all_of_fold(is_positive, 1, 2, -3, 4, 5);
    Logger::getInstance().log(LogLevel::INFO, std::format("all positive (with negative): {}", not_all_positive));
    assert(!not_all_positive && "all_of_fold with a negative number should be false");

    // any_of with a predicate
    auto is_even = [](int x) { return x % 2 == 0; };
    bool any_even = any_of_fold(is_even, 1, 3, 5, 7, 8);
    Logger::getInstance().log(LogLevel::INFO, std::format("any even: {}", any_even));
    assert(any_even && "any_of_fold with an even number should be true");

    bool none_even = any_of_fold(is_even, 1, 3, 5, 7, 9);
    Logger::getInstance().log(LogLevel::INFO, std::format("any even (all odd): {}", none_even));
    assert(!none_even && "any_of_fold with all odd numbers should be false");

    // print using fold expression
    Logger::getInstance().log(LogLevel::INFO, std::format("Print using fold expression: "));
    print_fold(10, 20, 30, "hello", 3.14);

    Logger::getInstance().log(LogLevel::INFO, "fold expressions test passed");
}

// =============================================================================
// 11. constexpr if (c++17)
// =============================================================================

// function template using constexpr if for compile-time branching
template <typename T>
void process_value(const T& value) {
    // constexpr if evaluates the condition at compile-time
    if constexpr (std::is_integral_v<T>) {
        Logger::getInstance().log(LogLevel::INFO, std::format("processing integral value: {}", value));
        // integral-specific operations
        Logger::getInstance().log(LogLevel::INFO, std::format("squared: {}", value * value));
    }
    else if constexpr (std::is_floating_point_v<T>) {
        Logger::getInstance().log(LogLevel::INFO, std::format("processing floating-point value: {}", value));
        // floating-point specific operations
        Logger::getInstance().log(LogLevel::INFO, std::format("reciprocal: {}", 1.0 / value));
    }
    else if constexpr (std::is_same_v<T, std::string>) {
        Logger::getInstance().log(LogLevel::INFO, std::format("processing string value: {}", value));
        // string-specific operations
        Logger::getInstance().log(LogLevel::INFO, std::format("length: {}", value.length()));
    }
    else {
        Logger::getInstance().log(LogLevel::INFO, std::format("processing unknown type value: {}", value));
    }
}

// test function for constexpr if
void test_constexpr_if() {
    Logger::getInstance().log(LogLevel::INFO, "testing constexpr if (c++17)");

    // test with different types
    process_value(42);                   // int
    process_value(3.14159);              // double
    process_value(std::string("hello")); // string
    process_value('X');                  // char (will use the fallback)

    Logger::getInstance().log(LogLevel::INFO, "constexpr if test passed");
}

// =============================================================================
// 12. concepts (c++20) - simulated if compiler doesn't support c++20
// =============================================================================

// if c++20 is available, use actual concepts
#if __cplusplus >= 202002L

// concept to check if a type is addable
template <typename T>
concept Addable = requires(T a, T b) {
    { a + b } -> std::convertible_to<T>;
};

// concept to check if a type is printable
template <typename T>
concept Printable = requires(std::ostream& os, T a) {
    { os << a } -> std::convertible_to<std::ostream&>;
};

// function that works only for types satisfying the Addable concept
template <Addable T>
T add(T a, T b) {
    return a + b;
}

// function that works only for types satisfying the Printable concept
template <Printable T>
void print_value(const T& value) {
    std::stringstream ss;
    ss << "Value: " << value;
    Logger::getInstance().log(LogLevel::INFO, ss.str());
}
#else
// for pre-c++20, use sfinae to simulate concepts

// simulate the Addable concept
template <typename T, typename = void>
struct is_addable : std::false_type {};

template <typename T>
struct is_addable<T, std::void_t<decltype(std::declval<T>() + std::declval<T>())>>
    : std::true_type {};

// simulate the Printable concept
template <typename T, typename = void>
struct is_printable : std::false_type {};

template <typename T>
struct is_printable<T, std::void_t<decltype(std::declval<std::ostream&>() << std::declval<T>())>>
    : std::true_type {};

// function that works only for types satisfying the addable requirement
template <typename T,
          typename std::enable_if<is_addable<T>::value, int>::type = 0>
T add(T a, T b) {
    return a + b;
}

// function that works only for types satisfying the printable requirement
template <typename T,
          typename std::enable_if<is_printable<T>::value, int>::type = 0>
void print_value(const T& value) {
    std::cout << "Value: " << value << std::endl;
}

#endif

// a custom type that is addable
struct AddableType {
    int value;

    explicit AddableType(const int val) : value(val) {}

    AddableType operator+(const AddableType& other) const {
        return AddableType(value + other.value);
    }

    friend std::ostream& operator<<(std::ostream& os, const AddableType& obj) {
        return os << "AddableType(" << obj.value << ")";
    }
};

// a type that is not addable
struct NonAddableType {
    std::string name;

    explicit NonAddableType(std::string  n) : name(std::move(n)) {}

    friend std::ostream& operator<<(std::ostream& os, const NonAddableType& obj) {
        return os << "NonAddableType(" << obj.name << ")";
    }

    // no operator+ defined
};

// test function for concepts (or simulated concepts)
void test_concepts() {
    Logger::getInstance().log(LogLevel::INFO, "testing concepts (or simulated concepts)");

    // test with built-in types
    int sum_int = add(10, 20);
    Logger::getInstance().log(LogLevel::INFO, std::format("sum of integers: {}", sum_int));
    assert(sum_int == 30 && "Integer addition incorrect");

    double sum_double = add(3.5, 2.5);
    Logger::getInstance().log(LogLevel::INFO, std::format("sum of doubles: {}", sum_double));
    assert(std::abs(sum_double - 6.0) < 0.001 && "Double addition incorrect");

    // test with custom addable type
    const AddableType a1(10);
    const AddableType a2(20);

    const AddableType sum_custom = add(a1, a2);
    Logger::getInstance().log(LogLevel::INFO, "sum of custom addable types:");
    print_value(sum_custom);
    assert(sum_custom.value == 30 && "Custom type addition incorrect");

    // print various types
    print_value(42);
    print_value("hello");
    print_value(3.14);

    // can't call add with non-addable type due to sfinae or concepts
    // uncommenting the next line would cause a compilation error:
    // NonAddableType n1("hello"), n2("world");
    // NonAddableType sum_invalid = add(n1, n2);  // error!

    Logger::getInstance().log(LogLevel::INFO, "concepts test passed");
}

// =============================================================================
// main runner function to execute all tests
// =============================================================================

void run_all_template_tests() {
    // run tests in order of increasing complexity
    test_basic_template();
    test_class_template();
    test_template_specialization();
    test_template_default_params();
    test_variadic_templates();
    test_template_metaprogramming();
    test_sfinae();
    test_template_template_params();
    test_crtp();
    test_fold_expressions();
    test_constexpr_if();
    test_concepts();
}