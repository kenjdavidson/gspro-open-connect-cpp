#include "pch.h"

#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include "../OpenConnectV1/Data.h"

using namespace OpenConnectV1;
using json = nlohmann::json;

// Test that BallData can be parsed from JSON
TEST(BallDataTest, FromJson) {
    // Create a sample JSON object to simulate the received JSON
    json j = R"({
        "Speed": 120.5,
        "SpinAxis": 10.2,
        "TotalSpin": 3000,
        "BackSpin": 1500,
        "SideSpin": 100,
        "HLA": 5.0,
        "VLA": 10.0,
        "CarryDistance": 250.0
    })"_json;

    BallData ballData;
    BallData::from_json(j, ballData);  // Use the from_json function

    // Check if the values are correctly parsed
    EXPECT_FLOAT_EQ(ballData.Speed, 120.5);
    EXPECT_FLOAT_EQ(ballData.SpinAxis, 10.2);
    EXPECT_FLOAT_EQ(ballData.TotalSpin, 3000);
    EXPECT_FLOAT_EQ(ballData.BackSpin, 1500);
    EXPECT_FLOAT_EQ(ballData.SideSpin, 100);
    EXPECT_FLOAT_EQ(ballData.HLA, 5.0);
    EXPECT_FLOAT_EQ(ballData.VLA, 10.0);
    EXPECT_FLOAT_EQ(ballData.CarryDistance, 250.0);
}

TEST(BallDataTest, FromJsonHandlesNaNValuesAsEmpty) {
    json j = {
        {"Speed", 120.5},
        {"SpinAxis", std::numeric_limits<float>::quiet_NaN()},
        {"TotalSpin", 2500},
        {"BackSpin", std::numeric_limits<float>::quiet_NaN()},
        {"SideSpin", 300},
        {"HLA", std::numeric_limits<float>::quiet_NaN()},
        {"VLA", 15.2},
        {"CarryDistance", 250.7}
    };

    BallData ballData;
    BallData::from_json(j, ballData);

    // Check if values exist (not NaN)
    EXPECT_FALSE(std::isnan(ballData.Speed));
    EXPECT_FALSE(std::isnan(ballData.TotalSpin));
    EXPECT_FALSE(std::isnan(ballData.SideSpin));
    EXPECT_FALSE(std::isnan(ballData.VLA));
    EXPECT_FALSE(std::isnan(ballData.CarryDistance));

    // Check if values are missing (NaN)
    EXPECT_TRUE(std::isnan(ballData.SpinAxis));
    EXPECT_TRUE(std::isnan(ballData.BackSpin));
    EXPECT_TRUE(std::isnan(ballData.HLA));

    // Compare values
    EXPECT_FLOAT_EQ(ballData.Speed, 120.5f);
    EXPECT_FLOAT_EQ(ballData.TotalSpin, 2500.0f);
    EXPECT_FLOAT_EQ(ballData.SideSpin, 300.0f);
    EXPECT_FLOAT_EQ(ballData.VLA, 15.2f);
    EXPECT_FLOAT_EQ(ballData.CarryDistance, 250.7f);
}

// Test that ClubData can be parsed from JSON
TEST(ClubDataTest, FromJson) {
    // Create a sample JSON object for ClubData
    json j = R"({
        "Speed": 95.5,
        "AngleOfAttack": 3.0,
        "FaceToTarget": 0.5,
        "Lie": 1.5,
        "Loft": 12.0,
        "Path": 1.0,
        "SpeedAtImpact": 98.5,
        "VerticalFaceImpact": 0.2,
        "HorizontalFaceImpact": 0.1,
        "ClosureRate": 1.2
    })"_json;

    ClubData clubData;
    ClubData::from_json(j, clubData);  // Use the from_json function

    // Check if the values are correctly parsed
    EXPECT_FLOAT_EQ(clubData.Speed, 95.5);
    EXPECT_FLOAT_EQ(clubData.AngleOfAttack, 3.0);
    EXPECT_FLOAT_EQ(clubData.FaceToTarget, 0.5);
    EXPECT_FLOAT_EQ(clubData.Lie, 1.5);
    EXPECT_FLOAT_EQ(clubData.Loft, 12.0);
    EXPECT_FLOAT_EQ(clubData.Path, 1.0);
    EXPECT_FLOAT_EQ(clubData.SpeedAtImpact, 98.5);
    EXPECT_FLOAT_EQ(clubData.VerticalFaceImpact, 0.2);
    EXPECT_FLOAT_EQ(clubData.HorizontalFaceImpact, 0.1);
    EXPECT_FLOAT_EQ(clubData.ClosureRate, 1.2);
}

TEST(ClubDataTest, FromJsonHandlesNaNValuesAsEmpty) {
    json j = {
        {"Speed", 120.5},
        {"AngleOfAttack", std::numeric_limits<float>::quiet_NaN()},
        {"FaceToTarget", 0.5},
        {"Lie", std::numeric_limits<float>::quiet_NaN()},
        {"Loft", 15.0},
        {"Path", std::numeric_limits<float>::quiet_NaN()},
        {"SpeedAtImpact", 130.2},
        {"VerticalFaceImpact", std::numeric_limits<float>::quiet_NaN()},
        {"HorizontalFaceImpact", 0.7},
        {"ClosureRate", std::numeric_limits<float>::quiet_NaN()}
    };

    ClubData clubData;
    ClubData::from_json(j, clubData);

    // Check if values exist (not NaN)
    EXPECT_FALSE(std::isnan(clubData.Speed));
    EXPECT_FALSE(std::isnan(clubData.FaceToTarget));
    EXPECT_FALSE(std::isnan(clubData.Loft));
    EXPECT_FALSE(std::isnan(clubData.SpeedAtImpact));
    EXPECT_FALSE(std::isnan(clubData.HorizontalFaceImpact));

    // Check if values are missing (NaN)
    EXPECT_TRUE(std::isnan(clubData.AngleOfAttack));
    EXPECT_TRUE(std::isnan(clubData.Lie));
    EXPECT_TRUE(std::isnan(clubData.Path));
    EXPECT_TRUE(std::isnan(clubData.VerticalFaceImpact));
    EXPECT_TRUE(std::isnan(clubData.ClosureRate));

    // Compare values
    EXPECT_FLOAT_EQ(clubData.Speed, 120.5f);
    EXPECT_FLOAT_EQ(clubData.FaceToTarget, 0.5f);
    EXPECT_FLOAT_EQ(clubData.Loft, 15.0f);
    EXPECT_FLOAT_EQ(clubData.SpeedAtImpact, 130.2f);
    EXPECT_FLOAT_EQ(clubData.HorizontalFaceImpact, 0.7f);
}

// Test that ShotDataOptions can be parsed from JSON
TEST(ShotDataOptionsTest, FromJson) {
    // Create a sample JSON object for ShotDataOptions
    json j = R"({
        "ContainsBallData": true,
        "ContainsClubData": true,
        "LaunchMonitorIsReady": false,
        "LaunchMonitorBallDetected": true,
        "IsHeartBeat": false
    })"_json;

    ShotDataOptions shotDataOptions;
    ShotDataOptions::from_json(j, shotDataOptions);  // Use the from_json function

    // Check if the values are correctly parsed
    EXPECT_TRUE(shotDataOptions.ContainsBallData);
    EXPECT_TRUE(shotDataOptions.ContainsClubData);
    EXPECT_FALSE(shotDataOptions.LaunchMonitorIsReady);
    EXPECT_TRUE(shotDataOptions.LaunchMonitorBallDetected);
    EXPECT_FALSE(shotDataOptions.IsHeartBeat);
}

// Test that ShotData can be parsed from JSON
TEST(ShotDataTest, FromJson) {
    // Create a sample JSON object for ShotData
    json j = R"({
        "DeviceID": "TestDevice",
        "Units": "Yards",
        "ShotNumber": 1,
        "APIversion": "1",
        "BallData": {
            "Speed": 120.5,
            "SpinAxis": 10.2,
            "TotalSpin": 3000,
            "BackSpin": 1500,
            "SideSpin": 100,
            "HLA": 5.0,
            "VLA": 10.0,
            "CarryDistance": 250.0
        },
        "ClubData": {
            "Speed": 95.5,
            "AngleOfAttack": 3.0,
            "FaceToTarget": 0.5,
            "Lie": 1.5,
            "Loft": 12.0,
            "Path": 1.0,
            "SpeedAtImpact": 98.5,
            "VerticalFaceImpact": 0.2,
            "HorizontalFaceImpact": 0.1,
            "ClosureRate": 1.2
        },
        "ShotDataOptions": {
            "ContainsBallData": true,
            "ContainsClubData": true,
            "LaunchMonitorIsReady": false,
            "LaunchMonitorBallDetected": true,
            "IsHeartBeat": false
        }
    })"_json;

    ShotData shotData;
    ShotData::from_json(j, shotData);  // Use the from_json function

    // Check if the values are correctly parsed
    EXPECT_EQ(shotData.DeviceID, "TestDevice");
    EXPECT_EQ(shotData.Units, "Yards");
    EXPECT_EQ(shotData.ShotNumber, 1);
    EXPECT_EQ(shotData.APIversion, "1");

    // Check that nested objects are correctly parsed
    EXPECT_FLOAT_EQ(shotData.BallData.Speed, 120.5);
    EXPECT_FLOAT_EQ(shotData.ClubData.Speed, 95.5);
    EXPECT_TRUE(shotData.ShotDataOptions.ContainsBallData);
}

