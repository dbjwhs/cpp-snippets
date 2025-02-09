// MIT License
// Copyright (c) 2025 dbjwhs

#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include "../../../../headers/project_utils.hpp"

// this is the object whose state we want to save
class Document {
private:
    std::string m_content;
    std::string m_font_name;
    std::size_t m_font_size;
    bool m_is_bold;

public:
    Document() : m_font_name("Arial"), m_font_size(12), m_is_bold(false) {}

    // methods to change document state
    void set_content(const std::string& content) { m_content = content; }
    void set_font_name(const std::string& font) { m_font_name = font; }
    void set_font_size(const std::size_t size) { m_font_size = size; }
    void set_bold(const bool bold) { m_is_bold = bold; }

    // getters
    [[nodiscard]] std::string get_content() const { return m_content; }
    [[nodiscard]] std::string get_font_name() const { return m_font_name; }
    [[nodiscard]] std::size_t get_font_size() const { return m_font_size; }
    [[nodiscard]] bool is_bold() const { return m_is_bold; }

    // key piece: creates a memento containing the current state
    class Memento {
    private:
        std::string m_content;
        std::string m_font_name;
        std::size_t m_font_size;
        bool m_is_bold;

        // only Document can access Memento's constructor
        friend class Document;

        Memento(std::string  content, std::string  font_name,
                std::size_t font_size, const bool is_bold)
            : m_content(std::move(content)), m_font_name(std::move(font_name)),
              m_font_size(font_size), m_is_bold(is_bold) {}

    public:
        // make copying explicit and controlled
        Memento(const Memento&) = default;
        Memento& operator=(const Memento&) = default;
    };

    // creates a memento containing the current state
    // note
    // using curly braces {} for initialization instead of parentheses ();
    // this is a modern C++ style recommendation that helps prevent some common issues.
    //
    // example
    // int x(5.3); -> Will compile, silently truncates to 5
    // int x{5.3}; -> Won't compile, prevents accidental data loss
    [[nodiscard]] Memento create_memento() const {
        return Memento{m_content, m_font_name, m_font_size, m_is_bold};
    }

    // restores state from a memento
    void restore_from_memento(const Memento& memento) {
        m_content = memento.m_content;
        m_font_name = memento.m_font_name;
        m_font_size = memento.m_font_size;
        m_is_bold = memento.m_is_bold;
    }
};

// caretaker class that manages document history
class DocumentHistory {
private:
    Document& m_document;
    std::vector<Document::Memento> m_history;
    size_t m_current_index;

public:
    explicit DocumentHistory(Document& doc)
        : m_document(doc), m_current_index(0) {
        // save initial state
        save_state();
    }

    void save_state() {
        // when saving a new state, remove any redo states
        if (m_current_index < m_history.size()) {
            // Warning about vector::erase with size_t index:
            // When adding size_t (unsigned long) to vector::iterator, it converts to
            // difference_type (signed long). This conversion from unsigned->signed could
            // lose data for very large indices; if you encounter that issue I suggest
            // this (IMHO) ugly cast...
            // m_history.erase(m_history.begin() + static_cast<std::ptrdiff_t>(m_current_index), m_history.end());
            // ...I tested it, it works, but ugly so nope.
            m_history.erase(m_history.begin() + m_current_index, m_history.end());
        }
        m_history.push_back(m_document.create_memento());
        m_current_index = m_history.size();
    }

    void undo() {
        if (m_current_index > 0) {
            m_current_index--;
            m_document.restore_from_memento(m_history[m_current_index-1]);
        }
    }

    void redo() {
        if (m_current_index < m_history.size() - 1) {
            m_current_index++;
            m_document.restore_from_memento(m_history[m_current_index]);
        }
    }
};

// example usage and tests
void run_tests(Logger& logger) {
    Document doc;
    DocumentHistory history(doc);

    logger.log(LogLevel::INFO, "Testing basic state changes...");

    // make some changes and save states
    doc.set_content("Hello");
    doc.set_font_size(14);
    history.save_state();
    assert(doc.get_content() == "Hello" && doc.get_font_size() == 14);
    logger.log(LogLevel::INFO, "State 1 saved: content='Hello', font size=14");

    doc.set_content("Hello World");
    doc.set_bold(true);
    doc.set_font_name("Arial");
    doc.set_font_size(14);
    history.save_state();
    assert(doc.get_content() == "Hello World" && doc.is_bold());
    logger.log(LogLevel::INFO, "State 2 saved: content='Hello World', bold=true");

    doc.set_font_name("Times");
    doc.set_font_size(16);
    history.save_state();
    assert(doc.get_font_name() == "Times" && doc.get_font_size() == 16);
    logger.log(LogLevel::INFO, "State 3 saved: font='Times', font size=16");

    // test undo
    logger.log(LogLevel::INFO, "Testing undo...");
    history.undo();
    assert(doc.get_font_name() == "Arial" && doc.get_font_size() == 14);
    logger.log(LogLevel::INFO, "Undo 1: returned to State 2");

    history.undo();
    assert(doc.get_content() == "Hello" && !doc.is_bold());
    logger.log(LogLevel::INFO, "Undo 2: returned to State 1");

    // test redo
    logger.log(LogLevel::INFO, "Testing redo...");
    history.redo();
    assert(doc.get_content() == "Hello World" && doc.is_bold());
    logger.log(LogLevel::INFO, "Redo 1: restored to State 2");

    // test branching history
    logger.log(LogLevel::INFO, "Testing branching history...");
    doc.set_content("New branch");
    history.save_state();
    assert(doc.get_content() == "New branch");
    logger.log(LogLevel::INFO, "Created new branch with content='New branch'");

    // ensure old redo states are discarded
    history.redo();  // should do nothing
    assert(doc.get_content() == "New branch");
    logger.log(LogLevel::INFO, "Verified old redo states are discarded");

    logger.log(LogLevel::INFO, "All tests passed!");
}

int main() {
    Logger logger("../custom.log");

    try {
        run_tests(logger);
    } catch (const std::exception& e) {
        logger.log(LogLevel::CRITICAL, "Error during testing: " + std::string(e.what()));
        return 1;
    }
    return 0;
}
