// MIT License
// Copyright (c) 2025 dbjwhs

#include <iostream>
#include <ranges>
#include <string>
#include <stack>
#include <cassert>
#include <map>
#include "../../../../headers/project_utils.hpp"

// key piece: abstract command class defining the interface for all concrete commands
// this is critical as it allows us to treat all commands uniformly and store them for undo/redo
class Command {
public:
    virtual ~Command() = default;
    virtual void execute() = 0;    // performs the command
    virtual void undo() = 0;       // reverses the command
    [[nodiscard]] virtual std::unique_ptr<Command> clone() const = 0;
};

// document class that commands will modify
class Document {
private:
    std::string m_content;
public:
    void insert(const std::string& text, const std::size_t position) {
        m_content.insert(position, text);
    }

    void erase(const std::size_t position, const std::size_t length) {
        m_content.erase(position, length);
    }

    [[nodiscard]] std::string get_content() const { return m_content; }
};

// concrete command for inserting text
class InsertCommand final : public Command {
private:
    Document& m_document;
    std::string m_text;
    int m_position;

public:
    InsertCommand(Document& doc, std::string  text, int position)
        : m_document(doc), m_text(std::move(text)), m_position(position) {}

    void execute() override {
        m_document.insert(m_text, m_position);
    }

    void undo() override {
        m_document.erase(m_position, m_text.length());
    }

    [[nodiscard]] std::unique_ptr<Command> clone() const override {
        return std::make_unique<InsertCommand>(m_document, m_text, m_position);
    }
};

// concrete command for erasing text
class EraseCommand final : public Command {
private:
    Document& m_document;
    int m_position;
    int m_length;
    std::string m_erased_text;  // stores erased text for undo

public:
    EraseCommand(Document& doc, int position, int length)
        : m_document(doc), m_position(position), m_length(length) {
        m_erased_text = doc.get_content().substr(position, length);
    }

    void execute() override {
        m_document.erase(m_position, m_length);
    }

    void undo() override {
        m_document.insert(m_erased_text, m_position);
    }

    [[nodiscard]] std::unique_ptr<Command> clone() const override {
        auto clone = std::make_unique<EraseCommand>(m_document, m_position, m_length);
        clone->m_erased_text = m_erased_text;  // need to copy the stored text
        return clone;
    }
};

// key piece: command invoker class that maintains command history
// this class is crucial for implementing undo/redo functionality and
// demonstrates how commands can be treated as objects
class DocumentEditor {
private:
    Document& m_document;
    std::stack<std::unique_ptr<Command>> m_undo_stack;
    std::stack<std::unique_ptr<Command>> m_redo_stack;

public:
    explicit DocumentEditor(Document& doc) : m_document(doc) {}

    // executes a command and adds it to undo stack
    void execute_command(std::unique_ptr<Command> command) {
        command->execute();
        m_undo_stack.push(std::move(command));
        // clear redo stack as a new command breaks redo chain
        while (!m_redo_stack.empty()) {
            m_redo_stack.pop();
        }
    }

    // undoes last command
    void undo() {
        if (m_undo_stack.empty()) {
            return;
        }
        m_undo_stack.top()->undo();
        m_redo_stack.push(std::move(m_undo_stack.top()));
        m_undo_stack.pop();
    }

    // redoes last undone command
    void redo() {
        if (m_redo_stack.empty()) {
            return;
        }
        m_redo_stack.top()->execute();
        m_undo_stack.push(std::move(m_redo_stack.top()));
        m_redo_stack.pop();
    }
};

// comprehensive test function
void run_document_tests(Logger& logger) {
    Document doc;
    DocumentEditor editor(doc);

    logger.log(LogLevel::INFO, "testing basic insert command...");
    editor.execute_command(std::make_unique<InsertCommand>(doc, "Hello", 0));
    assert(doc.get_content() == "Hello");
    logger.log(LogLevel::INFO, "content after insert: " + doc.get_content());

    logger.log(LogLevel::INFO, "testing basic erase command...");
    editor.execute_command(std::make_unique<EraseCommand>(doc, 0, 2));
    assert(doc.get_content() == "llo");
    logger.log(LogLevel::INFO, "content after erase: " + doc.get_content());

    logger.log(LogLevel::INFO, "testing undo functionality...");
    editor.undo();  // undo erase
    assert(doc.get_content() == "Hello");
    logger.log(LogLevel::INFO, "content after undo: " + doc.get_content());

    logger.log(LogLevel::INFO, "testing redo functionality...");
    editor.redo();  // redo erase
    assert(doc.get_content() == "llo");
    logger.log(LogLevel::INFO, "content after redo: " + doc.get_content());

    logger.log(LogLevel::INFO, "testing multiple commands...");
    editor.execute_command(std::make_unique<InsertCommand>(doc, " World", 3));
    assert(doc.get_content() == "llo World");
    editor.execute_command(std::make_unique<EraseCommand>(doc, 3, 1));  // erase the space
    assert(doc.get_content() == "lloWorld");
    logger.log(LogLevel::INFO, "content after multiple commands: " + doc.get_content());

    logger.log(LogLevel::INFO, "testing multiple undos...");
    editor.undo();  // undo last erase
    editor.undo();  // undo last insert
    editor.undo();  // undo first erase
    assert(doc.get_content() == "Hello");
    logger.log(LogLevel::INFO, "content after multiple undos: " + doc.get_content());

    logger.log(LogLevel::INFO, "all tests passed!");
}

class SmartDevice {
private:
    const std::string m_id;
    std::size_t m_brightness;
    std::size_t m_temperature;
    bool m_is_on;

public:
    explicit SmartDevice(std::string id) : m_id(std::move(id)), m_brightness(0),
                                        m_temperature(20), m_is_on(false) {}

    void power(const bool state) {
        m_is_on = state;
    }

    void set_brightness(const std::size_t level) {
        m_brightness = level;
    }

    void set_temperature(const std::size_t temp) {
        m_temperature = temp;
    }

    // getters
    [[nodiscard]] bool is_on() const { return m_is_on; }
    [[nodiscard]] std::size_t get_brightness() const { return m_brightness; }
    [[nodiscard]] std::size_t get_temperature() const { return m_temperature; }
};

// concrete commands for device control
class PowerCommand final : public Command {
private:
    SmartDevice& m_device;
    bool m_previous_state;
    bool m_new_state;

public:
    PowerCommand(SmartDevice& device, bool state)
        : m_device(device), m_previous_state(device.is_on()), m_new_state(state) {}

    void execute() override {
        m_device.power(m_new_state);
    }

    void undo() override {
        m_device.power(m_previous_state);
    }

    [[nodiscard]] std::unique_ptr<Command> clone() const override {
        return std::make_unique<PowerCommand>(m_device, m_new_state);
    }
};

class SetTemperatureCommand final : public Command {
private:
    SmartDevice& m_device;
    std::size_t m_previous_temp;
    std::size_t m_new_temp;

public:
    SetTemperatureCommand(SmartDevice& device, std::size_t temp)
        : m_device(device), m_previous_temp(device.get_temperature()), m_new_temp(temp) {}

    void execute() override {
        m_device.set_temperature(m_new_temp);
    }

    void undo() override {
        m_device.set_temperature(m_previous_temp);
    }

    [[nodiscard]] std::unique_ptr<Command> clone() const override {
        return std::make_unique<SetTemperatureCommand>(m_device, m_new_temp);
    }
};

// composite command for scene setting
class SceneCommand final : public Command {
private:
    std::vector<std::unique_ptr<Command>> m_commands;

public:
    // add default constructor
    SceneCommand() = default;

    // delete copy constructor and assignment operator
    SceneCommand(const SceneCommand&) = delete;
    SceneCommand& operator=(const SceneCommand&) = delete;

    // add move constructor and assignment operator
    SceneCommand(SceneCommand&&) = default;
    SceneCommand& operator=(SceneCommand&&) = default;

    void add_command(std::unique_ptr<Command> command) {
        m_commands.push_back(std::move(command));
    }

    void execute() override {
        for (const auto& cmd : m_commands) {
            cmd->execute();
        }
    }

    void undo() override {
        for (const auto & m_command : std::ranges::reverse_view(m_commands)) {
            m_command->undo();
        }
    }

    // add clone method for making deep copies
    [[nodiscard]] std::unique_ptr<Command> clone() const override {
        auto new_scene = std::make_unique<SceneCommand>();
        for (const auto& cmd : m_commands) {
            new_scene->add_command(cmd->clone());
        }
        return new_scene;
    }
};

class HomeAutomationSystem {
private:
    std::stack<std::unique_ptr<Command>> m_history;
    std::map<std::string, std::unique_ptr<SceneCommand>> m_scenes;

public:
    void execute_command(std::unique_ptr<Command> command) {
        command->execute();
        m_history.push(std::move(command));
    }

    void undo_last() {
        if (!m_history.empty()) {
            m_history.top()->undo();
            m_history.pop();
        }
    }

    void create_scene(const std::string& name, std::unique_ptr<SceneCommand> scene) {
        m_scenes[name] = std::move(scene);
    }

    void activate_scene(const std::string& name) {
        if (auto it = m_scenes.find(name); it != m_scenes.end()) {
            // clone the scene instead of trying to copy it
            execute_command(it->second->clone());
        }
    }
};

void run_smart_device_tests(Logger& logger) {
    logger.log(LogLevel::INFO, "Starting Smart Home Automation Tests...");

    SmartDevice living_room_light("LR_LIGHT_1");
    const SmartDevice bedroom_light("BR_LIGHT_1");
    SmartDevice thermostat("THERM_1");

    // verify initial states
    logger.log(LogLevel::INFO, "Verifying initial device states...");
    assert(!living_room_light.is_on() && "Light should be off initially");
    assert(!bedroom_light.is_on() && "Light should be off initially");
    assert(thermostat.get_temperature() == 20 && "Thermostat should start at 20°C");
    logger.log(LogLevel::INFO, "✓ All devices initialized correctly");

    HomeAutomationSystem home;

    logger.log(LogLevel::INFO, "Creating 'movie time' scene...");
    auto movie_scene = std::make_unique<SceneCommand>();
    movie_scene->add_command(std::make_unique<PowerCommand>(living_room_light, false));
    movie_scene->add_command(std::make_unique<SetTemperatureCommand>(thermostat, 22));

    home.create_scene("movie_time", std::move(movie_scene));
    logger.log(LogLevel::INFO, "✓ Scene created successfully");

    logger.log(LogLevel::INFO, "Activating 'movie time' scene...");
    home.activate_scene("movie_time");
    assert(!living_room_light.is_on() && "Living room light should be off");
    assert(thermostat.get_temperature() == 22 && "Temperature should be 22°C");
    logger.log(LogLevel::INFO, "✓ Scene activated: lights dimmed and temperature set to 22°C");

    logger.log(LogLevel::INFO, "Adjusting temperature for cold person...");
    home.execute_command(std::make_unique<SetTemperatureCommand>(thermostat, 24));
    assert(thermostat.get_temperature() == 24 && "Temperature should be 24°C");
    logger.log(LogLevel::INFO, "✓ Temperature increased to 24°C");

    logger.log(LogLevel::INFO, "Testing undo functionality for temperature change...");
    home.undo_last();
    assert(thermostat.get_temperature() == 22 && "Temperature should be back to 22°C");
    logger.log(LogLevel::INFO, "✓ Temperature successfully reverted to 22°C");

    // test edge cases
    logger.log(LogLevel::INFO, "Testing edge cases...");

    logger.log(LogLevel::INFO, "Testing scene activation with non-existent scene...");
    home.activate_scene("non_existent_scene");
    assert(!living_room_light.is_on() && "Light state shouldn't change");
    assert(thermostat.get_temperature() == 22 && "Temperature shouldn't change");
    logger.log(LogLevel::INFO, "✓ Non-existent scene handled correctly");

    logger.log(LogLevel::INFO, "Testing multiple undo operations...");
    home.undo_last(); // should undo the movie scene activation
    home.undo_last(); // should do nothing as stack is empty
    logger.log(LogLevel::INFO, "✓ Multiple undos handled without errors");

    logger.log(LogLevel::INFO, "All Smart Home Automation tests passed successfully!");
}

int main() {
    Logger& logger = Logger::getInstance();

    // two examples of this design pattern both seemingly different but implemented
    // the same with the command design patten.
    try {
        run_smart_device_tests(logger);
    } catch (const std::exception& e) {
        logger.log(LogLevel::CRITICAL, "error during document testing: " + std::string(e.what()));
        return 1;
    }
    try {
        run_document_tests(logger);
    } catch (const std::exception& e) {
        logger.log(LogLevel::CRITICAL, "error during smart device testing: " + std::string(e.what()));
        return 1;
    }
    return 0;
}
