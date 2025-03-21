// MIT License
// Copyright (c) 2025 dbjwhs

// Include the headers for already extracted components
#include "../include/error.hpp"
#include "../include/buffer.hpp"
#include "../include/socket.hpp"
#include "../include/completion_handler.hpp"
#include "../include/async_operation.hpp"
#include "../include/operation_types.hpp"
#include "../include/event_queue.hpp"
#include "../include/proactor.hpp"
#include "../include/operations/operations.hpp"
#include "../include/logger.hpp"

// Standard library includes
#include <iostream>
#include <memory>
#include <utility>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>
#include <cassert>
#include <chrono>
#include <format>
#include <atomic>
#include <future>
#include <map>
#include <cstring>

// System includes for networking
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <cerrno>
#include <algorithm>
#include <fstream>

using namespace proactor;

//------------------------------------------------------------------------------
// Client class
//------------------------------------------------------------------------------
class Client : public std::enable_shared_from_this<Client> {
public:
    // callback type for data received
    using DataReceivedCallback = std::function<void(Buffer)>;

    // callback type for connection status
    using ConnectionStatusCallback = std::function<void(bool, const Error&)>;

    // constructor taking a proactor
    explicit Client(std::shared_ptr<Proactor> proactor)
        : m_proactor(std::move(proactor)), m_connected(false) {}

    // destructor disconnects if connected
    ~Client() {
        disconnect();
    }
    
    // Method to adopt an existing socket (for server-side clients)
    void adoptSocket(Socket socket) {
        if (m_connected) {
            disconnect();
        }
        
        m_socket = std::move(socket);
        m_connected = true;
    }

    // connect to the specified address and port
    void connect(const std::string& address, int port) {
        Logger::getInstance().log(LogLevel::INFO, 
            std::format("Client connecting to {}:{}", address, port));
            
        // disconnect if already connected
        if (m_connected) {
            disconnect();
        }

        // create a socket
        m_socket = Socket::createTcp();
        if (!m_socket.isValid()) {
            notifyConnectionStatus(false, Error(errno, "Failed to create socket"));
            return;
        }

        // create a completion handler for the connect operation
        class ConnectCompletionHandler : public CompletionHandler {
        public:
            explicit ConnectCompletionHandler(const std::shared_ptr<Client>& client)
                : m_client(client) {}

            // handle completion of the connect operation
            void handleCompletion(ssize_t result, Buffer buffer) override {
                if (auto client = m_client.lock()) {
                    if (result >= 0) {
                        client->handleConnectSuccess();
                    } else {
                        client->handleConnectFailure(Error(-1, "Connect operation failed"));
                    }
                }
            }

        private:
            std::weak_ptr<Client> m_client;
        };

        // create and initiate an asynchronous connect operation
        auto handler = std::make_shared<ConnectCompletionHandler>(shared_from_this());
        const auto op = std::make_shared<AsyncConnectOperation>(handler, m_socket, address, port);
        op->initiate(m_proactor);
    }

    // disconnect from the server
    void disconnect() {
        Logger::getInstance().log(LogLevel::INFO, "Client disconnecting");
        m_socket.close();
        m_connected = false;
    }

    // send data to the server
    void send(const std::string& data) {
        Logger::getInstance().log(LogLevel::INFO, 
            std::format("Client sending {} bytes of data", data.size()));
        Logger::getInstance().log(LogLevel::INFO, 
            std::format("Sending data: '{}'", data));
        if (!m_connected) {
            Logger::getInstance().log(LogLevel::ERROR, "Not connected");
            return;
        }

        // create a completion handler for the write operation
        class WriteCompletionHandler : public CompletionHandler {
        public:
            explicit WriteCompletionHandler(const std::shared_ptr<Client>& client)
                : m_client(client) {}

            // handle completion of the write operation
            void handleCompletion(ssize_t result, Buffer buffer) override {
                if (auto client = m_client.lock()) {
                    if (result >= 0) {
                        Logger::getInstance().log(LogLevel::INFO, 
                            std::format("Write operation completed, {} bytes written", result));
                    } else {
                        Logger::getInstance().log(LogLevel::ERROR, "Write operation failed");
                    }
                }
            }

        private:
            std::weak_ptr<Client> m_client;
        };

        // create a buffer with the data to send
        Buffer buffer(data.data(), data.size());

        // create and initiate an asynchronous write operation
        auto handler = std::make_shared<WriteCompletionHandler>(shared_from_this());
        if (const auto op = std::make_shared<AsyncWriteOperation>(handler, m_socket, std::move(buffer)); op->initiate(m_proactor)) {
            Logger::getInstance().log(LogLevel::INFO, "Write operation initiated successfully");
        } else {
            Logger::getInstance().log(LogLevel::ERROR, "Failed to initiate write operation");
        }
    }

    // read data from the server
    void receive() {
        if (!m_connected) {
            Logger::getInstance().log(LogLevel::ERROR, "Not connected");
            return;
        }

        // create a completion handler for the read operation
        class ReadCompletionHandler : public CompletionHandler {
        public:
            explicit ReadCompletionHandler(const std::shared_ptr<Client>& client)
                : m_client(client) {}

            // handle completion of the read operation
            void handleCompletion(const ssize_t result, Buffer buffer) override {
                if (const auto client = m_client.lock()) {
                    if (result > 0) {
                        client->handleDataReceived(std::move(buffer));
                    } else if (result == 0) {
                        Logger::getInstance().log(LogLevel::INFO, "Connection closed by peer");
                        client->disconnect();
                    } else {
                        Logger::getInstance().log(LogLevel::ERROR, "Read operation failed");
                    }
                }
            }

        private:
            std::weak_ptr<Client> m_client;
        };

        // create and initiate an asynchronous read operation
        auto handler = std::make_shared<ReadCompletionHandler>(shared_from_this());
        if (const auto op = std::make_shared<AsyncReadOperation>(handler, m_socket); op->initiate(m_proactor)) {
            Logger::getInstance().log(LogLevel::INFO, "Read operation initiated successfully");
        } else {
            Logger::getInstance().log(LogLevel::ERROR, "Failed to initiate read operation");
        }
    }

    // set the callback for data received
    void setDataReceivedCallback(DataReceivedCallback callback) {
        m_dataReceivedCallback = std::move(callback);
    }

    // set the callback for connection status
    void setConnectionStatusCallback(ConnectionStatusCallback callback) {
        m_connectionStatusCallback = std::move(callback);
    }

private:
    // handle successful connection
    void handleConnectSuccess() {
        Logger::getInstance().log(LogLevel::INFO, "Connection established");
        m_connected = true;
        notifyConnectionStatus(true, Error(0, ""));
    }

    // handle failed connection
    void handleConnectFailure(const Error& error) {
        Logger::getInstance().log(LogLevel::ERROR, 
            std::format("Connection failed: {}", error.message()));
        m_connected = false;
        notifyConnectionStatus(false, error);
    }

    // handle data received
    void handleDataReceived(Buffer buffer) const {
        std::string data(buffer.data(), buffer.size());
        Logger::getInstance().log(LogLevel::INFO, 
            std::format("Received {} bytes: {}", buffer.size(), data));
        if (m_dataReceivedCallback) {
            m_dataReceivedCallback(std::move(buffer));
        }
    }

    // notify connection status
    void notifyConnectionStatus(bool connected, const Error& error) const {
        if (m_connectionStatusCallback) {
            m_connectionStatusCallback(connected, error);
        }
    }

    // proactor for asynchronous operations
    std::shared_ptr<Proactor> m_proactor;

    // socket for the connection
    Socket m_socket;

    // flag indicating whether the client is connected
    bool m_connected;

    // callback for data received
    DataReceivedCallback m_dataReceivedCallback;

    // callback for connection status
    ConnectionStatusCallback m_connectionStatusCallback;
};

//------------------------------------------------------------------------------
// Server class
//------------------------------------------------------------------------------
class Server : public std::enable_shared_from_this<Server> {
public:
    // callback type for client connected
    using ClientConnectedCallback = std::function<void(Socket)>;

    // constructor taking a proactor
    explicit Server(std::shared_ptr<Proactor> proactor)
        : m_proactor(std::move(proactor)), m_listening(false) {}

    // destructor stops listening
    ~Server() {
        stop();
    }

    // start listening on the specified port
    bool start(int port) {
        Logger::getInstance().log(LogLevel::INFO, 
            std::format("Server starting on port {}", port));
            
        // stop if already listening
        if (m_listening) {
            stop();
        }

        // create a socket
        m_socket = Socket::createTcp();
        if (!m_socket.isValid()) {
            Logger::getInstance().log(LogLevel::ERROR, "Failed to create socket");
            return false;
        }

        // set socket options
        Error error = m_socket.setReuseAddress();
        if (error) {
            Logger::getInstance().log(LogLevel::ERROR, 
                std::format("Failed to set socket options: {}", error.message()));
            return false;
        }

        // bind to the specified port
        error = m_socket.bind("0.0.0.0", port);
        if (error) {
            Logger::getInstance().log(LogLevel::ERROR, 
                std::format("Failed to bind socket: {}", error.message()));
            return false;
        }

        // start listening
        error = m_socket.listen();
        if (error) {
            Logger::getInstance().log(LogLevel::ERROR, 
                std::format("Failed to listen on socket: {}", error.message()));
            return false;
        }

        m_listening = true;
        Logger::getInstance().log(LogLevel::INFO, 
            std::format("Server listening on port {}", port));

        // start accepting connections
        accept();

        return true;
    }

    // stop listening
    void stop() {
        Logger::getInstance().log(LogLevel::INFO, "Server stopping");
        m_socket.close();
        m_listening = false;
    }

    // set the callback for client connected
    void setClientConnectedCallback(ClientConnectedCallback callback) {
        m_clientConnectedCallback = std::move(callback);
    }

private:
    // Accept a new connection
    void accept() {
        Logger::getInstance().log(LogLevel::INFO, "Server accepting connections");
        if (!m_listening) {
            Logger::getInstance().log(LogLevel::ERROR, "Not listening");
            return;
        }

        // create a completion handler for the accept operation
        class AcceptCompletionHandler : public CompletionHandler {
        public:
            explicit AcceptCompletionHandler(const std::shared_ptr<Server>& server)
                : m_server(server) {}

            // handle completion of the accept operation
            void handleCompletion(ssize_t result, Buffer buffer) override {
                if (const auto server = m_server.lock()) {
                    if (result >= 0) {
                        // the accept operation completed successfully
                        // create a socket for the new connection using the socket descriptor
                        Socket clientSocket(result);
                        server->handleClientConnected(std::move(clientSocket));
                    } else {
                        Logger::getInstance().log(LogLevel::ERROR, "Accept operation failed");
                    }

                    // continue accepting connections
                    server->accept();
                }
            }

        private:
            std::weak_ptr<Server> m_server;
        };

        // create and initiate an asynchronous accept operation
        auto handler = std::make_shared<AcceptCompletionHandler>(shared_from_this());
        auto op = std::make_shared<AsyncAcceptOperation>(handler, m_socket);
        if (op->initiate(m_proactor)) {
            Logger::getInstance().log(LogLevel::INFO, "Accept operation initiated successfully");
        } else {
            Logger::getInstance().log(LogLevel::ERROR, "Failed to initiate accept operation");
        }
    }

    // handle a new client connection
    void handleClientConnected(Socket clientSocket) const {
        // We can't get peer address directly from the socket anymore
        Logger::getInstance().log(LogLevel::INFO, "Client connected");
        if (m_clientConnectedCallback) {
            m_clientConnectedCallback(std::move(clientSocket));
        }
    }

    // proactor for asynchronous operations
    std::shared_ptr<Proactor> m_proactor;

    // socket for listening
    Socket m_socket;

    // flag indicating whether the server is listening
    bool m_listening;

    // callback for a client connected
    ClientConnectedCallback m_clientConnectedCallback;
};

//------------------------------------------------------------------------------
// EchoServer class
//------------------------------------------------------------------------------
class EchoServer {
public:
    // constructor taking a proactor
    explicit EchoServer(const std::shared_ptr<Proactor>& proactor)
        : m_proactor(proactor), m_server(std::make_shared<Server>(proactor)) {
        m_server->setClientConnectedCallback(
            [this](Socket clientSocket) {
                handleClientConnected(std::move(clientSocket));
            }
        );
    }

    // start the echo server on the specified port
    [[nodiscard]] bool start(int port) const {
        return m_server->start(port);
    }

    // stop the echo server
    void stop() {
        m_server->stop();
        m_clients.clear();
    }

private:
    // Inner class to handle a client connection
    class EchoClientHandler : public std::enable_shared_from_this<EchoClientHandler> {
    public:
        // constructor taking a proactor and client socket
        EchoClientHandler(const std::shared_ptr<Proactor>& proactor, Socket clientSocket)
            : m_proactor(proactor), m_client(std::make_shared<Client>(proactor)) {
            m_client->adoptSocket(std::move(clientSocket));
            m_client->setDataReceivedCallback(
                [this](Buffer buffer) {
                    handleDataReceived(std::move(buffer));
                }
            );
        }

        // start handling the client
        void start() const {
            m_client->receive();
        }

    private:
        // handle data received from the client
        void handleDataReceived(Buffer buffer) const {
            // echo the data back to the client
            std::string data(buffer.data(), buffer.size());
            Logger::getInstance().log(LogLevel::INFO, 
                std::format("Echo server received: {}", data));
            m_client->send(data);

            // continue receiving data
            m_client->receive();
        }

        // proactor for asynchronous operations
        std::shared_ptr<Proactor> m_proactor;

        // client object
        std::shared_ptr<Client> m_client;
    };

    // handle a new client connection
    void handleClientConnected(Socket clientSocket) {
        const auto handler = std::make_shared<EchoClientHandler>(m_proactor, std::move(clientSocket));
        m_clients.push_back(handler);
        handler->start();
    }

    // proactor for asynchronous operations
    std::shared_ptr<Proactor> m_proactor;

    // server object
    std::shared_ptr<Server> m_server;

    // list of client handlers
    std::vector<std::shared_ptr<EchoClientHandler>> m_clients;
};

// Client test with single connection - this function would normally be in tests.cpp
void runClientTest() {
    Logger::getInstance().log(LogLevel::INFO, "Starting client test");

    // Create a proactor
    auto proactor = std::make_shared<Proactor>();
    proactor->start();

    // Create a client
    const auto client = std::make_shared<Client>(proactor);

    // Promises for async notifications
    std::promise<bool> connectPromise;
    auto connectFuture = connectPromise.get_future();
    std::promise<std::string> dataPromise;
    auto dataFuture = dataPromise.get_future();

    // Set callbacks
    client->setConnectionStatusCallback([&connectPromise](bool connected, const Error& error) {
        Logger::getInstance().log(LogLevel::INFO, 
            std::format("Connection status: {}", connected ? "connected" : "disconnected"));
        connectPromise.set_value(connected);
    });

    client->setDataReceivedCallback([&dataPromise](Buffer buffer) {
        std::string data(buffer.data(), buffer.size());
        Logger::getInstance().log(LogLevel::INFO, 
            std::format("Data received: {}", data));
        dataPromise.set_value(data);
    });

    // Connect to the server
    client->connect("localhost", 8080);

    // Wait for connection
    if (const auto connectStatus = connectFuture.wait_for(std::chrono::seconds(5)); connectStatus == std::future_status::timeout) {
        Logger::getInstance().log(LogLevel::ERROR, "Connection timeout");
        proactor->stop();
        return;
    }

    bool connected = connectFuture.get();
    if (!connected) {
        Logger::getInstance().log(LogLevel::ERROR, "Failed to connect");
        proactor->stop();
        return;
    }

    // Send a message
    client->send("Hello, server!");

    // Wait for a response
    if (const auto dataStatus = dataFuture.wait_for(std::chrono::seconds(5)); dataStatus == std::future_status::timeout) {
        Logger::getInstance().log(LogLevel::ERROR, "Data receive timeout");
        client->disconnect();
        proactor->stop();
        Logger::getInstance().log(LogLevel::ERROR, "Test aborted, resources cleaned up");
        return;
    }

    // Get the response
    std::string receivedData = dataFuture.get();
    Logger::getInstance().log(LogLevel::INFO, 
        std::format("Received response: {}", receivedData));

    // Disconnect
    client->disconnect();

    // Stop the proactor
    proactor->stop();

    Logger::getInstance().log(LogLevel::INFO, "Client test completed");
}

// EchoServer test with multiple clients - this function would normally be in tests.cpp
void runEchoServerTest() {
    Logger::getInstance().log(LogLevel::INFO, "Starting echo server test");

    // Create a proactor
    auto proactor = std::make_shared<Proactor>();
    proactor->start();

    // Create and start the echo server
    auto server = std::make_shared<EchoServer>(proactor);
    if (!server->start(8080)) {
        Logger::getInstance().log(LogLevel::ERROR, "Failed to start server");
        proactor->stop();
        return;
    }

    // Number of test clients
    constexpr int numClients = 5;

    // Create clients
    std::vector<std::shared_ptr<Client>> clients;
    std::vector<std::promise<bool>> connectionPromises(numClients);
    std::vector<std::future<bool>> connectionFutures;
    std::vector<std::promise<std::string>> dataPromises(numClients);
    std::vector<std::future<std::string>> dataFutures;

    for (int ndx = 0; ndx < numClients; ++ndx) {
        connectionFutures.push_back(connectionPromises[ndx].get_future());
        dataFutures.push_back(dataPromises[ndx].get_future());

        auto client = std::make_shared<Client>(proactor);
        clients.push_back(client);

        client->setConnectionStatusCallback([ndx, &connectionPromises](const bool connected, const Error& error) {
            Logger::getInstance().log(LogLevel::INFO,
                std::format("Client {} connection status: {}",
                    ndx, connected ? "connected" : "disconnected"));
            connectionPromises[ndx].set_value(connected);
        });

        client->setDataReceivedCallback([ndx, &dataPromises](Buffer buffer) {
            std::string data(buffer.data(), buffer.size());
            Logger::getInstance().log(LogLevel::INFO,
                std::format("Client {} received: {}", ndx, data));
            dataPromises[ndx].set_value(data);
        });
    }

    // Connect all clients
    for (int ndx = 0; ndx < numClients; ++ndx) {
        clients[ndx]->connect("localhost", 8080);
    }

    // Wait for all connections
    for (int ndx = 0; ndx < numClients; ++ndx) {
        if (const auto clientConnectStatus = connectionFutures[ndx].wait_for(std::chrono::seconds(10));
                clientConnectStatus == std::future_status::timeout) {
            Logger::getInstance().log(LogLevel::ERROR,
                std::format("Client {} connection timeout", ndx));
            // Stop all clients and the server
            for (const auto& client : clients) {
                client->disconnect();
            }
            server->stop();
            proactor->stop();
            return;
        }

        if (const bool connected = connectionFutures[ndx].get(); !connected) {
            Logger::getInstance().log(LogLevel::ERROR,
                std::format("Client {} failed to connect", ndx));
            return;
        }
    }

    // Send a message from each client
    for (int ndx = 0; ndx < numClients; ++ndx) {
        std::string message = std::format("Hello from client {}!", ndx);
        clients[ndx]->send(message);
    }

    // Wait for responses
    for (int ndx = 0; ndx < numClients; ++ndx) {
        if (const auto clientDataStatus = dataFutures[ndx].wait_for(std::chrono::seconds(5));
                clientDataStatus == std::future_status::timeout) {
            Logger::getInstance().log(LogLevel::ERROR,
                std::format("Client {} data receive timeout", ndx));
            continue;
        }

        std::string receivedData = dataFutures[ndx].get();
        if (std::string expectedData = std::format("Hello from client {}!", ndx); receivedData != expectedData) {
            Logger::getInstance().log(LogLevel::ERROR,
                std::format("Client {} received unexpected data: {}", ndx, receivedData));
        } else {
            Logger::getInstance().log(LogLevel::INFO,
                std::format("Client {} echo test passed", ndx));
        }
    }

    // Disconnect all clients
    for (auto& client : clients) {
        client->disconnect();
    }

    // Stop the server
    server->stop();

    // Stop the proactor
    proactor->stop();

    Logger::getInstance().log(LogLevel::INFO, "Echo server test completed");
}