# Design Patterns TODO List

## Missing GoF Patterns to Add

### Behavioral Patterns (to add to `behavioral/`)
- [X] **Null Object** - Provides a default object to avoid null references
- [ ] **Specification** - Business rule pattern for complex validation logic

### Creational Patterns (to add to `creational/`)
- [ ] **Object Pool** - Manages reusable objects to improve performance
- [ ] **Dependency Injection** - Provides dependencies from external sources
- [ ] **Lazy Initialization** - Delays object creation until needed
- [ ] **Multiton** - Variant of Singleton that manages named instances

### Structural Patterns (to add to `structural/`)
- [ ] **Module** - Groups related functionality together
- [ ] **Twin** - Simulates multiple inheritance in single-inheritance languages
- [ ] **Extension Object** - Adds functionality to objects without modifying their structure

### Architectural Patterns (to add to `architectural/`)

#### Concurrency Patterns (to add to `architectural/concurrency/`)
- [ ] **Reactor** - Synchronous event demultiplexer (complement to existing Proactor)
- [ ] **Thread Pool** - Manages worker threads for task execution
- [ ] **Producer-Consumer** - Decouples data producers from consumers
- [ ] **Read-Write Lock** - Allows concurrent reads with exclusive writes
- [ ] **Actor Model** - Message-passing concurrency pattern
- [ ] **Future/Promise** - Asynchronous computation result handling
- [ ] **Lock-Free Data Structures** - Thread-safe structures without locks
- [ ] **Monitor Object** - Synchronizes access to object state
- [ ] **Active Object** - Decouples method execution from invocation
- [ ] **Half-Sync/Half-Async** - Combines sync and async processing layers

#### Distributed System Patterns (to add to `architectural/distributed/`)
- [ ] **Microservices** - Decompose applications into small services
- [ ] **API Gateway** - Single entry point for client requests
- [ ] **Circuit Breaker** - Prevents cascading failures in distributed systems
- [ ] **Bulkhead** - Isolates critical resources
- [ ] **Saga** - Manages distributed transactions
- [ ] **Event Sourcing** - Store state changes as events
- [ ] **CQRS** (Command Query Responsibility Segregation) - Separate read/write models
- [ ] **Service Mesh** - Infrastructure layer for service communication
- [ ] **Sidecar** - Deploy helper functionality alongside main service
- [ ] **Ambassador** - Proxy for external service communication

#### Layered Architecture Patterns (to add to `architectural/layered/`)
- [ ] **MVC** (Model-View-Controller) - Separates application concerns
- [ ] **MVP** (Model-View-Presenter) - Variant of MVC with passive view
- [ ] **MVVM** (Model-View-ViewModel) - Data binding variant of MVC
- [ ] **Repository** - Encapsulates data access logic
- [ ] **Unit of Work** - Maintains list of objects affected by business transaction
- [ ] **Service Layer** - Defines application boundary and service operations
- [ ] **Domain Model** - Object model of domain incorporating behavior and data
- [ ] **Data Mapper** - Layer that moves data between objects and database
- [ ] **Active Record** - Object that wraps database row and encapsulates access

#### Enterprise Integration Patterns (to add to `architectural/integration/`)
- [ ] **Message Bus** - Enables decoupled communication between components
- [ ] **Publish-Subscribe** - Asynchronous message delivery pattern
- [ ] **Request-Reply** - Synchronous message exchange
- [ ] **Message Queue** - Asynchronous point-to-point communication
- [ ] **Scatter-Gather** - Send message to multiple recipients and collect responses
- [ ] **Content-Based Router** - Routes messages based on content
- [ ] **Message Translator** - Converts between different message formats

## Modern C++ Patterns and Idioms

### C++ Specific Patterns (create `cpp-idioms/` folder)
- [ ] **RAII** (Resource Acquisition Is Initialization) - Automatic resource management
- [ ] **CRTP** (Curiously Recurring Template Pattern) - Static polymorphism
- [ ] **Policy-Based Design** - Compile-time strategy pattern
- [ ] **Tag Dispatching** - Function overload resolution technique
- [ ] **SFINAE** (Substitution Failure Is Not An Error) - Template metaprogramming
- [ ] **Type Erasure** - Hide type information while preserving interface
- [ ] **Expression Templates** - Optimize mathematical expressions
- [ ] **Mixin** - Add functionality through multiple inheritance
- [ ] **Barton-Nackman Trick** - Enable ADL for operators
- [ ] **Copy-and-Swap** - Exception-safe assignment operator

### Modern C++ Features (create `modern-cpp/` folder)
- [ ] **Concepts** (C++20) - Template constraints
- [ ] **Coroutines** (C++20) - Asynchronous programming
- [ ] **Ranges** (C++20) - Functional programming constructs
- [ ] **Modules** (C++20) - Modern modularization
- [ ] **Reflection** (C++26) - Runtime type information
- [ ] **Move Semantics** - Efficient resource transfer
- [ ] **Perfect Forwarding** - Template argument forwarding
- [ ] **Variadic Templates** - Variable argument templates
- [ ] **Constexpr** - Compile-time computation
- [ ] **Lambda Expressions** - Anonymous function objects

## Memory Management Patterns (create `memory-management/` folder)
- [ ] **Smart Pointers** - Automatic memory management
- [ ] **Memory Pool** - Custom memory allocation strategy
- [ ] **Garbage Collection Patterns** - Automatic memory reclamation
- [ ] **Reference Counting** - Shared ownership tracking
- [ ] **Weak References** - Non-owning references
- [ ] **Custom Allocators** - Specialized memory allocation

## Performance Patterns (create `performance/` folder)
- [ ] **Copy Elision** - Avoid unnecessary copies
- [ ] **Small String Optimization** - Inline small strings
- [ ] **Flyweight with Intrinsic State** - Advanced flyweight optimization
- [ ] **Template Specialization** - Optimize for specific types
- [ ] **Branch Prediction Optimization** - CPU-friendly code patterns
- [ ] **Cache-Friendly Data Structures** - Memory access optimization
- [ ] **SIMD Patterns** - Single Instruction Multiple Data

## Security Patterns (create `security/` folder)
- [ ] **Secure by Default** - Default configurations are secure
- [ ] **Principle of Least Privilege** - Minimal access rights
- [ ] **Input Validation** - Sanitize external inputs
- [ ] **Authentication** - Identity verification patterns
- [ ] **Authorization** - Access control patterns
- [ ] **Cryptographic Patterns** - Secure data handling
- [ ] **Audit Trail** - Security event logging

## Testing Patterns (create `testing/` folder)
- [ ] **Mock Object** - Test doubles for dependencies
- [ ] **Test Fixture** - Consistent test environment setup
- [ ] **Test Data Builder** - Flexible test data creation
- [ ] **Page Object Model** - UI testing abstraction
- [ ] **Arrange-Act-Assert** - Test structure pattern
- [ ] **Given-When-Then** - BDD test structure

## Recommended Folder Structure

```
design-patterns/
├── architectural/
│   ├── concurrency/           # Existing + new concurrency patterns
│   ├── distributed/           # New: Distributed system patterns
│   ├── layered/              # New: Layered architecture patterns
│   └── integration/          # New: Enterprise integration patterns
├── behavioral/               # Existing GoF behavioral patterns only
├── creational/              # Existing + missing GoF creational patterns
├── structural/              # Existing GoF structural patterns only
├── cpp-idioms/              # New: C++-specific idioms (move Pimpl here)
├── modern-cpp/              # New: Modern C++ features and patterns
├── memory-management/       # New: Memory management patterns
├── performance/             # New: Performance optimization patterns
├── security/                # New: Security-related patterns
├── testing/                 # New: Testing patterns
└── programming-techniques/  # New: General techniques (move Fail-Fast, FSM here)
```

## Priority Implementation Order

### High Priority (Core GoF patterns missing)
1. Null Object (behavioral)
2. Object Pool (creational)
3. Reactor (architectural/concurrency) - complement to Proactor

### Medium Priority (Important modern patterns)
1. RAII (cpp-idioms)
2. Smart Pointers (memory-management)
3. Thread Pool (architectural/concurrency)
4. Circuit Breaker (architectural/distributed)

### Low Priority (Advanced/Specialized patterns)
1. CRTP (cpp-idioms)
2. Coroutines (modern-cpp)
3. Actor Model (architectural/concurrency)

## Notes
- Each pattern should include comprehensive documentation similar to existing patterns
- Include practical C++ examples with modern C++ features (C++17/20/23)
- Provide CMake build configuration for each pattern
- Include unit tests where applicable
- Add performance benchmarks for performance-critical patterns

Targeting C++23 I like to create a basic example of the pattern I just picked. I want this to be a deep dive. It should also have comprehensive testing via output and asserts. All key points should have robust and detailed comments, all comments on their own line above the code or variable they are describing, all class variables names to start with m_, and all C++ comments using // with single line comments all lower case and block comments standard case. I have my own logging class Logger that logs in the following manner, so please include this when coding for output.

Please move all cpp files into a src directory and all hpp files into a headers directory.

Please move the main.cpp files into the src directory.

Feel free to overwrite the main.cpp file and CMakeList.txt files in this project they are just starting stub and their code is not needed. If need delete the stock main.cpp file.

When it makes sense use ranged based for loops, all other for loop should use ndx for loop value e.g. for (int ndx == 0; etc

When possible be strict with const correctness e.g. use constexpr when it makes sense

For any C++ container being added to in a loop if the container size is known ahead of time use reserve and emplace_back

All member variables initialized using braced initialization {}

Please use my logger class for all logging the path to my logger class is cpp-snippets/headers/project_utils.hpp
Log info using the following
LOG_INFO_PRINT("logging string here");
LOG_INFO_PRINT("logging string here with format {}", 5);

Log warnings using the following
LOG_WARNING_PRINT("logging string here");
LOG_WARNING_PRINT("logging string here with format {}", 5);

Log errors using the following
LOG_ERROR_PRINT("logging string here");
LOG_ERROR_PRINT("logging string here with format {}", 5);

If you have a negative test and will print to std::error but is a successful test run please use my RAII class Logger::StderrSuppressionGuard to suppress std::error output for that specific test logging.

Also create a c++ // header comment right under the #define's that give a detailed history of this pattern, how it is created, what it does. along with common usages

I do not want exceptions for errors, please use std::expected and follow the Railway-Oriented Programming pattern

please no same line if statements, e.g. I do not want code like this...
if (ndx < 3) std::print(", ");

I want this instead
if (ndx < 3) {
	std::print(", ");
}

In all example code in readmes please use  std::print not std::cout.

Make sure all files have a blank newline at the end of the file.

Fix all clang errors and warning to ensure file is c++ correct

Please use CTest to create and integrated comprehensive test suite for this code.

Please ignore the cmake-build-debug folder this is for my IDE and not for Claude.

Fix all build errors or warnings.

Please use Modern C++ Template Parameter Naming (2025): The most cutting-edge approach combines concept-constrained parameters with descriptive names, such as

template<std::copyable ElementType>
  instead of the traditional template<typename T>.

This modern pattern emerged from C++20 concepts and represents current best practice because it provides self-documenting interfaces, significantly better compiler error messages, enables aggressive optimizations, and makes code intent crystal clear - major codebases like Google's, Microsoft's, and LLVM have adopted this approach as of 2023-2024, making concept-constrained descriptive naming the gold standard for new C++ code in 2025, though traditional T naming remains acceptable for simple generic utilities.

Finally when done coding build and and run all tests to verify correctness.

At the very top of the file please include these two lines

// MIT License
// Copyright (c) 2025 dbjwhs


I need a README.md file created, the first paragraph should be a detail description of this pattern and its history. The next paragraph should be use cases and different problems this pattern fixes, the rest of the README should be examples, usage, good bad, and books that have contained this pattern. All long paragraphs should text break at column 120. This is for the mark down, not what will be visible to the reader.

I would like the last two paragraphs to be the following.


## License

This code is provided under the MIT License. Feel free to use, modify, and distribute as needed.

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.
