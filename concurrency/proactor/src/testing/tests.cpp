// MIT License
// Copyright (c) 2025 dbjwhs

#include "../../include/proactor.hpp"
#include "../../include/socket.hpp"
#include "../../include/buffer.hpp"
#include "../../include/logger.hpp"
#include <format>
#include <future>
#include <thread>
#include <chrono>
#include <string>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace proactor;

// Function for running a direct socket test (bypassing proactor)
void runDirectSocketTest() {
    Logger::getInstance().log(LogLevel::INFO, "Starting direct socket test...");
    
    // Create a server socket
    Socket serverSocket = Socket::createTcp();
    if (!serverSocket.isValid()) {
        Logger::getInstance().log(LogLevel::ERROR, "Failed to create server socket");
        return;
    }
    
    // Set socket options
    Error error = serverSocket.setReuseAddress();
    if (error) {
        Logger::getInstance().log(LogLevel::ERROR, std::format("Failed to set socket options: {}", error.message()));
        return;
    }
    
    // Bind to local port
    const int serverPort = 8081;
    error = serverSocket.bind("0.0.0.0", serverPort);
    if (error) {
        Logger::getInstance().log(LogLevel::ERROR, std::format("Failed to bind server socket: {}", error.message()));
        return;
    }
    
    // Listen for connections
    error = serverSocket.listen(5);
    if (error) {
        Logger::getInstance().log(LogLevel::ERROR, std::format("Failed to listen on server socket: {}", error.message()));
        return;
    }
    
    Logger::getInstance().log(LogLevel::INFO, std::format("Server listening on port {}", serverPort));
    
    // Flag to indicate when all client threads are ready
    std::mutex mtx;
    std::condition_variable cv;
    bool serverReady = false;
    
    // Start server thread
    std::thread serverThread([&]() {
        // Array of client handler threads
        std::vector<std::thread> clientThreads;
        
        // Notify that we're ready to accept connections
        {
            std::lock_guard<std::mutex> lock(mtx);
            serverReady = true;
        }
        cv.notify_all();
        
        // Accept loop
        for (int i = 0; i < 5; i++) {
            auto [clientSocket, acceptError] = serverSocket.accept();
            if (acceptError) {
                Logger::getInstance().log(LogLevel::ERROR, 
                    std::format("Direct server: Accept failed: {}", acceptError.message()));
                break;
            }
            
            // Get peer address
            std::string clientAddr = "unknown";
            
            Logger::getInstance().log(LogLevel::INFO, 
                std::format("Direct server: Accepted connection from client {}", i));
            
            // Start a thread to handle this client
            clientThreads.emplace_back([clientSocket = std::move(clientSocket)]() mutable {
                // Echo loop
                char buffer[1024];
                while (true) {
                    // Read data
                    auto [bytesRead, readError] = clientSocket.read(buffer, sizeof(buffer));
                    if (readError) {
                        Logger::getInstance().log(LogLevel::ERROR, 
                            std::format("Direct server: Read failed: {}", readError.message()));
                        break;
                    }
                    
                    if (bytesRead <= 0) {
                        break;
                    }
                    
                    // Log the received data
                    std::string data(buffer, bytesRead);
                    Logger::getInstance().log(LogLevel::INFO, 
                        std::format("Direct server: Read {} bytes: {}", bytesRead, data));
                    
                    // Echo it back
                    auto [bytesWritten, writeError] = clientSocket.write(buffer, bytesRead);
                    if (writeError) {
                        Logger::getInstance().log(LogLevel::ERROR, 
                            std::format("Direct server: Write failed: {}", writeError.message()));
                        break;
                    }
                    
                    Logger::getInstance().log(LogLevel::INFO, 
                        std::format("Direct server: Echoed {} bytes", bytesWritten));
                }
                
                Logger::getInstance().log(LogLevel::INFO, "Direct server: Client thread exiting");
            });
        }
        
        // Wait for all client threads to finish
        for (auto& thread : clientThreads) {
            if (thread.joinable()) {
                thread.join();
            }
        }
        
        Logger::getInstance().log(LogLevel::INFO, "Direct server: All client threads finished");
    });
    
    // Wait for server to be ready
    {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [&serverReady]{ return serverReady; });
    }
    
    // Small delay to ensure server is listening
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // Connect 5 clients and send data
    std::vector<std::thread> clientThreads;
    for (int i = 0; i < 5; i++) {
        clientThreads.emplace_back([i, serverPort]() {
            // Create client socket
            Socket clientSocket = Socket::createTcp();
            if (!clientSocket.isValid()) {
                Logger::getInstance().log(LogLevel::ERROR, 
                    std::format("Client {}: Failed to create socket", i));
                return;
            }
            
            // Connect to server
            Error error = clientSocket.connect("127.0.0.1", serverPort);
            if (error) {
                Logger::getInstance().log(LogLevel::ERROR, 
                    std::format("Client {}: Failed to connect: {}", i, error.message()));
                return;
            }
            
            Logger::getInstance().log(LogLevel::INFO, 
                std::format("Client {}: Connected to server", i));
            
            // Send data
            std::string message = std::format("Hello from client {}!", i);
            auto [bytesWritten, writeError] = clientSocket.write(message.data(), message.size());
            if (writeError) {
                Logger::getInstance().log(LogLevel::ERROR, 
                    std::format("Client {}: Write failed: {}", i, writeError.message()));
                return;
            }
            
            Logger::getInstance().log(LogLevel::INFO, 
                std::format("Client {}: Sent {} bytes: {}", i, bytesWritten, message));
            
            // Read response
            char buffer[1024];
            auto [bytesRead, readError] = clientSocket.read(buffer, sizeof(buffer));
            if (readError) {
                Logger::getInstance().log(LogLevel::ERROR, 
                    std::format("Client {}: Read failed: {}", i, readError.message()));
                return;
            }
            
            if (bytesRead > 0) {
                std::string response(buffer, bytesRead);
                Logger::getInstance().log(LogLevel::INFO, 
                    std::format("Client {}: Received {} bytes: {}", i, bytesRead, response));
                
                // Verify echo
                if (response == message) {
                    Logger::getInstance().log(LogLevel::INFO, 
                        std::format("Client {}: Echo test passed", i));
                } else {
                    Logger::getInstance().log(LogLevel::ERROR, 
                        std::format("Client {}: Echo test failed. Expected: '{}', Got: '{}'", 
                            i, message, response));
                }
            }
        });
    }
    
    // Wait for client threads to finish
    for (auto& thread : clientThreads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    
    // Small delay to allow server to process final connections
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    
    // Close the server socket to terminate the accept loop
    serverSocket.close();
    
    // Wait for server thread to finish
    if (serverThread.joinable()) {
        serverThread.join();
    }
    
    Logger::getInstance().log(LogLevel::INFO, "Direct socket test completed");
}

// Simple function to run basic tests for the proactor pattern
void runTests() {
    Logger::getInstance().log(LogLevel::INFO, "Starting proactor pattern tests");

    // Run the direct socket test first to verify basic socket functionality
    runDirectSocketTest();

    // Create the proactor
    auto proactor = std::make_shared<Proactor>();

    // Start the proactor event loop
    proactor->start();

    // Wait a short time to allow the proactor to start
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // Log test completion
    Logger::getInstance().log(LogLevel::INFO, "Tests completed");
    
    // Stop the proactor
    proactor->stop();
}
