#ifndef OPEN_CONNECT_SERVER_H
#define OPEN_CONNECT_SERVER_H

#include <winsock2.h>
#include <stdio.h>
#include <mutex>
#include <atomic>
#include <memory>

#include "Data.h"
#include "ShotDataListener.h"
#include "ServerStatus.h"

#pragma comment(lib, "Ws2_32.lib")

namespace OpenConnectV1 {
	class Server {
	public:
		Server(int port);
		~Server();

		void startup();
		void shutdown();
		void sendResponse(OpenConnectV1::Response& response);
		
		ServerStatus getStatus();

		void addShotDataListener(std::shared_ptr<ShotDataListener> listener);
		void removeShotDataListener();
		void addStatusListener(std::shared_ptr<StatusListener> listener);
		void removeStatusListener();

	private:
		int port;
		std::atomic<ServerStatus> connectionStatus;
		std::atomic<bool> shutdownRequested;

		WSADATA wsaData;

		SOCKET listenSocket = INVALID_SOCKET;
		SOCKADDR_IN serverAddress;

		SOCKET clientSocket = INVALID_SOCKET;
		SOCKADDR_IN clientAddress;

		std::shared_ptr<ShotDataListener> shotDataListener;
		std::shared_ptr<StatusListener> statusListener;
		std::mutex listenersMutex;

		void notify(const OpenConnectV1::ShotData& shotData);
		void changeStatus(const ServerStatus& status);
		void cleanup();
	};
}

#endif
