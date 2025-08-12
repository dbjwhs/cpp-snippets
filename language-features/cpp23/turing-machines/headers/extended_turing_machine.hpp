// MIT License
// Copyright (c) 2025 dbjwhs

#ifndef EXTENDED_TURING_MACHINE_HPP
#define EXTENDED_TURING_MACHINE_HPP

/*
 * ExtendedTuringMachine
 *
 * History: Building on the basic Turing machine concept introduced by Alan Turing in 1936,
 * this implementation extends functionality with a multi-character alphabet, more sophisticated
 * state transitions, and support for multi-track tapes. This enhanced model still maintains
 * the theoretical properties of the original Turing machine while providing more practical
 * capabilities for demonstration and educational purposes.
 *
 * Common usages:
 * - Simulating more complex algorithms
 * - Pattern recognition and transformation
 * - String processing demonstrations
 * - Formal language recognition
 */

#include <string>
#include <utility>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <stdexcept>
#include "../../../../headers/project_utils.hpp"

namespace turing {

/**
 * class ExtendedTransition
 *
 * represents a more sophisticated state transition in the extended turing machine
 * includes multiple symbols for multi-track operations
 */
class ExtendedTransition {
public:
    // the next state to transition to
    std::string m_nextState{};

    // the symbols to write to the current tape cell (for multi-track tapes)
    std::vector<char> m_writeSymbols{};

    // direction to move the head after writing (L = left, R = right, N = none)
    char m_moveDirection{};

    /**
     * default constructor required for use in containers
     */
    ExtendedTransition() = default;

    /**
     * constructor for single-track operation
     *
     * @param nextState state to transition to
     * @param writeSymbol symbol to write
     * @param moveDirection direction to move head
     */
    ExtendedTransition(std::string nextState, char writeSymbol, const char moveDirection)
        : m_nextState{std::move(nextState)}, m_writeSymbols{writeSymbol}, m_moveDirection{moveDirection} {}

    /**
     * constructor for multi-track operation
     *
     * @param nextState state to transition to
     * @param writeSymbols vector of symbols to write (one per track)
     * @param moveDirection direction to move head
     */
    ExtendedTransition(std::string nextState, std::vector<char> writeSymbols, const char moveDirection)
        : m_nextState{std::move(nextState)}, m_writeSymbols{std::move(writeSymbols)}, m_moveDirection{moveDirection} {}
};

/**
 * class ExtendedTuringMachine
 *
 * implements a more feature-rich turing machine with support for:
 * - multiple tracks on the tape
 * - larger alphabet
 * - conditional transitions
 * - execution statistics
 */
class ExtendedTuringMachine {
private:
    // number of tracks on the tape
    int m_numTracks{1};

    // the tape represented as a 2D vector [track][position]
    std::vector<std::vector<char>> m_tape{};

    // current position of the head on the tape
    int m_headPosition{0};

    // current state of the machine
    std::string m_currentState{"start"};

    // the transition function: maps (state, symbol combination) to a Transition
    std::unordered_map<std::string, std::unordered_map<std::string, ExtendedTransition>> m_transitions{};

    // set of halting states where the machine stops execution
    std::unordered_set<std::string> m_haltingStates{"halt"};

    // the blank symbol used for uninitialized tape cells
    char m_blankSymbol{'_'};

    // the alphabet of allowed symbols
    std::unordered_set<char> m_alphabet{'0', '1', '_'};

    // statistics
    int m_stepCount{0};
    int m_stateChanges{0};

public:
    /**
     * default constructor with configurable number of tracks
     *
     * @param numTracks number of parallel tracks on the tape
     */
    explicit ExtendedTuringMachine(int numTracks = 1) : m_numTracks{numTracks} {
        // initialize with empty tapes
        m_tape.resize(m_numTracks, std::vector<char>(1, m_blankSymbol));
        m_headPosition = 0;
        
        // Make sure the blank symbol is in the alphabet
        ensureBlankSymbol();

        LOG_INFO(std::format("ExtendedTuringMachine created with {} tracks", numTracks));
    }

    /**
     * constructor with initial tape content
     *
     * @param initialTape the initial content for the machine's tape
     * @param numTracks number of parallel tracks
     */
    explicit ExtendedTuringMachine(const std::string& initialTape, int numTracks = 1) : m_numTracks{numTracks} {
        m_tape.resize(m_numTracks, std::vector<char>(1, m_blankSymbol));
        
        // Make sure the blank symbol is in the alphabet
        ensureBlankSymbol();
        
        // Add all characters in the initial tape to the alphabet
        for (char c : initialTape) {
            addToAlphabet(c);
        }
        
        setTape(initialTape, 0);  // set the first track

        LOG_INFO(std::format("ExtendedTuringMachine created with {} tracks, first track: {}",
                numTracks, initialTape));
    }

    /**
     * adds a valid symbol to the machine's alphabet
     *
     * @param symbol the character to add to the alphabet
     * @return true if the symbol was added, false if it was already in the alphabet
     */
    bool addToAlphabet(char symbol) {
        auto [_, inserted] = m_alphabet.insert(symbol);
        if (inserted) {
            LOG_INFO(std::format("Added symbol '{}' to alphabet", symbol));
        }
        return inserted;
    }
    
    /**
     * ensures that the blank symbol '_' is in the alphabet
     */
    void ensureBlankSymbol() {
        m_alphabet.insert('_');
    }

    /**
     * adds multiple symbols to the alphabet
     *
     * @param symbols string containing all symbols to add
     * @return number of new symbols added
     */
    int addToAlphabet(const std::string& symbols) {
        int count = 0;
        for (char symbol : symbols) {
            if (addToAlphabet(symbol)) {
                count++;
            }
        }
        LOG_INFO(std::format("Added {} symbols to alphabet", count));
        return count;
    }

    /**
     * adds a transition rule for a single-track machine
     *
     * @param currentState the state when this rule applies
     * @param readSymbol the symbol that must be under the head for this rule to apply
     * @param nextState the next state to transition to
     * @param writeSymbol the symbol to write at the current position
     * @param moveDirection which way to move the head ('L', 'R', or 'N')
     */
    void addTransition(const std::string& currentState, char readSymbol,
                       const std::string& nextState, char writeSymbol, char moveDirection) {
        // ensure a blank symbol is in the alphabet
        ensureBlankSymbol();
        
        // add symbols to the alphabet if they're not already there
        if (!m_alphabet.contains(readSymbol)) {
            LOG_INFO(std::format("Adding read symbol '{}' to alphabet", readSymbol));
            m_alphabet.insert(readSymbol);
        }
        
        if (!m_alphabet.contains(writeSymbol)) {
            LOG_INFO(std::format("Adding write symbol '{}' to alphabet", writeSymbol));
            m_alphabet.insert(writeSymbol);
        }

        // create the transition
        const std::string readKey(1, readSymbol);
        m_transitions[currentState][readKey] = ExtendedTransition(nextState, writeSymbol, moveDirection);

        LOG_INFO(std::format("Added transition: State={}, Read={}, NextState={}, Write={}, Move={}",
                currentState, readSymbol, nextState, writeSymbol, moveDirection));
    }

    /**
     * adds a transition rule for a multi-track machine
     *
     * @param currentState the state when this rule applies
     * @param readSymbols the symbols from each track that must be under the head
     * @param nextState the next state to transition to
     * @param writeSymbols the symbols to write to each track
     * @param moveDirection which way to move the head ('L', 'R', or 'N')
     */
    void addTransition(const std::string& currentState, const std::vector<char>& readSymbols,
                       const std::string& nextState, const std::vector<char>& writeSymbols,
                       char moveDirection) {
        // validate track counts
        if (readSymbols.size() != static_cast<size_t>(m_numTracks) ||
            writeSymbols.size() != static_cast<size_t>(m_numTracks)) {
            LOG_ERROR("Symbol count mismatch for multi-track transition");
            throw std::invalid_argument("Symbol count must match number of tracks");
        }

        // ensure a blank symbol is in the alphabet
        ensureBlankSymbol();
        
        // add all symbols to the alphabet if they're not already there
        for (char symbol : readSymbols) {
            if (!m_alphabet.contains(symbol)) {
                LOG_INFO(std::format("Adding read symbol '{}' to alphabet", symbol));
                m_alphabet.insert(symbol);
            }
        }

        for (char symbol : writeSymbols) {
            if (!m_alphabet.contains(symbol)) {
                LOG_INFO(std::format("Adding write symbol '{}' to alphabet", symbol));
                m_alphabet.insert(symbol);
            }
        }

        // create a key string from the read symbols
        std::string readKey(readSymbols.begin(), readSymbols.end());

        // create the transition
        m_transitions[currentState][readKey] = ExtendedTransition(nextState, writeSymbols, moveDirection);

        LOG_INFO(std::format("Added multi-track transition: State={}, NextState={}, Move={}",
                currentState, nextState, moveDirection));
    }

    /**
     * sets the content of a specific tape track
     *
     * @param tapeContent string representing the tape's content
     * @param trackIndex which track to modify (0-based)
     */
    void setTape(const std::string& tapeContent, int trackIndex = 0) {
        // validate track index
        if (trackIndex < 0 || trackIndex >= m_numTracks) {
            LOG_ERROR(std::format("Invalid track index: {}", trackIndex));
            throw std::out_of_range("Track index out of range");
        }

        // ensure all symbols are in the alphabet by adding them
        ensureBlankSymbol();
        for (char c : tapeContent) {
            if (!m_alphabet.contains(c)) {
                LOG_INFO(std::format("Adding symbol '{}' to alphabet", c));
                m_alphabet.insert(c);
            }
        }

        // clear the track and add the new content
        m_tape[trackIndex].clear();
        for (char c : tapeContent) {
            m_tape[trackIndex].push_back(c);
        }

        // ensure the tape has at least one cell
        if (m_tape[trackIndex].empty()) {
            m_tape[trackIndex].push_back(m_blankSymbol);
        }

        // reset head position to start of tape and machine state
        m_headPosition = 0;
        m_currentState = "start";
        m_stepCount = 0;
        m_stateChanges = 0;

        LOG_INFO(std::format("Set track {} tape to: {}", trackIndex, tapeContent));
    }

    /**
     * adds a halting state
     *
     * @param state the state name to add as a halting state
     */
    void addHaltingState(const std::string& state) {
        m_haltingStates.insert(state);
        LOG_INFO(std::format("Added halting state: {}", state));
    }

    /**
     * performs a single step of machine execution
     *
     * @return true if the machine can continue, false if it's halted
     */
    bool step() {
        // check if we've reached a halting state
        if (m_haltingStates.contains(m_currentState)) {
            LOG_INFO(std::format("Machine halted in state: {}", m_currentState));
            return false;
        }

        // ensure the head position has a valid cell
        ensureValidHeadPosition();

        // collect current symbols under the head from all tracks
        std::string currentSymbols;
        for (int trackNdx = 0; trackNdx < m_numTracks; trackNdx++) {
            currentSymbols += m_tape[trackNdx][m_headPosition];
        }

        // check if there's a transition for the current state and symbols
        if (m_transitions.contains(m_currentState) &&
            m_transitions[m_currentState].contains(currentSymbols)) {

            // get the transition details
            const ExtendedTransition& transition = m_transitions[m_currentState][currentSymbols];

            // apply the transition - write symbols to all tracks
            for (int trackNdx = 0; trackNdx < m_numTracks; trackNdx++) {
                m_tape[trackNdx][m_headPosition] = transition.m_writeSymbols[trackNdx];
            }

            // move the head
            if (transition.m_moveDirection == 'L') {
                m_headPosition--;
            } else if (transition.m_moveDirection == 'R') {
                m_headPosition++;
            }
            // 'N' means no movement, so we don't need to do anything

            // update statistics
            if (m_currentState != transition.m_nextState) {
                m_stateChanges++;
            }

            // update the current state
            m_currentState = transition.m_nextState;
            m_stepCount++;

            LOG_INFO(std::format("Executed transition: NewState={}, Step={}",
                    transition.m_nextState, m_stepCount));

            return true;
        } else {
            // no transition found, halt the machine
            LOG_WARNING(std::format("No transition found for state={}, symbols={}. Machine halted.",
                     m_currentState, currentSymbols));
            return false;
        }
    }

    /**
     * runs the machine until it halts
     *
     * @param maxSteps maximum number of steps to execute (to prevent infinite loops)
     * @return true if the machine halted normally, false if max steps reached
     */
    bool run(int maxSteps = 1000) {
        LOG_INFO("Starting machine execution");

        while (step() && m_stepCount < maxSteps) {
            // step function handles the execution
        }

        if (m_stepCount >= maxSteps) {
            LOG_WARNING(std::format("Machine reached maximum steps ({}). Execution forcibly halted.", maxSteps));
            return false;
        }

        LOG_INFO(std::format("Machine halted after {} steps with {} state changes", m_stepCount, m_stateChanges));
        return true;
    }

    /**
     * gets the current content of a specific tape track
     *
     * @param trackIndex which track to read (0-based)
     * @return string representing the tape content
     */
    [[nodiscard]] std::string getTape(int trackIndex = 0) const {
        // validate track index
        if (trackIndex < 0 || trackIndex >= m_numTracks) {
            LOG_ERROR(std::format("Invalid track index: {}", trackIndex));
            throw std::out_of_range("Track index out of range");
        }

        return {m_tape[trackIndex].begin(), m_tape[trackIndex].end()};
    }

    /**
     * gets the current state of the machine
     *
     * @return string name of the current state
     */
    [[nodiscard]] std::string getCurrentState() const {
        return m_currentState;
    }

    /**
     * gets the current head position
     *
     * @return integer position of the head on the tape
     */
    [[nodiscard]] int getHeadPosition() const {
        return m_headPosition;
    }

    /**
     * gets the number of steps executed so far
     *
     * @return integer step count
     */
    [[nodiscard]] int getStepCount() const {
        return m_stepCount;
    }

    /**
     * gets the number of state changes that occurred
     *
     * @return integer state change count
     */
    [[nodiscard]] int getStateChanges() const {
        return m_stateChanges;
    }

    /**
     * resets the machine to its initial state
     */
    void reset() {
        m_headPosition = 0;
        m_currentState = "start";
        m_stepCount = 0;
        m_stateChanges = 0;

        // clear all tapes
        for (auto& track : m_tape) {
            std::fill(track.begin(), track.end(), m_blankSymbol);
        }

        LOG_INFO("Machine reset to initial state");
    }

private:
    /**
     * ensures the head position is valid by expanding the tape if necessary
     * expands all tracks simultaneously to maintain alignment
     */
    void ensureValidHeadPosition() {
        // if the head moved left of the tape, expand all tapes
        if (m_headPosition < 0) {
            for (auto& track : m_tape) {
                track.insert(track.begin(), m_blankSymbol);
            }
            m_headPosition = 0;
            LOG_INFO("Expanded all tapes to the left");
        }
        // if the head moved right of the tape, expand all tapes
        else if (m_headPosition >= static_cast<int>(m_tape[0].size())) {
            for (auto& track : m_tape) {
                track.push_back(m_blankSymbol);
            }
            LOG_INFO("Expanded all tapes to the right");
        }
    }
};

} // namespace turing

#endif // EXTENDED_TURING_MACHINE_HPP
