#include "pch.h"

#include <gtest/gtest.h>
#include "../OpenConnectV1/Data.h"
#include "../OpenConnectV1/ShotDataListener.h"

// Test class for ShotDataListener
class ShotDataListenerTest : public ::testing::Test {
protected:
    // Subclass of ShotDataListener for testing purposes
    class TestShotDataListener : public ShotDataListener {
    public:
        bool wasCalled = false;  // Flag to track if onShotDataReceived is called

        void onShotDataReceived(const OpenConnectV1::ShotData& shotData) override {
            // Set the flag when the method is called
            wasCalled = true;
            // Optionally, you could also check the contents of shotData here
            receivedShotData = shotData;
        }

        OpenConnectV1::ShotData receivedShotData;  // Store the received data for validation
    };

    TestShotDataListener testListener;  // Instance of the test listener
    OpenConnectV1::ShotData mockShotData;  // Mock shot data for testing

    void SetUp() override {
        // Initialize mockShotData with sample values
        mockShotData.DeviceID = "TestDevice";
        mockShotData.Units = "Yards";
        mockShotData.ShotNumber = 1;
        mockShotData.APIversion = "1.0";

        // Initialize BallData
        mockShotData.BallData.Speed = 120.0f;
        mockShotData.BallData.SpinAxis = 5.0f;
        mockShotData.BallData.TotalSpin = 3000.0f;
        mockShotData.BallData.BackSpin = 2500.0f;
        mockShotData.BallData.SideSpin = 500.0f;
        mockShotData.BallData.HLA = 15.0f;
        mockShotData.BallData.VLA = 45.0f;
        mockShotData.BallData.CarryDistance = 250.0f;

        // Initialize ClubData
        mockShotData.ClubData.Speed = 95.0f;
        mockShotData.ClubData.AngleOfAttack = 5.0f;
        mockShotData.ClubData.FaceToTarget = 1.0f;
        mockShotData.ClubData.Lie = 58.0f;
        mockShotData.ClubData.Loft = 10.5f;
        mockShotData.ClubData.Path = 1.2f;
        mockShotData.ClubData.SpeedAtImpact = 100.0f;
        mockShotData.ClubData.VerticalFaceImpact = 2.0f;
        mockShotData.ClubData.HorizontalFaceImpact = -1.0f;
        mockShotData.ClubData.ClosureRate = 0.5f;

        // Initialize ShotDataOptions
        mockShotData.ShotDataOptions.ContainsBallData = true;
        mockShotData.ShotDataOptions.ContainsClubData = true;
        mockShotData.ShotDataOptions.LaunchMonitorIsReady = true;
        mockShotData.ShotDataOptions.LaunchMonitorBallDetected = false;
        mockShotData.ShotDataOptions.IsHeartBeat = false;
    }
};

TEST_F(ShotDataListenerTest, TestOnShotDataReceived) {
    // Call onShotDataReceived with mockShotData
    testListener.onShotDataReceived(mockShotData);

    // Check that onShotDataReceived was called
    EXPECT_TRUE(testListener.wasCalled);

    // Optionally, check if the receivedShotData matches the mock data
    EXPECT_EQ(testListener.receivedShotData.DeviceID, "TestDevice");
    EXPECT_EQ(testListener.receivedShotData.Units, "Yards");
    EXPECT_EQ(testListener.receivedShotData.ShotNumber, 1);
    EXPECT_EQ(testListener.receivedShotData.APIversion, "1.0");

    // Check BallData
    EXPECT_EQ(testListener.receivedShotData.BallData.Speed, 120.0f);
    EXPECT_EQ(testListener.receivedShotData.BallData.SpinAxis, 5.0f);
    EXPECT_EQ(testListener.receivedShotData.BallData.TotalSpin, 3000.0f);

    // Check ClubData
    EXPECT_EQ(testListener.receivedShotData.ClubData.Speed, 95.0f);
    EXPECT_EQ(testListener.receivedShotData.ClubData.AngleOfAttack, 5.0f);

    // Check ShotDataOptions
    EXPECT_TRUE(testListener.receivedShotData.ShotDataOptions.ContainsBallData);
    EXPECT_FALSE(testListener.receivedShotData.ShotDataOptions.LaunchMonitorBallDetected);
    EXPECT_FALSE(testListener.receivedShotData.ShotDataOptions.IsHeartBeat);
}
