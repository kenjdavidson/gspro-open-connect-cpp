#include <iostream>
#include <thread>

#include "Server.h"
#include "Logger.h"

using Logger = OpenConnectV1::Logger;

class ShotDataListenerImpl : public ShotDataListener {
public:
    void onShotDataReceived(const OpenConnectV1::ShotData& shotData) override {
        // This is where we'd do whatever we need to with the shot data, it will most likely
        // need to be converted into the internal type used within the application

        Logger::info("From Launch Monitor: ShotDataOptions\n");
        Logger::info("ContainsBallData: %s\n", shotData.ShotDataOptions.ContainsBallData ? "true" : "false");
        Logger::info("ContainsClubData: %s\n", shotData.ShotDataOptions.ContainsClubData ? "true" : "false");
        Logger::info("LaunchMonitorIsReady: %s\n", shotData.ShotDataOptions.LaunchMonitorIsReady ? "true" : "false");
        Logger::info("LaunchMonitorBallDetected: %s\n", shotData.ShotDataOptions.LaunchMonitorBallDetected ? "true" : "false");
        Logger::info("IsHeartBeat: %s\n", shotData.ShotDataOptions.IsHeartBeat ? "true" : "false");
    }
};

//int main()
//{
//    Logger::minLogLevel = OpenConnectV1::LogLevel::Info;
//    std::shared_ptr<ShotDataListenerImpl> listener = std::make_shared<ShotDataListenerImpl>();
//
//    OpenConnectV1::Server server(921);
//    server.addListener(listener);
//
//    std::thread serverThread(&OpenConnectV1::Server::startup, std::ref(server));
//
//    std::string club;
//    do {
//        fprintf(stderr, "Send change club request [DR, 3W, 5I, etc] (q to exit): ");
//        std::cin >> club;
//
//        if (club.compare("q") == 0) {
//            server.shutdown();
//        } else {
//            OpenConnectV1::Response response(OpenConnectV1::ResponseCode::OK, "GSPro Player Information", OpenConnectV1::PlayerData("RH", club));
//            server.sendResponse(response);
//        }
//    } while (club.compare("q") != 0);
//
//    serverThread.join();
//
//    fprintf(stderr, "Closing application!\n");
//}