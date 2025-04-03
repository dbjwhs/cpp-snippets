// MIT License
// Copyright (c) 2025 dbjwhs

#include <memory>
#include <string>
#include <cassert>
#include <format>
#include "../../../../headers/project_utils.hpp"

// abstract factory design pattern overview:
// - purpose: provides an interface for creating families of related or dependent objects
//   without specifying their concrete classes
//
// - key components:
//   1. abstract factory (uifactory): declares interface for operations that create abstract products
//   2. concrete factories (lightthemefactory, darkthemefactory): implement operations to create concrete products
//   3. abstract products (button, checkbox): declare interfaces for a family of related products
//   4. concrete products (lightbutton, darkbutton, etc): define products created by concrete factory
//
// - benefits:
//   - isolates concrete classes from client code
//   - ensures product families work together (e.g., all ui components match in theme)
//   - makes exchanging product families easy (switch entire theme by changing factory)
//   - promotes consistency among products (factory guarantees compatible products)
//
// - use when:
//   - system needs to be independent from how its products are created/composed
//   - system should be configured with one of multiple families of products
//   - family of related product objects is designed to be used together
//   - you want to provide a class library of products, revealing just their interfaces
//
// - real-world analogies:
//   - ui themes (as implemented here)
//   - cross-platform gui toolkits
//   - database connectors for different database systems
//   - network protocol implementations
//
// - pattern consequences:
//   + consistent product families guaranteed
//   + loose coupling between concrete products and client code
//   + single responsibility principle: product creation logic is centralized
//   - adding new product types requires changing abstract factory and all concrete factories
//   - can increase complexity when product families are not needed

// abstract product interfaces
class Button {
protected:
    std::string m_theme;
    std::string m_text;
public:
    explicit Button(std::string  text) : m_text(std::move(text)) {}
    virtual ~Button() = default;
    virtual void render() const = 0;
    [[nodiscard]] virtual std::string getTheme() const {
        return m_theme;
    }
};

class Checkbox {
protected:
    std::string m_theme;
    bool m_checked;
public:
    Checkbox() : m_checked(false) {}
    virtual ~Checkbox() = default;
    virtual void toggle() = 0;
    virtual void render() const = 0;
    [[nodiscard]] virtual std::string getTheme() const {
        return m_theme;
    }
    [[nodiscard]] bool isChecked() const {
        return m_checked;
    }
};

// concrete products for light theme
class LightButton final : public Button {
public:
    explicit LightButton(const std::string& text) : Button(text) {
        m_theme = "light";
    }
    void render() const override {
        Logger::getInstance().log(LogLevel::INFO,
            std::format("rendering light button with text: {}", m_text));
    }
};

class LightCheckbox final : public Checkbox {
public:
    LightCheckbox() { m_theme = "light"; }
    void toggle() override {
        m_checked = !m_checked;
        Logger::getInstance().log(LogLevel::INFO,
            std::format("light checkbox toggled to: {}", m_checked));
    }
    void render() const override {
        Logger::getInstance().log(LogLevel::INFO,
            std::format("rendering light checkbox, checked: {}", m_checked));
    }
};

// concrete products for dark theme
class DarkButton final : public Button {
public:
    explicit DarkButton(const std::string& text) : Button(text) {
        m_theme = "dark";
    }
    void render() const override {
        Logger::getInstance().log(LogLevel::INFO,
            std::format("rendering dark button with text: {}", m_text));
    }
};

class DarkCheckbox final : public Checkbox {
public:
    DarkCheckbox() {
        m_theme = "dark";
    }
    void toggle() override {
        m_checked = !m_checked;
        Logger::getInstance().log(LogLevel::INFO,
            std::format("dark checkbox toggled to: {}", m_checked));
    }
    void render() const override {
        Logger::getInstance().log(LogLevel::INFO,
            std::format("rendering dark checkbox, checked: {}", m_checked));
    }
};

// abstract factory interface
class UIFactory {
public:
    virtual ~UIFactory() = default;
    virtual std::unique_ptr<Button> createButton(const std::string& text) = 0;
    virtual std::unique_ptr<Checkbox> createCheckbox() = 0;
    [[nodiscard]] virtual std::string getTheme() const = 0;
};

// concrete factories
class LightThemeFactory final : public UIFactory {
public:
    std::unique_ptr<Button> createButton(const std::string& text) override {
        return std::make_unique<LightButton>(text);
    }

    std::unique_ptr<Checkbox> createCheckbox() override {
        return std::make_unique<LightCheckbox>();
    }

    [[nodiscard]] std::string getTheme() const override {
        return "light";
    }
};

class DarkThemeFactory : public UIFactory {
public:
    std::unique_ptr<Button> createButton(const std::string& text) override {
        return std::make_unique<DarkButton>(text);
    }

    std::unique_ptr<Checkbox> createCheckbox() override {
        return std::make_unique<DarkCheckbox>();
    }

    [[nodiscard]] std::string getTheme() const override {
        return "dark";
    }
};

// test helper function to verify theme consistency
void testThemeConsistency(const UIFactory& factory, const Button& button, const Checkbox& checkbox) {
    assert(factory.getTheme() == button.getTheme() && "button theme must match factory theme");
    assert(factory.getTheme() == checkbox.getTheme() && "checkbox theme must match factory theme");
    Logger::getInstance().log(LogLevel::INFO,
        std::format("theme consistency verified for {} theme", factory.getTheme()));
}

int main() {
    // test light theme
    LOG_INFO("starting light theme tests");
    const auto lightFactory = std::make_unique<LightThemeFactory>();
    const auto lightButton = lightFactory->createButton("OK");
    const auto lightCheckbox = lightFactory->createCheckbox();

    // verify light theme consistency
    testThemeConsistency(*lightFactory, *lightButton, *lightCheckbox);

    // test light theme components
    lightButton->render();
    lightCheckbox->render();
    assert(!lightCheckbox->isChecked() && "checkbox should start unchecked");
    lightCheckbox->toggle();
    assert(lightCheckbox->isChecked() && "checkbox should be checked after toggle");

    // test dark theme
    LOG_INFO("starting dark theme tests");
    const auto darkFactory = std::make_unique<DarkThemeFactory>();
    const auto darkButton = darkFactory->createButton("Cancel");
    const auto darkCheckbox = darkFactory->createCheckbox();

    // verify dark theme consistency
    testThemeConsistency(*darkFactory, *darkButton, *darkCheckbox);

    // test dark theme components
    darkButton->render();
    darkCheckbox->render();
    assert(!darkCheckbox->isChecked() && "checkbox should start unchecked");
    darkCheckbox->toggle();
    assert(darkCheckbox->isChecked() && "checkbox should be checked after toggle");

    LOG_INFO("all tests completed successfully");
    return 0;
}
