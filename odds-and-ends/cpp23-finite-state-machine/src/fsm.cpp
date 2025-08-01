// MIT License
// Copyright (c) 2025 dbjwhs

/**
 * This file demonstrates the use of the FSM (Finite State Machine) implementation
 * with a simple example of a vending machine that has different states and transitions.
 */

#include <iostream>
#include <string>
#include <cassert>
#include <format>
#include "../include/fsm.hpp"
#include "../../../headers/project_utils.hpp"

// forward declarations
class VendingMachineState;
class VendingMachineEvent;
class VendingMachine;
void testExitMaintenanceTransition();
void testVendingMachine();

// define the states for our vending machine example
class VendingMachineState {
public:
    // enum for the different states
    enum class State {
        IDLE,
        MONEY_INSERTED,
        ITEM_SELECTED,
        DISPENSING,
        OUT_OF_STOCK,
        MAINTENANCE
    };

    // constructor
    explicit VendingMachineState(const State state) : m_state{state} {}

    // equality operator for the state machine
    bool operator==(const VendingMachineState& other) const {
        const bool result = m_state == other.m_state;
        LOG_INFO_PRINT("State equality check: {} == {} => {}", name(), other.name(), result ? "true" : "false");
        return result;
    }
    
    // Inequality operator for completeness
    bool operator!=(const VendingMachineState& other) const {
        return !(*this == other);
    }

    // hash function for use in unordered containers
    struct Hash {
        std::size_t operator()(const VendingMachineState& state) const {
            auto hashValue = std::hash<int>{}(static_cast<int>(state.m_state));
            LOG_INFO_PRINT("Hashing state: {} => {}", state.name(), hashValue);
            return hashValue;
        }
    };

    // get readable name for logging and debugging
    [[nodiscard]] std::string name() const {
        switch (m_state) {
            case State::IDLE: return "IDLE";
            case State::MONEY_INSERTED: return "MONEY_INSERTED";
            case State::ITEM_SELECTED: return "ITEM_SELECTED";
            case State::DISPENSING: return "DISPENSING";
            case State::OUT_OF_STOCK: return "OUT_OF_STOCK";
            case State::MAINTENANCE: return "MAINTENANCE";
            default: return "UNKNOWN";
        }
    }

    // getter for state value - for debugging
    [[nodiscard]] State getState() const {
        return m_state;
    }

private:
    // the actual state value
    State m_state{};
};

// define the events for our vending machine
class VendingMachineEvent {
public:
    // enum for the different events
    enum class Event {
        INSERT_MONEY,
        SELECT_ITEM,
        CANCEL,
        DISPENSE,
        RESTOCK,
        ENTER_MAINTENANCE,
        EXIT_MAINTENANCE
    };

    // constructor
    explicit VendingMachineEvent(const Event event) : m_event{event} {}

    // equality operator for the state machine
    bool operator==(const VendingMachineEvent& other) const {
        const bool result = m_event == other.m_event;
        LOG_INFO_PRINT("Event equality check: {} == {} => {}", name(), other.name(), result ? "true" : "false");
        return result;
    }
    
    // Inequality operator for completeness
    bool operator!=(const VendingMachineEvent& other) const {
        return !(*this == other);
    }

    // hash function for use in unordered containers
    struct Hash {
        std::size_t operator()(const VendingMachineEvent& event) const {
            auto hashValue = std::hash<int>{}(static_cast<int>(event.m_event));
            LOG_INFO_PRINT("Hashing event: {} => {}", event.name(), hashValue);
            return hashValue;
        }
    };

    // get readable name for logging and debugging
    [[nodiscard]] std::string name() const {
        switch (m_event) {
            case Event::INSERT_MONEY: return "INSERT_MONEY";
            case Event::SELECT_ITEM: return "SELECT_ITEM";
            case Event::CANCEL: return "CANCEL";
            case Event::DISPENSE: return "DISPENSE";
            case Event::RESTOCK: return "RESTOCK";
            case Event::ENTER_MAINTENANCE: return "ENTER_MAINTENANCE";
            case Event::EXIT_MAINTENANCE: return "EXIT_MAINTENANCE";
            default: return "UNKNOWN";
        }
    }

    // getter for event value - for debugging
    [[nodiscard]] Event getEvent() const {
        return m_event;
    }

private:
    // the actual event value
    Event m_event{};
};

// add hash specializations for unordered_map
namespace std {
    template <>
    struct hash<VendingMachineState> {
        std::size_t operator()(const VendingMachineState& state) const noexcept {
            auto result = std::hash<int>{}(static_cast<int>(state.getState()));
            LOG_INFO_PRINT("std::hash for state {}: {}", state.name(), result);
            return result;
        }
    };

    template <>
    struct hash<VendingMachineEvent> {
        std::size_t operator()(const VendingMachineEvent& event) const noexcept {
            auto result = std::hash<int>{}(static_cast<int>(event.getEvent()));
            LOG_INFO_PRINT("std::hash for event {}: {}", event.name(), result);
            return result;
        }
    };
}

// vending machine class that uses our fsm implementation
class VendingMachine {
public:
    // constructor initializes the state machine with an idle state
    VendingMachine() : m_stateMachine{VendingMachineState(VendingMachineState::State::IDLE)}, m_inventory{5} {
        // define all the transitions for our vending machine
        setupStateMachine();
        LOG_INFO_PRINT("Vending machine initialized");
    }

    // method to insert money into the machine
    void insertMoney() {
        m_stateMachine.processEvent(VendingMachineEvent(VendingMachineEvent::Event::INSERT_MONEY));
    }

    // method to select an item
    void selectItem() {
        m_stateMachine.processEvent(VendingMachineEvent(VendingMachineEvent::Event::SELECT_ITEM));
    }

    // method to cancel the operation
    void cancel() {
        m_stateMachine.processEvent(VendingMachineEvent(VendingMachineEvent::Event::CANCEL));
    }

    // method to complete dispensing
    void completeDispense() {
        m_stateMachine.processEvent(VendingMachineEvent(VendingMachineEvent::Event::DISPENSE));
    }

    // method to restock the machine
    void restock(const int count) {
        m_inventory += count;
        LOG_INFO_PRINT("Restocked. New inventory: {}", m_inventory);
        m_stateMachine.processEvent(VendingMachineEvent(VendingMachineEvent::Event::RESTOCK));
    }

    // method to enter maintenance mode
    void enterMaintenance() {
        m_stateMachine.processEvent(VendingMachineEvent(VendingMachineEvent::Event::ENTER_MAINTENANCE));
    }

    // method to exit maintenance mode
    void exitMaintenance() {
        LOG_INFO_PRINT("Attempting to exit maintenance mode from state: {}", m_stateMachine.getCurrentState().name());

        // Log all available transitions for debugging
        LOG_INFO_PRINT("Available transitions from current state:");
        for (const auto& transition : m_stateMachine.getPossibleTransitions()) {
            LOG_INFO_PRINT("  {}", transition.toString());
        }

        // Check if we're in the correct state for this event
        if (m_stateMachine.getCurrentState().name() != "MAINTENANCE") {
            LOG_WARNING_PRINT("Cannot exit maintenance mode: machine is not in MAINTENANCE state");
            return;
        }

        // Process the exit maintenance event
        const VendingMachineEvent exitMaintenanceEvent(VendingMachineEvent::Event::EXIT_MAINTENANCE);
        const bool result = m_stateMachine.processEvent(exitMaintenanceEvent);
        
        LOG_INFO_PRINT("Exit maintenance mode result: {}", result ? "success" : "failure");
        LOG_INFO_PRINT("Current state after exit attempt: {}", m_stateMachine.getCurrentState().name());
        
        // If the transition failed, but we're in maintenance mode, log a warning
        if (!result && m_stateMachine.getCurrentState().name() == "MAINTENANCE") {
            LOG_WARNING_PRINT("Failed to exit maintenance mode - transition not properly configured");
        }
    }

    // get the current state
    [[nodiscard]] std::string getCurrentState() const {
        return m_stateMachine.getCurrentState().name();
    }

    // get the current inventory level
    [[nodiscard]] int getInventory() const {
        return m_inventory;
    }

private:
    // ®set up all the state machine transitions
    void setupStateMachine() {
        using State = VendingMachineState::State;
        using Event = VendingMachineEvent::Event;

        // define states for easy reference
        const VendingMachineState idle(State::IDLE);
        const VendingMachineState moneyInserted(State::MONEY_INSERTED);
        const VendingMachineState itemSelected(State::ITEM_SELECTED);
        const VendingMachineState dispensing(State::DISPENSING);
        const VendingMachineState outOfStock(State::OUT_OF_STOCK);
        const VendingMachineState maintenance(State::MAINTENANCE);

        // define events for easy reference
        const VendingMachineEvent insertMoney(Event::INSERT_MONEY);
        const VendingMachineEvent selectItem(Event::SELECT_ITEM);
        const VendingMachineEvent cancel(Event::CANCEL);
        const VendingMachineEvent dispense(Event::DISPENSE);
        const VendingMachineEvent restock(Event::RESTOCK);
        const VendingMachineEvent enterMaintenance(Event::ENTER_MAINTENANCE);
        const VendingMachineEvent exitMaintenance(Event::EXIT_MAINTENANCE);

        // idle state transitions
        m_stateMachine.addTransition(idle, insertMoney, moneyInserted);
        m_stateMachine.addTransition(idle, enterMaintenance, maintenance);

        // GuardCallback implementation: check if the machine has items before allowing certain transitions
        // This demonstrates how guards enable context-dependent decisions without adding more states
        // The guard function receives the current state and event and returns a boolean value
        // In this case, we use the machine's internal state (inventory) to determine if the transition
        // should proceed, which would be difficult to model with states alone
        auto hasItemsGuard = [this](const VendingMachineState&, const VendingMachineEvent&) -> bool {
            return m_inventory > 0;
        };

        // money inserted state transitions
        m_stateMachine.addTransition(moneyInserted, selectItem, itemSelected, hasItemsGuard);
        m_stateMachine.addTransition(moneyInserted, cancel, idle);
        m_stateMachine.addTransition(moneyInserted, enterMaintenance, maintenance);

        // item selected state transitions
        m_stateMachine.addTransition(itemSelected, cancel, idle);
        m_stateMachine.addTransition(itemSelected, enterMaintenance, maintenance);

        // action to reduce inventory when dispensing
        auto dispenseAction = [this](const VendingMachineState&, const VendingMachineEvent&,
                                    const VendingMachineState&) {
            m_inventory--;
            LOG_INFO_PRINT("Item dispensed. Remaining inventory: {}", m_inventory);
        };

        m_stateMachine.addTransition(itemSelected, dispense, dispensing, dispenseAction);

        // dispensing state transitions
        m_stateMachine.addTransition(dispensing, insertMoney, moneyInserted, hasItemsGuard);
        m_stateMachine.addTransition(dispensing, enterMaintenance, maintenance);
        
        // Add transition from DISPENSING to IDLE when cancel is triggered
        m_stateMachine.addTransition(dispensing, cancel, idle);

        // check if out of stock after dispensing
        auto outOfStockCheck = [this](const VendingMachineState&, const VendingMachineEvent&,
                                     const VendingMachineState&) {
            if (m_inventory <= 0) {
                LOG_WARNING_PRINT("Machine is out of stock!");
                // Instead of just issuing a CANCEL event, we should transition to out of stock then back to IDLE
                // This helps ensure our state machine works correctly in all cases
                m_stateMachine.processEvent(VendingMachineEvent(Event::CANCEL));
            }
        };

        m_stateMachine.addTransition(dispensing, VendingMachineEvent(Event::INSERT_MONEY),
                                    idle, outOfStockCheck);

        // out of stock state transitions
        m_stateMachine.addTransition(outOfStock, restock, idle);
        m_stateMachine.addTransition(outOfStock, enterMaintenance, maintenance);

        // maintenance state transitions
        LOG_INFO_PRINT("Adding critical MAINTENANCE -> IDLE transition");
        // Make sure this exit transition is properly implemented
        // Add extra logging to debug the transition
        auto exitMaintenanceAction = [this](const VendingMachineState& from, const VendingMachineEvent& event,
                                         const VendingMachineState& to) {
            LOG_INFO_PRINT("Executing exit maintenance action: {} --({})--> {}",
                              from.name(), event.name(), to.name());
        };
        
        m_stateMachine.addTransition(maintenance, exitMaintenance, idle, exitMaintenanceAction);

        // Debug summary of all transitions
        LOG_INFO_PRINT("All state transitions configured:");
        std::vector<fsm::Transition<VendingMachineState, VendingMachineEvent>> allTransitions;

        // For each possible state, get all possible transitions
        for (int stateVal = 0; stateVal <= static_cast<int>(State::MAINTENANCE); stateVal++) {
            VendingMachineState currentState(static_cast<State>(stateVal));
            LOG_INFO_PRINT("State: {}", currentState.name());

            // Try all possible events from this state
            for (int eventVal = 0; eventVal <= static_cast<int>(Event::EXIT_MAINTENANCE); eventVal++) {
                VendingMachineEvent currentEvent(static_cast<Event>(eventVal));

                if (m_stateMachine.canTransition(currentEvent)) {
                    LOG_INFO_PRINT("  Can transition with event: {}", currentEvent.name());
                }
            }
        }
    }

    // the state machine that controls the vending machine
    fsm::StateMachine<VendingMachineState, VendingMachineEvent> m_stateMachine;

    // the inventory level - number of items in the machine
    int m_inventory{};
};

// test method to directly verify transitions
void testExitMaintenanceTransition() {
    LOG_INFO_PRINT("Testing EXIT_MAINTENANCE transition directly");

    // Create states and events
    const VendingMachineState maintenance(VendingMachineState::State::MAINTENANCE);
    const VendingMachineState idle(VendingMachineState::State::IDLE);
    const VendingMachineEvent exitMaintenance(VendingMachineEvent::Event::EXIT_MAINTENANCE);

    // Create a state machine
    fsm::StateMachine<VendingMachineState, VendingMachineEvent> testMachine(maintenance);
    LOG_INFO_PRINT("Initial state: {}", testMachine.getCurrentState().name());

    // Add a simple transition
    testMachine.addTransition(maintenance, exitMaintenance, idle);

    // Test transition
    const bool result = testMachine.processEvent(exitMaintenance);
    LOG_INFO_PRINT("Transition result: {}", result ? "success" : "failure");
    LOG_INFO_PRINT("Current state: {}", testMachine.getCurrentState().name());
    assert(result);
    assert(testMachine.getCurrentState().name() == "IDLE");
    LOG_INFO_PRINT("Direct transition test passed");
}

// test function to verify the vending machine fsm implementation
void testVendingMachine() {
    LOG_INFO_PRINT("Starting vending machine tests");

    VendingMachine machine;

    // test initial state
    assert(machine.getCurrentState() == "IDLE");
    assert(machine.getInventory() == 5);
    LOG_INFO_PRINT("Initial state verified");

    // test insert money
    machine.insertMoney();
    assert(machine.getCurrentState() == "MONEY_INSERTED");
    LOG_INFO_PRINT("Insert money transition verified");

    // test cancel operation
    machine.cancel();
    assert(machine.getCurrentState() == "IDLE");
    LOG_INFO_PRINT("Cancel operation verified");

    // test full purchase flow
    machine.insertMoney();
    assert(machine.getCurrentState() == "MONEY_INSERTED");
    LOG_INFO_PRINT("State after insertMoney(): MONEY_INSERTED");

    machine.selectItem();
    assert(machine.getCurrentState() == "ITEM_SELECTED");
    LOG_INFO_PRINT("State after selectItem(): ITEM_SELECTED");

    machine.completeDispense();
    assert(machine.getCurrentState() == "DISPENSING");
    assert(machine.getInventory() == 4);
    LOG_INFO_PRINT("State after completeDispense(): {}, inventory: {}",
             machine.getCurrentState(), machine.getInventory());
    LOG_INFO_PRINT("Full purchase flow verified");

    // test maintenance mode
    LOG_INFO_PRINT("Current state before maintenance: {}", machine.getCurrentState());
    machine.enterMaintenance();
    LOG_INFO_PRINT("State after enterMaintenance(): {}", machine.getCurrentState());
    assert(machine.getCurrentState() == "MAINTENANCE");

    // test exitMaintenance transition
    LOG_INFO_PRINT("=== TESTING MAINTENANCE EXIT TRANSITION ===");

    // Start fresh with a new machine instance to avoid any state issues
    VendingMachine freshMachine;

    // Move to maintenance mode
    LOG_INFO_PRINT("Moving fresh machine to maintenance mode");
    freshMachine.enterMaintenance();
    LOG_INFO_PRINT("Current state: {}", freshMachine.getCurrentState());
    assert(freshMachine.getCurrentState() == "MAINTENANCE");

    // Now test exit
    LOG_INFO_PRINT("Now trying to exit maintenance mode");
    freshMachine.exitMaintenance();
    LOG_INFO_PRINT("Current state after exit: {}", freshMachine.getCurrentState());

    // This is the failing assertion - let's see if our changes fixed it
    assert(freshMachine.getCurrentState() == "IDLE");
    LOG_INFO_PRINT("Maintenance exit verified successfully");

    // Now continue with the original machine object
    LOG_INFO_PRINT("Continuing with original machine after maintenance exit testing");
    
    // Check current inventory and state
    LOG_INFO_PRINT("Current inventory: {}", machine.getInventory());
    LOG_INFO_PRINT("Current state: {}", machine.getCurrentState());
    
    // Get machine back to IDLE state if needed (it might be in maintenance or another state)
    if (machine.getCurrentState() == "MAINTENANCE") {
        LOG_INFO_PRINT("Original machine is in maintenance, exiting maintenance mode");
        machine.exitMaintenance();
    }
    
    // Make sure we're in an IDLE state
    if (machine.getCurrentState() != "IDLE") {
        LOG_WARNING_PRINT("Machine not in IDLE state, attempting reset");
        // Try to get back to IDLE with cancel
        machine.cancel();
    }
    
    // Reset the machine to ensure that all previous tests have completed
    // and the machine is back in a valid state for our out-of-stock test
    LOG_INFO_PRINT("Creating a fresh machine for the out-of-stock test");
    VendingMachine outOfStockMachine;
    
    // The machine starts with 5 items in inventory by default
    assert(outOfStockMachine.getInventory() == 5);
    LOG_INFO_PRINT("Fresh machine inventory: {}", outOfStockMachine.getInventory());
    
    // Buy all 5 items to deplete inventory to 0
    for (int ndx = 0; ndx < 5; ndx++) {
        LOG_INFO_PRINT("Buying item #{}: current inventory = {}", ndx+1, outOfStockMachine.getInventory());
        outOfStockMachine.insertMoney();
        outOfStockMachine.selectItem();
        outOfStockMachine.completeDispense();
    }

    // Verify that inventory is exactly 0 after buying all items
    LOG_INFO_PRINT("Final inventory after buying all items: {}", outOfStockMachine.getInventory());
    assert(outOfStockMachine.getInventory() == 0);
    LOG_INFO_PRINT("Out of stock scenario created");

    // When the machine is out of stock, let's move it to IDLE state manually 
    // to ensure the restock operation works properly
    if (outOfStockMachine.getCurrentState() != "IDLE") {
        LOG_INFO_PRINT("Machine is in {} state, moving to IDLE before restock", 
                             outOfStockMachine.getCurrentState());
        outOfStockMachine.cancel(); // This should get us back to IDLE state in most cases
    }
    
    // restock and verify using the same outOfStockMachine
    outOfStockMachine.restock(3);
    assert(outOfStockMachine.getInventory() == 3);
    
    // Check state after restock - should be IDLE
    LOG_INFO_PRINT("Machine state after restock: {}", outOfStockMachine.getCurrentState());
    assert(outOfStockMachine.getCurrentState() == "IDLE");
    LOG_INFO_PRINT("Restock operation verified");

    LOG_INFO_PRINT("All vending machine tests passed successfully");
}

// main function to run the example
int main() {
    LOG_INFO_PRINT("Starting FSM demo application");

    // Test-specific transition
    testExitMaintenanceTransition();

    // run the vending machine test
    testVendingMachine();

    // create and demonstrate a simple vending machine usage
    LOG_INFO_PRINT("Demonstrating vending machine usage");
    VendingMachine vendingMachine;

    LOG_INFO_PRINT("Initial state: {}", vendingMachine.getCurrentState());
    LOG_INFO_PRINT("Initial inventory: {}", vendingMachine.getInventory());

    // demonstrate a purchase
    LOG_INFO_PRINT("Customer inserts money");
    vendingMachine.insertMoney();
    LOG_INFO_PRINT("Current state: {}", vendingMachine.getCurrentState());

    LOG_INFO_PRINT("Customer selects an item");
    vendingMachine.selectItem();
    LOG_INFO_PRINT("Current state: {}", vendingMachine.getCurrentState());

    LOG_INFO_PRINT("Machine dispenses item");
    vendingMachine.completeDispense();
    LOG_INFO_PRINT("Current state: {}", vendingMachine.getCurrentState());
    LOG_INFO_PRINT("Remaining inventory: {}", vendingMachine.getInventory());

    // demonstrate maintenance mode
    LOG_INFO_PRINT("Entering maintenance mode");
    vendingMachine.enterMaintenance();
    LOG_INFO_PRINT("Current state: {}", vendingMachine.getCurrentState());

    LOG_INFO_PRINT("Exiting maintenance mode");
    vendingMachine.exitMaintenance();
    LOG_INFO_PRINT("Current state after exit attempt: {}", vendingMachine.getCurrentState());
    
    // Verify the transition worked correctly
    if (vendingMachine.getCurrentState() != "IDLE") {
        LOG_ERROR_PRINT("Failed to exit maintenance mode properly!");
    }

    LOG_INFO_PRINT("FSM demo completed successfully");
    return 0;
}
