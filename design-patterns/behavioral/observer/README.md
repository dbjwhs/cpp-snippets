# Advanced Observer Pattern Implementation in C++

This repository demonstrates a robust implementation of the Observer design pattern in C++, featuring automatic notification handling and modern C++ practices. The example uses a weather monitoring system to showcase the pattern's functionality.

## Overview

The Observer pattern is a behavioral design pattern that establishes a one-to-many relationship between objects. When one object (the subject) changes state, all its dependents (observers) are notified and updated automatically.

### Key Features

- **Automatic Notification**: Uses a custom `WeatherNotifier` class to automatically trigger notifications when values change
- **Modern C++ Features**: Implements smart pointers, STL containers, and C++20 features
- **Type Safety**: Ensures type-safe operations through static methods and strong typing
- **Memory Safety**: Utilizes shared pointers for automatic memory management
- **Flexible Observer Management**: Easy registration and deregistration of observers

## Design Details

### Core Components

1. **Subject Interface (`Subject`)**
    - Defines the contract for attaching and detaching observers
    - Includes notification mechanism
    - Pure virtual interface for flexibility

2. **Observer Interface (`Observer`)**
    - Defines the update method that observers must implement
    - Allows for different types of observers
    - Provides type-safe update parameters

3. **WeatherNotifier Class**
    - Combines value setting with automatic notification
    - Uses function pointers for flexible setter methods
    - Eliminates manual notification calls

4. **WeatherStation Class**
    - Concrete implementation of the Subject interface
    - Manages multiple weather measurements
    - Uses WeatherNotifier for automatic updates
    - Thread-safe observer management

5. **Concrete Observers**
    - `DisplayDevice`: Shows current weather values
    - `WeatherAlert`: Triggers alerts based on thresholds

### Implementation Highlights

#### Automatic Notification
```cpp
class WeatherNotifier {
    using SetterMethod = void(*)(double& target, double value);
    // ... implementation
};
```
- Encapsulates the setter logic with notification
- Provides clean syntax for value updates
- Ensures notifications are never forgotten

#### Observer Management
```cpp
void detach(std::shared_ptr<Observer> observer) override {
    std::erase_if(observers, [observer](const std::shared_ptr<Observer>& obj) {
        return obj == observer;
    });
}
```
- Uses C++20's `std::erase_if` for efficient removal
- Thread-safe implementation
- Memory-safe using shared pointers

## Usage Example

```cpp
// Create the weather station
auto weatherStation = std::make_shared<WeatherStation>();

// Create and attach observers
auto display = std::make_shared<DisplayDevice>("Display 1");
auto alert = std::make_shared<WeatherAlert>(30.0);

weatherStation->attach(display);
weatherStation->attach(alert);

// Update values - notifications happen automatically
weatherStation->setTemperature(32.0);
```

## Best Practices Demonstrated

1. **RAII Principles**
    - Proper resource management
    - Automatic cleanup through smart pointers

2. **Modern C++ Features**
    - Smart pointers for memory management
    - STL algorithms and containers
    - C++20 features where appropriate

3. **Design Pattern Best Practices**
    - Clear separation of concerns
    - Interface-based design
    - Encapsulated implementation details

4. **Code Organization**
    - Well-documented interfaces
    - Logical component separation
    - Clear naming conventions

## Requirements

- C++20 compatible compiler
- Standard Template Library (STL)
- CMake 3.12 or higher (for building)

## Extensions and Modifications

The implementation can be extended in several ways:

1. Add thread safety for concurrent updates
2. Implement priority-based observer notification
3. Add filtered notifications based on value changes
4. Extend to support multiple types of measurements
5. Add serialization for weather data

## Contributing

Feel free to submit issues and enhancement requests!

## License

This project is licensed under the MIT License - see the LICENSE file for details.
