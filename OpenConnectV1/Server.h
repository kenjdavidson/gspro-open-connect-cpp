#ifndef OPEN_CONNECT_SERVER_H
#define OPEN_CONNECT_SERVER_H

#include <winsock2.h>
#include <stdio.h>
#include <mutex>
#include <atomic>
#include <memory>

#include "Data.h"
#include "ShotDataListener.h"

#pragma comment(lib, "Ws2_32.lib")

namespace OpenConnectV1 {
	enum class ServerStatus {
		Disconnected = 0,
		Listening = 1,
		Connected = 2
	};

	class Server {
	public:
		Server(int port);
		~Server();

		void startup();
		void shutdown();
		void sendResponse(OpenConnectV1::Response& response);
		ServerStatus getStatus();
		void addListener(std::shared_ptr<ShotDataListener> listener);
		void removeListener();

	private:
		int port;
		std::atomic<ServerStatus> connectionStatus;
		std::atomic<bool> shutdownRequested;

		WSADATA wsaData;

		SOCKET listenSocket = INVALID_SOCKET;
		SOCKADDR_IN serverAddress;

		SOCKET clientSocket = INVALID_SOCKET;
		SOCKADDR_IN clientAddress;

		std::shared_ptr<ShotDataListener> listener;
		std::mutex listenersMutex;

		void notify(const OpenConnectV1::ShotData& shotData);
		void cleanup();
	};
}

#endif
