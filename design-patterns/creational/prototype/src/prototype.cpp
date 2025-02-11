// MIT License
// Copyright (c) 2025 dbjwhs

#include <iostream>
#include <string>
#include <cassert>
#include <typeinfo>
#include "./../../../../headers/project_utils.hpp"

// design pattern: prototype
// category: creational pattern
//
// purpose: creates new objects by cloning an existing instance (prototype),
//         serving as a blueprint for objects with similar properties. this
//         pattern is particularly useful when object creation is more costly
//         than cloning, or when creating objects at runtime based on dynamic
//         conditions.
//
// uml class structure:
// - prototype (abstract)
//   |-- concrete prototype a
//   |-- concrete prototype b
//
// key components:
// - prototype (abstract class/interface):
//   - declares interface for cloning itself
//   - typically includes a pure virtual clone() method
// - concrete prototype:
//   - implements cloning operation
//   - copies its own object's state to the clone
// - client:
//   - creates new objects by cloning the prototype
//
// memory considerations:
// - shallow copy vs deep copy decisions critical
// - smart pointers (unique_ptr/shared_ptr) recommended for ownership
// - copy constructors and assignment operators may need custom implementation
// - consider resource handling in clone operations
// - watch for circular references in complex object structures
//
// thread safety:
// - prototype registry should be thread-safe if shared
// - clone operations should be thread-safe
// - consider mutex protection for shared prototypes
// - ensure atomic operations for reference counting
//
// performance implications:
// - cloning typically faster than construction
// - deep copying can be expensive for complex objects
// - memory overhead from maintaining prototype instances
// - consider lazy initialization for expensive prototypes
//
// common implementations:
// 1. registry-based:
//    maintains a collection of prototype instances
//    prototypes[key]->clone()
//
// 2. factory-based:
//    combines with factory pattern
//    factory.createProduct(type).clone()
//
// 3. singleton registry:
//    global access to prototype collection
//    PrototypeRegistry::instance().getPrototype(key)
//
// implementation variants:
// - deep vs shallow copying
// - cloning with parameters
// - prototype manager/registry
// - prototype factory
// - cached prototype
//
// best practices:
// - use smart pointers for memory management
// - implement virtual destructor
// - consider both deep and shallow copy needs
// - document cloning behavior
// - validate cloned objects
// - handle null/invalid cases
// - use const correctness
// - provide clear error messages
//
// pitfalls to avoid:
// - circular references in deep copying
// - memory leaks in clone implementation
// - inconsistent copy semantics
// - unnecessary deep copying
// - ignoring resource cleanup
// - forgetting virtual destructor
// - neglecting error handling
//
// example usage:
// class ConcretePrototype : public Prototype {
//     private:
//         // resource handles, pointers, complex data
//         std::unique_ptr<Resource> m_resource;
//         std::vector<DataBlock> m_data;
//
//     public:
//         std::unique_ptr<Prototype> clone() const override {
//             // deep copy implementation
//             auto clone = std::make_unique<ConcretePrototype>();
//             clone->m_resource = std::make_unique<Resource>(*m_resource);
//             clone->m_data = m_data;  // vector handles its own deep copy
//             return clone;
//         }
// };
//
// complexity analysis:
// - time: O(n) for deep copy, O(1) for shallow copy
// - space: O(n) for deep copy, O(1) for shallow copy
//   where n is the size/complexity of the object
//
// related patterns:
// - abstract factory: can create prototypes
// - composite: often used with prototype for part hierarchies
// - decorator: can be used to add features to clones
// - command: can store prototypes for undo/redo
//
// this implementation:
// class: Prototype
// - uses unique_ptr for automatic resource management
// - provides virtual interface for cloning
// - implements basic attribute handling (name, price)
// - supports both deep and shallow copy through derived classes
// - includes debugging support via print_details()
//
// ************************************************************************
// historical note on inheritance and object slicing:
//
// during the 1990s-2005 era, a common architectural pattern was to create
// a single root/uber base class (similar to java's Object class) from which
// all other classes would inherit. examples include:
//   - microsoft's mfc cObject
//   - borland's toObject
//   - early game engines' gameObject
//
// advantages of this approach:
// - enabled polymorphic behavior across entire codebase
// - simplified container storage (heterogeneous collections)
// - provided common serialization mechanisms
// - unified error handling and runtime type information (rtti)
// - facilitated common memory management strategies
//
// critical warning - object slicing:
// when passing derived objects by value instead of by pointer/reference,
// the derived portion of the object gets "sliced off", leaving only the
// base class portion. example:
//   class Base { int x; };
//   class Derived : public Base { int y; };
//   void func(Base val) { ... }  // slicing occurs here
//   Derived d;
//   func(d);  // only Base::x is copied, Derived::y is lost
//
// modern best practices (2024):
// - prefer composition over inheritance
// - use virtual interfaces for polymorphic behavior
// - leverage smart pointers (unique_ptr, shared_ptr)
// - consider std::variant for type-safe polymorphism
// - use references/pointers when polymorphism is needed
// - explicitly delete copy operations if slicing is a concern:
//   Base(const Base&) = delete;
// ************************************************************************
class Prototype {

protected:
    Logger& m_logger = Logger::getInstance();
    std::string m_name;
    float m_price{};

public:
    Prototype() = default;

    // getters for testing
    [[nodiscard]] std::string get_name() const { return m_name; }
    [[nodiscard]] float get_price() const { return m_price; }

    Prototype(std::string  name, const float price) : m_name(std::move(name)), m_price(price) {}
    virtual ~Prototype() = default;

    // pure virtual clone method that concrete prototypes must implement
    [[nodiscard]] virtual std::unique_ptr<Prototype> clone() const = 0;

    // virtual method to get product details
    virtual void print_details() const {
        m_logger.log(LogLevel::NORMAL, "name: " + m_name + ", price: $" + std::to_string(m_price));
    }
};

// concrete prototype class for electronics products
class ElectronicProduct final : public Prototype {
private:
    int m_warranty_months;
    std::string m_manufacturer;

public:
    ElectronicProduct(const std::string& name, const float price, const int warranty_months, std::string  manufacturer)
        : Prototype(name, price) /* note base class initialized */
        , m_warranty_months(warranty_months)
        , m_manufacturer(std::move(manufacturer)) {}

    // implementation of clone method for electronic products
    [[nodiscard]] std::unique_ptr<Prototype> clone() const override {
        return std::make_unique<ElectronicProduct>(*this);
    }

    // override print details to include electronic-specific information
    void print_details() const override {
        Prototype::print_details();
        m_logger.log(LogLevel::INFO, "warranty: " + std::to_string(m_warranty_months) + " months, "
                  + "manufacturer: " + m_manufacturer);
    }
};

// concrete prototype class for clothing products
class ClothingProduct final : public Prototype {
private:
    std::string m_size;
    std::string m_material;

public:
    ClothingProduct(const std::string& name, const float price, std::string  size, std::string  material)
        : Prototype(name, price) /* note base class initialized */
        , m_size(std::move(size))
        , m_material(std::move(material)) {}

    // implementation of clone method for clothing products
    [[nodiscard]] std::unique_ptr<Prototype> clone() const override {
        return std::make_unique<ClothingProduct>(*this);
    }

    // override print details to include clothing-specific information
    void print_details() const override {
        Prototype::print_details();
        m_logger.log(LogLevel::INFO, "size: " + m_size + ", material: " + m_material);
    }
};

// prototype manager class to store and manage prototypes
class PrototypeManager {
private:
    std::unique_ptr<Prototype> m_electronic_prototype;
    std::unique_ptr<Prototype> m_clothing_prototype;

public:
    PrototypeManager() {
        // initialize with default prototypes
        m_electronic_prototype = std::make_unique<ElectronicProduct>("default electronic", 0.0f, 12, "unknown");
        m_clothing_prototype = std::make_unique<ClothingProduct>("default clothing", 0.0f, "M", "cotton");
    }

    // register new prototypes
    void register_electronic(std::unique_ptr<Prototype> prototype) {
        m_electronic_prototype = std::move(prototype);
    }

    void register_clothing(std::unique_ptr<Prototype> prototype) {
        m_clothing_prototype = std::move(prototype);
    }

    // create clones from registered prototypes
    [[nodiscard]] std::unique_ptr<Prototype> create_electronic() const {
        return m_electronic_prototype->clone();
    }

    [[nodiscard]] std::unique_ptr<Prototype> create_clothing() const {
        return m_clothing_prototype->clone();
    }
};

// helper class for testing prototype attributes
class ProductTester {
public:
    static void verify_electronic(const Prototype* product,
                                const std::string& expected_name,
                                const float expected_price) {
        // verify object type
        assert(typeid(*product) == typeid(ElectronicProduct) && "product type mismatch: expected ElectronicProduct");

        // verify basic attributes through base class
        const auto* electronic =
            dynamic_cast<const ElectronicProduct*>(product);
        assert(electronic != nullptr && "dynamic cast failed");

        // verify name and price
        assert(electronic->get_name() == expected_name && "electronic product name mismatch");
        assert(std::abs(electronic->get_price() - expected_price) < 0.001f && "electronic product price mismatch");
    }

    static void verify_clothing(const Prototype* product,
                              const std::string& expected_name,
                              const float expected_price) {
        // verify object type
        assert(typeid(*product) == typeid(ClothingProduct) && "product type mismatch: expected ClothingProduct");

        // verify basic attributes through base class
        const auto* clothing =
            dynamic_cast<const ClothingProduct*>(product);
        assert(clothing != nullptr && "dynamic cast failed");

        // verify name and price
        assert(clothing->get_name() == expected_name && "clothing product name mismatch");
        assert(std::abs(clothing->get_price() - expected_price) < 0.001f && "clothing product price mismatch");
    }
};

int main() {
    Logger& logger = Logger::getInstance();

    // test case 1: prototype manager initialization
    logger.log(LogLevel::INFO, "case 1: prototype manager initialization");
    PrototypeManager manager;

    // register test prototypes
    const std::string electronic_name = "smartphone";
    constexpr float electronic_price = 999.99f;
    constexpr int electronic_warranty = 24;
    const std::string electronic_manufacturer = "techcorp";

    const std::string clothing_name = "t-shirt";
    constexpr float clothing_price = 29.99f;
    const std::string clothing_size = "L";
    const std::string clothing_material = "organic cotton";

    manager.register_electronic(
        std::make_unique<ElectronicProduct>(
            electronic_name, electronic_price,
            electronic_warranty, electronic_manufacturer));

    manager.register_clothing(
        std::make_unique<ClothingProduct>(
            clothing_name, clothing_price,
            clothing_size, clothing_material));

    // test case 2: verify correct prototype cloning
    logger.log(LogLevel::INFO, "case 2: verify correct prototype cloning");
    auto electronic1 = manager.create_electronic();
    auto clothing1 = manager.create_clothing();

    ProductTester::verify_electronic(electronic1.get(), electronic_name, electronic_price);
    ProductTester::verify_clothing(clothing1.get(), clothing_name, clothing_price);

    // test case 3: verify clone independence
    logger.log(LogLevel::INFO, "case 3: verify clone independence");
    auto electronic2 = manager.create_electronic();
    auto clothing2 = manager.create_clothing();

    // verify pointers are different
    assert(electronic1.get() != electronic2.get() &&
           "cloned objects share same memory address");
    assert(clothing1.get() != clothing2.get() &&
           "cloned objects share same memory address");

    // test case 4: prototype replacement
    logger.log(LogLevel::INFO, "case 4: prototype replacement");
    const std::string new_electronic_name = "laptop";
    constexpr float new_electronic_price = 1499.99f;
    constexpr int new_electronic_warranty = 36;
    const std::string new_electronic_manufacturer = "computech";

    manager.register_electronic(
        std::make_unique<ElectronicProduct>(
            new_electronic_name, new_electronic_price,
            new_electronic_warranty, new_electronic_manufacturer));

    auto electronic3 = manager.create_electronic();
    ProductTester::verify_electronic(electronic3.get(), new_electronic_name, new_electronic_price);

    // verify original clones remain unchanged
    ProductTester::verify_electronic(electronic1.get(), electronic_name, electronic_price);
    ProductTester::verify_electronic(electronic2.get(), electronic_name, electronic_price);

    logger.log(LogLevel::INFO, "tests passed successfully!");
    return 0;
}
