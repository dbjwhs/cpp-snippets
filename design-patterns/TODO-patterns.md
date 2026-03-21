# Design Patterns TODO

Patterns to add beyond the existing GoF collection. Organized by priority.

## High Priority — C++-Specific Patterns

These patterns are idiomatic to C++ and widely used in production codebases.

### CRTP (Curiously Recurring Template Pattern)
- **Category**: Structural / Metaprogramming
- **Location**: `cpp-specific/crtp/`
- **What to demonstrate**: Static polymorphism, mixin classes, compile-time interface enforcement
- **Industry use**: Boost libraries, STL (`std::enable_shared_from_this`), game engines, embedded systems
- **Why it matters**: Eliminates virtual dispatch overhead while preserving polymorphic behavior. The most "C++" pattern there is — no equivalent in other languages
- **Key concepts**: Base class parameterized by derived class, static dispatch, zero-cost abstractions
- [x] Implemented

### Type Erasure
- **Category**: Structural
- **Location**: `cpp-specific/type-erasure/`
- **What to demonstrate**: How `std::function`, `std::any`, `std::move_only_function` work internally. Hand-rolled type-erased container
- **Industry use**: Plugin systems, callback registries, heterogeneous containers, Sean Parent's "Runtime Polymorphism" talk
- **Why it matters**: Provides polymorphism without inheritance — the modern C++ alternative to virtual base classes. Powers most of the standard library's vocabulary types
- **Key concepts**: SBO (Small Buffer Optimization), concept-based polymorphism, value semantics with polymorphic behavior
- [ ] Implemented

### Policy-Based Design
- **Category**: Structural / Metaprogramming
- **Location**: `cpp-specific/policy-based-design/`
- **What to demonstrate**: Compile-time strategy selection via template parameters, host classes, policy composition
- **Industry use**: STL allocators, smart pointer deleters, Loki library, logging frameworks
- **Why it matters**: Alexandrescu's contribution from *Modern C++ Design*. Compile-time Strategy pattern with zero runtime cost. Used extensively in STL and Boost
- **Key concepts**: Template parameters as behavioral axes, orthogonal policy decomposition, combinatorial flexibility
- [ ] Implemented

### PIMPL (Pointer to Implementation)
- **Category**: Structural
- **Location**: `cpp-specific/pimpl/`
- **What to demonstrate**: Compilation firewall, ABI stability, reducing header dependencies
- **Industry use**: Qt, large-scale C++ projects (Lakos), shared library APIs, embedded systems with limited rebuild capacity
- **Why it matters**: Distinct from Bridge — PIMPL is about build system and ABI concerns, not abstraction hierarchies. Essential for any C++ library with a stable API
- **Key concepts**: Forward declaration, `std::unique_ptr<Impl>`, compilation time reduction, ABI compatibility across releases
- [ ] Implemented

## Medium Priority — Modern Systems Patterns

### Object Pool
- **Category**: Creational
- **Location**: `creational/object-pool/`
- **What to demonstrate**: Pre-allocated object reuse, RAII pool handles, thread-safe checkout/checkin
- **Industry use**: Game engines (particle pools), database connection pools, network buffer pools, memory allocators
- **Why it matters**: Avoids allocation/deallocation overhead on hot paths. Natural companion to Flyweight
- **Key concepts**: Fixed-size pool, free list, scope-guarded handles, thread-safe access
- [ ] Implemented

### Modern Visitor (std::variant + std::visit)
- **Category**: Behavioral
- **Location**: `behavioral/modern-visitor/`
- **What to demonstrate**: `std::variant`, `std::visit`, overload pattern, pattern matching without virtual dispatch
- **Industry use**: Compiler frontends, AST processing, event handling, protocol parsing
- **Why it matters**: Shows the modern C++ alternative to classical Visitor — no double dispatch, no `accept()` boilerplate, closed set of types at compile time. Good contrast to existing Visitor implementation
- **Key concepts**: Sum types, `std::variant`, `std::visit`, overloaded lambda trick, exhaustive matching
- [ ] Implemented

### Monostate
- **Category**: Creational
- **Location**: `creational/monostate/`
- **What to demonstrate**: All instances share state via static members, normal construction/destruction semantics
- **Industry use**: Configuration systems, counters, global registries
- **Why it matters**: Alternative to Singleton that avoids the "one instance" constraint while sharing state. Objects can be passed by value, used in containers, and tested more easily
- **Key concepts**: Static data members, normal object lifetime, testability advantages over Singleton
- [ ] Implemented

### Signal-Slot / Event Bus
- **Category**: Behavioral
- **Location**: `behavioral/signal-slot/`
- **What to demonstrate**: Type-safe event system, connection management, automatic disconnection, thread-safe signals
- **Industry use**: Qt (signal/slot), game engines, GUI frameworks, reactive systems
- **Why it matters**: Evolution beyond Observer — type-safe, supports automatic lifetime management, decoupled event routing without subject/observer coupling
- **Key concepts**: Signal objects, slot connections, scoped connections, multi-threaded delivery
- [ ] Implemented

## Architectural Patterns (Planned Section — Already Has Directories)

Fill in the existing `architectural/` placeholders:

### Circuit Breaker
- **Location**: `architectural/distributed/circuit-breaker/`
- **What to demonstrate**: Closed/Open/Half-Open states, failure counting, timeout recovery, `std::chrono` integration
- **Industry use**: Microservices, API clients, database connections, network resilience
- **Why it matters**: Prevents cascading failures in distributed systems. Good showcase of State pattern applied to infrastructure
- **Key concepts**: State machine, failure threshold, recovery timeout, fallback behavior
- [ ] Implemented

### Repository
- **Location**: `architectural/layered/repository/`
- **What to demonstrate**: Data access abstraction, in-memory vs persistent backends, unit of work
- **Industry use**: Data-driven applications, clean architecture, testable data layers
- **Why it matters**: Clean separation between domain logic and data access. Makes testing trivial with in-memory implementations
- **Key concepts**: Interface-based data access, backend swapping, query abstraction
- [ ] Implemented

### Event Sourcing + CQRS
- **Location**: `architectural/distributed/event-sourcing/`
- **What to demonstrate**: Event store, event replay, command/query separation, aggregate reconstruction
- **Industry use**: Financial systems, audit trails, collaborative editing, distributed systems
- **Why it matters**: Pairs naturally with the existing Command pattern. Provides full audit history and temporal queries
- **Key concepts**: Event store, aggregate root, projections, command handlers, query handlers
- [ ] Implemented
