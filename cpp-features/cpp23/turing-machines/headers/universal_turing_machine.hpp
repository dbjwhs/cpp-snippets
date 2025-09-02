// MIT License
// Copyright (c) 2025 dbjwhs

#ifndef UNIVERSAL_TURING_MACHINE_HPP
#define UNIVERSAL_TURING_MACHINE_HPP

/*
 * UniversalTuringMachine
 *
 * History: Alan Turing introduced the concept of a Universal Turing Machine (UTM) in his
 * seminal 1936 paper "On Computable Numbers." A UTM is a Turing machine that can simulate any
 * other Turing machine when provided with an appropriate encoding of that machine. This concept
 * was revolutionary because it demonstrated that a single fixed machine could compute anything
 * that is computable, given the right program. The UTM serves as the theoretical foundation for
 * general-purpose computers, demonstrating that hardware and software can be separated. Modern
 * computers are practical implementations of Turing's theoretical concept - they can run any
 * program that can be expressed algorithmically, just as a UTM can simulate any other Turing
 * machine.
 *
 * Common usages:
 * - Demonstrating the universality of computation
 * - Theoretical proofs in computability theory
 * - Educational demonstrations of the foundation of modern computing
 * - Exploring the limits of what is computable
 * - Theoretical basis for programming language design
 */

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <memory>
#include <functional>
#include <fstream>
#include <sstream>
#include "../../../../headers/project_utils.hpp"
#include "comprehensive_turing_machine.hpp"

namespace turing {

/**
 * struct EncodedMachine
 *
 * represents an encoded turing machine for the universal machine to simulate
 */
struct EncodedMachine {
    // unique identifier for this machine
    std::string m_id{};

    // human-readable name
    std::string m_name{};

    // description of what this machine does
    std::string m_description{};

    // state set (state IDs)
    std::vector<std::string> m_states{};

    // input alphabet (symbol IDs)
    std::vector<std::string> m_inputAlphabet{};

    // tape alphabet (symbol IDs)
    std::vector<std::string> m_tapeAlphabet{};

    // initial state ID
    std::string m_initialState{};

    // blank symbol ID
    std::string m_blankSymbol{};

    // accepting states
    std::vector<std::string> m_acceptingStates{};

    // transition function encoded as strings:
    // "currentState,readSymbol,nextState,writeSymbol,moveDirection"
    std::vector<std::string> m_transitions{};

    /**
     * default constructor
     */
    EncodedMachine() = default;

    /**
     * constructor with basic parameters
     *
     * @param id unique identifier
     * @param name human-readable name
     * @param description what this machine does
     */
    EncodedMachine(std::string id, std::string name, std::string description)
        : m_id{std::move(id)}, m_name{std::move(name)}, m_description{std::move(description)} {}
};

/**
 * class UniversalTuringMachine
 *
 * implements a universal turing machine that can simulate any other turing machine
 * uses three tapes: description tape, simulation tape, and working tape
 */
class UniversalTuringMachine {
private:
    // the underlying machine implementation
    ComprehensiveTuringMachine m_machine;

    // collection of encoded machines that can be simulated
    std::map<std::string, EncodedMachine> m_encodedMachines;

    // the currently loaded machine for simulation
    std::string m_currentMachineId{};

    // special tape names
    static const std::string DESCRIPTION_TAPE;
    static const std::string SIMULATION_TAPE;
    static const std::string WORKING_TAPE;

    // special state names for the universal machine
    static const std::string START_STATE;
    static const std::string DECODE_STATE;
    static const std::string SIMULATE_STATE;
    static const std::string MOVE_HEAD_STATE;
    static const std::string HALT_STATE;
    static const std::string ACCEPT_STATE;
    static const std::string REJECT_STATE;

public:
    /**
     * constructor initializes a universal turing machine
     */
    UniversalTuringMachine()
        : m_machine("Universal Turing Machine", "Simulates any other Turing machine", MachineType::MultiTape) {
        initialize();
        LOG_INFO("Created Universal Turing Machine");
    }

    /**
     * initializes the universal machine with necessary states and tapes
     */
    void initialize() {
        try {
            // Try to read the tapes to see if they already exist
            m_machine.getTapeContent(DESCRIPTION_TAPE);
            m_machine.getTapeContent(SIMULATION_TAPE);
            m_machine.getTapeContent(WORKING_TAPE);
            
            // If we made it here, all tapes exist
            LOG_INFO("UTM tapes already initialized");
            return;
        } catch (const std::exception& e) {
            LOG_INFO("Initializing UTM tapes");
            // Create the tapes since they don't exist or had errors
            m_machine.addTape(DESCRIPTION_TAPE);
            m_machine.addTape(SIMULATION_TAPE);
            m_machine.addTape(WORKING_TAPE);
        }

        // add special symbols for UTM operation
        m_machine.addSymbol("#", '#');  // delimiter
        m_machine.addSymbol("@", '@');  // marker

        // add states for the universal machine
        m_machine.addState(State(START_STATE, "Start", "Starting state of the UTM"));
        m_machine.addState(State(DECODE_STATE, "Decode", "Decoding the machine description"));
        m_machine.addState(State(SIMULATE_STATE, "Simulate", "Simulating a step of the encoded machine"));
        m_machine.addState(State(MOVE_HEAD_STATE, "MoveHead", "Moving the head according to transition"));
        m_machine.addState(State(HALT_STATE, "Halt", "Simulation halted", true, false));
        m_machine.addState(State(ACCEPT_STATE, "Accept", "Input accepted by simulated machine", true, true));
        m_machine.addState(State(REJECT_STATE, "Reject", "Input rejected by simulated machine", true, false));

        LOG_INFO("Initialized UTM with required tapes and states");
    }

    /**
     * adds an encoded machine to the UTM's collection
     *
     * @param machine the encoded machine to add
     */
    void addEncodedMachine(const EncodedMachine& machine) {
        m_encodedMachines[machine.m_id] = machine;
        LOG_INFO(std::format("Added encoded machine: {}", machine.m_name));
    }

    /**
     * loads an encoded machine for simulation
     *
     * @param machineId ID of the machine to load
     */
    void loadMachine(const std::string& machineId) {
        if (!m_encodedMachines.contains(machineId)) {
            LOG_ERROR(std::format("Encoded machine not found: {}", machineId));
            throw std::invalid_argument("Encoded machine not found");
        }

        m_currentMachineId = machineId;
        const EncodedMachine& machine = m_encodedMachines[machineId];

        // encode the machine onto the description tape
        std::string encodedDescription = encodeMachine(machine);
        m_machine.setTapeContent(encodedDescription, DESCRIPTION_TAPE);

        // clear the other tapes
        m_machine.setTapeContent("", SIMULATION_TAPE);
        m_machine.setTapeContent("", WORKING_TAPE);

        LOG_INFO(std::format("Loaded encoded machine: {}", machine.m_name));
    }

    /**
     * sets the input for the simulated machine
     *
     * @param input the input string
     */
    void setInput(const std::string& input) {
        if (m_currentMachineId.empty()) {
            LOG_ERROR("No encoded machine loaded");
            throw std::runtime_error("No machine loaded for simulation");
        }

        // validate input against the machine's input alphabet
        const EncodedMachine& machine = m_encodedMachines[m_currentMachineId];
        for (char c : input) {
            std::string symbolId(1, c);
            bool valid = false;
            for (const auto& inputSymbol : machine.m_inputAlphabet) {
                if (symbolId == inputSymbol) {
                    valid = true;
                    break;
                }
            }
            if (!valid) {
                LOG_ERROR(std::format("Invalid input symbol: {}", symbolId));
                throw std::invalid_argument("Input contains symbols not in the machine's input alphabet");
            }
        }

        // set the input on the simulation tape
        m_machine.setTapeContent(input, SIMULATION_TAPE);

        LOG_INFO(std::format("Set input for simulation: {}", input));
    }

    /**
     * runs the universal machine to simulate the encoded machine on the given input
     *
     * @param maxSteps maximum number of simulation steps
     * @return result of the simulation (accept/reject/halt)
     */
    std::string run(size_t maxSteps = 10000) {
        if (m_currentMachineId.empty()) {
            LOG_ERROR("No encoded machine loaded");
            throw std::runtime_error("No machine loaded for simulation");
        }

        try {
            // reset the UTM to start state
            m_machine.reset();
    
            // set the maximum steps
            m_machine.setMaxSteps(maxSteps);
    
            // enable history recording for debugging
            m_machine.enableHistory(true);
    
            // run the UTM
            std::string result = m_machine.run();
    
            LOG_INFO(std::format("Simulation completed with result: {}", result));
            return result;
        } catch (const std::exception& e) {
            LOG_WARNING(std::format("Error during UTM simulation: {}", e.what()));
            return "error";
        }
    }

    /**
     * gets the current state of the simulated machine
     *
     * @return state ID of the simulated machine
     */
    [[nodiscard]] static std::string getSimulatedState() {
        // In a real implementation, this would parse the working tape
        // to extract the current state of the simulated machine
        // For simplicity, we'll return a placeholder value
        return "simulated_state";
    }

    /**
     * gets the content of the simulation tape (the input/output tape)
     *
     * @return string content
     */
    [[nodiscard]] std::string getSimulationTape() const {
        try {
            return m_machine.getTapeContent(SIMULATION_TAPE);
        } catch (const std::exception& e) {
            LOG_WARNING(std::format("Error getting simulation tape: {}", e.what()));
            return ""; // Return empty string on error
        }
    }

    /**
     * sets the machine configuration for step-by-step simulation
     *
     * @param delayMs visualization delay in milliseconds
     */
    void configureSimulation(int delayMs = 500) {
        m_machine.setVisualizationDelay(delayMs);
    }

    /**
     * creates and returns a binary increment machine encoding
     *
     * @return encoded turing machine for binary increment
     */
    static EncodedMachine createBinaryIncrementMachine() {
        EncodedMachine machine("increment", "Binary Incrementer", "Adds 1 to a binary number");

        // states
        machine.m_states = {"start", "scan_right", "increment", "carry", "halt"};
        machine.m_initialState = "start";
        machine.m_acceptingStates = {"halt"};

        // alphabet
        machine.m_inputAlphabet = {"0", "1"};
        machine.m_tapeAlphabet = {"0", "1", "_"};
        machine.m_blankSymbol = "_";

        // transitions
        machine.m_transitions = {
            "start,0,scan_right,0,R",
            "start,1,scan_right,1,R",
            "start,_,increment,_,L",
            "scan_right,0,scan_right,0,R",
            "scan_right,1,scan_right,1,R",
            "scan_right,_,increment,_,L",
            "increment,0,halt,1,N",
            "increment,1,carry,0,L",
            "increment,_,halt,1,N",
            "carry,0,halt,1,N",
            "carry,1,carry,0,L",
            "carry,_,halt,1,N"
        };

        return machine;
    }

    /**
     * creates and returns a palindrome checker machine encoding
     *
     * @return encoded turing machine for checking palindromes
     */
    static EncodedMachine createPalindromeCheckerMachine() {
        EncodedMachine machine("palindrome", "Palindrome Checker", "Checks if input is a palindrome");

        // states
        machine.m_states = {"start", "scan_right", "mark_right", "scan_left", "compare", "accept", "reject"};
        machine.m_initialState = "start";
        machine.m_acceptingStates = {"accept"};

        // alphabet
        machine.m_inputAlphabet = {"0", "1"};
        machine.m_tapeAlphabet = {"0", "1", "X", "Y", "_"};  // X, Y for marking
        machine.m_blankSymbol = "_";

        // transitions
        machine.m_transitions = {
            "start,0,scan_right,X,R",
            "start,1,scan_right,Y,R",
            "start,_,accept,_,N",
            "scan_right,0,scan_right,0,R",
            "scan_right,1,scan_right,1,R",
            "scan_right,_,mark_right,_,L",
            "mark_right,0,scan_left,0,L",
            "mark_right,1,scan_left,1,L",
            "scan_left,0,scan_left,0,L",
            "scan_left,1,scan_left,1,L",
            "scan_left,X,compare,X,R",
            "scan_left,Y,compare,Y,R",
            "compare,0,reject,0,N",  // mismatch
            "compare,1,reject,1,N",  // mismatch
            "compare,X,scan_right,X,R",  // match for 0
            "compare,Y,scan_right,Y,R",   // match for 1
            "compare,_,accept,_,N"   // reached end with all matches
        };

        return machine;
    }

    /**
     * creates a binary divisibility-by-3 checker machine encoding
     *
     * @return encoded turing machine for checking divisibility by 3
     */
    static EncodedMachine createDivisibilityByThreeMachine() {
        EncodedMachine machine("div3", "Divisibility by 3 Checker",
                              "Checks if a binary number is divisible by 3");

        // states based on remainder when divided by 3
        machine.m_states = {"rem0", "rem1", "rem2", "accept", "reject"};
        machine.m_initialState = "rem0";
        machine.m_acceptingStates = {"accept"};

        // alphabet
        machine.m_inputAlphabet = {"0", "1"};
        machine.m_tapeAlphabet = {"0", "1", "_"};
        machine.m_blankSymbol = "_";

        // transitions - using the fact that:
        // If n is divisible by 3, then 2n is divisible by 3, and 2n+1 gives remainder 1
        // If n gives remainder 1 when divided by 3, then 2n gives remainder 2, and 2n+1 gives remainder 0
        // If n gives remainder 2 when divided by 3, then 2n gives remainder 1, and 2n+1 gives remainder 2
        machine.m_transitions = {
            // rem0 = number seen so far is divisible by 3
            "rem0,0,rem0,0,R",     // 2n+0 (still divisible by 3)
            "rem0,1,rem1,1,R",     // 2n+1 (now has remainder 1)
            "rem0,_,accept,_,N",   // end of input, divisible by 3

            // rem1 = number seen so far has remainder 1 when divided by 3
            "rem1,0,rem2,0,R",     // 2n+0 (now has remainder 2)
            "rem1,1,rem0,1,R",     // 2n+1 (now divisible by 3)
            "rem1,_,reject,_,N",   // end of input, not divisible by 3

            // rem2 = number seen so far has remainder 2 when divided by 3
            "rem2,0,rem1,0,R",     // 2n+0 (now has remainder 1)
            "rem2,1,rem2,1,R",     // 2n+1 (still has remainder 2)
            "rem2,_,reject,_,N"    // end of input, not divisible by 3
        };

        return machine;
    }

    /**
     * performs a step-by-step simulation of an encoded machine
     *
     * @param verbose whether to output detailed steps
     * @param maxSteps maximum number of steps
     * @return result of simulation (accept/reject/halt)
     */
    std::string simulateStepByStep(bool verbose = false, size_t maxSteps = 1000) {
        if (m_currentMachineId.empty()) {
            LOG_ERROR("No encoded machine loaded");
            throw std::runtime_error("No machine loaded for simulation");
        }

        try {
            const EncodedMachine& machine = m_encodedMachines[m_currentMachineId];
    
            // manual simulation setup
            std::string input;
            try {
                input = getSimulationTape();
            } catch (const std::exception& e) {
                LOG_WARNING(std::format("Error getting simulation tape: {}", e.what()));
                input = ""; // Default to empty if we can't get the tape
            }
            
            std::string currentState = machine.m_initialState;
            int headPosition = 0;
            size_t steps = 0;

        // create a tape representation with the input
        std::vector<std::string> tape;
        for (char c : input) {
            tape.emplace_back(1, c);
        }

        if (verbose) {
            LOG_INFO(std::format("Starting simulation of {} with input: {}",
                    machine.m_name, input));
            LOG_INFO(std::format("Initial state: {}", currentState));
        }

        // simulation loop
        while (steps < maxSteps) {
            // check if current state is accepting
            for (const auto& acceptingState : machine.m_acceptingStates) {
                if (currentState == acceptingState) {
                    if (verbose) {
                        LOG_INFO(std::format("Reached accepting state: {} after {} steps",
                                currentState, steps));
                    }
                    return "accept";
                }
            }

            // ensure head position is valid
            while (headPosition >= static_cast<int>(tape.size())) {
                tape.push_back(machine.m_blankSymbol);
            }

            // read current symbol
            std::string currentSymbol = (headPosition >= 0 && headPosition < static_cast<int>(tape.size()))
                                      ? tape[headPosition]
                                      : machine.m_blankSymbol;

            // find matching transition
            bool foundTransition = false;
            for (const auto& transitionStr : machine.m_transitions) {
                std::vector<std::string> parts = utils::split(transitionStr, ',');
                if (parts.size() != 5) continue;

                if (parts[0] == currentState && parts[1] == currentSymbol) {
                    // found matching transition
                    std::string nextState = parts[2];
                    std::string writeSymbol = parts[3];
                    std::string moveDirection = parts[4];

                    if (verbose) {
                        LOG_INFO(std::format("Step {}: State={}, Read={}, Write={}, Move={}, NextState={}",
                                steps, currentState, currentSymbol, writeSymbol, moveDirection, nextState));
                    }

                    // apply transition
                    tape[headPosition] = writeSymbol;

                    if (moveDirection == "L") {
                        headPosition--;
                        if (headPosition < 0) {
                            // expand tape to the left
                            tape.insert(tape.begin(), machine.m_blankSymbol);
                            headPosition = 0;
                        }
                    } else if (moveDirection == "R") {
                        headPosition++;
                    }
                    // else N - don't move

                    currentState = nextState;
                    foundTransition = true;
                    break;
                }
            }

            if (!foundTransition) {
                if (verbose) {
                    LOG_WARNING(std::format("No transition found for state={}, symbol={}. Halting.",
                             currentState, currentSymbol));
                }
                return "halt";
            }

            steps++;
        }

        if (verbose) {
            LOG_WARNING(std::format("Reached maximum steps ({}). Possible infinite loop.", maxSteps));
        }
        return "timeout";
        } catch (const std::exception& e) {
            LOG_WARNING(std::format("Error during step-by-step simulation: {}", e.what()));
            return "error";
        }
    }

    /**
     * decodes an encoded machine description
     *
     * @param encodedDescription the encoded machine description
     * @return decoded EncodedMachine object
     */
    static EncodedMachine decodeMachine(const std::string& encodedDescription) {
        EncodedMachine machine("decoded", "Decoded Machine", "Machine decoded from description");

        // parse the encoding
        size_t pos = 0;

        while (pos < encodedDescription.size()) {
            // find section marker
            if (encodedDescription[pos] == '#') {
                pos++;
                size_t sectionEnd = encodedDescription.find(':', pos);
                if (sectionEnd == std::string::npos) break;

                std::string currentSection = encodedDescription.substr(pos, sectionEnd - pos);
                pos = sectionEnd + 1;

                // find section content end
                size_t contentEnd = encodedDescription.find('#', pos);
                if (contentEnd == std::string::npos) break;

                std::string content = encodedDescription.substr(pos, contentEnd - pos);
                pos = contentEnd + 1;

                // parse section content
                if (currentSection == "states") {
                    machine.m_states = utils::split(content, ',');
                    // remove empty elements
                    std::erase_if(machine.m_states,
                                  [](const std::string& s) { return s.empty(); });
                }
                else if (currentSection == "input_alphabet") {
                    machine.m_inputAlphabet = utils::split(content, ',');
                    std::erase_if(machine.m_inputAlphabet,
                                  [](const std::string& s) { return s.empty(); });
                }
                else if (currentSection == "tape_alphabet") {
                    machine.m_tapeAlphabet = utils::split(content, ',');
                    std::erase_if(machine.m_tapeAlphabet,
                                  [](const std::string& s) { return s.empty(); });
                }
                else if (currentSection == "initial_state") {
                    machine.m_initialState = content;
                }
                else if (currentSection == "blank_symbol") {
                    machine.m_blankSymbol = content;
                }
                else if (currentSection == "accepting_states") {
                    machine.m_acceptingStates = utils::split(content, ',');
                    std::erase_if(machine.m_acceptingStates,
                                  [](const std::string& s) { return s.empty(); });
                }
                else if (currentSection == "transitions") {
                    machine.m_transitions = utils::split(content, ';');
                    std::erase_if(machine.m_transitions,
                                  [](const std::string& s) { return s.empty(); });
                }
            }
            else {
                pos++;
            }
        }

        return machine;
    }

    /**
     * creates a machine that recognizes the language a^n b^n
     *
     * @return encoded turing machine for a^n b^n recognition
     */
    static EncodedMachine createAnBnMachine() {
        EncodedMachine machine("anbn", "a^n b^n Recognizer", "Recognizes strings of form a^n b^n");

        // states
        machine.m_states = {"start", "count_a", "check_b", "verify", "cleanup", "accept", "reject"};
        machine.m_initialState = "start";
        machine.m_acceptingStates = {"accept"};

        // alphabet
        machine.m_inputAlphabet = {"a", "b"};
        machine.m_tapeAlphabet = {"a", "b", "X", "Y", "_"};
        machine.m_blankSymbol = "_";

        // transitions
        machine.m_transitions = {
            // initialize and start checking
            "start,a,count_a,X,R",
            "start,b,reject,b,N",
            "start,_,accept,_,N",  // empty string is valid

            // count a's by marking them with X
            "count_a,a,count_a,a,R",
            "count_a,b,check_b,b,L",
            "count_a,_,reject,_,N",  // no b's found after a's

            // move back to find an unmarked 'a'
            "check_b,a,check_b,a,L",
            "check_b,X,verify,X,R",

            // verify an equal number of b's by marking one b for each a
            "verify,a,verify,a,R",
            "verify,b,count_a,Y,R",
            "verify,Y,verify,Y,R",
            "verify,_,cleanup,_,L",

            // clean up and check if all a's and b's are matched
            "cleanup,Y,cleanup,Y,L",
            "cleanup,X,cleanup,X,L",
            "cleanup,a,reject,a,N",  // unmatched a found
            "cleanup,b,reject,b,N",  // unmatched b found
            "cleanup,_,accept,_,N"   // all matched
        };

        return machine;
    }

private:
    /**
     * encodes a machine description for the UTM
     *
     * @param machine the machine to encode
     * @return encoded string representation
     */
    static std::string encodeMachine(const EncodedMachine& machine) {
        std::stringstream ss;

        // encode the machine components with delimiters
        // states
        ss << "#states:";
        for (const auto& state : machine.m_states) {
            ss << state << ",";
        }
        ss << "#";

        // input alphabet
        ss << "#input_alphabet:";
        for (const auto& symbol : machine.m_inputAlphabet) {
            ss << symbol << ",";
        }
        ss << "#";

        // tape alphabet
        ss << "#tape_alphabet:";
        for (const auto& symbol : machine.m_tapeAlphabet) {
            ss << symbol << ",";
        }
        ss << "#";

        // initial state
        ss << "#initial_state:" << machine.m_initialState << "#";

        // blank symbol
        ss << "#blank_symbol:" << machine.m_blankSymbol << "#";

        // accepting states
        ss << "#accepting_states:";
        for (const auto& state : machine.m_acceptingStates) {
            ss << state << ",";
        }
        ss << "#";

        // transitions
        ss << "#transitions:";
        for (const auto& transition : machine.m_transitions) {
            ss << transition << ";";
        }
        ss << "#";

        return ss.str();
    }

    /**
     * performs detailed parsing of a machine transition
     *
     * @param transitionStr the transition string in format "currentState,readSymbol,nextState,writeSymbol,moveDirection"
     * @return pair of (success, transition details)
     */
    static std::pair<bool, std::tuple<std::string, std::string, std::string, std::string, char>>
    parseTransition(const std::string& transitionStr) {
        std::vector<std::string> parts = utils::split(transitionStr, ',');
        if (parts.size() != 5) {
            return {false, {}};
        }

        std::string currentState = parts[0];
        std::string readSymbol = parts[1];
        std::string nextState = parts[2];
        std::string writeSymbol = parts[3];

        char moveDirection = 'N';  // default is no movement
        if (parts[4] == "L") {
            moveDirection = 'L';
        } else if (parts[4] == "R") {
            moveDirection = 'R';
        }

        return {true, {currentState, readSymbol, nextState, writeSymbol, moveDirection}};
    }
};

// initialize static constants
const std::string UniversalTuringMachine::DESCRIPTION_TAPE = "description";
const std::string UniversalTuringMachine::SIMULATION_TAPE = "simulation";
const std::string UniversalTuringMachine::WORKING_TAPE = "working";
const std::string UniversalTuringMachine::START_STATE = "utm_start";
const std::string UniversalTuringMachine::DECODE_STATE = "utm_decode";
const std::string UniversalTuringMachine::SIMULATE_STATE = "utm_simulate";
const std::string UniversalTuringMachine::MOVE_HEAD_STATE = "utm_move_head";
const std::string UniversalTuringMachine::HALT_STATE = "utm_halt";
const std::string UniversalTuringMachine::ACCEPT_STATE = "utm_accept";
const std::string UniversalTuringMachine::REJECT_STATE = "utm_reject";

} // namespace turing

#endif // UNIVERSAL_TURING_MACHINE_HPP
