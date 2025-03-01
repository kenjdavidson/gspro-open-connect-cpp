#include <iostream>
#include <nlohmann/json.hpp>
#include "Server.h"
#include "Logger.h"

namespace OpenConnectV1 {
    Server::Server(int port)
        : port(port), connectionStatus(ServerStatus::Disconnected), shutdownRequested(false),
        serverAddress{}, clientAddress{}, shotDataListener(nullptr),
        clientSocket(std::make_shared<SOCKET>(INVALID_SOCKET)) {

        Logger::debug("Initializing Winsock; should get a popup asking for access to the network...");
        int startupResult = WSAStartup(MAKEWORD(2, 2), &this->wsaData);
        if (startupResult != 0) {
            std::string errorMsg = "Failed to initialize WSAStartup, due to: " + std::to_string(WSAGetLastError());
            Logger::error(errorMsg.c_str());
            throw std::runtime_error(errorMsg);
        }
    }

    Server::~Server() {
        Logger::debug("Cleaning up Server and shutting down Winsock.");
        this->cleanup();
        WSACleanup();
    }

    OpenConnectV1::ServerStatus Server::getStatus() {
        return this->connectionStatus.load();
    }

    void Server::startup() {
        try {
            initializeSocket();
            bindSocket();
            listenOnSocket();
            acceptConnections();
        }
        catch (const std::runtime_error& e) {
            Logger::error("Server startup failed: %s", e.what());
            this->cleanup();
            throw;
        }
    }

    void Server::initializeSocket() {
        Logger::debug("Creating listening socket and waiting for connection(s)...");
        this->listenSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (this->listenSocket == INVALID_SOCKET) {
            std::string errorMsg = "Error at socket(): " + std::to_string(WSAGetLastError());
            Logger::error(errorMsg.c_str());
            throw std::runtime_error(errorMsg);
        }
    }

    void Server::bindSocket() {
        this->serverAddress.sin_addr.s_addr = INADDR_ANY;
        this->serverAddress.sin_family = AF_INET;
        this->serverAddress.sin_port = htons(this->port);

        Logger::debug("Attempting to bind to socket...");
        auto bindResult = bind(this->listenSocket, reinterpret_cast<SOCKADDR*>(&this->serverAddress), sizeof(this->serverAddress));
        if (bindResult == SOCKET_ERROR) {
            std::string errorMsg = "Bind failed with error: " + std::to_string(WSAGetLastError());
            Logger::error(errorMsg.c_str());
            throw std::runtime_error(errorMsg);
        }
    }

    void Server::listenOnSocket() {
        Logger::debug("Start listening on the requested port: %d...", this->port);
        auto listenResult = listen(this->listenSocket, 0);
        if (listenResult == SOCKET_ERROR) {
            std::string errorMsg = "Listen failed with error: " + std::to_string(WSAGetLastError());
            Logger::error(errorMsg.c_str());
            throw std::runtime_error(errorMsg);
        }
    }

    void Server::acceptConnections() {
        int clientSocketSize = sizeof(this->clientAddress);
        while (!this->shutdownRequested.load()) {
            Logger::debug("Attempting to accept client connection...");
            this->connectionStatus.store(OpenConnectV1::ServerStatus::Listening);
            *this->clientSocket = accept(this->listenSocket, reinterpret_cast<SOCKADDR*>(&this->clientAddress), &clientSocketSize);
            if (*this->clientSocket == INVALID_SOCKET) {
                if (this->shutdownRequested.load()) {
                    break;
                }
                std::string errorMsg = "Accepting connection failed with error: " + std::to_string(WSAGetLastError());
                Logger::error(errorMsg.c_str());
                Logger::debug("See: https://learn.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-accept ");
                continue; // Attempt to accept the next connection
            }
            this->connectionStatus.store(OpenConnectV1::ServerStatus::Connected);
            handleClientCommunication();
        }
    }

    void Server::handleClientCommunication() {
        const int BUFFER_SIZE = 4092;
        char buffer[BUFFER_SIZE]{};

        while (!this->shutdownRequested.load()) {
            int bytesReceived = recv(*this->clientSocket, buffer, BUFFER_SIZE, 0);
            if (bytesReceived > 0) {
                Logger::debug("Raw data: %s", buffer);
                try {
                    std::string jsonStr(buffer, bytesReceived);
                    json j = json::parse(jsonStr);
                    ShotData shotData;
                    ShotData::from_json(j, shotData);
                    this->notify(shotData);

                    Logger::debug("Raw: %s", jsonStr.c_str());
                    Logger::debug("From Launch Monitor: ShotDataOptions");
                    Logger::debug("ContainsBallData: %s", shotData.ShotDataOptions.ContainsBallData ? "true" : "false");
                    Logger::debug("ContainsClubData: %s", shotData.ShotDataOptions.ContainsClubData ? "true" : "false");
                    Logger::debug("LaunchMonitorIsReady: %s", shotData.ShotDataOptions.LaunchMonitorIsReady ? "true" : "false");
                    Logger::debug("LaunchMonitorBallDetected: %s", shotData.ShotDataOptions.LaunchMonitorBallDetected ? "true" : "false");
                    Logger::debug("IsHeartBeat: %s", shotData.ShotDataOptions.IsHeartBeat ? "true" : "false");
                }
                catch (const std::exception& e) {
                    Logger::error("Failed to deserialize ShotData: %s", e.what());
                }
                memset(buffer, 0, BUFFER_SIZE);
            }
            else if (!this->shutdownRequested.load()) {
                std::string errorMsg = "Client disconnect or error: " + std::to_string(WSAGetLastError());
                Logger::error(errorMsg.c_str());
                Logger::debug("See: https://learn.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-recv ");
                break;
            }
        }
        closesocket(*this->clientSocket);
        *this->clientSocket = INVALID_SOCKET;
    }

    void Server::addShotDataListener(std::shared_ptr<ShotDataListener> listener) {
        std::lock_guard<std::mutex> lock(this->listenersMutex);
        this->shotDataListener = listener;
    }

    void Server::removeShotDataListener() {
        std::lock_guard<std::mutex> lock(this->listenersMutex);
        this->shotDataListener = nullptr;
    }

    void Server::addStatusListener(std::shared_ptr<StatusListener> listener) {
        std::lock_guard<std::mutex> lock(this->listenersMutex);
        this->statusListener = listener;
    }

    void Server::removeStatusListener() {
        std::lock_guard<std::mutex> lock(this->listenersMutex);
        this->statusListener = nullptr;
    }

    void Server::notify(const OpenConnectV1::ShotData& shotData) {
        std::lock_guard<std::mutex> lock(this->listenersMutex);
        if (this->shotDataListener) {
            this->shotDataListener->onShotDataReceived(shotData);  // Notify each listener
        }
    }

    void Server::changeStatus(const ServerStatus& status) {
        std::lock_guard<std::mutex> lock(this->listenersMutex);
        if (this->connectionStatus.load() != status) {
            if (this->statusListener) {
                this->statusListener->onStatusChanged(status);
            }
        }
    }

    void Server::shutdown() {
        this->shutdownRequested.store(true);
        this->connectionStatus.store(OpenConnectV1::ServerStatus::Disconnected);
        this->cleanup();
    }

    void Server::cleanup() {
        closeClientSocket();
        resetClientAddress();
        closeListenSocket();
        resetServerAddress();
    }

    void Server::closeClientSocket() {
        closesocket(*this->clientSocket);
        *this->clientSocket = INVALID_SOCKET;
    }

    void Server::resetClientAddress() {
        memset(&this->clientAddress, 0, sizeof(this->clientAddress));
    }

    void Server::closeListenSocket() {
        closesocket(this->listenSocket);
        this->listenSocket = INVALID_SOCKET;
    }

    void Server::resetServerAddress() {
        memset(&this->serverAddress, 0, sizeof(this->serverAddress));
    }

    void Server::sendResponse(OpenConnectV1::Response& response) {
        std::string jsonStr = createJsonResponse(response);
        Logger::debug("Simulating response to monitor/client: %s", jsonStr.c_str());

        // Debug output for clientSocket
        Logger::debug("clientSocket address: %p, clientSocket value: %d", static_cast<void*>(clientSocket.get()), *clientSocket);

        sendJsonResponse(jsonStr);
    }

    std::string Server::createJsonResponse(OpenConnectV1::Response& response) {
        nlohmann::json jsonResponse = {
            {"Code", response.Code},
            {"Message", response.Message},
            {"Player", {
                {"Handed", response.Player.Handed},
                {"Club", response.Player.Club}
            }}
        };
        return jsonResponse.dump() + "\n";
    }

    void Server::sendJsonResponse(const std::string& jsonStr) {
        if (*this->clientSocket != INVALID_SOCKET) {
            int bytesSent = send(*this->clientSocket, jsonStr.c_str(), jsonStr.length(), 0);
            if (bytesSent < 0) {
                std::string errorMsg = "Unable to send response to monitor/client: " + std::to_string(WSAGetLastError());
                Logger::error(errorMsg.c_str());
                Logger::debug("See: https://learn.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-send ");
            }
            else {
                Logger::debug("Write was successful: %d of %d bytes sent", bytesSent, jsonStr.length());
            }
        }
        else {
            Logger::debug("Client is not connected!");
        }
    }
}