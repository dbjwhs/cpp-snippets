// MIT License
// Copyright (c) 2025 dbjwhs

#ifndef STATE_PATTERN_HPP
#define STATE_PATTERN_HPP

#include <memory>
#include <string>
#include <cassert>
#include <vector>
#include <format>
#include "../../../../headers/project_utils.hpp"

// state design pattern
//
// history:
// the state pattern is a behavioral design pattern first described in the 
// "gang of four" book (design patterns: elements of reusable object-oriented software)
// published in 1994 by erich gamma, richard helm, ralph johnson, and john vlissides.
// the pattern originated from the concept of finite state machines in computer science.
//
// purpose:
// the state pattern allows an object to alter its behavior when its internal state changes.
// the object appears to change its class, as it delegates state-specific behavior to the 
// current state object rather than implementing all behaviors itself.
//
// structure:
// 1. context: maintains an instance of a concrete state that defines the current state
// 2. state: defines an interface for encapsulating the behavior associated with a particular state
// 3. concrete states: implement behavior associated with a state of the context
//
// common usage examples:
// 1. workflow or approval processes (draft, review, approved states)
// 2. media players (playing, paused, stopped states)
// 3. network connections (connected, disconnected, connecting states)
// 4. order processing systems (new, paid, shipped, delivered states)
// 5. game character behavior (idle, walking, running, jumping states)
//
// advantages:
// - eliminates large conditional statements
// - makes state transitions explicit
// - state-specific behavior is localized to concrete state classes
// - new states can be added without changing existing state or context classes
//
// disadvantages:
// - can introduce many small classes
// - can be overkill for simple state management scenarios
//
// class relationships visual diagram:
//
// ┌─────────────────────────────────────────────────────────────────────┐
// │                                                                     │
// │                            document                                 │
// │ (context)                                                           │
// │ ┌─────────────────────────────────────────────────────────────┐     │
// │ │ - m_currentstate: std::unique_ptr<documentstate>            │     │
// │ │ - m_content: std::string                                    │     │
// │ │ - m_author: std::string                                     │     │
// │ │ - m_reviewcomments: std::vector<std::string>                │     │
// │ ├─────────────────────────────────────────────────────────────┤     │
// │ │ + draft()                                                    │    │
// │ │ + review()                                                   │    │
// │ │ + approve()                                                  │    │
// │ │ + reject()                                                   │    │
// │ │ + changestate(std::unique_ptr<documentstate>)                │    │
// │ └──────────────────────────────────────────────────────────────┘    │
// │                             │                                       │
// │                             │ has-a                                 │
// │                             ▼                                       │
// │  ┌───────────────────────────────────────────────────────────┐      │
// │  │                     <<interface>>                         │      │
// │  │                     documentstate                         │      │
// │  ├───────────────────────────────────────────────────────────┤      │
// │  │ + draft(document&)                                        │      │
// │  │ + review(document&)                                       │      │
// │  │ + approve(document&)                                      │      │
// │  │ + reject(document&)                                       │      │
// │  │ + getname(): std::string                                  │      │
// │  └───────────────────────────────────────────────────────────┘      │
// │                             ▲                                       │
// │                             │ implements                            │
// │                             │                                       │
// │  ┌────────────────────┬─────┴───────────┬─────────────────┐         │
// │  │                    │                 │                 │         │
// │  ▼                    ▼                 ▼                 ▼         │
// │ ┌────────────┐ ┌────────────┐ ┌─────────────────┐ ┌─────────────┐   │
// │ │ draftstate │ │reviewstate │ │ approvedstate   │ │rejectedstate│   │
// │ └────────────┘ └────────────┘ └─────────────────┘ └─────────────┘   │
// │                                                                     │
// └─────────────────────────────────────────────────────────────────────┘
//
// state transition diagram:
//
// ┌─────────────┐  review()   ┌─────────────┐ approve() ┌─────────────┐
// │             │------------>│             │---------->│             │
// │  draftstate │             │ reviewstate │           │approvedstate│
// │             │<------------|             │<----------|             │
// └─────────────┘   draft()   └─────────────┘  review() └─────────────┘
//       ▲                      /     ▲                      │
//       │                     /      │                      │
//       │                    /reject()                      │
//       │                   /        │                      │
//       │          reject()/         │                      │
//       │                 /          │                      │
//       │                ▼           │                      │
//       │         ┌─────────────┐    │                      │
//       │         │             │    │                      │
//       └---------|rejectedstate|<---┘                      │
//         draft() │             │<--------------------------┘
//                 └─────────────┘    reject()
//
// flow of execution:
//
// 1. client code creates a document (context)
// 2. document starts in draft state
// 3. client calls operations on the document object:
//    * document.draft()
//    * document.review()
//    * document.approve()
//    * document.reject()
// 4. each operation gets delegated to the current state object
// 5. state object performs operation and may change document's state
// 6. document delegates to different state objects as it transitions

// forward declaration of context class
class Document;

// state interface
class DocumentState {
public:
    // virtual destructor for proper cleanup in derived classes
    virtual ~DocumentState() = default;
    
    // state operations
    virtual void draft(Document& document) = 0;
    virtual void review(Document& document) = 0;
    virtual void approve(Document& document) = 0;
    virtual void reject(Document& document) = 0;
    
    // getter for state name
    [[nodiscard]] virtual std::string getName() const = 0;
};

// context class
class Document {
private:
    // current state of the document
    std::unique_ptr<DocumentState> m_currentState;
    
    // document metadata
    std::string m_content;
    std::string m_author;
    std::vector<std::string> m_reviewComments;

public:
    // constructor
    explicit Document(std::string author);
    
    // state transition methods
    void draft();
    void review();
    void approve();
    void reject();
    
    // content management
    void setContent(const std::string& content);
    [[nodiscard]] std::string getContent() const;
    
    // comment management
    void addReviewComment(const std::string& comment);
    [[nodiscard]] const std::vector<std::string>& getReviewComments() const;
    
    // state management
    void changeState(std::unique_ptr<DocumentState> newState);
    [[nodiscard]] std::string getCurrentStateName() const;
    
    // getter for author
    [[nodiscard]] std::string getAuthor() const;
};

// concrete state: draft state
class DraftState final : public DocumentState {
public:
    // implement state operations
    void draft(Document& document) override;
    void review(Document& document) override;
    void approve(Document& document) override;
    void reject(Document& document) override;
    
    // return state name
    [[nodiscard]] std::string getName() const override;
};

// concrete state: review state
class ReviewState final : public DocumentState {
public:
    // implement state operations
    void draft(Document& document) override;
    void review(Document& document) override;
    void approve(Document& document) override;
    void reject(Document& document) override;
    
    // return state name
    [[nodiscard]] std::string getName() const override;
};

// concrete state: approved state
class ApprovedState final : public DocumentState {
public:
    // implement state operations
    void draft(Document& document) override;
    void review(Document& document) override;
    void approve(Document& document) override;
    void reject(Document& document) override;
    
    // return state name
    [[nodiscard]] std::string getName() const override;
};

// concrete state: rejected state
class RejectedState : public DocumentState {
public:
    // implement state operations
    void draft(Document& document) override;
    void review(Document& document) override;
    void approve(Document& document) override;
    void reject(Document& document) override;
    
    // return state name
    [[nodiscard]] std::string getName() const override;
};

// document constructor implementation
Document::Document(std::string author) : m_author(std::move(author)) {
    // initialize with draft state
    m_currentState = std::make_unique<DraftState>();
    LOG_INFO(std::format("Document created by {}, initial state: {}",
                                                        m_author, m_currentState->getName()));
}

// document state transition methods implementation
void Document::draft() {
    m_currentState->draft(*this);
}

void Document::review() {
    m_currentState->review(*this);
}

void Document::approve() {
    m_currentState->approve(*this);
}

void Document::reject() {
    m_currentState->reject(*this);
}

// document content management implementation
void Document::setContent(const std::string& content) {
    m_content = content;
    LOG_INFO("Document content updated");
}

std::string Document::getContent() const {
    return m_content;
}

// document comment management implementation
void Document::addReviewComment(const std::string& comment) {
    m_reviewComments.push_back(comment);
    LOG_INFO(std::format("Review comment added: {}", comment));
}

const std::vector<std::string>& Document::getReviewComments() const {
    return m_reviewComments;
}

// document state management implementation
void Document::changeState(std::unique_ptr<DocumentState> newState) {
    LOG_INFO(std::format("State changing from {} to {}", 
                                                        m_currentState->getName(), newState->getName()));
    m_currentState = std::move(newState);
}

std::string Document::getCurrentStateName() const {
    return m_currentState->getName();
}

// document author getter implementation
std::string Document::getAuthor() const {
    return m_author;
}

// draft state implementation
void DraftState::draft(Document& document) {
    LOG_INFO("Already in Draft state");
}

void DraftState::review(Document& document) {
    // check if a document has content before transitioning to review
    if (document.getContent().empty()) {
        Logger::StderrSuppressionGuard stderr_guard;
        LOG_ERROR("Cannot review empty document");
        return;
    }
    document.changeState(std::make_unique<ReviewState>());
}

void DraftState::approve(Document& document) {
    Logger::StderrSuppressionGuard stderr_guard;
    LOG_ERROR("Cannot approve document in Draft state");
}

void DraftState::reject(Document& document) {
    Logger::StderrSuppressionGuard stderr_guard;
    LOG_ERROR("Cannot reject document in Draft state");
}

std::string DraftState::getName() const {
    return "Draft";
}

// review state implementation
void ReviewState::draft(Document& document) {
    document.changeState(std::make_unique<DraftState>());
}

void ReviewState::review(Document& document) {
    Logger::StderrSuppressionGuard stderr_guard;
    LOG_ERROR("Already in Review state");
}

void ReviewState::approve(Document& document) {
    document.changeState(std::make_unique<ApprovedState>());
}

void ReviewState::reject(Document& document) {
    document.changeState(std::make_unique<RejectedState>());
}

std::string ReviewState::getName() const {
    return "Review";
}

// approved state implementation
void ApprovedState::draft(Document& document) {
    document.changeState(std::make_unique<DraftState>());
}

void ApprovedState::review(Document& document) {
    document.changeState(std::make_unique<ReviewState>());
}

void ApprovedState::approve(Document& document) {
    Logger::StderrSuppressionGuard stderr_guard;
    LOG_ERROR("Already in Approved state");
}

void ApprovedState::reject(Document& document) {
    document.changeState(std::make_unique<RejectedState>());
}

std::string ApprovedState::getName() const {
    return "Approved";
}

// rejected state implementation
void RejectedState::draft(Document& document) {
    document.changeState(std::make_unique<DraftState>());
}

void RejectedState::review(Document& document) {
    Logger::StderrSuppressionGuard stderr_guard;
    LOG_ERROR("Cannot review rejected document, must be drafted first");
}

void RejectedState::approve(Document& document) {
    Logger::StderrSuppressionGuard stderr_guard;
    LOG_ERROR("Cannot approve rejected document, must be drafted and reviewed first");
}

void RejectedState::reject(Document& document) {
    Logger::StderrSuppressionGuard stderr_guard;
    LOG_ERROR("Already in Rejected state");
}

std::string RejectedState::getName() const {
    return "Rejected";
}

// main function with comprehensive testing
int main() {
    LOG_INFO("Starting State Pattern Test");

    // create a document
    Document doc("John Doe");
    
    // test initial state
    assert(doc.getCurrentStateName() == "Draft");
    LOG_INFO(std::format("Initial state: {}", doc.getCurrentStateName()));
    
    // test invalid transitions from draft state
    doc.approve();  // should log error and not change state
    assert(doc.getCurrentStateName() == "Draft");
    
    doc.reject();   // should log error and not change state
    assert(doc.getCurrentStateName() == "Draft");
    
    // test attempted review with empty content
    doc.review();   // should log error about empty content
    assert(doc.getCurrentStateName() == "Draft");
    
    // add content and successfully transition to review
    doc.setContent("This is a sample document for testing the State Pattern.");
    doc.review();
    assert(doc.getCurrentStateName() == "Review");
    LOG_INFO(std::format("After adding content and reviewing, state: {}", 
                                                        doc.getCurrentStateName()));
    
    // add review comments
    doc.addReviewComment("Needs more details in section 2.");
    doc.addReviewComment("Well-structured overall.");
    assert(doc.getReviewComments().size() == 2);
    
    // test transitions from review state
    doc.approve();
    assert(doc.getCurrentStateName() == "Approved");
    LOG_INFO(std::format("After approval, state: {}", doc.getCurrentStateName()));
    
    // test transitions from approved state
    doc.review();
    assert(doc.getCurrentStateName() == "Review");
    
    // test rejection process
    doc.reject();
    assert(doc.getCurrentStateName() == "Rejected");
    LOG_INFO(std::format("After rejection, state: {}", doc.getCurrentStateName()));
    
    // test transitions from rejected state
    doc.review();  // should log error - can't review a rejected document
    assert(doc.getCurrentStateName() == "Rejected");
    
    doc.approve(); // should log error - can't approve rejected document
    assert(doc.getCurrentStateName() == "Rejected");
    
    // test returning to draft from rejected
    doc.draft();
    assert(doc.getCurrentStateName() == "Draft");
    LOG_INFO(std::format("After returning to draft, state: {}", 
                                                        doc.getCurrentStateName()));
    
    // comprehensive workflow test
    LOG_INFO("Starting comprehensive workflow test");
    
    // draft -> review -> rejected -> draft -> review -> approved
    doc.review();
    assert(doc.getCurrentStateName() == "Review");
    
    doc.reject();
    assert(doc.getCurrentStateName() == "Rejected");
    
    doc.draft();
    assert(doc.getCurrentStateName() == "Draft");
    
    // update content before review
    doc.setContent("Revised document with additional details in section 2.");
    
    doc.review();
    assert(doc.getCurrentStateName() == "Review");
    
    doc.addReviewComment("Much better, approved.");
    
    doc.approve();
    assert(doc.getCurrentStateName() == "Approved");
    
    LOG_INFO("All tests passed successfully!");
    
    return 0;
}

#endif // state_pattern_hpp
