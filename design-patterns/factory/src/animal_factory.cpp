// MIT License
// Copyright (c) 2025 dbjwhs

#include <iostream>
#include <memory>
#include <string>
#include <stdexcept>

// abc for generic animal
class Animal {
public:
    virtual ~Animal() = default;
    virtual void makeSound() = 0;
    virtual void move() = 0;
};

// concrete Products
class Dog final : public Animal {
public:
    void makeSound() override {
        std::cout << "Woof!" << std::endl;
    }

    void move() override {
        std::cout << "Dog runs on four legs" << std::endl;
    }
};

class Cat final : public Animal {
public:
    void makeSound() override {
        std::cout << "Meow!" << std::endl;
    }

    void move() override {
        std::cout << "Cat walks gracefully" << std::endl;
    }
};

class Bird final : public Animal {
public:
    void makeSound() override {
        std::cout << "Tweet!" << std::endl;
    }

    void move() override {
        std::cout << "Bird flies in the air" << std::endl;
    }
};

// animal Factory with static method
class AnimalFactory {
public:
    // static factory method
    [[nodiscard]] static std::unique_ptr<Animal> createAnimal(const std::string& animalType) {
        if (animalType == "dog") {
            return std::make_unique<Dog>();
        }
        else if (animalType == "cat") {
            return std::make_unique<Cat>();
        }
        else if (animalType == "bird") {
            return std::make_unique<Bird>();
        }
        throw std::runtime_error("Unknown animal type: " + animalType);
    }
};

// example of a factory with instance configuration (alternative approach)
class ConfigurableAnimalFactory {
private:
    bool makeNoisyAnimals;

public:
    explicit ConfigurableAnimalFactory(const bool noisy) : makeNoisyAnimals(noisy) {}

    [[nodiscard]] std::unique_ptr<Animal> createAnimal(const std::string& animalType) const {
        auto animal = AnimalFactory::createAnimal(animalType);
        if (makeNoisyAnimals) {
            animal->makeSound(); // Animals make sound immediately when created
        }
        return animal;
    }
};

// example usage
int main() {
    std::cout << "=== Testing Static Factory ===\n" << std::endl;
    try {
        // using static factory method
        const auto dog = AnimalFactory::createAnimal("dog");
        const auto cat = AnimalFactory::createAnimal("cat");
        const auto bird = AnimalFactory::createAnimal("bird");

        std::cout << "Dog behavior:" << std::endl;
        dog->makeSound();
        dog->move();

        std::cout << "\nCat behavior:" << std::endl;
        cat->makeSound();
        cat->move();

        std::cout << "\nBird behavior:" << std::endl;
        bird->makeSound();
        bird->move();

        std::cout << "\n=== Testing Configurable Factory ===\n" << std::endl;

        // test noisy factory
        std::cout << "Creating animals with noisy factory:" << std::endl;
        ConfigurableAnimalFactory noisyFactory(true);
        std::cout << "\nCreating dog:" << std::endl;
        const auto noisyDog = noisyFactory.createAnimal("dog");  // Will make sound automatically
        std::cout << "Making dog move:" << std::endl;
        noisyDog->move();

        std::cout << "\nCreating cat:" << std::endl;
        const auto noisyCat = noisyFactory.createAnimal("cat");  // Will make sound automatically
        std::cout << "Making cat move:" << std::endl;
        noisyCat->move();

        // test quiet factory
        std::cout << "\nCreating animals with quiet factory:" << std::endl;
        ConfigurableAnimalFactory quietFactory(false);
        std::cout << "\nCreating dog:" << std::endl;
        auto quietDog = quietFactory.createAnimal("dog");  // Won't make sound automatically
        std::cout << "Making dog move and sound explicitly:" << std::endl;
        quietDog->move();
        quietDog->makeSound();

        std::cout << "\nCreating cat:" << std::endl;
        const auto quietCat = quietFactory.createAnimal("cat");  // Won't make sound automatically
        std::cout << "Making cat move and sound explicitly:" << std::endl;
        quietCat->move();
        quietCat->makeSound();

        // test error handling
        std::cout << "\n=== Testing Error Handling ===\n" << std::endl;
        std::cout << "Attempting to create unknown animal type..." << std::endl;
        auto unknown = AnimalFactory::createAnimal("fish");
    }
    catch (const std::runtime_error& e) {
        std::cout << "Error caught successfully: " << e.what() << std::endl;
    }

    return 0;
}