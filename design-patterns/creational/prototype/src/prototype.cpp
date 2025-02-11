// MIT License
// Copyright (c) 2025 dbjwhs

#include <iostream>
#include <string>
#include <cassert>
#include <typeinfo>
#include "./../../../../headers/project_utils.hpp"

// prototype pattern
//
// intent: specify the kinds of objects to create using a prototypical
// instance, and create new objects by cloning this prototype. this pattern
// is used when the cost of creating an object is more expensive than copying.
//
// key aspects of the pattern:
// - avoids subclassing a creator class, like in factory method pattern
// - avoids the inherent cost of creating a new object in the standard way
// - allows dynamically adding/removing products at runtime
// - specifies new objects by varying values rather than classes
// - reduces the need for creating subclasses
//
// typical use cases:
// - when a system needs to be independent of how its products are created
// - when classes to instantiate are specified at run-time
// - when avoiding building a class hierarchy of factories
// - when instances of a class can have one of only a few different combinations
//   of state
//
// advantages:
// - hides concrete product classes from the client
// - allows adding/removing products at runtime
// - specifies new objects by varying values
// - reduces subclass proliferation
// - configures an application with classes dynamically
//
// disadvantages:
// - each concrete prototype subclass must implement the clone operation
// - cloning complex objects with circular references can be challenging
//
// class: prototype
// purpose: declares an interface for cloning itself. serves as the base
//          prototype from which all concrete prototypes derive.
//
// key members:
// - m_name: identifier for the prototype instance
// - m_price: cost value associated with the prototype
// - clone(): pure virtual method that concrete classes must implement to
//           create a copy of themselves
// - print_details(): virtual method to display prototype information
//
// implementation notes:
// - uses smart pointers (unique_ptr) for memory safety
// - implements virtual destructor for proper cleanup of derived classes
// - provides base functionality for common attributes (name, price)
// - uses modern c++ features for better type safety and performance
//
// memory management:
// - clone() returns unique_ptr to ensure proper resource management
// - virtual destructor ensures proper cleanup of derived classes
// - copy operations should be carefully implemented in derived classes
//
// threading considerations:
// - cloning operations are independent and thread-safe
// - shared prototypes should be protected if accessed concurrently
//
// extensibility:
// - new concrete prototypes can be added by inheriting from this class
// - additional common functionality can be added to this base class
// - clone method can be overridden to implement deep or shallow copying
//
// usage example:
// class ConcretePrototype : public Prototype {
//     std::unique_ptr<Prototype> clone() const override {
//         return std::make_unique<ConcretePrototype>(*this);
//     }
// };
//
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
    ElectronicProduct(const std::string& name, float price,
                     int warranty_months, std::string  manufacturer)
        : Prototype(name, price)
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
    ClothingProduct(const std::string& name, float price,
                   std::string  size, std::string  material)
        : Prototype(name, price)
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
