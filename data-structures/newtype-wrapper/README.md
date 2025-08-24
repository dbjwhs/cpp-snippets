# Strong Type Wrapper (Newtype Pattern) in C++23

The strong type wrapper pattern, also known as the "newtype" pattern, is a fundamental design technique that originated 
in functional programming languages like Haskell and has evolved into a cornerstone of modern C++ type safety. This 
pattern creates distinct types that are separate from their underlying representation at compile-time while maintaining 
zero runtime overhead. The concept emerged from the need to prevent logical errors that occur when values of the same 
underlying type but different semantic meanings are accidentally mixed—such as confusing meters with feet, or user IDs 
with product IDs. In Haskell, the `newtype` keyword creates a compile-time wrapper around existing types, inspiring 
similar implementations across various programming languages. The C++ adaptation leverages template metaprogramming, 
CRTP (Curiously Recurring Template Pattern), and modern language features like concepts to achieve the same goals while 
integrating seamlessly with the C++ type system and standard library.

Strong types solve critical software engineering problems by eliminating entire classes of bugs at compile-time rather 
than runtime. They prevent unit confusion in scientific and engineering applications, eliminate ID mixing in database 
systems, provide clear API contracts that are self-documenting, enable better compiler optimizations through type 
information, and support domain-driven design by making business concepts explicit in the code. The pattern is 
particularly valuable in financial systems where mixing currencies can cause catastrophic errors, in measurement 
systems where unit confusion has historically led to mission failures, in web applications where different types of 
IDs must not be interchangeable, and in any system where type safety directly correlates with correctness and 
maintainability. Modern C++23 implementations extend beyond simple wrappers to include composable mixins that provide 
arithmetic operations, comparison operators, hash support, and stream operations as needed, creating a flexible 
foundation for type-safe programming.

## Features

- **Zero Runtime Overhead**: Strong types compile down to their underlying representation with no performance penalty
- **Composable Mixins**: Choose exactly which operations your strong type supports through template mixins
- **Modern C++23**: Leverages concepts, three-way comparison, `std::expected`, and other cutting-edge features
- **Railway-Oriented Programming**: Built-in support for error handling patterns and functional transformations
- **Standard Library Integration**: Works seamlessly with STL containers, algorithms, and utilities
- **Comprehensive Testing**: Extensive test suite covering edge cases, performance, and correctness

## Quick Start

```cpp
#include "headers/strong_type.hpp"

// Define strong types for different business concepts
DEFINE_STRONG_TYPE(UserId, std::size_t, EqualityComparable, Hashable);
DEFINE_STRONG_TYPE(ProductId, std::size_t, EqualityComparable, Hashable);
DEFINE_STRONG_TYPE(Price, double, EqualityComparable, OrderedComparable, Arithmetic);

int main() {
    // Type safety prevents mixing incompatible IDs
    UserId user{12345};
    ProductId product{67890};
    
    // This would cause a compilation error - different types!
    // bool same = (user == product);  // Error: cannot compare UserId with ProductId
    
    // Safe arithmetic operations
    Price item_price{29.99};
    Price tax{2.50};
    Price total = item_price + tax;  // Price{32.49}
    
    std::print("Total price: ${}\n", total.value());
    return 0;
}
```

## Mixin System

The strength of this implementation lies in its composable mixin system. Each mixin adds specific capabilities:

### EqualityComparable
Enables `==` and `!=` operators:

```cpp
DEFINE_STRONG_TYPE(SessionId, std::string, EqualityComparable);

SessionId session1{"abc123"};
SessionId session2{"abc123"};
SessionId session3{"def456"};

assert(session1 == session2);  // true
assert(session1 != session3);  // true
```

### OrderedComparable
Provides full ordering with three-way comparison:

```cpp
DEFINE_STRONG_TYPE(Priority, int, OrderedComparable);

Priority high{10};
Priority medium{5};
Priority low{1};

assert(high > medium);      // true
assert(medium >= low);      // true
assert(low < high);         // true

// Works with standard algorithms
std::vector<Priority> priorities{medium, high, low};
std::sort(priorities.begin(), priorities.end());
// Result: [low, medium, high]
```

### Arithmetic
Adds mathematical operations with error handling:

```cpp
DEFINE_STRONG_TYPE(Distance, double, EqualityComparable, Arithmetic);

Distance d1{10.5};
Distance d2{3.2};

Distance sum = d1 + d2;           // Distance{13.7}
Distance diff = d1 - d2;          // Distance{7.3}
Distance product = d1 * d2;       // Distance{33.6}

// Safe division with error handling
auto division_result = d1 / d2;
if (division_result.has_value()) {
    std::print("Result: {}\n", division_result.value().value());
} else {
    std::print("Error: {}\n", division_result.error());
}

// Division by zero is handled safely
Distance zero{0.0};
auto error_result = d1 / zero;  // Returns std::unexpected("division by zero")
```

### Incrementable
Enables increment and decrement operations:

```cpp
DEFINE_STRONG_TYPE(Counter, int, EqualityComparable, Incrementable);

Counter count{5};

++count;        // Pre-increment: Counter{6}
count++;        // Post-increment: Counter{7}
--count;        // Pre-decrement: Counter{6}
count--;        // Post-decrement: Counter{5}
```

### Streamable
Provides stream input/output operators:

```cpp
DEFINE_STRONG_TYPE(Temperature, double, EqualityComparable, Streamable);

Temperature temp{23.5};

// Output
std::cout << "Current temperature: " << temp << "°C\n";
// Prints: Current temperature: 23.5°C

// Input
std::istringstream iss{"37.2"};
Temperature body_temp;
iss >> body_temp;
assert(body_temp.value() == 37.2);
```

### Hashable
Enables use in hash-based containers:

```cpp
DEFINE_STRONG_TYPE(AccountId, std::size_t, EqualityComparable, Hashable);

std::unordered_map<AccountId, std::string> account_names;
std::unordered_set<AccountId> active_accounts;

AccountId account{12345};
account_names[account] = "John Doe";
active_accounts.insert(account);
```

## Railway-Oriented Programming

The strong type wrapper includes built-in support for functional programming patterns:

```cpp
DEFINE_STRONG_TYPE(Celsius, double, EqualityComparable, Arithmetic, Streamable);

Celsius temp{25.0};

// Transform to Fahrenheit
auto fahrenheit = temp.transform([](double c) -> double {
    return c * 9.0 / 5.0 + 32.0;
});

if (fahrenheit.has_value()) {
    std::print("{}°C = {}°F\n", temp.value(), fahrenheit.value().value());
}

// Chain operations
auto description = temp.and_then([](double c) -> std::string {
    if (c < 0) return "freezing";
    if (c < 20) return "cold";
    if (c < 30) return "comfortable";
    return "hot";
});

std::print("Temperature is {}\n", description);  // "comfortable"

// Default values
Celsius unknown_temp{};
double safe_temp = unknown_temp.value_or(20.0);  // Default to 20°C
```

## Business Domain Examples

### Financial System

```cpp
namespace Finance {
    DEFINE_STRONG_TYPE(USD, double, EqualityComparable, OrderedComparable, Arithmetic, Streamable);
    DEFINE_STRONG_TYPE(EUR, double, EqualityComparable, OrderedComparable, Arithmetic, Streamable);
    DEFINE_STRONG_TYPE(AccountNumber, std::string, EqualityComparable, Hashable);
    
    struct BankAccount {
        AccountNumber number;
        USD balance;
        
        std::expected<void, std::string> withdraw(USD amount) {
            if (amount.value() <= 0) {
                return std::unexpected("Invalid withdrawal amount");
            }
            if (balance < amount) {
                return std::unexpected("Insufficient funds");
            }
            balance -= amount;
            return {};
        }
        
        void deposit(USD amount) {
            balance += amount;
        }
    };
}

// Usage prevents currency mixing at compile-time
Finance::USD dollars{100.0};
Finance::EUR euros{85.0};
// auto mixed = dollars + euros;  // Compilation error!

Finance::BankAccount account{
    Finance::AccountNumber{"ACC-123456"},
    Finance::USD{1000.0}
};

auto withdrawal_result = account.withdraw(Finance::USD{50.0});
if (!withdrawal_result.has_value()) {
    std::print("Withdrawal failed: {}\n", withdrawal_result.error());
}
```

### Scientific Computing

```cpp
namespace Physics {
    DEFINE_STRONG_TYPE(Meters, double, EqualityComparable, OrderedComparable, Arithmetic);
    DEFINE_STRONG_TYPE(Seconds, double, EqualityComparable, OrderedComparable, Arithmetic);
    DEFINE_STRONG_TYPE(Velocity, double, EqualityComparable, OrderedComparable, Arithmetic);
    DEFINE_STRONG_TYPE(Acceleration, double, EqualityComparable, OrderedComparable, Arithmetic);
    
    Velocity calculate_velocity(Meters distance, Seconds time) {
        auto result = distance / time;  // Returns std::expected<Meters, std::string>
        if (result.has_value()) {
            return Velocity{result.value().value()};
        }
        return Velocity{0.0};  // Handle division by zero
    }
    
    Acceleration calculate_acceleration(Velocity delta_v, Seconds time) {
        auto result = delta_v / time;
        if (result.has_value()) {
            return Acceleration{result.value().value()};
        }
        return Acceleration{0.0};
    }
}

// Type safety prevents unit confusion
Physics::Meters distance{100.0};
Physics::Seconds time{10.0};
Physics::Velocity speed = Physics::calculate_velocity(distance, time);

// This would be a compilation error:
// Physics::Velocity wrong = Physics::calculate_velocity(time, distance);
```

### Game Development

```cpp
namespace Game {
    DEFINE_STRONG_TYPE(PlayerId, std::size_t, EqualityComparable, OrderedComparable, Hashable);
    DEFINE_STRONG_TYPE(Score, int, EqualityComparable, OrderedComparable, Arithmetic, Incrementable);
    DEFINE_STRONG_TYPE(Level, int, EqualityComparable, OrderedComparable, Incrementable);
    DEFINE_STRONG_TYPE(HealthPoints, int, EqualityComparable, OrderedComparable, Arithmetic);
    
    struct Player {
        PlayerId id;
        std::string name;
        Score score;
        Level level;
        HealthPoints health;
        
        void take_damage(HealthPoints damage) {
            health -= damage;
            if (health.value() <= 0) {
                respawn();
            }
        }
        
        void gain_experience(Score points) {
            score += points;
            if (score.value() >= level.value() * 1000) {
                ++level;
                health = HealthPoints{100};  // Full health on level up
            }
        }
        
    private:
        void respawn() {
            health = HealthPoints{100};
            // Respawn logic...
        }
    };
}

// Usage with type safety
std::unordered_map<Game::PlayerId, Game::Player> players;

Game::PlayerId player_id{12345};
players[player_id] = Game::Player{
    player_id,
    "Alice",
    Game::Score{0},
    Game::Level{1},
    Game::HealthPoints{100}
};

players[player_id].gain_experience(Game::Score{500});
players[player_id].take_damage(Game::HealthPoints{25});
```

## Performance Characteristics

The strong type wrapper is designed for zero runtime overhead:

- **Construction/Destruction**: Identical performance to underlying type
- **Arithmetic Operations**: Inline to raw operations, ~5-15% overhead in debug builds, ~0% in optimized builds
- **Container Operations**: Sorts and searches perform identically to underlying types
- **Hash Operations**: Direct delegation to underlying type's hash function
- **Memory Layout**: Exactly the same as underlying type (verified with `sizeof` and `alignof`)

## Advanced Features

### Custom Mixins

Create your own mixins for domain-specific operations:

```cpp
template<typename Derived>
struct Percentage {
    std::expected<void, std::string> validate() const {
        const auto& value = static_cast<const Derived&>(*this).value();
        if (value < 0.0 || value > 100.0) {
            return std::unexpected("Percentage must be between 0 and 100");
        }
        return {};
    }
    
    Derived normalize() const {
        const auto& value = static_cast<const Derived&>(*this).value();
        return Derived{std::clamp(value, 0.0, 100.0)};
    }
};

DEFINE_STRONG_TYPE(BatteryLevel, double, EqualityComparable, OrderedComparable, Percentage);

BatteryLevel battery{95.5};
auto validation = battery.validate();  // OK
BatteryLevel normalized = battery.normalize();  // 95.5 (no change needed)
```

### Template Specialization

Specialize behavior for specific strong types:

```cpp
// Specialized formatter for temperature types
template<>
struct std::formatter<Physics::Temperature> {
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }
    
    auto format(const Physics::Temperature& temp, std::format_context& ctx) const {
        return std::format_to(ctx.out(), "{:.1f}°C", temp.value());
    }
};

// Usage
Physics::Temperature room_temp{22.5};
std::print("Room temperature: {}\n", room_temp);  // "Room temperature: 22.5°C"
```

## Common Pitfalls and Solutions

### Problem: Implicit Conversions

```cpp
// Bad: Easy to accidentally mix types
void process_user(int user_id);
void process_product(int product_id);

int id = get_some_id();
process_user(id);      // Which function should this call?
process_product(id);   // Both accept the same type!
```

```cpp
// Good: Strong types prevent confusion
DEFINE_STRONG_TYPE(UserId, int, EqualityComparable, Hashable);
DEFINE_STRONG_TYPE(ProductId, int, EqualityComparable, Hashable);

void process_user(UserId user_id);
void process_product(ProductId product_id);

UserId user_id{123};
ProductId product_id{456};

process_user(user_id);        // OK
process_product(product_id);  // OK
// process_user(product_id);  // Compilation error!
```

### Problem: Unit Confusion

```cpp
// Bad: No indication of units
double calculate_distance(double time, double velocity) {
    return time * velocity;  // What units? Seconds? Hours? m/s? mph?
}

double time = 3.5;      // Hours? Seconds?
double speed = 60.0;    // mph? m/s? km/h?
double distance = calculate_distance(time, speed);
```

```cpp
// Good: Units are explicit and enforced
DEFINE_STRONG_TYPE(Hours, double, EqualityComparable, Arithmetic);
DEFINE_STRONG_TYPE(MPH, double, EqualityComparable, Arithmetic);
DEFINE_STRONG_TYPE(Miles, double, EqualityComparable, Arithmetic);

Miles calculate_distance(Hours time, MPH velocity) {
    auto product = time * velocity;
    return Miles{product.value()};  // Clear unit conversion
}

Hours time{3.5};
MPH speed{60.0};
Miles distance = calculate_distance(time, speed);  // 210 miles
```

### Problem: API Misuse

```cpp
// Bad: Easy to swap parameters
bool transfer_money(int from_account, int to_account, double amount);

// These calls look identical but do different things:
transfer_money(12345, 67890, 100.0);  // Transfer from 12345 to 67890
transfer_money(67890, 12345, 100.0);  // Transfer from 67890 to 12345 (oops!)
```

```cpp
// Good: Parameter types make intent clear
DEFINE_STRONG_TYPE(AccountId, int, EqualityComparable, Hashable);
DEFINE_STRONG_TYPE(Amount, double, EqualityComparable, Arithmetic);

struct TransferRequest {
    AccountId from;
    AccountId to;
    Amount amount;
};

bool transfer_money(const TransferRequest& request);

// Intent is crystal clear:
AccountId savings{12345};
AccountId checking{67890};
Amount transfer_amount{100.0};

transfer_money(TransferRequest{
    .from = savings,
    .to = checking,
    .amount = transfer_amount
});
```

## Build and Test

### Prerequisites

- C++23 compatible compiler (GCC 13+, Clang 16+, MSVC 2022+)
- CMake 3.25+
- (Optional) Valgrind for memory leak detection

### Building

```bash
# Create build directory
mkdir build && cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the project
cmake --build . --parallel

# Run all tests
ctest --parallel --verbose
```

### Test Targets

```bash
# Run only unit tests
cmake --build . --target run_unit_tests

# Run performance benchmarks  
cmake --build . --target run_performance_tests

# Run sanitizer tests (requires Clang/GCC)
cmake --build . --target run_sanitizer_tests

# Run memory leak detection (requires Valgrind)
ctest -L memory
```

## Books and References

Several influential books have covered the strong type pattern and related concepts:

- **"Effective C++" by Scott Meyers** - Discusses type safety and preventing implicit conversions
- **"Modern C++ Design" by Andrei Alexandrescu** - Covers policy-based design and template metaprogramming
- **"C++ Templates: The Complete Guide" by David Vandevoorde** - Deep dive into template techniques
- **"Domain-Driven Design" by Eric Evans** - Emphasizes explicit domain modeling with distinct types  
- **"Clean Code" by Robert Martin** - Advocates for self-documenting, type-safe interfaces
- **"Programming: Principles and Practice Using C++" by Bjarne Stroustrup** - Discusses type safety fundamentals
- **"Functional Programming in C++" by Ivan Čukić** - Covers Railway-Oriented Programming patterns
- **"Real-World Haskell" by Bryan O'Sullivan** - Original source of the newtype pattern concept

The pattern has also been extensively discussed in:
- CppCon talks on type safety and domain modeling
- Blog posts by Arne Mertz, Jonathan Boccara, and other C++ experts  
- The C++ Core Guidelines (particularly regarding type safety)
- Academic papers on type systems and software engineering

## License

This code is provided under the MIT License. Feel free to use, modify, and distribute as needed.

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.
