#ifndef OPEN_CONNECT_SERVER_H
#define OPEN_CONNECT_SERVER_H

#include <winsock2.h>
#include <stdio.h>
#include <mutex>
#include <atomic>
#include <memory>
#include <string>
#include <nlohmann/json.hpp>
#include "Data.h"

#pragma comment(lib, "Ws2_32.lib")

namespace OpenConnectV1 {
    enum class ServerStatus {
        Disconnected = 0,
        Listening = 1,
        Connected = 2
    };

    class ServerListener {
    public:
        virtual ~ServerListener() = default;
        virtual void onShotDataReceived(const OpenConnectV1::ShotData& shotData) = 0;
        virtual void onStatusChanged(const ServerStatus& status) = 0;
    };

    class Server {
    public:
        Server();
        ~Server();

        void startup(int port);
        void shutdown();
        void sendResponse(OpenConnectV1::Response& response);

        ServerStatus getStatus();

        void setListener(std::shared_ptr<ServerListener> listener);
        void removeListener();

    private:
        int port;
        std::atomic<ServerStatus> connectionStatus;
        std::atomic<bool> shutdownRequested;

        WSADATA wsaData;

        SOCKET listenSocket = INVALID_SOCKET;
        SOCKADDR_IN serverAddress;

        std::shared_ptr<SOCKET> clientSocket;
        SOCKADDR_IN clientAddress;

        std::shared_ptr<ServerListener> serverListener;
        std::mutex listenersMutex;

        void notifyShotData(const OpenConnectV1::ShotData& shotData);
        void notifyStatus(const ServerStatus& status);
        void cleanup();

        void initializeSocket();
        void bindSocket();
        void listenOnSocket();
        void acceptConnections();
        void handleClientCommunication();

        void closeClientSocket();
        void resetClientAddress();
        void closeListenSocket();
        void resetServerAddress();

        std::string createJsonResponse(OpenConnectV1::Response& response);
        void sendJsonResponse(const std::string& jsonStr);
    };
}

#endif