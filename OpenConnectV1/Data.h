#ifndef OPEN_CONNECT_DATA_H
#define OPEN_CONNECT_DATA_H

#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

/**
 * Shot data as defined by the GSPro Open Connect V1 Specification
 * https://gsprogolf.com/GSProConnectV1.html
 */
namespace OpenConnectV1 {
    struct BallData {
        float Speed = std::numeric_limits<float>::quiet_NaN();
        float SpinAxis = std::numeric_limits<float>::quiet_NaN();
        float TotalSpin = std::numeric_limits<float>::quiet_NaN();
        float BackSpin = std::numeric_limits<float>::quiet_NaN();
        float SideSpin = std::numeric_limits<float>::quiet_NaN();
        float HLA = std::numeric_limits<float>::quiet_NaN();
        float VLA = std::numeric_limits<float>::quiet_NaN();
        float CarryDistance = std::numeric_limits<float>::quiet_NaN();

        BallData();
        BallData(float speed, float spinAxis, float totalSpin, float backSpin, float sideSpin,
            float hla, float vla, float carryDistance);

        static void from_json(const json& j, BallData& b);
    };

    struct ClubData {
        float Speed = std::numeric_limits<float>::quiet_NaN();
        float AngleOfAttack = std::numeric_limits<float>::quiet_NaN();
        float FaceToTarget = std::numeric_limits<float>::quiet_NaN();
        float Lie = std::numeric_limits<float>::quiet_NaN();
        float Loft = std::numeric_limits<float>::quiet_NaN();
        float Path = std::numeric_limits<float>::quiet_NaN();
        float SpeedAtImpact = std::numeric_limits<float>::quiet_NaN();
        float VerticalFaceImpact = std::numeric_limits<float>::quiet_NaN();
        float HorizontalFaceImpact = std::numeric_limits<float>::quiet_NaN();
        float ClosureRate = std::numeric_limits<float>::quiet_NaN();

        ClubData();
        ClubData(float speed, float angleOfAttack, float faceToTarget, float lie, float loft,
            float path, float speedAtImpact, float verticalFaceImpact, float horizontalFaceImpact,
            float closureRate);

        static void from_json(const json& j, ClubData& c);
    };

    struct ShotDataOptions {
        bool ContainsBallData;
        bool ContainsClubData;
        bool LaunchMonitorIsReady;
        bool LaunchMonitorBallDetected;
        bool IsHeartBeat;

        ShotDataOptions();
        ShotDataOptions(bool containsBallData, bool containsClubData, bool launchMonitorIsReady,
            bool launchMonitorBallDetected, bool isHeartBeat);

        static void from_json(const json& j, ShotDataOptions& s);
    };

    struct ShotData {
        std::string DeviceID;
        std::string Units;
        int ShotNumber;
        std::string APIversion;
        BallData BallData;
        ClubData ClubData;
        ShotDataOptions ShotDataOptions;

        ShotData();       
        ShotData(std::string deviceID, std::string units, int shotNumber, std::string apiVersion,
            OpenConnectV1::BallData ballData, OpenConnectV1::ClubData clubData, OpenConnectV1::ShotDataOptions shotDataOptions);
        ~ShotData();

        static void from_json(const json& j, ShotData& s);
    };

    // Function declarations for serialization
    void to_json(json& j, const BallData& b);
    void to_json(json& j, const ClubData& c);
    void to_json(json& j, const ShotDataOptions& sdo);
    void to_json(json& j, const ShotData& s);

}

namespace OpenConnectV1 {
    enum class ResponseCode {
        OK = 200,
        PlayerInfo = 201,
        Error = 500,
        NotImplemented = 501
    };

    struct PlayerData {
        std::string Handed; // RH/LH
        std::string Club;	// DR/3W/5I/etc

        PlayerData(const std::string& handed = "", const std::string& club = "")
            : Handed(handed), Club(club) {}
    };

    struct Response {
        ResponseCode Code;
        std::string Message;
        PlayerData Player;

        Response(ResponseCode code, const std::string& message, const PlayerData& player = {})
            : Code(code), Message(message), Player(player) {}
    };
}

#endif // OPEN_CONNECT_DATA_H
