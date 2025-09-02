// MIT License
// Copyright (c) 2025 dbjwhs

#ifndef FSM_HPP
#define FSM_HPP

/**
 * Finite State Machine (FSM) Implementation
 *
 * History:
 * Finite State Machines date back to the 1950s when they were first formalized by mathematicians
 * studying computation theory. They were extensively used in language theory and compiler design.
 * In software engineering, FSMs have been a fundamental pattern for modeling systems with discrete states
 * and well-defined transitions between those states. The concept has evolved from simple state tables to
 * sophisticated implementations leveraging modern language features.
 *
 * Common usages of FSMs include:
 * - Protocol implementations (TCP/IP, HTTP)
 * - Game character AI and behavior
 * - User interface flows
 * - Embedded systems control logic
 * - Parser implementations
 * - Workflow engines
 *
 * This implementation leverages C++23 features to create a type-safe, compile-time validated FSM
 * that prevents invalid state transitions and provides clear, maintainable code structure.
 */

#include <concepts>
#include <functional>
#include <optional>
#include <string>
#include <unordered_map>
#include <type_traits>
#include <vector>
#include <format>
#include <stdexcept>
#include "../../../headers/project_utils.hpp"

namespace fsm {

/**
 * concept defining requirements for state types used in the state machine
 * state types must be equality-comparable and provide a name for debugging
 */
template<typename T>
concept State = requires(T a, T b) {
    { a == b } -> std::convertible_to<bool>;
    { a.name() } -> std::convertible_to<std::string>;
};

/**
 * concept defining requirements for event types used in the state machine
 * events must be equality-comparable and provide a name for debugging
 */
template<typename T>
concept Event = requires(T a, T b) {
    { a == b } -> std::convertible_to<bool>;
    { a.name() } -> std::convertible_to<std::string>;
};

/**
 * transition represents a possible state change in the fsm
 * it holds the current state, the triggering event, and the resulting next state
 */
template<State StateType, Event EventType>
class Transition {
public:
    // constructor to initialize a transition
    Transition(const StateType& from, const EventType& event, const StateType& to)
        : m_from{from}, m_event{event}, m_to{to} {
        LOG_INFO(std::format("Created transition: {}", toString()));
    }

    // getter for the source state
    const StateType& from() const { return m_from; }

    // getter for the event
    const EventType& event() const { return m_event; }

    // getter for the destination state
    const StateType& to() const { return m_to; }

    // string representation for debugging
    [[nodiscard]] std::string toString() const {
        return std::format("{} --({})--> {}", m_from.name(), m_event.name(), m_to.name());
    }

private:
    // the source state of this transition
    StateType m_from{};

    // the event that triggers this transition
    EventType m_event{};

    // the destination state after this transition
    StateType m_to{};
};

/**
 * const the main state machine class that manages states and transitions
 * implemented as a template to work with any state and event types that satisfy the concepts
 */
template<State StateType, Event EventType>
class StateMachine {
public:
    // define aliases for improved readability
    // this feature enhances code locality by allowing type aliases to be defined exactly
    // where they're needed, reducing scope pollution and improving code readability
    using TransitionType = Transition<StateType, EventType>;
    using ActionCallback = std::function<void(const StateType&, const EventType&, const StateType&)>;
    
    // GuardCallback represents a conditional check for state transitions
    // It takes the current state and triggering event as parameters and returns a boolean value
    // Returns true if the transition should be allowed to proceed, false to block the transition
    // Guards enable dynamic, context-dependent conditions to be evaluated at runtime
    // rather than having to create additional states to represent all possible conditions
    using GuardCallback = std::function<bool(const StateType&, const EventType&)>;

    // constructor to initialize the state machine with a starting state
    explicit StateMachine(const StateType& initialState)
        : m_currentState{initialState} {
        LOG_INFO(std::format("State machine initialized with state: {}", m_currentState.name()));
    }

    // add a simple transition without guards or actions
    void addTransition(const StateType& from, const EventType& event, const StateType& to) {
        // create a new transition and store it
        TransitionType transition(from, event, to);
        m_transitions.emplace_back(transition);
        LOG_INFO(std::format("Added transition: {}", transition.toString()));
    }

    // add a transition with an action to be executed when the transition occurs
    void addTransition(const StateType& from, const EventType& event, const StateType& to,
                      ActionCallback action) {
        // create a new transition and store it
        TransitionType transition(from, event, to);
        m_transitions.emplace_back(transition);

        // store the action associated with this transition
        m_actions[from][event] = action;
        LOG_INFO(std::format("Added transition with action: {}", transition.toString()));
    }

    // add a transition with a guard condition that must be satisfied for the transition to occur
    // Guard conditions allow for runtime checking of whether a transition should be allowed
    // This is useful for implementing business rules or context-dependent transitions
    // without having to create additional states for every possible condition
    void addTransition(const StateType& from, const EventType& event, const StateType& to,
                      GuardCallback guard) {
        // create a new transition and store it
        TransitionType transition(from, event, to);
        m_transitions.emplace_back(transition);

        // store the guard associated with this transition
        // The guard will be evaluated before allowing the transition during processEvent
        m_guards[from][event] = guard;
        LOG_INFO(std::format("Added transition with guard: {}", transition.toString()));
    }

    // add a transition with both guard and action
    void addTransition(const StateType& from, const EventType& event, const StateType& to,
                      GuardCallback guard, ActionCallback action) {
        // create a new transition and store it
        TransitionType transition(from, event, to);
        m_transitions.emplace_back(transition);

        // store both the guard and action
        m_guards[from][event] = guard;
        m_actions[from][event] = action;
        LOG_INFO(std::format("Added transition with guard and action: {}", transition.toString()));
    }

    // process an event and potentially transition to a new state
    bool processEvent(const EventType& event) {
        LOG_INFO(std::format("Processing event: {} in state: {}", event.name(), m_currentState.name()));

        // search for a valid transition
        for (const auto& transition : m_transitions) {
            if (transition.from() == m_currentState && transition.event() == event) {
                // check if there's a guard and evaluate it
                // Guards act as conditional checks that determine whether a transition should proceed
                // This allows for dynamic conditions beyond just the state and event
                auto guardIt = m_guards.find(m_currentState);
                if (guardIt != m_guards.end()) {
                    auto eventGuardIt = guardIt->second.find(event);
                    if (eventGuardIt != guardIt->second.end()) {
                        // Execute the guard callback to check if the transition is allowed
                        // The guard receives the current state and event and returns a boolean
                        // if guard exists but returns false, reject the transition
                        if (!eventGuardIt->second(m_currentState, event)) {
                            LOG_WARNING(std::format("Transition guard rejected: {}", transition.toString()));
                            return false;
                        }
                    }
                }

                // store the next state
                const StateType nextState = transition.to();

                // execute action if one exists
                auto actionIt = m_actions.find(m_currentState);
                if (actionIt != m_actions.end()) {
                    auto eventActionIt = actionIt->second.find(event);
                    if (eventActionIt != actionIt->second.end()) {
                        eventActionIt->second(m_currentState, event, nextState);
                    }
                }

                // update current state
                m_currentState = nextState;
                LOG_INFO(std::format("Transitioned to state: {}", m_currentState.name()));
                return true;
            }
        }

        // no valid transition found
        LOG_WARNING(std::format("No valid transition for event: {} in state: {}",
                               event.name(), m_currentState.name()));
        return false;
    }

    // get the current state
    const StateType& getCurrentState() const {
        return m_currentState;
    }

    // check if a specific transition is defined
    bool canTransition(const EventType& event) const {
        for (const auto& transition : m_transitions) {
            if (transition.from() == m_currentState && transition.event() == event) {
                // check guard if it exists
                auto guardIt = m_guards.find(m_currentState);
                if (guardIt != m_guards.end()) {
                    auto eventGuardIt = guardIt->second.find(event);
                    if (eventGuardIt != guardIt->second.end()) {
                        return eventGuardIt->second(m_currentState, event);
                    }
                }
                return true;
            }
        }
        return false;
    }

    // get all possible transitions from the current state
    std::vector<TransitionType> getPossibleTransitions() const {
        std::vector<TransitionType> possibleTransitions;
        possibleTransitions.reserve(m_transitions.size()); // reserve space for efficiency

        for (const auto& transition : m_transitions) {
            if (transition.from() == m_currentState) {
                possibleTransitions.emplace_back(transition);
            }
        }

        return possibleTransitions;
    }

private:
    // the current state of the state machine
    StateType m_currentState{};

    // list of all defined transitions
    std::vector<TransitionType> m_transitions{};

    // map of actions to execute on transitions
    // indexed by from-state and then by event
    std::unordered_map<StateType,
                      std::unordered_map<EventType, ActionCallback>> m_actions{};

    // map of guards that control whether transitions are allowed
    // indexed by from-state and then by event
    // Guards are optional conditions that must be satisfied for a transition to occur
    // They allow the state machine to make decisions based on runtime context
    // without requiring additional states for every possible condition
    std::unordered_map<StateType,
                      std::unordered_map<EventType, GuardCallback>> m_guards{};
};

} // namespace fsm

#endif // FSM_HPP
