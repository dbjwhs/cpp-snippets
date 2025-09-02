# C++ Design Patterns Implementation

A comprehensive collection of **26 design patterns** implemented using modern **C++23** features, demonstrating production-ready code with advanced language capabilities, comprehensive testing, and detailed documentation.

## 📚 Pattern Categories

### 🎭 [Behavioral Patterns](./behavioral/)
*Communication between objects and assignment of responsibilities*

| Pattern | Description | Industry Problems Solved | ✅ Pros | ❌ Cons |
|---------|-------------|-------------------------|---------|---------|
| [Chain of Responsibility](./behavioral/chain-of-responsibility/) | Passes requests along a chain of handlers until one handles it | Multi-level approval systems, GUI event handling, middleware processing | Decouples sender/receiver, dynamic chain configuration | Performance overhead, debugging complexity |
| [Command](./behavioral/command/) | Encapsulates requests as objects enabling undo/redo, queuing, logging | Text editors, macro recording, transactional systems, GUI buttons | Undo/redo support, request queuing, logging | Memory overhead, object proliferation |
| [Interpreter](./behavioral/interpreter/) | Defines grammar representation and interpreter for domain languages | SQL parsers, configuration languages, mathematical expressions | Language flexibility, extensible grammar | Performance limitations, complexity for simple cases |
| [Iterator](./behavioral/iterator/) | Provides sequential access to elements without exposing internal structure | Database result sets, collection traversal, stream processing | Uniform access interface, multiple traversal algorithms | Memory overhead for complex iterators, iterator invalidation |
| [Mediator](./behavioral/mediator/) | Defines loose coupling between objects through centralized communication | Chat systems, air traffic control, workflow engines, UI dialogs | Reduced coupling, centralized control logic | Mediator complexity, potential bottleneck |
| [Memento](./behavioral/memento/) | Captures and restores object state without violating encapsulation | Game save states, document version control, transaction rollback | Encapsulation preservation, snapshot management | Memory consumption, state size limitations |
| [Null Object](./behavioral/null-object/) | Provides neutral behavior to eliminate null checks and exceptions | Default configurations, optional services, logging systems | Eliminates null checks, simplified client code | Hidden absence of real functionality, mask actual problems |
| [Observer](./behavioral/observer/) | Establishes one-to-many dependency with automatic state change notification | Model-View architectures, event systems, stock price monitoring | Loose coupling, broadcast communication, dynamic subscriptions | Memory leaks potential, notification storms, debugging difficulty |
| [State](./behavioral/state/) | Allows objects to alter behavior when internal state changes | State machines, game AI, protocol implementations, workflow systems | Clean state transitions, extensible states | State explosion, complex state relationships |
| [Strategy](./behavioral/strategy/) | Defines interchangeable algorithm families | Payment processing, sorting algorithms, compression methods, pricing strategies | Runtime algorithm switching, extensible algorithms | Strategy proliferation, context coupling |
| [Template Method](./behavioral/template-method/) | Defines algorithm skeleton with customizable steps in subclasses | Data processing pipelines, framework extension points, report generation | Code reuse, controlled customization | Inheritance-based coupling, limited flexibility |
| [Visitor](./behavioral/visitor/) | Defines operations on object structures without modifying classes | Compiler AST processing, tax calculation systems, document rendering | New operations without modification, separation of concerns | Difficult to add new element types, circular dependencies |

### 🏭 [Creational Patterns](./creational/)
*Object creation mechanisms and instantiation control*

| Pattern | Description | Industry Problems Solved | ✅ Pros | ❌ Cons |
|---------|-------------|-------------------------|---------|---------|
| [Abstract Factory](./creational/abstract-factory/) | Creates families of related objects without specifying concrete classes | Cross-platform UI components, database adapters, theme systems | Consistent product families, platform independence | Factory proliferation, rigid product families |
| [Builder](./creational/builder/) | Constructs complex objects step by step using fluent interface | Configuration objects, SQL query building, report generation | Complex object construction, fluent API, immutable objects | Builder complexity, verbose for simple objects |
| [Factory](./creational/factory/) | Creates objects without specifying exact classes | Plugin systems, object serialization, dependency injection | Flexible object creation, loose coupling, extensible | Factory method proliferation, parameter complexity |
| [Multiton](./creational/multiton/) | Manages finite set of named instances (evolution of Singleton) | Database connection pools, cache managers, logger instances | Controlled instance management, named access, resource pooling | Memory persistence, key management complexity |
| [Prototype](./creational/prototype/) | Creates objects by cloning existing instances | Document templates, game object spawning, configuration copying | Avoids expensive initialization, dynamic object creation | Deep copy complexity, clone management |
| [Singleton](./creational/singleton/) | Ensures single instance with global access point | Configuration managers, logging systems, device drivers | Global access, controlled instantiation, resource sharing | Global state issues, testing difficulties, hidden dependencies |

### 🏗️ [Structural Patterns](./structural/)
*Object composition and relationships between entities*

| Pattern | Description | Industry Problems Solved | ✅ Pros | ❌ Cons |
|---------|-------------|-------------------------|---------|---------|
| [Adapter](./structural/adapter/) | Allows incompatible interfaces to work together | Legacy system integration, third-party library wrapping, API versioning | Interface compatibility, legacy code reuse | Additional abstraction layer, potential performance impact |
| [Bridge](./structural/bridge/) | Separates abstraction from implementation | Cross-platform development, device drivers, rendering systems | Platform independence, implementation hiding | Complexity increase, abstraction overhead |
| [Composite](./structural/composite/) | Composes objects into tree structures for part-whole hierarchies | GUI component systems, file systems, organizational charts | Uniform treatment, recursive structures | Type safety challenges, performance for deep trees |
| [Decorator](./structural/decorator/) | Adds behavior to objects dynamically without subclassing | I/O streams, middleware components, UI component enhancement | Flexible enhancement, composition over inheritance | Decorator proliferation, complexity increase |
| [Facade](./structural/facade/) | Provides unified interface to subsystem with simplified access | Game engines, multimedia frameworks, enterprise service layers | Simplified interface, subsystem decoupling | Hidden complexity, potential oversimplification |
| [Flyweight](./structural/flyweight/) | Minimizes memory usage by sharing common state efficiently | Game particles, text rendering, icon systems | Memory optimization, shared state management | Complexity increase, state management challenges |
| [Proxy](./structural/proxy/) | Provides placeholder controlling access to expensive objects | Virtual objects, access control, caching, logging | Access control, lazy loading, additional functionality | Indirection complexity, potential performance impact |

### 🏛️ [Architectural Patterns](./architectural/)
*High-level system organization and structure*

| Pattern | Description | Status | Planned Implementation |
|---------|-------------|--------|----------------------|
| [Distributed](./architectural/distributed/) | Microservices, Circuit Breaker, Event Sourcing | 🚧 Planned | Distributed system patterns for scalability and resilience |
| [Integration](./architectural/integration/) | Message Bus, Pub-Sub, API Gateway | 🚧 Planned | System integration and communication patterns |
| [Layered](./architectural/layered/) | MVC, MVP, MVVM, Repository | 🚧 Planned | Architectural layering and separation of concerns |

## 🚀 Modern C++23 Features

This implementation showcases cutting-edge C++ capabilities:

### **Language Features**
- **Concepts & Constraints**: Type-safe template programming with descriptive error messages
- **Ranges & Algorithms**: Modern iteration and functional programming approaches  
- **Smart Pointers**: Automatic memory management with `std::unique_ptr`, `std::shared_ptr`
- **RAII Principles**: Resource management and exception safety throughout

### **Advanced Techniques**
- **Railway-Oriented Programming**: Explicit error handling using `std::expected`
- **Template Metaprogramming**: Compile-time optimizations and type computations
- **Const Correctness**: Extensive use of `constexpr`, `const`, and immutable designs
- **Thread Safety**: Modern synchronization with atomic operations and thread-safe patterns

### **Code Quality Standards**
- **Zero Raw Pointers**: Memory-safe implementations using smart pointers
- **Comprehensive Testing**: CTest integration with extensive unit test coverage
- **Documentation**: Detailed README files and inline code documentation
- **Modern Build System**: CMake with contemporary configuration practices

## 📁 Repository Structure

```
design-patterns/
├── behavioral/           # 13 behavioral patterns
│   ├── chain-of-responsibility/
│   ├── command/
│   ├── interpreter/
│   ├── iterator/
│   ├── mediator/
│   ├── memento/
│   ├── null-object/
│   ├── observer/
│   ├── state/
│   ├── strategy/
│   ├── template-method/
│   └── visitor/
├── creational/           # 6 creational patterns  
│   ├── abstract-factory/
│   ├── builder/
│   ├── factory/
│   ├── multiton/
│   ├── prototype/
│   └── singleton/
├── structural/           # 7 structural patterns
│   ├── adapter/
│   ├── bridge/
│   ├── composite/
│   ├── decorator/
│   ├── facade/
│   ├── flyweight/
│   └── proxy/
└── architectural/        # 3 architectural patterns (planned)
    ├── distributed/
    ├── integration/
    └── layered/
```

Each pattern directory contains:
- **`src/`** - Implementation source files
- **`headers/`** - Header files with class declarations
- **`CMakeLists.txt`** - Build configuration
- **`README.md`** - Pattern-specific documentation with examples

## 🛠️ Building and Testing

### Prerequisites
- **C++23 compliant compiler** (GCC 13+, Clang 16+, MSVC 19.35+)
- **CMake 3.20+**
- **CTest** for running tests

### Quick Start
```bash
# Clone the repository
git clone <repository-url>
cd cpp-snippets/design-patterns

# Build all patterns
mkdir build && cd build
cmake .. -DCMAKE_CXX_STANDARD=23
make -j$(nproc)

# Run all tests
ctest --output-on-failure

# Build specific pattern
cd behavioral/observer
mkdir build && cd build
cmake .. -DCMAKE_CXX_STANDARD=23
make -j$(nproc)
```

## 🎯 Design Pattern Selection Guide

### **When to Use Behavioral Patterns**
- **Observer**: Real-time notifications, event-driven architectures
- **Strategy**: Multiple algorithm implementations, runtime algorithm selection
- **Command**: Undo/redo functionality, macro recording, queued operations
- **State**: Complex state machines, protocol implementations

### **When to Use Creational Patterns**
- **Factory**: Plugin architectures, object creation flexibility
- **Builder**: Complex object construction, fluent APIs
- **Singleton**: Global configuration, resource managers
- **Abstract Factory**: Cross-platform compatibility, product families

### **When to Use Structural Patterns**
- **Adapter**: Legacy system integration, third-party library compatibility
- **Decorator**: Dynamic behavior addition, composition over inheritance
- **Facade**: Complex subsystem simplification, API design
- **Proxy**: Access control, lazy loading, caching

## 📈 Industry Applications

### **Enterprise Software**
- **Observer**: Stock trading systems, real-time dashboards
- **Factory**: Plugin architectures, dependency injection containers
- **Adapter**: Enterprise service bus, legacy system modernization

### **Game Development**
- **State**: Game AI, character behavior systems
- **Flyweight**: Particle systems, tile-based games
- **Command**: Input handling, replay systems

### **System Software**
- **Singleton**: Device drivers, system configuration
- **Proxy**: Virtual memory, network proxies
- **Bridge**: Cross-platform frameworks, driver architectures

## 🔮 Future Enhancements

### **Planned Additions**
- **C++26 Features**: Reflection, pattern matching, coroutines integration
- **Performance Patterns**: Lock-free data structures, memory pools
- **Concurrency Patterns**: Actor model, thread pools, async patterns
- **Security Patterns**: Secure coding practices, authentication patterns

### **Advanced Topics**
- **CRTP (Curiously Recurring Template Pattern)**
- **Policy-Based Design**
- **Expression Templates**
- **Compile-Time Design Patterns**

## 📄 License

This project is licensed under the **MIT License** - see individual pattern directories for specific license files.

## 🤝 Contributing

Contributions are welcome! Please feel free to submit pull requests, report issues, or suggest improvements. See individual pattern READMEs for specific contribution guidelines.

## 📚 References

- **Gang of Four**: Design Patterns: Elements of Reusable Object-Oriented Software
- **Modern C++ Design**: Andrei Alexandrescu
- **Effective Modern C++**: Scott Meyers
- **C++23 Standard**: ISO/IEC 14882:2023

---

*This repository demonstrates production-ready implementations of classic design patterns using modern C++23 features, suitable for both educational purposes and real-world applications.*