#include "pch.h"

#include <gtest/gtest.h>
#include <thread>
#include <chrono>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <nlohmann/json.hpp>

#include "../OpenConnectV1/Server.h"
#include "../OpenConnectV1/Data.h"
#include "../OpenConnectV1/Logger.h"

#pragma comment(lib, "Ws2_32.lib")

class ServerTest : public ::testing::Test {
protected:
    static constexpr int TEST_PORT = 5001;
    OpenConnectV1::Server* server;
    std::thread serverThread;

    void SetUp() override {
        // Start server in a separate thread
        server = new OpenConnectV1::Server(TEST_PORT);
        serverThread = std::thread([this] { server->startup(); });

        // Allow server to start up
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    void TearDown() override {
        // Shutdown server and join thread
        server->shutdown();
        if (serverThread.joinable()) {
            serverThread.join();
        }
        delete server;
    }

    SOCKET createClientSocket() {
        WSADATA wsaData;
        WSAStartup(MAKEWORD(2, 2), &wsaData);

        SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Failed to create client socket, error code: " << WSAGetLastError() << std::endl;
            return -1;
        }

        sockaddr_in serverAddr{};
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(TEST_PORT);
        serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

        int result = connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
        if (result != 0) {
            std::cerr << "Failed to connect to server, error code: " << WSAGetLastError() << std::endl;
            return -1;
        }

        return clientSocket;
    }
};

TEST_F(ServerTest, TestServerStartupAndShotDataReception) {
    SOCKET clientSocket = createClientSocket();

    // Create and send a ShotData object
    OpenConnectV1::ShotData shotData;
    shotData.ShotDataOptions.ContainsBallData = true;
    shotData.ShotDataOptions.ContainsClubData = true;
    shotData.ShotDataOptions.LaunchMonitorIsReady = true;
    shotData.ShotDataOptions.LaunchMonitorBallDetected = false;
    shotData.ShotDataOptions.IsHeartBeat = false;

    nlohmann::json jsonShotData;
    OpenConnectV1::to_json(jsonShotData, shotData);
    std::string jsonStr = jsonShotData.dump();

    int bytesSent = send(clientSocket, jsonStr.c_str(), jsonStr.size(), 0);
    ASSERT_GT(bytesSent, 0) << "Failed to send ShotData to server";

    closesocket(clientSocket);
    WSACleanup();
}

TEST_F(ServerTest, TestServerSendResponse) {
    SOCKET clientSocket = createClientSocket();

    // Server sends a response
    OpenConnectV1::Response response(OpenConnectV1::ResponseCode::OK, "GSPro Player Information", OpenConnectV1::PlayerData("RH", "DR"));

    server->sendResponse(response);

    // Receive response
    char buffer[4096];
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
    ASSERT_GT(bytesReceived, 0) << "Failed to receive response from server";

    std::string receivedStr(buffer, bytesReceived);
    nlohmann::json receivedJson = nlohmann::json::parse(receivedStr);

    EXPECT_EQ(receivedJson["Code"], response.Code);
    EXPECT_EQ(receivedJson["Message"], response.Message);
    EXPECT_EQ(receivedJson["Player"]["Handed"], response.Player.Handed);
    EXPECT_EQ(receivedJson["Player"]["Club"], response.Player.Club);

    closesocket(clientSocket);
    WSACleanup();
}
