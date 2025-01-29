// MIT License
// Copyright (c) 2025 dbjwhs

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <memory>
#include <ranges>

// forward declaration to resolve circular dependency
class Observer;

/**
 * Subject interface that defines the core observer pattern methods.
 * Any class that wants to notify observers must implement these methods.
 */
class Subject {
public:
    virtual ~Subject() = default;
    virtual void attach(std::shared_ptr<Observer> observer) = 0;  // add an observer
    virtual void detach(std::shared_ptr<Observer> observer) = 0;  // remove an observer
    virtual void notify() = 0;  // notify all observers of state change
};

/**
 * Observer interface that defines how observers receive updates.
 * Any class that wants to receive notifications must implement this interface.
 */
class Observer {
public:
    virtual ~Observer() = default;
    /**
     * Update method called by the Subject when state changes
     * @param message Description of the update
     * @param value New value to be processed
     */
    virtual void update(const std::string& message, double value) = 0;
};

/**
 * Helper class that combines setting a value with notifying observers.
 * This eliminates the need to manually call notify() after every state change.
 */
class WeatherNotifier {
    // define a type for setter methods that modify weather values
    using SetterMethod = void(*)(double& target, double value);

private:
    double& m_target;        // reference to the variable being modified
    SetterMethod m_setter;   // pointer to the method that sets the value
    Subject& m_subject;      // reference to subject for notifications

public:
    /**
     * Constructor that binds together the variable, setter method, and subject
     * @param target_var Reference to the variable to be modified
     * @param setter_method Pointer to the method that will modify the variable
     * @param subject_ref Reference to the subject that will notify observers
     */
    WeatherNotifier(double& target_var, SetterMethod setter_method, Subject& subject_ref)
        : m_target(target_var), m_setter(setter_method), m_subject(subject_ref) {}

    /**
     * Operator that combines setting the value and notifying observers
     * @param value New value to be set
     */
    void operator()(double value) const {
        m_setter(m_target, value);    // set the new value
        m_subject.notify();         // notify all observers
    }
};

/**
 * Concrete implementation of the Subject interface that monitors weather conditions.
 * Uses WeatherNotifier to automatically notify observers when values change.
 */
class WeatherStation : public Subject {
private:
    std::vector<std::shared_ptr<Observer>> m_observers;  // list of registered observers
    double m_temperature; // current temperature
    double m_humidity;    // current humidity
    double m_pressure;    // current pressure

    // static setter methods used by WeatherNotifier
    static void setTemp(double& target, const double value) { target = value; }
    static void setHumid(double& target, const double value) { target = value; }
    static void setPress(double& target, const double value) { target = value; }

    // notifier objects that handle automatic notification
    WeatherNotifier temp_notifier_;
    WeatherNotifier humid_notifier_;
    WeatherNotifier pressure_notifier_;

public:
    /**
     * Constructor initializes weather values and sets up notifiers
     */
    WeatherStation()
        : m_temperature(0.0), m_humidity(0.0), m_pressure(0.0),
          temp_notifier_(m_temperature, setTemp, *this),
          humid_notifier_(m_humidity, setHumid, *this),
          pressure_notifier_(m_pressure, setPress, *this) {}

    /**
     * Register a new observer to receive notifications
     * @param observer Shared pointer to the observer
     */
    void attach(const std::shared_ptr<Observer> observer) override {
        m_observers.push_back(observer);
    }

    /**
     * Remove an observer from the notification list
     * @param observer Shared pointer to the observer to remove
     */
    void detach(std::shared_ptr<Observer> observer) override {
        std::erase_if(m_observers, [observer](const std::shared_ptr<Observer>& obj) {
            return obj == observer;
        });
    }

    /**
     * Notify all registered observers of the current temperature
     */
    void notify() override {
        for (const auto& observer : m_observers) {
            observer->update("Weather Update", m_temperature);
        }
    }

    // public methods that use notifiers to automatically update and notify
    void setTemperature(const double temp) { temp_notifier_(temp); }
    void setHumidity(const double humid) { humid_notifier_(humid); }
    void setPressure(const double press) { pressure_notifier_(press); }

    // getter methods for current weather values
    [[nodiscard]] double getTemperature() const { return m_temperature; }
    [[nodiscard]] double getHumidity() const { return m_humidity; }
    [[nodiscard]] double getPressure() const { return m_pressure; }
};

/**
 * Concrete observer that displays weather updates
 */
class DisplayDevice final : public Observer {
private:
    std::string m_deviceId;  // unique identifier for this display
    double m_lastValue;      // last received value

public:
    explicit DisplayDevice(std::string id) : m_deviceId(std::move(id)), m_lastValue(0.0) {}

    /**
     * Handle updates from the weather station
     */
    void update(const std::string& message, double value) override {
        m_lastValue = value;
        display();
    }

    /**
     * Display the current weather value
     */
    void display() const {
        std::cout << "Device " << m_deviceId << " received update. Value: "
                  << m_lastValue << std::endl;
    }
};

/**
 * Concrete observer that triggers alerts based on temperature thresholds
 */
class WeatherAlert final : public Observer {
private:
    double m_temperatureThreshold;  // Temperature threshold for alerts

public:
    explicit WeatherAlert(const double threshold) : m_temperatureThreshold(threshold) {}

    /**
     * Check if temperature exceeds threshold and trigger alert if needed
     */
    void update(const std::string& message, const double value) override {
        if (value > m_temperatureThreshold) {
            std::cout << "ALERT: Temperature exceeded threshold! Current: "
                      << value << "°C" << std::endl;
        }
    }
};

// example usage of the weather monitoring system
int main() {
    // create the weather station
    const auto weatherStation = std::make_shared<WeatherStation>();

    // create different types of observers
    const auto display1 = std::make_shared<DisplayDevice>("Display 1");
    const auto display2 = std::make_shared<DisplayDevice>("Display 2");
    const auto alert = std::make_shared<WeatherAlert>(30.0);

    // register observers with the weather station
    weatherStation->attach(display1);
    weatherStation->attach(display2);
    weatherStation->attach(alert);

    // simulate weather changes and demonstrate automatic notifications
    std::cout << "Updating temperature to 25°C\n";
    weatherStation->setTemperature(25.0);

    std::cout << "\nUpdating temperature to 32°C\n";
    weatherStation->setTemperature(32.0);

    // demonstrate observer removal
    std::cout << "\nDetaching Display 1\n";
    weatherStation->detach(display1);

    std::cout << "\nUpdating temperature to 28°C\n";
    weatherStation->setTemperature(28.0);

    return 0;
}