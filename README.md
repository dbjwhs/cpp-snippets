# WIP

# C++ Code Snippets

A collection of self-contained C++ code snippets, examples, and mini-projects. Each snippet is independently buildable and includes its own CMake configuration.

## Building the Snippets

Each snippet is contained in its own directory with a CMakeLists.txt file. To build a specific snippet:

```bash
cd <snippet-directory>
mkdir build && cd build
cmake ..
make
```

Or you can elect to build everything at once.

1. `build_all.sh` that:
   - Automatically finds and builds all CMake projects in your directory structure
   - Creates build directories as needed
   - Provides colored output and detailed logging
   - Shows a comprehensive summary of successful and failed builds with executable paths
   - `--dry-run` | `-d` argument to perform a *dry run*
   - `--run` | `-r` argument to perform *run* executable after build

2. `clean_all.sh` that:
   - Finds and removes all build directories
   - Provides similar colored output and logging
   - Shows a summary of cleaned projects
   - `--dry-run` | `-d` argument to perform a *dry run*

### Prerequisites

- CMake 3.30 or higher
- C++20 compliant compiler (see gotcha's below for linux)
- Git

## Snippets Index

### Algorithms
- [binary-search/](algorithms/binary-search/) - Binary Search is a fundamental divide-and-conquer algorithm that emerged in the early days of computer science, with its
- [bloom-filter/](algorithms/bloom-filter/) - This is a high-performance, memory-efficient Bloom filter implementation in C++, designed for robust probabilistic set
- [compression/](algorithms/compression/) - This project implements three classical lossless compression algorithms: Run-Length Encoding (RLE), Lempel-Ziv-Welch (LZW), and Huffman Coding. RLE, developed in the 1960s, is one of the simplest forms of data compression, working by replacing sequences of identical data elements with a count and the element itself. LZW, published by Abraham Lempel, Jacob Ziv, and Terry Welch in 1984, builds on earlier LZ77 and LZ78 algorithms and creates a dictionary of repeated patterns dynamically during compression. Huffman coding, invented by David Huffman in 1952 while he was a graduate student at MIT, creates optimal prefix codes by building a binary tree based on character frequencies, assigning shorter codes to more frequent characters.
- [hash-table/](algorithms/hash-table/) - Hash tables, first introduced by Hans Peter Luhn at IBM in 1953, represent a fundamental data structure that implements
- [hyperloglog/](algorithms/hyperloglog/) - This project implements a simplified version of the HyperLogLog algorithm, a probabilistic data structure designed for
- [merkle-tree/](algorithms/merkle-tree/) - A Merkle tree, also known as a hash tree, is a fundamental data structure in cryptography and computer science, invented by Ralph Merkle in 1979. The structure is built as a binary tree of cryptographic hashes, where each leaf node contains the hash of a data block, and each non-leaf node contains the hash of its child nodes' hashes combined. This creates a hierarchical structure that allows for efficient and secure verification of large data sets. The tree culminates in a single hash at the root (the Merkle root), which essentially serves as a cryptographic fingerprint of all the data contained in the tree. This pattern revolutionized data verification by enabling the validation of large datasets without requiring the entire dataset to be present.
- [quick-sort/](algorithms/quick-sort/) - In 1999, Jon Bentley's seminal work [Programming Pearls](https://wiki.c2.com/?ProgrammingPearls) hit the shelves of brick-and-mortar bookstores, back when discovering programming books meant wandering through physical aisles rather than scrolling through online reviews. The book became a touchstone for a generation of programmers, featuring among its many gems an implementation of QuickSort using what we now know as the Lomuto partition scheme. This was during an era when C dominated systems programming and before C++ had achieved its current ubiquity.

### Concurrency
- [active-object-pattern/](concurrency/active-object-pattern/) - The Active Object Pattern solves several critical concurrency challenges in software design:
- [barrier-example/](concurrency/barrier-example/) - The Barrier Pattern is a synchronization mechanism used in concurrent programming to ensure that multiple threads wait for each other to reach a specific point before any of them can proceed further. It's like establishing a checkpoint where all threads must arrive before the race can continue.
- [cpp20-latch/](concurrency/cpp20-latch/) - The `std::latch` is a powerful synchronization primitive introduced in C++20 as part of the concurrency library. It provides a
- [dining-philosophers/](concurrency/dining-philosophers/) - The Dining Philosophers Problem is a classic computer science problem that illustrates challenges in resource allocation and deadlock prevention. Originally formulated by Edsger Dijkstra in 1965, it models a scenario where five philosophers sit around a circular table, each facing a bowl of spaghetti. Between each pair of philosophers is a single fork, and a philosopher needs both their left and right forks to eat.
- [jthreads/](concurrency/jthreads/) - The std::jthread class, introduced in C++20, represents a significant evolution in C++'s thread management capabilities.
- [pipelining/](concurrency/pipelining/) - A thread pipeline is a concurrent programming pattern where a series of processing stages are connected in sequence, with each stage running in its own thread. Data flows through these stages like an assembly line, with each stage performing a specific operation on the data before passing it to the next stage.
- [producer-consumer/](concurrency/producer-consumer/) - This guide details a robust implementation of the Producer-Consumer pattern using modern C++ features, with a focus on thread safety, RAII principles, and clean design patterns.
- [reader-writer/](concurrency/reader-writer/) - This project implements a thread-safe Readers-Writers synchronization mechanism in C++. The implementation features RAII (Resource Acquisition Is Initialization) pattern, thread-safe logging using a singleton pattern, and prevention of writer starvation.
- [shared_mutex/](concurrency/shared_mutex/) - The reader-writer lock pattern is a synchronization primitive that allows concurrent access for read-only operations while
- [thread-local-storage/](concurrency/thread-local-storage/) - Thread Local Storage (TLS) is a mechanism that provides each thread with its own unique copy of data variables, ensuring thread
- [thread-pool/](concurrency/thread-pool/) - This repository contains a modern C++17 implementation of a thread pool pattern. The thread pool manages a collection of worker threads that can execute tasks asynchronously, providing an efficient way to parallelize work across multiple threads.
- [thread-pool-load-balance/](concurrency/thread-pool-load-balance/) - The Thread Pool pattern is a software design pattern that emerged in the mid-1990s as a solution to the performance overhead
- [thread-safe-logging/](concurrency/thread-safe-logging/) - A modern C++ thread-safe logging implementation that supports multiple log levels, file and console output, and concurrent access from multiple threads.
- [timed_mutex/](concurrency/timed_mutex/) - The `std::timed_mutex` is a synchronization primitive introduced in C++11 as part of the standard threading library. It

### Data Structures
- [binary-tree/](data-structures/binary-tree/) - Attention readers of the code the following will be added soon.
- [linked-list/](data-structures/linked-list/) - A robust, template-based linked list implementation in modern C++ that emphasizes type safety, memory safety, and flexibility. This implementation uses smart pointers for automatic memory management and supports any data type that satisfies basic requirements.
- [lru-cache/](data-structures/lru-cache/) - An LRU (Least Recently Used) cache is a data structure that stores a limited number of items and discards the least recently used item when the cache becomes full. This implementation strategy is based on the principle that recently used items are likely to be used again soon.

### Design Patterns

#### Creational
- [abstract-factory/](design-patterns/creational/abstract-factory/) - The Abstract Factory pattern, introduced in 1994 by the "Gang of Four" (GoF) in their seminal work "Design Patterns: Elements of Reusable Object-Oriented Software," is a creational design pattern that provides an interface for creating families of related or dependent objects without specifying their concrete classes. This pattern emerged from the need to handle platform independence in object-oriented systems, particularly in GUI frameworks where different operating systems required different widget implementations. The pattern's origins can be traced back to earlier systems like InterViews and ET++ that needed to manage cross-platform compatibility while maintaining consistent object families.
- [builder/](design-patterns/creational/builder/) - The Builder pattern is a creational design pattern that enables the construction of complex objects step by step. First introduced in the
- [factory/](design-patterns/creational/factory/) - This project demonstrates two implementations of the Factory Pattern in C++: a static factory and a configurable instance-based factory.
- [prototype/](design-patterns/creational/prototype/) - The Prototype pattern, first introduced in the seminal "Design Patterns: Elements of Reusable Object-Oriented Software" book by the Gang of Four in 1994, is a creational design pattern that enables object creation by cloning an existing object, known as the prototype. This pattern emerged from the need to create new objects without tightly coupling the client code to specific classes, and to avoid the computational overhead of creating objects from scratch. The pattern was inspired by biological processes where cells divide to produce copies, and it brings this concept into software engineering. Historically, the pattern gained prominence in frameworks requiring runtime object composition, particularly in graphical editing systems where users could clone existing shapes and modifications.
- [singleton/](design-patterns/creational/singleton/) - The Singleton pattern was first introduced by the "Gang of Four" (Erich Gamma, Richard Helm, Ralph Johnson, and John Vlissides) in their seminal 1994 book "Design Patterns: Elements of Reusable Object-Oriented Software." However, the concept of ensuring a single instance of a class existed long before its formal documentation.

#### Structural
- [adapter/](design-patterns/structural/adapter/) - The Adapter pattern is a structural design pattern that allows objects with incompatible interfaces to collaborate. First introduced in the book "Design Patterns: Elements of Reusable Object-Oriented Software" by the Gang of Four in 1994, it acts as a wrapper between two objects. The pattern converts the interface of one class into another interface that clients expect, enabling classes to work together that couldn't otherwise because of incompatible interfaces. Think of it like a power adapter that allows you to charge your devices in different countries - it doesn't change the underlying electricity or your device, it just makes them work together.
- [bridge/](design-patterns/structural/bridge/) - The Bridge pattern is a structural design pattern that divides business logic or huge class into separate class hierarchies that can be
- [composite/](design-patterns/structural/composite/) - The Composite pattern is a structural design pattern that was introduced in 1994 as part of the influential "Gang of Four" design patterns.
- [decorator/](design-patterns/structural/decorator/) - The Decorator pattern is a structural design pattern that was first introduced in the influential "Gang of Four" (GoF) Design Patterns
- [facade/](design-patterns/structural/facade/) - The Facade design pattern, introduced in 1994 by the Gang of Four (GoF) in their seminal work on design patterns, represents one of
- [flyweight/](design-patterns/structural/flyweight/) - The Flyweight Pattern is a structural design pattern that was first introduced in 1990 by Paul Calder and Mark Linton as part of the InterViews user interface toolkit at Stanford University. It was later popularized through its inclusion in the influential "Gang of Four" (GoF) design patterns book. The pattern's primary purpose is to minimize memory usage by sharing as much data as possible between similar objects. It achieves this by separating an object's intrinsic state (shared) from its extrinsic state (unique to each instance). The pattern's name comes from the boxing weight class category, suggesting its lightweight nature in terms of memory usage.
- [pimpl/](design-patterns/structural/pimpl/) - The Pimpl idiom, also known as the Compilation Firewall or Opaque Pointer pattern, was popularized in the C++ community during the early 1990s. Originally developed to combat the long compilation times in C++, it was extensively used in frameworks like Qt. The pattern was first described by Jeff Sumner as the "Cheshire Cat" technique, named after the disappearing cat in Alice in Wonderland, because it makes the implementation details vanish from the public interface.
- [proxy/](design-patterns/structural/proxy/) - The Proxy design pattern is a structural pattern that provides a surrogate or placeholder for another object to control

#### Behavioral
- [chain-of-responsibility/](design-patterns/behavioral/chain-of-responsibility/) - The Chain of Responsibility is a behavioral design pattern that lets you pass requests along a chain of handlers. Upon receiving a request, each handler decides either to process the request or to pass it to the next handler in the chain. The pattern was first introduced by Erich Gamma, Richard Helm, Ralph Johnson, and John Vlissides (known as the "Gang of Four") in their seminal 1994 book "Design Patterns: Elements of Reusable Object-Oriented Software". This pattern emerged from the need to decouple senders and receivers of requests, allowing multiple objects to handle the request without the sender needing to know which object will ultimately process it. The pattern promotes loose coupling and adheres to the Single Responsibility and Open/Closed principles of object-oriented design.
- [command/](design-patterns/behavioral/command/) - The Command pattern, introduced in the Gang of Four's seminal 1994 book "Design Patterns: Elements of Reusable Object-Oriented Software," is a behavioral design pattern that turns requests into stand-alone objects. It was first notably used in the early 1980s in the Smalltalk-80 system for implementing the user interface, where commands were used to implement undoable operations. Today, it's ubiquitous in modern software development, found in everything from text editors (Microsoft Word's undo system) to game development (for input handling and replay systems) to distributed systems (for implementing message queues and transaction management). The pattern's versatility is demonstrated well by comparing a Document editing system with a SmartDevice home automation system - while these domains seem quite different at first glance (text manipulation vs. device control), they can both be elegantly implemented using the Command pattern, encapsulating operations like text insertion or temperature changes as command objects. The pattern is particularly prevalent in GUI applications, where each button or menu item typically encapsulates a command object.
- [fail-fast/](design-patterns/behavioral/fail-fast/) - The Fail-Fast pattern is a robust error-handling strategy that emerged in the 1970s during the early days of defensive programming.
- [interpreter/](design-patterns/behavioral/interpreter/) - The Interpreter pattern is a behavioral design pattern that defines a grammatical representation for a language and provides an
- [iterator/](design-patterns/behavioral/iterator/) - The Iterator Pattern is a behavioral design pattern that provides a way to sequentially access elements of an aggregate
- [mediator/](design-patterns/behavioral/mediator/) - The Mediator pattern is a behavioral design pattern that was introduced in the seminal "Gang of Four" (GoF) design
- [memento/](design-patterns/behavioral/memento/) - The Memento pattern is a behavioral design pattern that provides the ability to restore an object to its previous state
- [observer/](design-patterns/behavioral/observer/) - This repository demonstrates a robust implementation of the Observer design pattern in C++, featuring automatic notification handling and modern C++ practices. The example uses a weather monitoring system to showcase the pattern's functionality.
- [state/](design-patterns/behavioral/state/) - The State pattern is a behavioral design pattern that allows an object to alter its behavior when its internal state
- [strategy/](design-patterns/behavioral/strategy/) - The Strategy Pattern is a behavioral design pattern that was introduced by the "Gang of Four" (Erich Gamma, Richard Helm, Ralph Johnson, and John Vlissides) in their seminal 1994 book "Design Patterns: Elements of Reusable Object-Oriented Software". This pattern enables defining a family of algorithms, encapsulating each one, and making them interchangeable. It lets the algorithm vary independently from clients that use it. The pattern emerged from the need to provide a way to configure a class with one of many behaviors, and to provide a way to change or extend those behaviors without altering the class itself. This aligns perfectly with the Open/Closed Principle, one of the fundamental principles of object-oriented design.
- [template-method/](design-patterns/behavioral/template-method/) - The Template Method is a behavioral design pattern that was first formally introduced in the seminal book "Design Patterns: Elements of Reusable Object-Oriented Software" (1994) by the Gang of Four (Erich Gamma, Richard Helm, Ralph Johnson, and John Vlissides). The pattern defines the skeleton of an algorithm in a base class but lets subclasses override specific steps of the algorithm without changing its structure. This pattern emerged from the observation that many algorithms follow similar patterns with only slight variations in their implementation details. The Template Method pattern crystallized these observations into a formal design solution, making it one of the fundamental patterns in object-oriented design. Its roots can be traced back to early frameworks and libraries where developers needed a way to provide default behaviors while allowing for customization.
- [vistor/](design-patterns/behavioral/vistor/) - The Visitor pattern is a behavioral design pattern that allows you to separate algorithms from the objects on which they

### Utilities
- [dates/](utilities/dates/) - The date library provides modern C++ date and time zone functionality. It's not part of the standard library, so we need to build it from source:
- [dir_scanner/](utilities/dir_scanner/) - A platform-agnostic C++ directory scanner that uses an iterative approach with a queue to traverse directories efficiently. This implementation uses modern C++ features and provides detailed file information including permissions, timestamps, and ownership details on POSIX systems.
- [mock-filesystem/](utilities/mock-filesystem/) - The concept of file systems dates back to the early days of computing in the 1950s. The first hierarchical file system was implemented in the Multics operating system (1965), which heavily influenced modern file system design.
- [string-utils/](utilities/string-utils/) - This repository contains a basic demonstration class for C++ string manipulation, focusing on `std::string` functionality including UTF-8 and Unicode support. As time goes on I will add more examples of functionality that I have used in the past, for for the time being this is a good starting point.
- [timer/](utilities/timer/) - A modern C++ implementation of a high-precision timer using the `std::chrono` library. This class provides accurate timing measurements with nanosecond precision and multiple output formats.

### Programming Paradigms
- [diammond-problem/](programming-paradigms/diammond-problem/) - The Diamond Problem is a classic challenge in object-oriented programming that emerges when a class inherits from two classes that share
- [negative-space/](programming-paradigms/negative-space/) - Negative space programming is a coding philosophy that emerged from the intersection of visual arts principles, defensive programming, and

### Odds And Ends
- [boost-asio/](odds-and-ends/boost-asio/) - Boost.Asio is a cross-platform C++ library for network and low-level I/O programming that provides a consistent asynchronous
- [const-constexper/](odds-and-ends/const-constexper/) - The `const` and `constexpr` keywords are fundamental parts of modern C++ programming that enable developers to express
- [cpp20-concepts/](odds-and-ends/cpp20-concepts/) - C++ Concepts is a language feature introduced in C++20 that provides a way to specify constraints on template parameters. The
- [cpp20-coroutines/](odds-and-ends/cpp20-coroutines/) - Coroutines represent one of the most significant additions to C++20, providing a standardized mechanism for cooperative task execution.
- [cpp20-counting_semaphore/](odds-and-ends/cpp20-counting_semaphore/) - The counting semaphore is a fundamental synchronization primitive in computer science, first introduced by Dutch computer
- [cpp20-designated-initializers/](odds-and-ends/cpp20-designated-initializers/) - Designated initializers are a feature in C++ that allows developers to initialize specific members of an aggregate type by
- [cpp20-modules/](odds-and-ends/cpp20-modules/) - ![Under Construction](https://img.shields.io/badge/Status-Under%20Construction-yellow) ![Requires C++20](https://img.shields.io/badge/Requires-C%2B%2B20-blue)
- [cpp20-ranges/](odds-and-ends/cpp20-ranges/) - C++ Ranges is a powerful library feature introduced in C++20 that revolutionizes how developers work with collections of data.
- [cpp20-spaceship/](odds-and-ends/cpp20-spaceship/) - The spaceship operator, officially known as the three-way comparison operator (`<=>`), is one of the most significant additions
- [cpp20-test-macros/](odds-and-ends/cpp20-test-macros/) - C++20 feature detection macros are a standardized set of preprocessor definitions that allow developers to determine at
- [hoare-logic/](odds-and-ends/hoare-logic/) - Hoare Logic is a formal system for reasoning about the correctness of computer programs, developed by British computer scientist
- [pagination/](odds-and-ends/pagination/)
- [phone-number-to-char/](odds-and-ends/phone-number-to-char/)
- [slicing/](odds-and-ends/slicing/) - Object slicing in C++ is a subtle but potentially serious issue that occurs when derived class objects are passed by value to functions expecting base class objects. While this might seem like a basic concept, it has caused numerous hard-to-debug issues in production code, especially in large codebases where object hierarchies are complex.
- [std-function/](odds-and-ends/std-function/)
- [std-move/](odds-and-ends/std-move/) - Move semantics is a fundamental design pattern in modern C++ that was introduced with C++11. This pattern represents a
- [std-optional/](odds-and-ends/std-optional/)
- [std-regex/](odds-and-ends/std-regex/) - Regular expressions (regex) are powerful pattern matching tools that have been an integral part of text processing since the
- [template-progression/](odds-and-ends/template-progression/) - C++ templates are a powerful feature of the language that enables generic programming. Templates were first introduced in

## Contributing

Feel free to suggest improvements or report issues. Each snippet should:

1. Be self-contained in its own directory
2. Include a CMakeLists.txt file
3. Have a README.md explaining its purpose and usage
4. Include comments explaining complex logic
5. Follow the repository's coding style

## Snippet Directory Structure

Each snippet directory should follow this structure:

```
snippet-name/
├── CMakeLists.txt          # Build configuration
├── README.md               # Usage and explanation
├── include/                # Header files
│   └── snippet.hpp
└── src/                    # Source files
    └── snippet.cpp
```

## Gotchas and errata

I am currently evaluating JetBrain's CLion which creates a directory in each project file called `cmake-build-debug` which if removed becomes a **PITA**.

### Recreating the cmake-build-debug Directory

If you accidentally removed the `cmake-build-debug` directory, you can easily recreate it by following these steps:

#### 1. Regenerate the Build Directory

Navigate to the root directory of your CMake project in your terminal or command prompt, then run:

```bash
cmake -DCMAKE_BUILD_TYPE=Debug ..
```

**Note:** Replace `..` with the path to your CMakeLists.txt file if it's not in the same directory.

This command will reconfigure your project and generate the `cmake-build-debug` directory along with the necessary build files.

#### 2. Rebuild Your Project

You can either:

* Rebuild your project from within your IDE (Visual Studio or CLion)

OR

* Use the following command from your terminal:

```bash
cmake --build . --target all
```

This will rebuild all the targets in your project.

## Linux

As of this writing 2/17/2025 most of the projects are building on linux, see below for my setup instructions. However there still are six (6) project not building, I am working through issues on them currently.

### C++20 std::format Build Issues and Resolution

### Problem Description
Build failures occurred when attempting to use `std::format` in C++20 code. While the code built successfully in CLion, it failed during command line cmake/make builds with the error:
```
fatal error: format: No such file or directory
```

The system was initially running GCC 12.3.0 which had incomplete C++20 library support.

### Root Cause
GCC 12 implementations shipped with partial C++20 support. Specifically, the `std::format` header was not included in the standard library headers, which explained why the `<format>` header could not be found during compilation.

### Solution
The issue was resolved by upgrading to GCC 13, which provides complete C++20 support including the `std::format` implementation.

#### Step 1: Add Required Repository
```bash
sudo add-apt-repository ppa:ubuntu-toolchain-r/test
sudo apt update
```

#### Step 2: Install GCC 13
```bash
sudo apt install gcc-13 g++-13 libstdc++-13-dev
```

#### Step 3: Set System Defaults
```bash
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-13 130
sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-13 130
```

### Result
- Successfully upgraded to GCC 13.1.0 (Ubuntu 13.1.0-8ubuntu1~22.04)
- Full C++20 support including `std::format`
- Code now builds correctly both in CLion and command line environments

### Key Learning (saving you three hours of headaches)
When implementing C++20 features, particularly `std::format`, GCC 13 or later is required xfor complete standard library support. Earlier versions may have incomplete implementations of the C++20 standard library features.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

---
**Note**: This repository is actively maintained and growing. Check back regularly for new snippets and updates!
