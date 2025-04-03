// MIT License
// Copyright (c) 2025 dbjwhs

#include <string>
#include <cassert>
#include <format>
#include "../../../../headers/project_utils.hpp"

// decorator pattern history and usage:
// the decorator pattern was introduced in 1994 by the "gang of four" (gof) in their book "design patterns: elements of reusable
// object-oriented software". it was created to solve the limitation of static inheritance by providing a more flexible way to extend
// object functionality at runtime. the pattern was inspired by earlier work in the smalltalk programming language, particularly
// the investigations into "wrapping" and "forwarding" mechanisms.
//
// the pattern allows behaviors to be added to individual objects without affecting other objects of the same class. it provides
// an alternative to subclassing where new functionality is added by wrapping objects with decorator classes. each decorator can
// add its own behavior either before and/or after delegating to the decorated object.
//
// common uses include:
// - gui components: adding borders, scrollbars, or visual effects to windows and controls
// - i/o streams: building chains of stream processors (buffering, compression, encryption)
// - middleware: adding logging, caching, authentication, or transaction management
// - text formatting: applying multiple formatting options (bold, italic, underline)
//
// key benefits:
// 1. supports single responsibility principle by separating core functionality from additional features
// 2. enables runtime modification of object behavior
// 3. provides greater flexibility than static inheritance
// 4. allows responsibilities to be combined in ways not anticipated in the original design
//
// implementation considerations:
// - decorators must conform to the interface of the components they decorate
// - decorators typically maintain a reference to a component object
// - multiple decorators can be chained, with order potentially affecting the final result
// - consider using smart pointers (like shared_ptr) for memory management
// - ensure decorators properly forward all relevant operations to their wrapped component

// base interface for text components
class TextComponent {
public:
    virtual ~TextComponent() = default;
    [[nodiscard]] virtual std::string render() const = 0;
};

// concrete component implementing base text functionality
class SimpleText final : public TextComponent {
private:
    std::string m_content;

public:
    explicit SimpleText(std::string content) : m_content(std::move(content)) {}
    
    [[nodiscard]] std::string render() const override {
        return m_content;
    }
};

// base decorator class maintaining reference to a wrapped component
class TextDecorator : public TextComponent {
protected:
    std::shared_ptr<TextComponent> m_component;

public:
    explicit TextDecorator(std::shared_ptr<TextComponent> component) : m_component(std::move(component)) {
        assert(m_component != nullptr);
    }
};

// concrete decorator adding bold formatting
class BoldDecorator final : public TextDecorator {
public:
    explicit BoldDecorator(std::shared_ptr<TextComponent> component) : TextDecorator(std::move(component)) {}

    [[nodiscard]] std::string render() const override {
        return "<b>" + m_component->render() + "</b>";
    }
};

// concrete decorator adding italics formatting
class ItalicDecorator final : public TextDecorator {
public:
    explicit ItalicDecorator(std::shared_ptr<TextComponent> component) : TextDecorator(std::move(component)) {}

    [[nodiscard]] std::string render() const override {
        return "<i>" + m_component->render() + "</i>";
    }
};

// concrete decorator adding underline formatting
class UnderlineDecorator final : public TextDecorator {
public:
    explicit UnderlineDecorator(std::shared_ptr<TextComponent> component) : TextDecorator(std::move(component)) {}

    [[nodiscard]] std::string render() const override {
        return "<u>" + m_component->render() + "</u>";
    }
};

// test helper function to validate decorator behavior
void testDecorator(const std::string& testName, const TextComponent& decorator, const std::string& expected) {
    const std::string actual = decorator.render();
    const bool passed = (actual == expected);
    
    LOG_INFO(std::format("Test {}: {}", testName, passed ? "PASSED" : "FAILED"));
    
    if (!passed) {
        LOG_ERROR(std::format("Expected: {}, Actual: {}", expected, actual));
    }
    assert(actual == expected);
}

int main() {
    LOG_INFO("Starting decorator pattern tests");

    // test 1: simple text without decorators
    auto baseText = std::make_shared<SimpleText>("Hello");
    testDecorator("Simple Text", *baseText, "Hello");

    // test 2: single decorator (bold)
    auto boldText = std::make_shared<BoldDecorator>(baseText);
    testDecorator("Bold Decorator", *boldText, "<b>Hello</b>");

    // test 3: nested decorators (bold and italic)
    auto boldItalicText = std::make_shared<ItalicDecorator>(boldText);
    testDecorator("Bold + Italic", *boldItalicText, "<i><b>Hello</b></i>");

    // test 4: triple-nested decorators (bold and italic + underline)
    const auto allDecorators = std::make_shared<UnderlineDecorator>(boldItalicText);
    testDecorator("All Decorators", *allDecorators, "<u><i><b>Hello</b></i></u>");

    // test 5: different nesting order
    const auto differentOrder = std::make_shared<BoldDecorator>(
        std::make_shared<UnderlineDecorator>(
            std::make_shared<ItalicDecorator>(
                std::make_shared<SimpleText>("Hello")
            )
        )
    );
    testDecorator("Different Nesting Order", *differentOrder, "<b><u><i>Hello</i></u></b>");

    // test 6: empty string handling
    const auto emptyText = std::make_shared<BoldDecorator>(
        std::make_shared<SimpleText>("")
    );
    testDecorator("Empty String", *emptyText, "<b></b>");

    // test 7: special characters handling
    const auto specialChars = std::make_shared<ItalicDecorator>(
        std::make_shared<SimpleText>("Hello & World!")
    );
    testDecorator("Special Characters", *specialChars, "<i>Hello & World!</i>");

    LOG_INFO("All decorator pattern tests completed");
    return 0;
}
