# State Design Pattern Implementation in C++

The State pattern is a behavioral design pattern that allows an object to alter its behavior when its internal state
changes. First formalized in the influential "Gang of Four" book published in 1994, this pattern has roots in finite state
machines from computer science theory. The pattern enables objects to appear to change their class at runtime by delegating
state-specific behavior to separate state objects rather than managing all behaviors within a single class. This elegant
solution transforms sprawling conditional logic into a clean, object-oriented structure where each state encapsulates its own
behavior, making the code more maintainable, extensible, and easier to understand.

The State pattern excels at solving several common software design problems. It's particularly valuable in workflow systems
(like document approvals), UI components with multiple modes (editing vs. viewing), network connections with different states
(connected, connecting, disconnected), game character behavior modeling, or any scenario where an object's behavior depends
heavily on its state. By extracting state-specific behavior into dedicated classes, the pattern eliminates complex conditional
statements that check the current state before executing an action. This separation of concerns allows for easier addition of
new states without modifying existing code, embodying the Open/Closed Principle from SOLID design.

## Implementation

This implementation demonstrates the State pattern using a document workflow system with the following states:
- Draft
- Review
- Approved
- Rejected

## Class Structure

```
┌─────────────────────────────────────────────────────────────────────┐
│                                                                     │
│                            Document                                 │
│ (Context)                                                           │
│ ┌─────────────────────────────────────────────────────────────┐     │
│ │ - m_currentState: std::unique_ptr<DocumentState>            │     │
│ │ - m_content: std::string                                    │     │
│ │ - m_author: std::string                                     │     │
│ │ - m_reviewComments: std::vector<std::string>                │     │
│ ├─────────────────────────────────────────────────────────────┤     │
│ │ + draft()                                                   │     │
│ │ + review()                                                  │     │
│ │ + approve()                                                 │     │
│ │ + reject()                                                  │     │
│ │ + changeState(std::unique_ptr<DocumentState>)               │     │
│ └─────────────────────────────────────────────────────────────┘     │
│                             │                                       │
│                             │ has-a                                 │
│                             ▼                                       │
│  ┌───────────────────────────────────────────────────────────┐      │ 
│  │                     <<interface>>                         │      │
│  │                     DocumentState                         │      │
│  ├───────────────────────────────────────────────────────────┤      │
│  │ + draft(Document&)                                        │      │
│  │ + review(Document&)                                       │      │
│  │ + approve(Document&)                                      │      │
│  │ + reject(Document&)                                       │      │
│  │ + getName(): std::string                                  │      │
│  └───────────────────────────────────────────────────────────┘      │
│                             ▲                                       │
│                             │ implements                            │
│                             │                                       │
│       ┌──────────────┬──────┴─────────┬──────────────────┐          │
│       │              │                │                  │          │
│       ▼              ▼                ▼                  ▼          │
│ ┌────────────┐ ┌────────────┐ ┌─────────────────┐ ┌─────────────┐   │
│ │ DraftState │ │ReviewState │ │  ApprovedStat   │ │RejectedState│   │
│ └────────────┘ └────────────┘ └─────────────────┘ └─────────────┘   │
│                                                                     │
└─────────────────────────────────────────────────────────────────────┘
```

## State Transition Diagram

```
┌─────────────┐  review()   ┌─────────────┐ approve() ┌─────────────┐
│             │------------>│             │---------->│             │
│  DraftState │             │ ReviewState │           │ApprovedState│
│             │<------------|             │<----------|             │
└─────────────┘   draft()   └─────────────┘  review() └─────────────┘
      ▲                      /     ▲                         │
      │                     /      │                         │
      │                    /reject()                         │
      │                   /        │                         │
      │          reject()/         │                         │
      │                 /          │                         │
      │                ▼           │                         │
      │         ┌─────────────┐    │                         │
      │         │             │    │                         │
      └---------|RejectedState|<---┘                         │
        draft() │             │<-----------------------------┘
                └─────────────┘    reject()                              
```

## Usage Examples

### Basic Usage

```cpp
// Create a document
Document doc("John Doe");

// Set content
doc.setContent("This is a sample document for testing the State Pattern.");

// Transition through states
doc.review();    // Draft -> Review
doc.approve();   // Review -> Approved
doc.review();    // Approved -> Review
doc.reject();    // Review -> Rejected
doc.draft();     // Rejected -> Draft
```

### Real-World Examples

1. **Order Processing Systems**
    - States: New, Paid, Shipped, Delivered, Cancelled
    - Transitions enforce business rules (can't ship unpaid orders)

2. **Media Player**
    - States: Playing, Paused, Stopped
    - Each state handles playback controls differently

3. **Network Connection**
    - States: Disconnected, Connecting, Connected, ConnectionFailed
    - Transitions handle reconnection attempts and timeouts

## Benefits and Drawbacks

### Advantages

- **Eliminates conditional complexity**: Replaces complex if/else or switch statements with polymorphism
- **Encapsulation**: Each state encapsulates its own behavior
- **Single Responsibility**: Each state class has one reason to change
- **Open/Closed**: New states can be added without modifying existing code
- **Explicit transitions**: State changes are clear and controlled

### Disadvantages

- **Class proliferation**: Creates multiple small classes which can increase complexity
- **Increased memory usage**: Each state object requires memory allocation
- **Potential state explosion**: Systems with many states may become unwieldy
- **Overhead**: May be excessive for simple state management needs

## References and Further Reading

- **Design Patterns: Elements of Reusable Object-Oriented Software** by Erich Gamma, Richard Helm, Ralph Johnson, and John Vlissides (1994)
- **Head First Design Patterns** by Eric Freeman and Elisabeth Robson
- **Refactoring to Patterns** by Joshua Kerievsky
- **Pattern-Oriented Software Architecture** by Frank Buschmann, Regine Meunier, Hans Rohnert, Peter Sommerlad, and Michael Stal

## License

This code is provided under the MIT License. Feel free to use, modify, and distribute as needed.

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.