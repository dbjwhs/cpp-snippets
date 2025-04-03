// MIT License
// Copyright (c) 2025 dbjwhs

#include <cassert>
#include <string>
#include <algorithm>
#include <format>
#include "../../../../headers/project_utils.hpp"

// bridge design pattern
// --------------------
// history and background:
// - introduced by the "gang of four" (gamma, helm, johnson, vlissides) in their seminal 1994 book
//   "design patterns: elements of reusable object-oriented software"
// - influenced by earlier work at xerox parc in the late 1980s
// - originally named "handle/body" pattern in some early literature
//
// purpose and intent:
// - separates an abstraction from its implementation allowing both to vary independently
// - promotes loose coupling between interface and implementation
// - helps avoid a permanent binding between an interface and its implementation
// - particularly useful when dealing with cross-platform implementations
//
// key components:
// - abstraction: defines the interface and maintains reference to implementor
// - refined abstraction: extends the interface defined by abstraction
// - implementor: defines the interface for implementation classes
// - concrete implementor: implements the implementor interface
//
// real-world usage examples:
// - gui frameworks separating window abstraction from os-specific window implementations
// - device drivers separating device interface from hardware-specific code
// - graphics libraries separating shape abstractions from rendering implementations
//
// advantages:
// - runtime flexibility to switch implementations
// - improved extensibility (can extend both abstractions and implementations)
// - implementation details hidden from abstraction layer
// - reduces compilation dependencies
//
// trade-offs:
// - increases complexity by introducing new abstract classes
// - can be overkill for simple cross-platform implementations
// - requires planning and forethought to create coherent abstraction
//
// this implementation demonstrates:
// - separation of remote control interfaces (abstraction) from device implementations
// - ability to mix and match different remotes with different devices
// - independent evolution of both remote features and device capabilities
// - clean separation of concerns with implementation details hidden

// bridge pattern example demonstrating separation between abstraction and implementation
// the pattern allows both hierarchies to evolve independently

// forward declarations
class DeviceImplementation;

// abstraction class that represents the remote control interface
class RemoteControl {
protected:
    // bridge to the implementation
    std::shared_ptr<DeviceImplementation> m_device;

public:
    explicit RemoteControl(std::shared_ptr<DeviceImplementation> device)
        : m_device(std::move(device)) {}

    virtual ~RemoteControl() = default;

    // core operations that will be bridged to the implementation
    virtual void togglePower() = 0;
    virtual void setVolume(int volume) = 0;
    [[nodiscard]] virtual bool isPowered() const = 0;
    [[nodiscard]] virtual int getVolume() const = 0;
};

// implementation interface
class DeviceImplementation {
protected:
    bool m_powered{false};
    int m_volume{0};

public:
    virtual ~DeviceImplementation() = default;

    virtual void setPower(bool state) {
        m_powered = state;
        Logger::getInstance().log(LogLevel::INFO,
            std::format("Device power state changed to: {}", m_powered));
    }

    virtual void setVolumeLevel(int volume) {
        m_volume = std::clamp(volume, 0, 100);
        Logger::getInstance().log(LogLevel::INFO,
            std::format("Device volume set to: {}", m_volume));
    }

    [[nodiscard]] virtual bool getPowerState() const { return m_powered; }
    [[nodiscard]] virtual int getVolumeLevel() const { return m_volume; }
};

// concrete implementation for a tv
class TvDevice final : public DeviceImplementation {
public:
    void setPower(bool state) override {
        DeviceImplementation::setPower(state);
        Logger::getInstance().log(LogLevel::INFO,
            std::format("TV specific power handling: {}", state));
    }
};

// concrete implementation for a radio
class RadioDevice final : public DeviceImplementation {
public:
    void setVolumeLevel(int volume) override {
        DeviceImplementation::setVolumeLevel(volume);
        Logger::getInstance().log(LogLevel::INFO,
            std::format("Radio specific volume handling: {}", volume));
    }
};

// refined abstraction - basic remote
class BasicRemote : public RemoteControl {
public:
    explicit BasicRemote(std::shared_ptr<DeviceImplementation> device)
        : RemoteControl(std::move(device)) {}

    void togglePower() override {
        m_device->setPower(!m_device->getPowerState());
    }

    void setVolume(int volume) override {
        m_device->setVolumeLevel(volume);
    }

    [[nodiscard]] bool isPowered() const override {
        return m_device->getPowerState();
    }

    [[nodiscard]] int getVolume() const override {
        return m_device->getVolumeLevel();
    }
};

// refined abstraction - advanced remote with additional features
class AdvancedRemote final : public BasicRemote {
public:
    explicit AdvancedRemote(std::shared_ptr<DeviceImplementation> device)
        : BasicRemote(std::move(device)) {}

    // additional feature - mute
    void mute() {
        m_device->setVolumeLevel(0);
        LOG_INFO("Advanced remote: Mute activated");
    }
};

// comprehensive test suite
void runTests() {
    LOG_INFO("Starting Bridge Pattern Tests");

    // test tv with basic remote
    {
        const auto tv = std::make_shared<TvDevice>();
        BasicRemote tvRemote(tv);

        // test power toggling
        assert(!tvRemote.isPowered() && "TV should start powered off");
        tvRemote.togglePower();
        assert(tvRemote.isPowered() && "TV should be powered on after toggle");
        tvRemote.togglePower();
        assert(!tvRemote.isPowered() && "TV should be powered off after second toggle");

        // test volume control
        tvRemote.setVolume(50);
        assert(tvRemote.getVolume() == 50 && "TV volume should be 50");
        tvRemote.setVolume(150);  // should clamp to 100
        assert(tvRemote.getVolume() == 100 && "TV volume should be clamped to 100");

        LOG_INFO("Basic TV remote tests passed");
    }

    // test radio with advanced remote
    {
        const auto radio = std::make_shared<RadioDevice>();
        AdvancedRemote radioRemote(radio);

        // test advanced features
        radioRemote.setVolume(75);
        assert(radioRemote.getVolume() == 75 && "Radio volume should be 75");
        radioRemote.mute();
        assert(radioRemote.getVolume() == 0 && "Radio should be muted (volume 0)");

        LOG_INFO("Advanced radio remote tests passed");
    }

    LOG_INFO("All Bridge Pattern Tests Completed Successfully");
}

int main() {
    try {
        runTests();
        return 0;
    } catch (const std::exception& e) {
        Logger::getInstance().log(LogLevel::ERROR,
            std::format("Test failed with exception: {}", e.what()));
        return 1;
    }
}
