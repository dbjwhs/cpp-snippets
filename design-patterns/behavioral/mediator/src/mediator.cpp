// MIT License
// Copyright (c) 2025 dbjwhs

#ifndef MEDIATOR_PATTERN_HPP
#define MEDIATOR_PATTERN_HPP

#include <cassert>
#include <format>
#include <unordered_map>
#include "../../../../headers/project_utils.hpp"

// mediator pattern - history and usage
// the mediator pattern is a behavioral design pattern introduced in the "gang of four" design patterns book (1994).
// it promotes loose coupling by keeping objects from referring to each other explicitly.
// instead, objects communicate through a mediator object that encapsulates how a set of objects interact.
//
// history:
// - originated from the work of erich gamma, richard helm, ralph johnson, and john vlissides
// - formalized in their book "design patterns: elements of reusable object-oriented software"
// - named after a real-world mediator - a neutral party that assists in negotiations
// - influenced by the law of demeter (principle of least knowledge)
//
// how it works:
// - defines an object (the mediator) that encapsulates how a set of objects interact
// - objects no longer communicate directly with each other, but instead communicate through the mediator
// - this promotes a many-to-many relationship to be replaced with a many-to-one relationship
// - reduces the dependencies between communicating objects, thereby reducing coupling
//
// common usages:
// - gui development: coordinating interactions between ui components
// - air traffic control systems: managing communications between aircraft without direct contact
// - chat applications: managing message distribution between participants
// - complex forms: coordinating validation and data flow between form elements
// - middleware systems: managing communications between various system components
// - event management systems: distributing events to appropriate handlers

// forward declarations
class Colleague;

// abstract mediator interface that defines how colleagues communicate
class Mediator : public std::enable_shared_from_this<Mediator> {
public:
    // virtual destructor for proper cleanup in derived classes
    virtual ~Mediator() = default;

    // method to register a colleague with the mediator
    virtual void registerColleague(std::shared_ptr<Colleague> colleague) = 0;

    // method to send a message from one colleague to others
    virtual void sendMessage(const std::string& message, const std::shared_ptr<Colleague>& sender) = 0;
};

// abstract colleague class that communicates with other colleagues through mediator
class Colleague : public std::enable_shared_from_this<Colleague> {
protected:
    // reference to the mediator through which all communication happens
    std::weak_ptr<Mediator> m_mediator;

    // unique identifier for the colleague
    std::string m_id;

    // last message received by this colleague
    std::string m_lastMessage;

    // flag to track if a new message was received
    bool m_receivedNewMessage = false;

public:
    // constructor taking a mediator and an id
    Colleague(std::weak_ptr<Mediator> mediator, std::string id)
        : m_mediator(std::move(mediator)), m_id(std::move(id)) {}

    // virtual destructor for proper cleanup in derived classes
    virtual ~Colleague() = default;

    // getter for colleague id
    std::string getId() const { return m_id; }

    // getter for last received message
    std::string getLastMessage() const { return m_lastMessage; }

    // check if a new message was received (pure query)
    [[nodiscard]] bool hasNewMessage() const {
        return m_receivedNewMessage;
    }

    // reset the message flag (pure command)
    void resetMessageFlag() {
        m_receivedNewMessage = false;
    }

    // method to send a message via the mediator
    virtual void send(const std::string& message) = 0;

    // method to receive a message from the mediator
    virtual void receive(const std::string& message) = 0;
};

// concrete colleague type a
class ConcreteColleagueA : public Colleague {
public:
    // constructor passing mediator and id to base class
    ConcreteColleagueA(std::weak_ptr<Mediator> mediator, const std::string& id)
        : Colleague(std::move(mediator), id) {}

    // implementation of send method
    void send(const std::string& message) override {
        Logger::getInstance().log(LogLevel::INFO, std::format("Colleague {} sends: {}", m_id, message));
        if (const auto mediator = m_mediator.lock()) {
            mediator->sendMessage(message, shared_from_this());
        }
    }

    // implementation of receive method
    void receive(const std::string& message) override {
        m_lastMessage = message;
        m_receivedNewMessage = true;
        Logger::getInstance().log(LogLevel::INFO, std::format("Colleague {} received: {}", m_id, message));
    }
};

// concrete colleague type b
class ConcreteColleagueB final : public Colleague {
public:
    // constructor passing mediator and id to base class
    ConcreteColleagueB(std::weak_ptr<Mediator> mediator, const std::string& id)
        : Colleague(std::move(mediator), id) {}

    // implementation of send method
    void send(const std::string& message) override {
        Logger::getInstance().log(LogLevel::INFO, std::format("Colleague {} sends: {}", m_id, message));
        if (const auto mediator = m_mediator.lock()) {
            mediator->sendMessage(message, shared_from_this());
        }
    }

    // implementation of receive method
    void receive(const std::string& message) override {
        m_lastMessage = message;
        m_receivedNewMessage = true;
        Logger::getInstance().log(LogLevel::INFO, std::format("Colleague {} received: {}", m_id, message));
    }
};

// concrete mediator implementation that handles communication between colleagues
class ConcreteMediator : public Mediator {
private:
    // collection of registered colleagues
    std::vector<std::shared_ptr<Colleague>> m_colleagues;

public:
    // implementation of register colleague method
    void registerColleague(std::shared_ptr<Colleague> colleague) override {
        Logger::getInstance().log(LogLevel::INFO, std::format("Mediator: Registering colleague {}", colleague->getId()));
        m_colleagues.push_back(std::move(colleague));
    }

    // implementation of send message method
    void sendMessage(const std::string& message, const std::shared_ptr<Colleague>& sender) override {
        Logger::getInstance().log(LogLevel::INFO, std::format("Mediator: Distributing message from {}", sender->getId()));

        // distribute the message to all colleagues except the sender
        for (const auto& colleague : m_colleagues) {
            if (colleague != sender) {
                colleague->receive(message);
            }
        }
    }
};

// enhanced mediator with filtering capabilities
class FilteringMediator final : public Mediator {
private:
    // map to store colleagues with their ids for quick lookup
    std::unordered_map<std::string, std::shared_ptr<Colleague>> m_colleaguesMap;

    // map to store colleague communication rules (who can talk to whom)
    std::unordered_map<std::string, std::vector<std::string>> m_communicationRules;

public:
    // implementation of register colleague method
    void registerColleague(std::shared_ptr<Colleague> colleague) override {
        Logger::getInstance().log(LogLevel::INFO, std::format("FilteringMediator: Registering colleague {}", colleague->getId()));
        m_colleaguesMap[colleague->getId()] = std::move(colleague);
    }

    // method to define communication rules between colleagues
    void setCommRules(const std::string& senderId, const std::vector<std::string>& receiverIds) {
        Logger::getInstance().log(LogLevel::INFO, std::format("FilteringMediator: Setting communication rules for {}", senderId));
        m_communicationRules[senderId] = receiverIds;
    }

    // implementation of send message method with filtering based on rules
    void sendMessage(const std::string& message, const std::shared_ptr<Colleague>& sender) override {
        Logger::getInstance().log(LogLevel::INFO, std::format("FilteringMediator: Processing message from {}", sender->getId()));

        // get sender's id
        const std::string senderId = sender->getId();

        // check if sender has communication rules
        if (m_communicationRules.contains(senderId)) {
            // get a list of allowed receivers
            const auto& allowedReceivers = m_communicationRules[senderId];

            // send a message only to allow receivers
            for (const auto& receiverId : allowedReceivers) {
                auto it = m_colleaguesMap.find(receiverId);
                if (it != m_colleaguesMap.end()) {
                    Logger::getInstance().log(LogLevel::INFO, std::format("FilteringMediator: Sending to {}", receiverId));
                    it->second->receive(message);
                } else {
                    Logger::getInstance().log(LogLevel::INFO, std::format("FilteringMediator: Receiver {} not found", receiverId));
                }
            }
        } else {
            // if no specific rules, broadcast to all except sender
            for (const auto& [id, colleague] : m_colleaguesMap) {
                if (id != senderId) {
                    Logger::getInstance().log(LogLevel::INFO, std::format("FilteringMediator: Broadcasting to {}", id));
                    colleague->receive(message);
                }
            }
        }
    }
};

// chat room example: a more complex application of the mediator pattern
class ChatUser;

// chat room mediator interface
class ChatRoomMediator {
public:
    virtual ~ChatRoomMediator() = default;
    virtual void addUser(std::shared_ptr<ChatUser> user) = 0;
    virtual void sendMessage(const std::string& message, const std::shared_ptr<ChatUser>& sender) = 0;
    virtual void sendPrivateMessage(const std::string& message
        , const std::shared_ptr<ChatUser>& sender
        , const std::string& receiverId) = 0;
};

// chat user class
class ChatUser : public std::enable_shared_from_this<ChatUser> {
private:
    // reference to the chat room mediator
    std::shared_ptr<ChatRoomMediator> m_mediator;

    // user's name
    std::string m_name;

    // collection of received messages
    std::vector<std::string> m_receivedMessages;

public:
    // constructor taking a mediator and name
    ChatUser(std::shared_ptr<ChatRoomMediator> mediator, std::string name)
        : m_mediator(std::move(mediator)), m_name(std::move(name)) {}

    // method to register with mediator
    void registerWithMediator() {
        if (m_mediator) {
            m_mediator->addUser(shared_from_this());
        }
    }

    // getter for user's name
    std::string getName() const {
        return m_name;
    }

    // method to send a message to all users
    void broadcast(const std::string& message) {
        if (m_mediator) {
            Logger::getInstance().log(LogLevel::INFO, std::format("User {} broadcasts: {}", m_name, message));
            m_mediator->sendMessage(message, shared_from_this());
        }
    }

    // method to send a private message to a specific user
    void sendPrivate(const std::string& message, const std::string& receiverName) {
        if (m_mediator) {
            Logger::getInstance().log(LogLevel::INFO, std::format("User {} sends private message to {}: {}",
                m_name, receiverName, message));
            m_mediator->sendPrivateMessage(message, shared_from_this(), receiverName);
        }
    }

    // method to receive a message
    void receive(const std::string& message) {
        m_receivedMessages.push_back(message);
        Logger::getInstance().log(LogLevel::INFO, std::format("User {} received: {}", m_name, message));
    }

    // method to get all received messages
    const std::vector<std::string>& getReceivedMessages() const {
        return m_receivedMessages;
    }

    // method to get the last received message
    std::string getLastMessage() const {
        return m_receivedMessages.empty() ? "" : m_receivedMessages.back();
    }

    // method to clearly receive messages
    void clearMessages() {
        m_receivedMessages.clear();
    }
};

// concrete chat room mediator implementation
class ChatRoom final : public ChatRoomMediator, public std::enable_shared_from_this<ChatRoom> {
private:
    // collection of users by their names
    std::unordered_map<std::string, std::shared_ptr<ChatUser>> m_users;

public:
    // implementation of add user method
    void addUser(std::shared_ptr<ChatUser> user) override {
        Logger::getInstance().log(LogLevel::INFO, std::format("ChatRoom: Adding user {}", user->getName()));
        m_users[user->getName()] = std::move(user);
    }

    // implementation of send message method (broadcast)
    void sendMessage(const std::string& message, const std::shared_ptr<ChatUser>& sender) override {
        const std::string formattedMsg = std::format("[{}]: {}", sender->getName(), message);
        Logger::getInstance().log(LogLevel::INFO, std::format("ChatRoom: Broadcasting message from {}", sender->getName()));

        // send to all users except the sender
        for (const auto& [name, user] : m_users) {
            if (user != sender) {
                user->receive(formattedMsg);
            }
        }
    }

    // implementation of send private message method
    void sendPrivateMessage(const std::string& message, const std::shared_ptr<ChatUser>& sender, const std::string& receiverId) override {
        // check if the receiver exists
        if (m_users.contains(receiverId)) {
            const std::string formattedMsg = std::format("[Private from {}]: {}", sender->getName(), message);
            Logger::getInstance().log(LogLevel::INFO, std::format("ChatRoom: Delivering private message from {} to {}",
                sender->getName(), receiverId));
            m_users[receiverId]->receive(formattedMsg);
        } else {
            Logger::getInstance().log(LogLevel::INFO, std::format("ChatRoom: User {} not found", receiverId));
        }
    }
};

// main function with comprehensive testing
int main() {
    Logger::getInstance().log(LogLevel::INFO, "Starting Mediator Pattern Tests");

    // test 1: basic mediator functionality
    {
        Logger::getInstance().log(LogLevel::INFO, "Test 1: Basic Mediator Functionality");

        // create mediator
        auto mediator = std::make_shared<ConcreteMediator>();

        // create colleagues
        const auto colleagueA = std::make_shared<ConcreteColleagueA>(mediator, "A");
        const auto colleagueB = std::make_shared<ConcreteColleagueB>(mediator, "B");
        const auto colleagueC = std::make_shared<ConcreteColleagueA>(mediator, "C");

        // register colleagues with mediator
        mediator->registerColleague(colleagueA);
        mediator->registerColleague(colleagueB);
        mediator->registerColleague(colleagueC);

        // send a message from a
        colleagueA->send("Hello from A");

        // verify that b and c received the message
        assert(colleagueB->getLastMessage() == "Hello from A" && "B should receive message from A");
        assert(colleagueC->getLastMessage() == "Hello from A" && "C should receive message from A");

        // send a message from b
        colleagueB->send("Response from B");

        // verify that a and c received the message
        assert(colleagueA->getLastMessage() == "Response from B" && "A should receive message from B");
        assert(colleagueC->getLastMessage() == "Response from B" && "C should receive message from B");

        Logger::getInstance().log(LogLevel::INFO, "Test 1: Passed");
    }

    // test 2: filtering mediator
    {
        Logger::getInstance().log(LogLevel::INFO, "Test 2: Filtering Mediator");

        // create filtering mediator
        auto mediator = std::make_shared<FilteringMediator>();

        // create colleagues
        const auto colleagueA = std::make_shared<ConcreteColleagueA>(mediator, "A");
        const auto colleagueB = std::make_shared<ConcreteColleagueB>(mediator, "B");
        const auto colleagueC = std::make_shared<ConcreteColleagueA>(mediator, "C");
        const auto colleagueD = std::make_shared<ConcreteColleagueB>(mediator, "D");

        // register colleagues with mediator
        mediator->registerColleague(colleagueA);
        mediator->registerColleague(colleagueB);
        mediator->registerColleague(colleagueC);
        mediator->registerColleague(colleagueD);

        // set communication rules: a can talk to b and c, but not d
        mediator->setCommRules("A", {"B", "C"});

        // set communication rules: b can talk only to d
        mediator->setCommRules("B", {"D"});

        // send a message from a
        colleagueA->send("Message from A");

        // verify that b and c received the message, but d did not
        assert(colleagueB->getLastMessage() == "Message from A" && "B should receive message from A");
        assert(colleagueC->getLastMessage() == "Message from A" && "C should receive message from A");
        assert(colleagueD->getLastMessage().empty() && "D should not receive message from A");

        // reset new message flags for all colleagues
        colleagueA->resetMessageFlag();
        colleagueB->resetMessageFlag();
        colleagueC->resetMessageFlag();
        colleagueD->resetMessageFlag();

        // send a message from b
        colleagueB->send("Message from B");

        // verify that only d received the message
        assert(!colleagueA->hasNewMessage() && "A should not receive message from B");
        assert(!colleagueC->hasNewMessage() && "C should not receive message from B");
        assert(colleagueD->hasNewMessage() && "D should receive message from B");
        assert(colleagueD->getLastMessage() == "Message from B" && "D's last message should be from B");

        // reset flags after verification
        colleagueA->resetMessageFlag();
        colleagueC->resetMessageFlag();
        colleagueD->resetMessageFlag();

        // send a message from c (no specific rules should go to all except c)
        colleagueC->send("Message from C");

        // verify that a, b, and d received the message
        assert(colleagueA->hasNewMessage() && "A should receive message from C");
        assert(colleagueB->hasNewMessage() && "B should receive message from C");
        assert(colleagueD->hasNewMessage() && "D should receive message from C");
        assert(colleagueA->getLastMessage() == "Message from C" && "A's last message should be from C");
        assert(colleagueB->getLastMessage() == "Message from C" && "B's last message should be from C");
        assert(colleagueD->getLastMessage() == "Message from C" && "D's last message should be from C");

        Logger::getInstance().log(LogLevel::INFO, "Test 2: Passed");
    }

    // test 3: chat room mediator
    {
        Logger::getInstance().log(LogLevel::INFO, "Test 3: Chat Room Mediator");

        // create chat room
        auto chatRoom = std::make_shared<ChatRoom>();

        // create users
        const auto alice = std::make_shared<ChatUser>(chatRoom, "Alice");
        const auto bob = std::make_shared<ChatUser>(chatRoom, "Bob");
        const auto charlie = std::make_shared<ChatUser>(chatRoom, "Charlie");
        const auto diana = std::make_shared<ChatUser>(chatRoom, "Diana");

        // register users with the chat room
        alice->registerWithMediator();
        bob->registerWithMediator();
        charlie->registerWithMediator();
        diana->registerWithMediator();

        // alice broadcasts a message
        alice->broadcast("Hello everyone!");

        // verify that all other users received the message
        assert(bob->getLastMessage() == "[Alice]: Hello everyone!" && "Bob should receive Alice's broadcast");
        assert(charlie->getLastMessage() == "[Alice]: Hello everyone!" && "Charlie should receive Alice's broadcast");
        assert(diana->getLastMessage() == "[Alice]: Hello everyone!" && "Diana should receive Alice's broadcast");

        // clear messages for all users
        alice->clearMessages();
        bob->clearMessages();
        charlie->clearMessages();
        diana->clearMessages();

        // bob sends a private message to charlie
        bob->sendPrivate("Hey, can we talk?", "Charlie");

        // verify that only charlie received the message
        assert(alice->getReceivedMessages().empty() && "Alice should not receive Bob's private message to Charlie");
        assert(charlie->getLastMessage() == "[Private from Bob]: Hey, can we talk?" && "Charlie should receive Bob's private message");
        assert(diana->getReceivedMessages().empty() && "Diana should not receive Bob's private message to Charlie");

        // charlie broadcasts a response without clearing previous messages
        charlie->broadcast("Sure, what's up?");

        // verify that all other users received the message
        assert(alice->getLastMessage() == "[Charlie]: Sure, what's up?" && "Alice should receive Charlie's broadcast");
        assert(bob->getLastMessage() == "[Charlie]: Sure, what's up?" && "Bob should receive Charlie's broadcast");
        assert(diana->getLastMessage() == "[Charlie]: Sure, what's up?" && "Diana should receive Charlie's broadcast");

        // test sending to non-existent user
        bob->sendPrivate("This won't be delivered", "Eve");

        // verify message counts
        assert(alice->getReceivedMessages().size() == 1 && "Alice should have exactly one message");
        assert(bob->getReceivedMessages().size() == 1 && "Bob should have exactly one message");
        assert(charlie->getReceivedMessages().size() == 1 && "Charlie should have exactly one message (from Bob)");
        assert(diana->getReceivedMessages().size() == 1 && "Diana should have exactly one message");

        Logger::getInstance().log(LogLevel::INFO, "Test 3: Passed");
    }

    Logger::getInstance().log(LogLevel::INFO, "All Mediator Pattern Tests Passed");

    return 0;
}

#endif // mediator_pattern_hpp
