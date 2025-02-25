#include <iostream>
#include <nlohmann/json.hpp>

#include "Server.h"
#include "Logger.h"

namespace OpenConnectV1 {
	Server::Server(int port) {
		this->port = port;
		this->connectionStatus = OpenConnectV1::ServerStatus::Disconnected;
		this->shutdownRequested = false;
		this->serverAddress = SOCKADDR_IN();
		this->clientAddress = SOCKADDR_IN();
		this->listener = nullptr;

		Logger::debug("Initializing Winsock; should get a popup asking for access to the network...\n");
		int startupResult = WSAStartup(MAKEWORD(2, 2), &this->wsaData);
		if (startupResult != 0) {
			Logger::debug("Failed to initialized WSAStartup, due to: %d\n", WSAGetLastError());
			throw "WSAStartup failed!!";
		}
	}

	Server::~Server() {
		Logger::debug("Cleaning up Server and shutting down Winsock.\n");

		this->cleanup();
		WSACleanup();
	}

	OpenConnectV1::ServerStatus Server::getStatus() {
		return this->connectionStatus;
	}

	void Server::startup() {
		Logger::debug("Creating listening socket and waiting for connection(s)...\n");		
		this->listenSocket = socket(AF_INET, SOCK_STREAM, 0);
		if (this->listenSocket == INVALID_SOCKET) {
			Logger::debug("Error at socket(): %ld\n", WSAGetLastError());
			this->cleanup();
			throw "Error attempting to create socket, please restart acceptConnections";
		}

		this->serverAddress.sin_addr.s_addr = INADDR_ANY;
		this->serverAddress.sin_family = AF_INET;
		this->serverAddress.sin_port = htons(this->port);

		// Setup the TCP listening Socket
		Logger::debug("Attempting to bind to socket...\n");
		auto bindResult = bind(this->listenSocket, reinterpret_cast<SOCKADDR*>(&this->serverAddress), sizeof(this->serverAddress));
		if (bindResult == SOCKET_ERROR) {
			Logger::error("bind failed with error: %d\n", WSAGetLastError());
			this->cleanup();
			throw "Error attempting to bind socket, please restart acceptConnections";
		}

		// Listening on the socket
		Logger::debug("Start listening on the requested port: %d...\n", this->port);
		auto listenResult = listen(this->listenSocket, 0);
		if (listenResult == SOCKET_ERROR) {
			Logger::error("Listen failed with error: %ld\n", WSAGetLastError());
			this->cleanup();
			throw "Error attempting to listen on socket, please restart acceptConnections";
		}

		// Continue to listen for connections, until the Server has requested shutdown.
		// Only one Client can be connected at a time, this thread will block until the client connection is closed or broken
		// then it will continue to look for a new request.
		int clientSocketSize = sizeof(this->clientAddress);
		while (!this->shutdownRequested) {
			Logger::debug("Attempting to accept client connection...\n");
			this->connectionStatus = OpenConnectV1::ServerStatus::Listening;
			this->clientSocket = accept(this->listenSocket, reinterpret_cast<SOCKADDR*>(&this->clientAddress), &clientSocketSize);
			if (this->clientSocket == INVALID_SOCKET) {
				if (this->shutdownRequested) {
					break;
				}

				Logger::error("Accepting connection failed with error: %d\n", WSAGetLastError());
				Logger::debug("See: https://learn.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-accept \n");
			}

			this->connectionStatus = OpenConnectV1::ServerStatus::Connected;

			const int BUFFER_SIZE = 4092;
			char buffer[BUFFER_SIZE]{};

			while (!this->shutdownRequested) {
				int bytesReceived = recv(this->clientSocket, buffer, BUFFER_SIZE, 0);
				if (bytesReceived > 0) {
					Logger::debug("Raw data: %s\n", buffer);
					try {
						std::string jsonStr(buffer, bytesReceived);
						json j = json::parse(jsonStr);  
						ShotData shotData;
						ShotData::from_json(j, shotData);
						this->notify(shotData);

						Logger::debug("From Launch Monitor: ShotDataOptions\n");
						Logger::debug("ContainsBallData: %s\n", shotData.ShotDataOptions.ContainsBallData ? "true" : "false");
						Logger::debug("ContainsClubData: %s\n", shotData.ShotDataOptions.ContainsClubData ? "true" : "false");
						Logger::debug("LaunchMonitorIsReady: %s\n", shotData.ShotDataOptions.LaunchMonitorIsReady ? "true" : "false");
						Logger::debug("LaunchMonitorBallDetected: %s\n", shotData.ShotDataOptions.LaunchMonitorBallDetected ? "true" : "false");
						Logger::debug("IsHeartBeat: %s\n", shotData.ShotDataOptions.IsHeartBeat ? "true" : "false");
					}
					catch (const std::exception& e) {
						Logger::error("Failed to deserialize ShotData: %s\n", e.what());
					}

					memset(buffer, 0, BUFFER_SIZE);
				} else if(!this->shutdownRequested) {
					Logger::error("Client disconnect or error: %d\n", WSAGetLastError());
					Logger::debug("See: https://learn.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-recv \n");
					break;
				}
			}

			closesocket(this->clientSocket);
		}
	}

	void Server::addListener(std::shared_ptr<ShotDataListener> listener) {
		std::lock_guard<std::mutex> lock(listenersMutex);
		this->listener = listener;
	}

	void Server::removeListener() {
		std::lock_guard<std::mutex> lock(listenersMutex);
		this->listener = nullptr;
	}

	void Server::notify(const OpenConnectV1::ShotData& shotData) {
		std::lock_guard<std::mutex> lock(listenersMutex);
		if (this->listener) {
			this->listener->onShotDataReceived(shotData);  // Notify each listener
		}
	}

	void Server::shutdown() {
		// Stop accepting and wait for acceptThread to finish
		this->shutdownRequested = true;
		this->connectionStatus = OpenConnectV1::ServerStatus::Disconnected;
		this->cleanup();
	}

	void Server::cleanup() {
		closesocket(this->clientSocket);
		memset(&this->clientAddress, 0, sizeof(this->clientAddress));
		this->clientSocket = INVALID_SOCKET;

		closesocket(this->listenSocket);		
		memset(&this->serverAddress, 0, sizeof(this->serverAddress));
		this->listenSocket = INVALID_SOCKET;
	}

	void Server::sendResponse(OpenConnectV1::Response& response) {
		nlohmann::json jsonResponse = {
			{"Code", response.Code},
			{"Message", response.Message},
			{"Player", {
				{"Handed", response.Player.Handed},
				{"Club", response.Player.Club}
			}}
		};

		std::string jsonStr = jsonResponse.dump();
		Logger::debug("Simulating response to monitor/client: %s", jsonStr.c_str());

		if (this->clientSocket > 0) {
			int bytesSent = send(clientSocket, jsonStr.c_str(), jsonStr.length(), 0);

			if (bytesSent < 0) {
				Logger::debug("Unable to send response to monitor/client: %d\n", WSAGetLastError());
				Logger::debug("See: https://learn.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-send \n");
			} else {
				Logger::debug("Write was successful: %d of %d bytes sent\n", bytesSent, sizeof(jsonStr));
			}
		} else {
			Logger::debug("Client is not connected!");			
		}
	}
}