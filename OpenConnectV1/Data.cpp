#include "Data.h"

namespace OpenConnectV1 {

    BallData::BallData()
        : Speed(0), SpinAxis(0), TotalSpin(0), BackSpin(0), SideSpin(0), HLA(0), VLA(0), CarryDistance(0) {}

    BallData::BallData(float speed, float spinAxis, float totalSpin, float backSpin, float sideSpin,
        float hla, float vla, float carryDistance)
        : Speed(speed), SpinAxis(spinAxis), TotalSpin(totalSpin), BackSpin(backSpin),
        SideSpin(sideSpin), HLA(hla), VLA(vla), CarryDistance(carryDistance) {}

    void BallData::from_json(const json& j, BallData& b) {
        auto safe_get = [](const json& j, const std::string& key) -> float {
            if (j.contains(key) && j[key].is_number()) {
                float value = j[key].get<float>();
                return std::isnan(value) ? std::numeric_limits<float>::quiet_NaN() : value;
            }
            return std::numeric_limits<float>::quiet_NaN();  // Use NaN for missing values
        };

        b.Speed = safe_get(j, "Speed");
        b.SpinAxis = safe_get(j, "SpinAxis");
        b.TotalSpin = safe_get(j, "TotalSpin");
        b.BackSpin = safe_get(j, "BackSpin");
        b.SideSpin = safe_get(j, "SideSpin");
        b.HLA = safe_get(j, "HLA");
        b.VLA = safe_get(j, "VLA");
        b.CarryDistance = safe_get(j, "CarryDistance");
    }

    ClubData::ClubData()
        : Speed(0), AngleOfAttack(0), FaceToTarget(0), Lie(0), Loft(0), Path(0), SpeedAtImpact(0),
        VerticalFaceImpact(0), HorizontalFaceImpact(0), ClosureRate(0) {}

    ClubData::ClubData(float speed, float angleOfAttack, float faceToTarget, float lie, float loft,
        float path, float speedAtImpact, float verticalFaceImpact, float horizontalFaceImpact,
        float closureRate)
        : Speed(speed), AngleOfAttack(angleOfAttack), FaceToTarget(faceToTarget), Lie(lie), Loft(loft),
        Path(path), SpeedAtImpact(speedAtImpact), VerticalFaceImpact(verticalFaceImpact),
        HorizontalFaceImpact(horizontalFaceImpact), ClosureRate(closureRate) {}

    void ClubData::from_json(const json& j, ClubData& c) {
        c.Speed = j["Speed"].get<float>();
        c.AngleOfAttack = j["AngleOfAttack"].get<float>();
        c.FaceToTarget = j["FaceToTarget"].get<float>();
        c.Lie = j["Lie"].get<float>();
        c.Loft = j["Loft"].get<float>();
        c.Path = j["Path"].get<float>();
        c.SpeedAtImpact = j["SpeedAtImpact"].get<float>();
        c.VerticalFaceImpact = j["VerticalFaceImpact"].get<float>();
        c.HorizontalFaceImpact = j["HorizontalFaceImpact"].get<float>();
        c.ClosureRate = j["ClosureRate"].get<float>();
    }

    ShotDataOptions::ShotDataOptions()
        : ContainsBallData(false), ContainsClubData(false), LaunchMonitorIsReady(false),
        LaunchMonitorBallDetected(false), IsHeartBeat(false) {}

    ShotDataOptions::ShotDataOptions(bool containsBallData, bool containsClubData, bool launchMonitorIsReady,
        bool launchMonitorBallDetected, bool isHeartBeat)
        : ContainsBallData(containsBallData), ContainsClubData(containsClubData),
        LaunchMonitorIsReady(launchMonitorIsReady), LaunchMonitorBallDetected(launchMonitorBallDetected),
        IsHeartBeat(isHeartBeat) {}

    void ShotDataOptions::from_json(const json& j, ShotDataOptions& s) {
        s.ContainsBallData = j["ContainsBallData"].get<bool>();
        s.ContainsClubData = j["ContainsClubData"].get<bool>();
        s.LaunchMonitorIsReady = j["LaunchMonitorIsReady"].get<bool>();
        s.LaunchMonitorBallDetected = j["LaunchMonitorBallDetected"].get<bool>();
        s.IsHeartBeat = j["IsHeartBeat"].get<bool>();
    }

    ShotData::ShotData()
        : DeviceID(""), Units(""), ShotNumber(0), APIversion("") {}

    ShotData::ShotData(std::string deviceID, std::string units, int shotNumber, std::string apiVersion,
        OpenConnectV1::BallData ballData, OpenConnectV1::ClubData clubData, OpenConnectV1::ShotDataOptions shotDataOptions)
        : DeviceID(deviceID), Units(units), ShotNumber(shotNumber), APIversion(apiVersion),
        BallData(ballData), ClubData(clubData), ShotDataOptions(shotDataOptions) {}

    ShotData::~ShotData() { }

    void ShotData::from_json(const json& j, ShotData& s) {
        s.DeviceID = j["DeviceID"].get<std::string>();
        s.Units = j["Units"].get<std::string>();
        s.ShotNumber = j["ShotNumber"].get<int>();
        s.APIversion = j["APIversion"].get<std::string>();
        BallData::from_json(j["BallData"], s.BallData);
        ClubData::from_json(j["ClubData"], s.ClubData);
        ShotDataOptions::from_json(j["ShotDataOptions"], s.ShotDataOptions);
    }

    void to_json(json& j, const BallData& b) {
        j = json{
            {"Speed", b.Speed},
            {"SpinAxis", b.SpinAxis},
            {"TotalSpin", b.TotalSpin},
            {"BackSpin", b.BackSpin},
            {"SideSpin", b.SideSpin},
            {"HLA", b.HLA},
            {"VLA", b.VLA},
            {"CarryDistance", b.CarryDistance}
        };
    }

    void to_json(json& j, const ClubData& c) {
        j = json{
            {"Speed", c.Speed},
            {"AngleOfAttack", c.AngleOfAttack},
            {"FaceToTarget", c.FaceToTarget},
            {"Lie", c.Lie},
            {"Loft", c.Loft},
            {"Path", c.Path},
            {"SpeedAtImpact", c.SpeedAtImpact},
            {"VerticalFaceImpact", c.VerticalFaceImpact},
            {"HorizontalFaceImpact", c.HorizontalFaceImpact},
            {"ClosureRate", c.ClosureRate}
        };

    }
    void to_json(json& j, const ShotDataOptions& s) {
        j = json{
            {"ContainsBallData", s.ContainsBallData},
            {"ContainsClubData", s.ContainsClubData},
            {"LaunchMonitorIsReady", s.LaunchMonitorIsReady},
            {"LaunchMonitorBallDetected", s.LaunchMonitorBallDetected},
            {"IsHeartBeat", s.IsHeartBeat}
        };

    }

    void to_json(json& j, const ShotData& s) {
        j = json{
            {"DeviceID", s.DeviceID},
            {"Units", s.Units},
            {"ShotNumber", s.ShotNumber},
            {"APIversion", s.APIversion},
            {"BallData", s.BallData},
            {"ClubData", s.ClubData},
            {"ShotDataOptions", s.ShotDataOptions}
        };
    }
}
