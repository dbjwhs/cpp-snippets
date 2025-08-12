// MIT License
// Copyright (c) 2025 dbjwhs

#ifndef COMPREHENSIVE_TURING_MACHINE_HPP
#define COMPREHENSIVE_TURING_MACHINE_HPP

/*
 * ComprehensiveTuringMachine
 *
 * History: The comprehensive Turing machine represents a modern synthesis of theoretical
 * Turing machine concepts with practical computing implementations. Alan Turing's original
 * 1936 paper introduced the concept as a mathematical model to define what is "effectively
 * calculable." As computing evolved, researchers expanded on Turing's work to create more
 * sophisticated models like multi-tape machines, non-deterministic Turing machines, and
 * machines with infinite alphabets. In theoretical computer science, these variations have
 * been proven equivalent in computational power to the original Turing machine, but they
 * provide more intuitive frameworks for analyzing specific types of computation. Modern
 * comprehensive implementations incorporate multiple tracks, rich alphabets, programmability,
 * and visualization features while maintaining theoretical equivalence to Turing's original model.
 *
 * Common usages:
 * - Formal language recognition and parsing
 * - Educational demonstrations of theoretical computer science concepts
 * - Modeling complex computational processes
 * - Validating computability proofs
 * - Analyzing computational complexity of algorithms
 * - Exploring limits of solvable problems (halting problem, etc.)
 */

#include <string>
#include <utility>
#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <functional>
#include <optional>
#include <variant>
#include <algorithm>
#include <chrono>
#include <thread>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include "../../../headers/project_utils.hpp"

namespace turing {

/**
 * enum class MachineType
 *
 * defines different types of turing machines with varying capabilities
 */
enum class MachineType {
    // standard single-tape deterministic turing machine
    Standard,

    // multi-tape turing machine
    MultiTape,

    // non-deterministic turing machine
    NonDeterministic,

    // universal turing machine (can simulate other machines)
    Universal
};

/**
 * struct Symbol
 *
 * represents a symbol in the turing machine alphabet
 * allows for rich symbols beyond single characters
 */
struct Symbol {
    // unique identifier for this symbol
    std::string m_id{};

    // display representation (for visualization)
    char m_representation{};

    // category/classification of this symbol
    std::string m_category{};

    // additional metadata for special symbols
    std::map<std::string, std::string> m_metadata{};

    /**
     * default constructor
     */
    Symbol() = default;

    /**
     * constructor for simple symbol
     *
     * @param id unique identifier
     * @param representation visual character
     */
    Symbol(std::string id, char representation)
        : m_id{std::move(id)}, m_representation{representation} {}

    /**
     * constructor for categorized symbol
     *
     * @param id unique identifier
     * @param representation visual character
     * @param category symbol classification
     */
    Symbol(std::string id, char representation, std::string category)
        : m_id{std::move(id)}, m_representation{representation}, m_category{std::move(category)} {}

    /**
     * equality operator
     *
     * @param other symbol to compare with
     * @return true if symbols have the same ID
     */
    bool operator==(const Symbol& other) const {
        return m_id == other.m_id;
    }

    /**
     * less-than operator for ordering
     *
     * @param other symbol to compare with
     * @return true if this symbol's ID comes before other's
     */
    bool operator<(const Symbol& other) const {
        return m_id < other.m_id;
    }
};

/**
 * class Alphabet
 *
 * defines the set of symbols usable by the turing machine
 */
class Alphabet {
private:
    // map from symbol ID to symbol
    std::map<std::string, Symbol> m_symbols{};

    // the blank symbol used for empty cells
    Symbol m_blankSymbol{"BLANK", '_'};

public:
    /**
     * default constructor
     */
    Alphabet() {
        // add blank symbol by default
        addSymbol(m_blankSymbol);

        LOG_INFO("Created default alphabet with blank symbol");
    }

    /**
     * adds a symbol to the alphabet
     *
     * @param symbol the symbol to add
     * @return true if added, false if already exists
     */
    bool addSymbol(const Symbol& symbol) {
        if (m_symbols.find(symbol.m_id) != m_symbols.end()) {
            LOG_WARNING(std::format("Symbol already exists in alphabet: {}", symbol.m_id));
            return false;
        }

        m_symbols[symbol.m_id] = symbol;
        LOG_INFO(std::format("Added symbol to alphabet: {}", symbol.m_id));
        return true;
    }

    /**
     * adds a simple symbol to the alphabet
     *
     * @param id unique identifier
     * @param representation visual character
     * @return true if added, false if already exists
     */
    bool addSymbol(const std::string& id, char representation) {
        return addSymbol(Symbol(id, representation));
    }

    /**
     * gets a symbol by its ID
     *
     * @param id the symbol identifier
     * @return reference to the symbol
     * @throws std::out_of_range if symbol not found
     */
    [[nodiscard]] const Symbol& getSymbol(const std::string& id) const {
        if (m_symbols.find(id) == m_symbols.end()) {
            LOG_ERROR(std::format("Symbol not found in alphabet: {}", id));
            throw std::out_of_range("Symbol not in alphabet");
        }

        return m_symbols.at(id);
    }

    /**
     * checks if a symbol exists in the alphabet
     *
     * @param id the symbol identifier
     * @return true if symbol exists
     */
    [[nodiscard]] bool hasSymbol(const std::string& id) const {
        return m_symbols.find(id) != m_symbols.end();
    }

    /**
     * gets the blank symbol
     *
     * @return reference to the blank symbol
     */
    [[nodiscard]] const Symbol& getBlankSymbol() const {
        return m_blankSymbol;
    }

    /**
     * gets all symbols in the alphabet
     *
     * @return vector of all symbols
     */
    [[nodiscard]] std::vector<Symbol> getAllSymbols() const {
        std::vector<Symbol> result;
        result.reserve(m_symbols.size());

        for (const auto& [_, symbol] : m_symbols) {
            result.push_back(symbol);
        }

        return result;
    }

    /**
     * gets the number of symbols in the alphabet
     *
     * @return size of the alphabet
     */
    [[nodiscard]] size_t size() const {
        return m_symbols.size();
    }
};

/**
 * class Tape
 *
 * represents the infinite tape of a turing machine
 * efficiently handles sparse storage of symbols
 */
class Tape {
private:
    // the alphabet used by this tape
    std::shared_ptr<Alphabet> m_alphabet{};

    // sparse storage of non-blank symbols (position -> symbol ID)
    std::map<int, std::string> m_cells{};

    // leftmost position used on tape
    int m_leftBound{0};

    // rightmost position used on tape
    int m_rightBound{0};

    // the name of this tape (for multi-tape machines)
    std::string m_name{};

public:
    /**
     * constructor with alphabet
     *
     * @param alphabet shared pointer to alphabet
     * @param name optional name for the tape
     */
    explicit Tape(std::shared_ptr<Alphabet> alphabet, const std::string& name = "")
        : m_alphabet{std::move(alphabet)}, m_name{name} {
        LOG_INFO(std::format("Created tape with name: {}", name.empty() ? "default" : name));
    }

    /**
     * writes a symbol to the tape at a specific position
     *
     * @param position where to write
     * @param symbolId ID of the symbol to write
     */
    void writeSymbol(int position, const std::string& symbolId) {
        try {
            // check if symbol exists in alphabet, and if not add it
            if (!m_alphabet->hasSymbol(symbolId)) {
                // Try to add it with the first character as representation
                // This is a fallback to make things more robust
                if (!symbolId.empty()) {
                    char representation = symbolId[0];
                    LOG_INFO(std::format("Adding missing symbol '{}' to alphabet", symbolId));
                    m_alphabet->addSymbol(symbolId, representation);
                } else {
                    LOG_ERROR("Cannot write empty symbol");
                    throw std::invalid_argument("Empty symbol");
                }
            }
    
            // if it's a blank symbol, we can just remove any existing symbol
            if (symbolId == m_alphabet->getBlankSymbol().m_id) {
                m_cells.erase(position);
            } else {
                m_cells[position] = symbolId;
            }
    
            // update bounds
            m_leftBound = std::min(m_leftBound, position);
            m_rightBound = std::max(m_rightBound, position);
    
            LOG_INFO(std::format("Wrote symbol {} at position {}", symbolId, position));
        } catch (const std::exception& e) {
            LOG_ERROR(std::format("Error writing symbol: {}", e.what()));
            throw; // Re-throw the exception
        }
    }

    /**
     * reads a symbol from the tape at a specific position
     *
     * @param position where to read from
     * @return ID of the symbol at that position
     */
    [[nodiscard]] std::string readSymbol(int position) const {
        try {
            const auto it = m_cells.find(position);
            if (it == m_cells.end()) {
                // return blank symbol if no symbol at this position
                return m_alphabet->getBlankSymbol().m_id;
            }
            
            if (!m_alphabet->hasSymbol(it->second)) {
                LOG_ERROR(std::format("Invalid read symbol: {}", it->second));
                throw std::invalid_argument("Read symbol not in alphabet");
            }

            return it->second;
        } catch (const std::exception& e) {
            LOG_ERROR(std::format("Invalid read symbol: _"));
            throw std::invalid_argument("Read symbol not in alphabet");
        }
    }

    /**
     * clears the tape (sets all cells to blank)
     */
    void clear() {
        m_cells.clear();
        m_leftBound = 0;
        m_rightBound = 0;

        LOG_INFO("Tape cleared");
    }

    /**
     * sets the content of the tape using a string representation
     *
     * @param content string where each character maps to a symbol
     * @param startPosition where to start writing on the tape
     * @param charToSymbolId function to map chars to symbol IDs
     */
    void setContent(const std::string& content, int startPosition = 0,
                   std::function<std::string(char)> charToSymbolId = nullptr) {
        clear();

        // if no mapping function provided, use default (character as the ID)
        if (!charToSymbolId) {
            charToSymbolId = [](char c) { return std::string(1, c); };
        }

        for (size_t i = 0; i < content.size(); ++i) {
            std::string symbolId = charToSymbolId(content[i]);
            writeSymbol(startPosition + static_cast<int>(i), symbolId);
        }

        LOG_INFO(std::format("Set tape content starting at position {}", startPosition));
    }

    /**
     * gets the content of the tape as a string
     *
     * @param start starting position (default: left bound)
     * @param end ending position (default: right bound)
     * @param symbolIdToChar function to map symbol IDs to chars
     * @return string representation of the tape content
     */
    std::string getContent(std::optional<int> start = std::nullopt,
                          std::optional<int> end = std::nullopt,
                          std::function<char(const std::string&)> symbolIdToChar = nullptr) const {
        // use bounds if not specified
        int startPos = start.value_or(m_leftBound);
        int endPos = end.value_or(m_rightBound);

        // if no mapping function provided, use default (get representation from alphabet)
        if (!symbolIdToChar) {
            symbolIdToChar = [this](const std::string& symbolId) {
                return m_alphabet->getSymbol(symbolId).m_representation;
            };
        }

        std::string result;
        for (int i = startPos; i <= endPos; ++i) {
            std::string symbolId = readSymbol(i);
            result += symbolIdToChar(symbolId);
        }

        return result;
    }

    /**
     * gets the name of the tape
     *
     * @return string name
     */
    [[nodiscard]] std::string getName() const {
        return m_name;
    }

    /**
     * sets the name of the tape
     *
     * @param name new name
     */
    void setName(const std::string& name) {
        m_name = name;
        LOG_INFO(std::format("Renamed tape to: {}", name));
    }

    /**
     * gets the left bound of used tape positions
     *
     * @return leftmost used position
     */
    [[nodiscard]] int getLeftBound() const {
        return m_leftBound;
    }

    /**
     * gets the right bound of used tape positions
     *
     * @return rightmost used position
     */
    [[nodiscard]] int getRightBound() const {
        return m_rightBound;
    }
    
    /**
     * gets the alphabet used by this tape
     *
     * @return shared pointer to the alphabet
     */
    [[nodiscard]] std::shared_ptr<Alphabet> getAlphabet() const {
        return m_alphabet;
    }
};

/**
 * class Head
 *
 * represents a read/write head for a turing machine
 * tracks position and movement
 */
class Head {
private:
    // current position of the head
    int m_position{0};

    // the tape this head is reading/writing
    std::shared_ptr<Tape> m_tape{};

    // name of this head (for multi-head machines)
    std::string m_name{};

public:
    /**
     * constructor with tape
     *
     * @param tape shared pointer to the tape
     * @param name optional name for the head
     */
    explicit Head(std::shared_ptr<Tape> tape, const std::string& name = "")
        : m_tape{std::move(tape)}, m_name{name} {
        LOG_INFO(std::format("Created head with name: {}", name.empty() ? "default" : name));
    }

    /**
     * moves the head left
     *
     * @param steps number of steps to move (default 1)
     */
    void moveLeft(int steps = 1) {
        m_position -= steps;
        LOG_INFO(std::format("Head moved left to position {}", m_position));
    }

    /**
     * moves the head right
     *
     * @param steps number of steps to move (default 1)
     */
    void moveRight(int steps = 1) {
        m_position += steps;
        LOG_INFO(std::format("Head moved right to position {}", m_position));
    }

    /**
     * moves the head to a specific position
     *
     * @param position absolute position to move to
     */
    void moveTo(int position) {
        m_position = position;
        LOG_INFO(std::format("Head moved to position {}", m_position));
    }

    /**
     * reads the symbol at the current head position
     *
     * @return symbol ID at current position
     */
    [[nodiscard]] std::string readSymbol() const {
        try {
            return m_tape->readSymbol(m_position);
        } catch (const std::exception& e) {
            LOG_WARNING(std::format("Exception in Head::readSymbol: {}", e.what()));
            return m_tape->getAlphabet()->getBlankSymbol().m_id;
        }
    }

    /**
     * writes a symbol at the current head position
     *
     * @param symbolId ID of the symbol to write
     */
    void writeSymbol(const std::string& symbolId) const {
        m_tape->writeSymbol(m_position, symbolId);
    }

    /**
     * gets the current position of the head
     *
     * @return integer position
     */
    [[nodiscard]] int getPosition() const {
        return m_position;
    }

    /**
     * gets the tape this head is operating on
     *
     * @return shared pointer to the tape
     */
    [[nodiscard]] std::shared_ptr<Tape> getTape() const {
        return m_tape;
    }

    /**
     * gets the name of the head
     *
     * @return string name
     */
    [[nodiscard]] std::string getName() const {
        return m_name;
    }

    /**
     * sets the name of the head
     *
     * @param name new name
     */
    void setName(const std::string& name) {
        m_name = name;
        LOG_INFO(std::format("Renamed head to: {}", name));
    }
};

/**
 * enum class Direction
 *
 * defines possible movement directions for the turing machine head
 */
enum class Direction {
    // move left
    Left,

    // move right
    Right,

    // stay in place
    None
};

/**
 * class State
 *
 * represents a state in the turing machine
 */
class State {
private:
    // unique identifier for this state
    std::string m_id{};

    // human-readable name
    std::string m_name{};

    // description of what this state does
    std::string m_description{};

    // whether this is a halting state
    bool m_isHalting{false};

    // whether this is an accepting state
    bool m_isAccepting{false};

    // additional metadata
    std::map<std::string, std::string> m_metadata{};

public:
    /**
     * default constructor
     */
    State() = default;

    /**
     * constructor with ID
     *
     * @param id unique identifier
     */
    explicit State(std::string id)
        : m_id{std::move(id)}, m_name{m_id} {}

    /**
     * constructor with ID and name
     *
     * @param id unique identifier
     * @param name human-readable name
     */
    State(std::string id, std::string name)
        : m_id{std::move(id)}, m_name{std::move(name)} {}

    /**
     * constructor with all basic fields
     *
     * @param id unique identifier
     * @param name human-readable name
     * @param description what this state does
     * @param isHalting whether this is a halting state
     * @param isAccepting whether this is an accepting state
     */
    State(std::string id, std::string name, std::string description,
         bool isHalting = false, bool isAccepting = false)
         : m_id{std::move(id)}, m_name{std::move(name)}, m_description{std::move(description)},
           m_isHalting{isHalting}, m_isAccepting{isAccepting} {}

    /**
     * gets the state ID
     *
     * @return string ID
     */
    [[nodiscard]] std::string getId() const {
        return m_id;
    }

    /**
     * gets the state name
     *
     * @return string name
     */
    [[nodiscard]] std::string getName() const {
        return m_name;
    }

    /**
     * gets the state description
     *
     * @return string description
     */
    [[nodiscard]] std::string getDescription() const {
        return m_description;
    }

    /**
     * checks if this is a halting state
     *
     * @return true if halting
     */
    [[nodiscard]] bool isHalting() const {
        return m_isHalting;
    }

    /**
     * checks if this is an accepting state
     *
     * @return true if accepting
     */
    [[nodiscard]] bool isAccepting() const {
        return m_isAccepting;
    }

    /**
     * sets whether this is a halting state
     *
     * @param halting new halting status
     */
    void setHalting(bool halting) {
        m_isHalting = halting;
    }

    /**
     * sets whether this is an accepting state
     *
     * @param accepting new accepting status
     */
    void setAccepting(bool accepting) {
        m_isAccepting = accepting;
    }

    /**
     * gets a metadata value
     *
     * @param key metadata key
     * @return value if found, empty string otherwise
     */
    [[nodiscard]] std::string getMetadata(const std::string& key) const {
        if (const auto it = m_metadata.find(key); it != m_metadata.end()) {
            return it->second;
        }
        return "";
    }

    /**
     * sets a metadata value
     *
     * @param key metadata key
     * @param value metadata value
     */
    void setMetadata(const std::string& key, const std::string& value) {
        m_metadata[key] = value;
    }

    /**
     * equality operator
     *
     * @param other state to compare with
     * @return true if states have the same ID
     */
    bool operator==(const State& other) const {
        return m_id == other.m_id;
    }

    /**
     * less-than operator for ordering
     *
     * @param other state to compare with
     * @return true if this state's ID comes before other's
     */
    bool operator<(const State& other) const {
        return m_id < other.m_id;
    }
};

/**
 * struct Transition
 *
 * represents a state transition in the turing machine
 * more complex than earlier implementations, supporting multiple heads
 */
struct Transition {
    // the starting state
    std::string m_currentStateId{};

    // the set of input symbols (one per head)
    std::vector<std::string> m_readSymbolIds{};

    // the resulting state
    std::string m_nextStateId{};

    // the symbols to write (one per head)
    std::vector<std::string> m_writeSymbolIds{};

    // the directions to move (one per head)
    std::vector<Direction> m_moveDirections{};

    /**
     * default constructor required for use in containers
     */
    Transition() = default;

    /**
     * constructor for single-head transition
     *
     * @param currentStateId starting state ID
     * @param readSymbolId input symbol ID
     * @param nextStateId resulting state ID
     * @param writeSymbolId symbol to write
     * @param moveDirection direction to move
     */
    Transition(std::string currentStateId, std::string readSymbolId,
              std::string nextStateId, std::string writeSymbolId,
              Direction moveDirection)
        : m_currentStateId{std::move(currentStateId)},
          m_readSymbolIds{std::move(readSymbolId)},
          m_nextStateId{std::move(nextStateId)},
          m_writeSymbolIds{std::move(writeSymbolId)},
          m_moveDirections{moveDirection} {}

    /**
     * constructor for multi-head transition
     *
     * @param currentStateId starting state ID
     * @param readSymbolIds input symbols (one per head)
     * @param nextStateId resulting state ID
     * @param writeSymbolIds symbols to write (one per head)
     * @param moveDirections directions to move (one per head)
     */
    Transition(std::string currentStateId, std::vector<std::string> readSymbolIds,
              std::string nextStateId, std::vector<std::string> writeSymbolIds,
              std::vector<Direction> moveDirections)
        : m_currentStateId{std::move(currentStateId)},
          m_readSymbolIds{std::move(readSymbolIds)},
          m_nextStateId{std::move(nextStateId)},
          m_writeSymbolIds{std::move(writeSymbolIds)},
          m_moveDirections{std::move(moveDirections)} {}
};

/**
 * class TransitionFunction
 *
 * represents the full transition function of a turing machine
 * maps (state, symbols) to (next state, write symbols, move directions)
 */
class TransitionFunction {
private:
    // maps (state ID, read symbols) to transition
    // multi-head reads are combined into a single string with delimiter
    std::unordered_map<std::string, std::unordered_map<std::string, Transition>> m_transitions{};

    // delimiter used for combining multi-head read symbols
    static constexpr char DELIMITER = '|';

    /**
     * creates a key for the read symbols
     *
     * @param readSymbols vector of symbol IDs
     * @return combined key string
     */
    static std::string createReadKey(const std::vector<std::string>& readSymbols) {
        if (readSymbols.empty()) {
            return "";
        }

        std::stringstream ss;
        ss << readSymbols[0];
        for (size_t i = 1; i < readSymbols.size(); ++i) {
            ss << DELIMITER << readSymbols[i];
        }

        return ss.str();
    }

    /**
     * splits a read key back into symbol IDs
     *
     * @param readKey combined key string
     * @return vector of symbol IDs
     */
    static std::vector<std::string> splitReadKey(const std::string& readKey) {
        std::vector<std::string> result;
        std::stringstream ss(readKey);
        std::string item;

        while (std::getline(ss, item, DELIMITER)) {
            result.push_back(item);
        }

        return result;
    }

public:
    /**
     * adds a transition rule
     *
     * @param transition the transition to add
     */
    void addTransition(Transition transition) {
        std::string readKey = createReadKey(transition.m_readSymbolIds);
        m_transitions[transition.m_currentStateId][readKey] = transition;

        LOG_INFO(std::format("Added transition from state {} to {}",
                transition.m_currentStateId, transition.m_nextStateId));
    }

    /**
     * adds a transition rule for single-head machine
     *
     * @param currentStateId starting state ID
     * @param readSymbolId input symbol ID
     * @param nextStateId resulting state ID
     * @param writeSymbolId symbol to write
     * @param moveDirection direction to move
     */
    void addTransition(const std::string& currentStateId, const std::string& readSymbolId,
                      const std::string& nextStateId, const std::string& writeSymbolId,
                      Direction moveDirection) {
        addTransition(Transition(currentStateId, readSymbolId, nextStateId, writeSymbolId, moveDirection));
    }

    /**
     * adds a transition rule for multi-head machine
     *
     * @param currentStateId starting state ID
     * @param readSymbolIds input symbols (one per head)
     * @param nextStateId resulting state ID
     * @param writeSymbolIds symbols to write (one per head)
     * @param moveDirections directions to move (one per head)
     */
    void addTransition(const std::string& currentStateId, const std::vector<std::string>& readSymbolIds,
                      const std::string& nextStateId, const std::vector<std::string>& writeSymbolIds,
                      const std::vector<Direction>& moveDirections) {
        addTransition(Transition(currentStateId, readSymbolIds, nextStateId, writeSymbolIds, moveDirections));
    }

    /**
     * gets the transition for a given state and read symbols
     *
     * @param currentStateId current state ID
     * @param readSymbolIds read symbols (one per head)
     * @return optional containing the transition, empty if no matching transition
     */
    [[nodiscard]] std::optional<Transition> getTransition(const std::string& currentStateId,
                                         const std::vector<std::string>& readSymbolIds) const {
        const auto stateIt = m_transitions.find(currentStateId);
        if (stateIt == m_transitions.end()) {
            return std::nullopt;
        }

        const std::string readKey = createReadKey(readSymbolIds);
        const auto transIt = stateIt->second.find(readKey);
        if (transIt == stateIt->second.end()) {
            return std::nullopt;
        }

        return transIt->second;
    }

    /**
     * checks if a transition exists for a given state and read symbols
     *
     * @param currentStateId current state ID
     * @param readSymbolIds read symbols (one per head)
     * @return true if a transition exists
     */
    [[nodiscard]] bool hasTransition(const std::string& currentStateId,
                      const std::vector<std::string>& readSymbolIds) const {
        return getTransition(currentStateId, readSymbolIds).has_value();
    }

    /**
     * gets all transitions from a specific state
     *
     * @param stateId state ID
     * @return vector of all transitions from that state
     */
    [[nodiscard]] std::vector<Transition> getTransitionsFromState(const std::string& stateId) const {
        std::vector<Transition> result;

        auto stateIt = m_transitions.find(stateId);
        if (stateIt != m_transitions.end()) {
            result.reserve(stateIt->second.size());
            for (const auto& [_, transition] : stateIt->second) {
                result.push_back(transition);
            }
        }

        return result;
    }

    /**
     * clears all transitions
     */
    void clear() {
        m_transitions.clear();
        LOG_INFO("Cleared all transitions");
    }
};

/**
 * class ComprehensiveTuringMachine
 *
 * full-featured turing machine with multiple tapes, programmability,
 * execution monitoring, and extensive state control
 */
class ComprehensiveTuringMachine {
private:
    // type of machine (standard, multi-tape, etc.)
    MachineType m_type{MachineType::Standard};

    // name of the machine
    std::string m_name{"Comprehensive Turing Machine"};

    // description of what the machine does
    std::string m_description{};

    // alphabet used by the machine
    std::shared_ptr<Alphabet> m_alphabet{std::make_shared<Alphabet>()};

    // collection of states (ID -> State)
    std::map<std::string, State> m_states{};

    // collection of tapes (name -> Tape)
    std::map<std::string, std::shared_ptr<Tape>> m_tapes{};

    // collection of heads (name -> Head)
    std::map<std::string, std::shared_ptr<Head>> m_heads{};

    // the transition function
    TransitionFunction m_transitionFunction{};

    // current state ID
    std::string m_currentStateId{"initial"};

    // initial state ID
    std::string m_initialStateId{"initial"};

    // step count
    size_t m_stepCount{0};

    // maximum allowed steps
    size_t m_maxSteps{10000};

    // visualization delay in milliseconds (for step-by-step viewing)
    int m_visualizationDelay{0};

    // execution history (for debugging/analysis)
    struct HistoryEntry {
        std::string m_stateId;
        std::vector<std::string> m_readSymbols;
        std::vector<std::string> m_writeSymbols;
        std::vector<Direction> m_moveDirections;
        std::vector<int> m_headPositions;
    };
    std::vector<HistoryEntry> m_history{};

    // whether to record history
    bool m_recordHistory{false};

public:
    /**
     * default constructor
     *
     * creates a standard single-tape machine
     */
    ComprehensiveTuringMachine()
        : m_type{MachineType::Standard} {
        initialize();
        LOG_INFO("Created ComprehensiveTuringMachine with type Standard");
    }

    /**
     * constructor with machine type
     *
     * @param type type of machine to create
     */
    explicit ComprehensiveTuringMachine(MachineType type)
        : m_type{type} {
        initialize();
        LOG_INFO(std::format("Created ComprehensiveTuringMachine with specified type"));
    }

    /**
     * constructor with name and description
     *
     * @param name machine name
     * @param description machine description
     * @param type type of machine to create
     */
    ComprehensiveTuringMachine(std::string name, std::string description, MachineType type = MachineType::Standard)
        : m_type{type}, m_name{std::move(name)}, m_description{std::move(description)} {
        initialize();
        LOG_INFO(std::format("Created ComprehensiveTuringMachine: {}", m_name));
    }

    /**
     * initializes the machine based on its type
     */
    void initialize() {
        // create initial state
        addState(State(m_initialStateId, "Initial", "Starting state of the machine"));
        m_currentStateId = m_initialStateId;

        // create halting states
        addState(State("halt", "Halt", "Machine halts", true, false));
        addState(State("accept", "Accept", "Machine accepts input", true, true));
        addState(State("reject", "Reject", "Machine rejects input", true, false));

        // create tapes and heads based on machine type
        if (m_type == MachineType::Standard || m_type == MachineType::Universal) {
            auto tape = std::make_shared<Tape>(m_alphabet, "main");
            auto head = std::make_shared<Head>(tape, "main");

            m_tapes["main"] = tape;
            m_heads["main"] = head;
        } else if (m_type == MachineType::MultiTape) {
            // create default 3 tapes for multi-tape machine
            for (int i = 0; i < 3; ++i) {
                std::string name = "tape" + std::to_string(i);
                auto tape = std::make_shared<Tape>(m_alphabet, name);
                auto head = std::make_shared<Head>(tape, name);

                m_tapes[name] = tape;
                m_heads[name] = head;
            }
        } else if (m_type == MachineType::NonDeterministic) {
            // non-deterministic machine starts with one tape like standard
            auto tape = std::make_shared<Tape>(m_alphabet, "main");
            auto head = std::make_shared<Head>(tape, "main");

            m_tapes["main"] = tape;
            m_heads["main"] = head;
        }

        // common setup for all machine types
        addStandardSymbols();

        LOG_INFO("Machine initialized with default configuration");
    }

    /**
     * adds common symbols to the alphabet
     */
    void addStandardSymbols() {
        // add binary digits
        m_alphabet->addSymbol("0", '0');
        m_alphabet->addSymbol("1", '1');

        // add basic arithmetic operators
        m_alphabet->addSymbol("+", '+');
        m_alphabet->addSymbol("-", '-');
        m_alphabet->addSymbol("*", '*');
        m_alphabet->addSymbol("/", '/');
        m_alphabet->addSymbol("=", '=');
        
        // ensure underscore is in the alphabet
        m_alphabet->addSymbol("_", '_');

        LOG_INFO("Added standard symbols to alphabet");
    }

    /**
     * adds a state to the machine
     *
     * @param state the state to add
     * @return reference to the added state
     */
    const State& addState(const State& state) {
        m_states[state.getId()] = state;
        LOG_INFO(std::format("Added state: {}", state.getId()));
        return m_states[state.getId()];
    }

    /**
     * adds a symbol to the alphabet
     *
     * @param symbol the symbol to add
     * @return true if added, false if already exists
     */
    bool addSymbol(const Symbol& symbol) {
        return m_alphabet->addSymbol(symbol);
    }

    /**
     * adds a simple symbol to the alphabet
     *
     * @param id symbol ID
     * @param representation visual character
     * @return true if added, false if already exists
     */
    bool addSymbol(const std::string& id, char representation) {
        return m_alphabet->addSymbol(id, representation);
    }

    /**
     * adds a tape to the machine
     *
     * @param name name for the new tape
     * @return shared pointer to the new tape
     */
    std::shared_ptr<Tape> addTape(const std::string& name) {
        if (m_tapes.find(name) != m_tapes.end()) {
            LOG_WARNING(std::format("Tape already exists: {}", name));
            return m_tapes[name];
        }

        auto tape = std::make_shared<Tape>(m_alphabet, name);
        m_tapes[name] = tape;

        // create a head for this tape
        addHead(name, tape);

        LOG_INFO(std::format("Added tape: {}", name));
        return tape;
    }

    /**
     * adds a head to the machine
     *
     * @param name name for the new head
     * @param tape tape for the head to operate on
     * @return shared pointer to the new head
     */
    std::shared_ptr<Head> addHead(const std::string& name, const std::shared_ptr<Tape>& tape) {
        if (m_heads.contains(name)) {
            LOG_WARNING(std::format("Head already exists: {}", name));
            return m_heads[name];
        }

        auto head = std::make_shared<Head>(tape, name);
        m_heads[name] = head;

        LOG_INFO(std::format("Added head: {}", name));
        return head;
    }

    /**
     * adds a transition rule for a single-head machine
     *
     * @param currentState current state
     * @param readSymbol symbol to read
     * @param nextState next state
     * @param writeSymbol symbol to write
     * @param moveDirection direction to move
     */
    void addTransition(const std::string& currentState, const std::string& readSymbol,
                      const std::string& nextState, const std::string& writeSymbol,
                      Direction moveDirection) {
        // validate states
        if (m_states.find(currentState) == m_states.end()) {
            LOG_ERROR(std::format("Invalid current state: {}", currentState));
            throw std::invalid_argument("Current state not found");
        }
        if (m_states.find(nextState) == m_states.end()) {
            LOG_ERROR(std::format("Invalid next state: {}", nextState));
            throw std::invalid_argument("Next state not found");
        }

        // validate symbols
        if (!m_alphabet->hasSymbol(readSymbol)) {
            LOG_ERROR(std::format("Invalid read symbol: {}", readSymbol));
            throw std::invalid_argument("Read symbol not in alphabet");
        }
        if (!m_alphabet->hasSymbol(writeSymbol)) {
            LOG_ERROR(std::format("Invalid write symbol: {}", writeSymbol));
            throw std::invalid_argument("Write symbol not in alphabet");
        }

        m_transitionFunction.addTransition(currentState, readSymbol, nextState, writeSymbol, moveDirection);
    }

    /**
     * adds a transition rule for a multi-head machine
     *
     * @param currentState current state
     * @param readSymbols symbols to read (one per head)
     * @param nextState next state
     * @param writeSymbols symbols to write (one per head)
     * @param moveDirections directions to move (one per head)
     */
    void addTransition(const std::string& currentState, const std::vector<std::string>& readSymbols,
                      const std::string& nextState, const std::vector<std::string>& writeSymbols,
                      const std::vector<Direction>& moveDirections) {
        // validate states
        if (m_states.find(currentState) == m_states.end()) {
            LOG_ERROR(std::format("Invalid current state: {}", currentState));
            throw std::invalid_argument("Current state not found");
        }
        if (m_states.find(nextState) == m_states.end()) {
            LOG_ERROR(std::format("Invalid next state: {}", nextState));
            throw std::invalid_argument("Next state not found");
        }

        // validate number of symbols matches number of heads
        if (readSymbols.size() != m_heads.size() ||
            writeSymbols.size() != m_heads.size() ||
            moveDirections.size() != m_heads.size()) {
            LOG_ERROR("Mismatch between symbols/directions and number of heads");
            throw std::invalid_argument("Number of symbols/directions must match number of heads");
        }

        // validate symbols
        for (const auto& symbol : readSymbols) {
            if (!m_alphabet->hasSymbol(symbol)) {
                LOG_ERROR(std::format("Invalid read symbol: {}", symbol));
                throw std::invalid_argument("Read symbol not in alphabet");
            }
        }
        for (const auto& symbol : writeSymbols) {
            if (!m_alphabet->hasSymbol(symbol)) {
                LOG_ERROR(std::format("Invalid write symbol: {}", symbol));
                throw std::invalid_argument("Write symbol not in alphabet");
            }
        }

        m_transitionFunction.addTransition(currentState, readSymbols, nextState, writeSymbols, moveDirections);
    }

    /**
     * sets the initial tape content
     *
     * @param content content string
     * @param tapeName which tape to set (default: first tape)
     * @param startPosition where to start writing (default: 0)
     */
    void setTapeContent(const std::string& content, const std::string& tapeName = "", int startPosition = 0) {
        try {
            // if no tape name provided, use the first tape
            std::string tapeToUse = tapeName.empty() ? getFirstTapeName() : tapeName;
    
            auto tapeIt = m_tapes.find(tapeToUse);
            if (tapeIt == m_tapes.end()) {
                LOG_ERROR(std::format("Tape not found: {}", tapeToUse));
                throw std::invalid_argument("Tape not found");
            }
    
            // Add all characters to the alphabet before setting the tape content
            for (char c : content) {
                std::string symbolId(1, c);
                if (!m_alphabet->hasSymbol(symbolId)) {
                    LOG_INFO(std::format("Adding symbol '{}' to alphabet", symbolId));
                    m_alphabet->addSymbol(symbolId, c);
                }
            }
    
            // simple character-to-symbol mapping
            auto charToSymbolId = [](char c) { return std::string(1, c); };
    
            tapeIt->second->setContent(content, startPosition, charToSymbolId);
            LOG_INFO(std::format("Set content of tape '{}' to: {}", tapeToUse, content));
        } catch (const std::exception& e) {
            LOG_ERROR(std::format("Error setting tape content: {}", e.what()));
            throw; // Re-throw the exception
        }
    }

    /**
     * gets the first tape name (typically "main" or "tape0")
     *
     * @return name of the first tape
     */
    [[nodiscard]] std::string getFirstTapeName() const {
        if (m_tapes.empty()) {
            LOG_ERROR("No tapes available");
            throw std::runtime_error("No tapes available");
        }

        return m_tapes.begin()->first;
    }

    /**
     * gets the content of a tape
     *
     * @param tapeName which tape to read (default: first tape)
     * @return string content
     */
    std::string getTapeContent(const std::string& tapeName = "") const {
        try {
            // if no tape name provided, use the first tape
            std::string tapeToUse = tapeName.empty() ? getFirstTapeName() : tapeName;
    
            auto tapeIt = m_tapes.find(tapeToUse);
            if (tapeIt == m_tapes.end()) {
                LOG_ERROR(std::format("Tape not found: {}", tapeToUse));
                throw std::invalid_argument("Tape not found");
            }
    
            return tapeIt->second->getContent();
        } catch (const std::exception& e) {
            // This indicates the tape doesn't exist yet or can't be accessed
            LOG_WARNING(std::format("Error getting tape content for '{}': {}", 
                                   tapeName.empty() ? "default" : tapeName, e.what()));
            throw; // Re-throw the exception to let callers handle it
        }
    }

    /**
     * resets the machine to its initial state
     */
    void reset() {
        m_currentStateId = m_initialStateId;
        m_stepCount = 0;
        m_history.clear();

        // reset all heads to position 0
        for (auto& [_, head] : m_heads) {
            head->moveTo(0);
        }

        LOG_INFO("Machine reset to initial state");
    }

    /**
     * performs a single step of the machine
     *
     * @return true if the machine can continue, false if halted
     */
    bool step() {
        // check if in halting state
        if (isHalted()) {
            LOG_INFO(std::format("Machine is in halting state: {}", m_currentStateId));
            return false;
        }

        // collect current symbols under all heads
        std::vector<std::string> readSymbols;
        for (const auto& [_, head] : m_heads) {
            readSymbols.push_back(head->readSymbol());
        }

        // find applicable transition
        auto transition = m_transitionFunction.getTransition(m_currentStateId, readSymbols);
        if (!transition) {
            // no transition found, machine halts
            LOG_WARNING(std::format("No transition found from state {} with symbols {}",
                     m_currentStateId, readSymbols));
            m_currentStateId = "halt";
            return false;
        }

        // record history if enabled
        if (m_recordHistory) {
            HistoryEntry entry;
            entry.m_stateId = m_currentStateId;
            entry.m_readSymbols = readSymbols;
            entry.m_writeSymbols = transition->m_writeSymbolIds;
            entry.m_moveDirections = transition->m_moveDirections;

            // record head positions
            for (const auto& [_, head] : m_heads) {
                entry.m_headPositions.push_back(head->getPosition());
            }

            m_history.push_back(entry);
        }

        // apply the transition
        // 1. write symbols
        int headIndex = 0;
        for (const auto& [_, head] : m_heads) {
            head->writeSymbol(transition->m_writeSymbolIds[headIndex]);
            headIndex++;
        }

        // 2. move heads
        headIndex = 0;
        for (const auto& [_, head] : m_heads) {
            Direction direction = transition->m_moveDirections[headIndex];
            if (direction == Direction::Left) {
                head->moveLeft();
            } else if (direction == Direction::Right) {
                head->moveRight();
            }
            // Direction::None means don't move

            headIndex++;
        }

        // 3. update state
        m_currentStateId = transition->m_nextStateId;

        // increment step counter
        m_stepCount++;

        // add visualization delay if enabled
        if (m_visualizationDelay > 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(m_visualizationDelay));
        }

        LOG_INFO(std::format("Executed step {}: State transitioned to {}", m_stepCount, m_currentStateId));

        return !isHalted();
    }

    /**
     * runs the machine until it halts
     *
     * @return execution result (accepts/rejects/halts/timeout)
     */
    std::string run() {
        LOG_INFO("Starting machine execution");

        try {
            while (step() && m_stepCount < m_maxSteps) {
                // step function handles the execution
            }
        } catch (const std::exception& e) {
            LOG_WARNING(std::format("Exception during execution: {}", e.what()));
            return "error";
        }

        if (m_stepCount >= m_maxSteps) {
            LOG_WARNING(std::format("Machine reached maximum steps ({}). Possible infinite loop.", m_maxSteps));
            return "timeout";
        }

        if (isAccepted()) {
            LOG_INFO("Machine accepted the input");
            return "accept";
        } else if (m_currentStateId == "reject") {
            LOG_INFO("Machine rejected the input");
            return "reject";
        } else {
            LOG_INFO("Machine halted without accepting or rejecting");
            return "halt";
        }
    }

    /**
     * checks if the machine is currently halted
     *
     * @return true if in a halting state
     */
    bool isHalted() const {
        const auto& state = m_states.at(m_currentStateId);
        return state.isHalting();
    }

    /**
     * checks if the machine has accepted the input
     *
     * @return true if in an accepting state
     */
    bool isAccepted() const {
        const auto& state = m_states.at(m_currentStateId);
        return state.isAccepting();
    }

    /**
     * gets the current state
     *
     * @return reference to current state
     */
    const State& getCurrentState() const {
        return m_states.at(m_currentStateId);
    }

    /**
     * gets the current state ID
     *
     * @return string state ID
     */
    std::string getCurrentStateId() const {
        return m_currentStateId;
    }

    /**
     * gets the step count
     *
     * @return number of steps executed
     */
    size_t getStepCount() const {
        return m_stepCount;
    }

    /**
     * sets the maximum allowed steps
     *
     * @param maxSteps new maximum
     */
    void setMaxSteps(size_t maxSteps) {
        m_maxSteps = maxSteps;
        LOG_INFO(std::format("Set maximum steps to {}", maxSteps));
    }

    /**
     * enables or disables history recording
     *
     * @param enable whether to record history
     */
    void enableHistory(bool enable) {
        m_recordHistory = enable;
        if (enable) {
            m_history.clear();
            LOG_INFO("Enabled history recording");
        } else {
            LOG_INFO("Disabled history recording");
        }
    }

    /**
     * gets the execution history
     *
     * @return vector of history entries
     */
    const std::vector<HistoryEntry>& getHistory() const {
        return m_history;
    }

    /**
     * sets the visualization delay
     *
     * @param milliseconds delay between steps
     */
    void setVisualizationDelay(int milliseconds) {
        m_visualizationDelay = milliseconds;
        LOG_INFO(std::format("Set visualization delay to {} ms", milliseconds));
    }

    /**
     * gets the machine type
     *
     * @return machine type enum
     */
    MachineType getType() const {
        return m_type;
    }

    /**
     * gets the machine name
     *
     * @return string name
     */
    std::string getName() const {
        return m_name;
    }

    /**
     * configures the machine for binary addition
     */
    void configureForAddition() {
        reset();

        // create the necessary states
        addState(State("scan_right", "Scan Right", "Scanning right to find the equals sign"));
        addState(State("add", "Add", "Performing addition"));
        addState(State("carry", "Carry", "Handling carry"));
        addState(State("done", "Done", "Addition complete", true, true));

        // create transitions for binary addition
        // simple example (not complete)
        addTransition("initial", "0", "scan_right", "0", Direction::Right);
        addTransition("initial", "1", "scan_right", "1", Direction::Right);
        addTransition("scan_right", "0", "scan_right", "0", Direction::Right);
        addTransition("scan_right", "1", "scan_right", "1", Direction::Right);
        addTransition("scan_right", "+", "scan_right", "+", Direction::Right);
        addTransition("scan_right", "=", "add", "=", Direction::Left);

        // add transitions for addition logic
        // 0+0=0, 0+1=1, 1+0=1, 1+1=10 (carry 1)
        // more transitions would be added for a complete implementation

        LOG_INFO("Configured machine for binary addition");
    }

    /**
     * configures the machine to recognize a specific language
     *
     * @param languageType type of language to recognize (e.g., "palindrome", "anbn")
     */
    void configureForLanguage(const std::string& languageType) {
        reset();

        if (languageType == "palindrome") {
            // configure to recognize palindromes
            addState(State("scan_right", "Scan Right", "Moving right to find end"));
            addState(State("scan_left", "Scan Left", "Moving left comparing symbols"));
            addState(State("match", "Match", "Symbols match"));

            // add transitions for palindrome recognition
            // more transitions would be added for a complete implementation

            LOG_INFO("Configured machine to recognize palindromes");
        } else if (languageType == "anbn") {
            // configure to recognize a^n b^n language
            addState(State("count_a", "Count A", "Counting as"));
            addState(State("count_b", "Count B", "Counting bs"));
            addState(State("verify", "Verify", "Verifying counts match"));

            // add transitions for a^n b^n recognition
            // more transitions would be added for a complete implementation

            LOG_INFO("Configured machine to recognize a^n b^n language");
        } else {
            LOG_ERROR(std::format("Unknown language type: {}", languageType));
            throw std::invalid_argument("Unsupported language type");
        }
    }
};

} // namespace turing

#endif // COMPREHENSIVE_TURING_MACHINE_HPP
