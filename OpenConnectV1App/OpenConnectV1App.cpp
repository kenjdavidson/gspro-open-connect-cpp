#include <iostream>
#include <thread>
#include <atomic>
#include <sstream>
#include "../OpenConnectV1/Server.h"
#include "../OpenConnectV1/Data.h"
#include "../OpenConnectV1/ShotDataListener.h"
#include "../OpenConnectV1/Logger.h"

class ConsoleApp : public OpenConnectV1::ShotDataListener {
public:
    std::atomic<bool> running{ true };

    void onShotDataReceived(const OpenConnectV1::ShotData& shotData) override {
        std::cout << "Received ShotData:\n"
            << "DeviceID: " << shotData.DeviceID << "\n"
            << "Units: " << shotData.Units << "\n"
            << "ShotNumber: " << shotData.ShotNumber << "\n"
            << "APIversion: " << shotData.APIversion << "\n"
            << std::endl;
    }

    void run() {
        OpenConnectV1::Server server(921);
        std::shared_ptr<ConsoleApp> listener = std::make_shared<ConsoleApp>();
        server.addListener(listener);

        // Start the server in a separate thread
        std::thread serverThread([&]() {
            server.startup();
        });

        // Main loop for user input
        while (true) {
            std::string input;
            std::cout << "Enter Handed/Club (e.g., RH/9I, LH/3W) or 'Q' to quit: ";
            std::cin >> input;

            // Handle exit condition
            if (input == "q" || input == "Q" || input == "Quit") {
                running = false;
                break;
            }

            std::string handed = "RH";  // Default handedness
            std::string club = "DR";    // Default club (Driver)

            // Parse user input
            size_t slashPos = input.find('/');
            if (slashPos != std::string::npos) {
                std::string firstPart = input.substr(0, slashPos);
                std::string secondPart = input.substr(slashPos + 1);

                if (!firstPart.empty()) {
                    handed = firstPart;  // Use provided handedness
                }
                if (!secondPart.empty()) {
                    club = secondPart;  // Use provided club
                }
            }
            else {
                // If no slash is present, assume it's a handed-only input
                handed = input;
            }

            OpenConnectV1::Response response(OpenConnectV1::ResponseCode::PlayerInfo, "GSPro Player Information", OpenConnectV1::PlayerData(handed, club));
            server.sendResponse(response);
        }

        // Gracefully stop the server and join the thread
        server.shutdown();  // Assuming this function exists in Server
        if (serverThread.joinable()) {
            serverThread.join();
        }

        std::cout << "Server shut down. Exiting application.\n";
    }
};

int main() {
    OpenConnectV1::Logger::minLogLevel = OpenConnectV1::LogLevel::Debug;

    ConsoleApp app;
    app.run();
    return 0;
}
