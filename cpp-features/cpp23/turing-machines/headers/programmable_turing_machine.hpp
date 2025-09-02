// MIT License
// Copyright (c) 2025 dbjwhs

#ifndef PROGRAMMABLE_TURING_MACHINE_HPP
#define PROGRAMMABLE_TURING_MACHINE_HPP

/*
 * ProgrammableTuringMachine
 *
 * History: The concept of a programmable Turing machine emerged naturally from Alan Turing's
 * 1936 paper, where he described a "universal Turing machine" (UTM) capable of simulating any
 * other Turing machine given its description. This revolutionary idea established that a single
 * machine could, in principle, compute anything that is algorithmically computable, forming the
 * theoretical foundation for modern computers. John von Neumann's later work in the 1940s
 * on stored-program computers drew heavily from Turing's UTM concept, implementing the idea that
 * code and data could be stored in the same memory, allowing machines to be "programmed" rather
 * than physically reconfigured for each new task.
 *
 * Common usages:
 * - Demonstrating the concept of universal computation
 * - Teaching fundamental programming constructs through state transitions
 * - Simulating higher-level programming languages in a Turing-complete framework
 * - Exploring the limits of computability and algorithmic efficiency
 */

#include <string>
#include <utility>
#include <vector>
#include <unordered_map>
#include <memory>
#include <fstream>
#include <sstream>
#include <functional>
#include "../../../../headers/project_utils.hpp"
#include "extended_turing_machine.hpp"

namespace turing {

/**
 * class Program
 *
 * represents a stored program for the programmable turing machine
 * contains a sequence of instructions and transition rules
 */
class Program {
public:
    // type definitions for command functions
    using CommandFunc = std::function<void(ExtendedTuringMachine&)>;

    // named command with associated function
    struct Command {
        // name of the command
        std::string m_name{};

        // description of what the command does
        std::string m_description{};

        // function to execute for this command
        CommandFunc m_function{};

        // default constructor required for use in containers
        Command() = default;

        // constructor for initializing all fields
        Command(std::string name, std::string description, CommandFunc function)
            : m_name{std::move(name)}, m_description{std::move(description)}, m_function{std::move(function)} {}
    };

    // instruction in the program
    struct Instruction {
        // command to execute
        std::string m_commandName{};

        // parameters for the command (if any)
        std::vector<std::string> m_params{};

        // default constructor required for use in containers
        Instruction() = default;

        // constructor for initializing all fields
        explicit Instruction(std::string commandName, std::vector<std::string> params = {})
            : m_commandName{std::move(commandName)}, m_params{std::move(params)} {}
    };

private:
    // name of the program
    std::string m_name{};

    // description of what the program does
    std::string m_description{};

    // sequence of instructions in the program
    std::vector<Instruction> m_instructions{};

    // available commands (name -> Command)
    std::unordered_map<std::string, Command> m_commandLibrary{};

    // program counter (which instruction is next)
    int m_programCounter{0};

public:
    /**
     * constructor to initialize a program with a name and description
     *
     * @param name the name of the program
     * @param description what the program does
     */
    Program(const std::string& name, std::string  description)
        : m_name{name}, m_description{std::move(description)} {
        LOG_INFO(std::format("Created program: {}", name));
    }

    /**
     * gets the name of the program
     *
     * @return string name
     */
    [[nodiscard]] std::string getName() const {
        return m_name;
    }

    /**
     * gets the description of the program
     *
     * @return string description
     */
    [[nodiscard]] std::string getDescription() const {
        return m_description;
    }

    /**
     * adds a command to the program's command library
     *
     * @param name command name
     * @param description what the command does
     * @param function the function to execute for this command
     */
    void addCommand(const std::string& name, const std::string& description, const CommandFunc &function) {
        m_commandLibrary[name] = Command(name, description, function);
        LOG_INFO(std::format("Added command to program: {}", name));
    }

    /**
     * adds an instruction to the program
     *
     * @param commandName which command to execute
     * @param params parameters for the command
     */
    void addInstruction(const std::string& commandName, const std::vector<std::string>& params = {}) {
        // check that the command exists
        if (!m_commandLibrary.contains(commandName)) {
            LOG_ERROR(std::format("Unknown command in instruction: {}", commandName));
            throw std::invalid_argument("Unknown command name");
        }

        m_instructions.emplace_back(commandName, params);
        LOG_INFO(std::format("Added instruction: {}", commandName));
    }

    /**
     * loads a program from a file
     *
     * @param filename path to the program file
     * @return true if the program was loaded successfully
     */
    bool loadFromFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            LOG_ERROR(std::format("Failed to open program file: {}", filename));
            return false;
        }

        // clear any existing instructions
        m_instructions.clear();

        std::string line;
        while (std::getline(file, line)) {
            // skip empty lines and comments
            if (line.empty() || line[0] == '#') {
                continue;
            }

            std::istringstream iss(line);
            std::string commandName;
            iss >> commandName;

            std::vector<std::string> params;
            std::string param;
            while (iss >> param) {
                params.push_back(param);
            }

            try {
                addInstruction(commandName, params);
            } catch (const std::exception& e) {
                LOG_ERROR(std::format("Error adding instruction from file: {}", e.what()));
                return false;
            }
        }

        LOG_INFO(std::format("Loaded program from file: {} with {} instructions",
                filename, m_instructions.size()));
        return true;
    }

    /**
     * resets the program counter to the beginning
     */
    void reset() {
        m_programCounter = 0;
        LOG_INFO("Program counter reset");
    }

    /**
     * executes the next instruction in the program
     *
     * @param machine the turing machine to execute on
     * @return true if an instruction was executed, false if program complete
     */
    bool executeNextInstruction(ExtendedTuringMachine& machine) {
        if (m_programCounter >= static_cast<int>(m_instructions.size())) {
            LOG_INFO("Program complete, no more instructions");
            return false;
        }

        const Instruction& instruction = m_instructions[m_programCounter];
        const Command& command = m_commandLibrary.at(instruction.m_commandName);

        LOG_INFO(std::format("Executing instruction {}: {}",
                m_programCounter, instruction.m_commandName));

        // execute the command
        command.m_function(machine);

        // move to the next instruction
        m_programCounter++;
        return true;
    }

    /**
     * executes the entire program from the current point
     *
     * @param machine the turing machine to execute on
     * @param maxInstructions safety limit on number of instructions to execute
     * @return true if program completed normally, false if hit max instruction limit
     */
    bool execute(ExtendedTuringMachine& machine, int maxInstructions = 1000) {
        LOG_INFO("Starting program execution");

        int instructionCount = 0;
        while (executeNextInstruction(machine) && instructionCount < maxInstructions) {
            instructionCount++;
        }

        if (instructionCount >= maxInstructions) {
            LOG_WARNING(std::format("Program execution reached limit of {} instructions", maxInstructions));
            return false;
        }

        LOG_INFO(std::format("Program execution complete after {} instructions", instructionCount));
        return true;
    }

    /**
     * gets the number of instructions in the program
     *
     * @return integer count of instructions
     */
    [[nodiscard]] int getInstructionCount() const {
        return static_cast<int>(m_instructions.size());
    }

    /**
     * gets the current program counter value
     *
     * @return integer program counter
     */
    [[nodiscard]] int getProgramCounter() const {
        return m_programCounter;
    }
};

/**
 * class ProgrammableTuringMachine
 *
 * extends the turing machine concept to allow it to run stored programs
 * implementing common programming constructs like loops and conditionals
 */
class ProgrammableTuringMachine {
private:
    // the underlying turing machine
    ExtendedTuringMachine m_machine;

    // the currently loaded program
    std::shared_ptr<Program> m_currentProgram{nullptr};

    // standard library of available commands
    std::unordered_map<std::string, Program::CommandFunc> m_standardLibrary{};

public:
    /**
     * constructor to initialize with a specific number of tracks
     *
     * @param numTracks number of tape tracks (default 1)
     */
    explicit ProgrammableTuringMachine(int numTracks = 1) : m_machine(numTracks) {
        initializeStandardLibrary();
        LOG_INFO(std::format("ProgrammableTuringMachine created with {} tracks", numTracks));
    }

    /**
     * creates a new program
     *
     * @param name the name of the program
     * @param description what the program does
     * @return shared pointer to the new Program object
     */
    std::shared_ptr<Program> createProgram(const std::string& name, const std::string& description) {
        auto program = std::make_shared<Program>(name, description);

        // add standard library commands to the program
        for (const auto& [cmdName, cmdFunc] : m_standardLibrary) {
            program->addCommand(cmdName, "Standard library command", cmdFunc);
        }

        LOG_INFO(std::format("Created new program: {}", name));
        return program;
    }

    /**
     * loads a program as the current program
     *
     * @param program the program to load
     */
    void loadProgram(const std::shared_ptr<Program>& program) {
        m_currentProgram = program;
        LOG_INFO(std::format("Loaded program: {}", program->getName()));
    }

    /**
     * executes the current program
     *
     * @param maxInstructions safety limit on instruction count
     * @return true if program completed normally
     */
    bool executeProgram(int maxInstructions = 1000) {
        if (!m_currentProgram) {
            LOG_ERROR("No program loaded");
            throw std::runtime_error("No program loaded");
        }

        LOG_INFO(std::format("Executing program: {}", m_currentProgram->getName()));
        return m_currentProgram->execute(m_machine, maxInstructions);
    }

    /**
     * gets the current state of the machine
     *
     * @return reference to the underlying turing machine
     */
    ExtendedTuringMachine& getMachine() {
        return m_machine;
    }

    /**
     * gets the current program
     *
     * @return shared pointer to the current program (nullptr if none)
     */
    [[nodiscard]] std::shared_ptr<Program> getCurrentProgram() const {
        return m_currentProgram;
    }

    /**
     * sets the content of the tape
     *
     * @param tapeContent string to set as tape content
     * @param trackIndex which track to set (default 0)
     */
    void setTape(const std::string& tapeContent, int trackIndex = 0) {
        m_machine.setTape(tapeContent, trackIndex);
    }

    /**
     * gets the current content of the tape
     *
     * @param trackIndex which track to read (default 0)
     * @return string representing tape content
     */
    [[nodiscard]] std::string getTape(int trackIndex = 0) const {
        return m_machine.getTape(trackIndex);
    }

private:
    /**
     * initializes the standard library of commands
     */
    void initializeStandardLibrary() {
        // Move right
        m_standardLibrary["MOVE_RIGHT"] = [](ExtendedTuringMachine& machine) {
            int headPos = machine.getHeadPosition();
            // This is a simplified implementation - in a real scenario, we would
            // use proper state transitions
            machine.addTransition(machine.getCurrentState(), machine.getTape()[headPos],
                               machine.getCurrentState(), machine.getTape()[headPos], 'R');
            machine.step();
        };

        // Move left
        m_standardLibrary["MOVE_LEFT"] = [](ExtendedTuringMachine& machine) {
            int headPos = machine.getHeadPosition();
            machine.addTransition(machine.getCurrentState(), machine.getTape()[headPos],
                               machine.getCurrentState(), machine.getTape()[headPos], 'L');
            machine.step();
        };

        // Write symbol
        m_standardLibrary["WRITE"] = [](ExtendedTuringMachine& machine) {
            // In a real implementation, this would take the symbol as a parameter
            int headPos = machine.getHeadPosition();
            machine.addTransition(machine.getCurrentState(), machine.getTape()[headPos],
                               machine.getCurrentState(), '1', 'N');
            machine.step();
        };

        // Read symbol
        m_standardLibrary["READ"] = [](const ExtendedTuringMachine& machine) {
            // This just reads the current symbol (doesn't change anything)
            const int headPos = machine.getHeadPosition();
            char symbol = machine.getTape()[headPos];
            LOG_INFO(std::format("Read symbol: {}", symbol));
        };

        LOG_INFO("Standard library initialized");
    }
};

} // namespace turing

#endif // PROGRAMMABLE_TURING_MACHINE_HPP
