#include "pch.h"

#include "../OpenConnectV1/Logger.h"
#include <gtest/gtest.h>
#include <sstream>

// Test fixture for Logger
class LoggerTest : public ::testing::Test {
protected:
    std::stringstream buffer;
    std::streambuf* oldCout;

    void SetUp() override {
        // Redirect std::cout to buffer
        oldCout = std::cout.rdbuf(buffer.rdbuf());
    }

    void TearDown() override {
        // Restore std::cout
        std::cout.rdbuf(oldCout);
    }
};

// Test logging at different levels
TEST_F(LoggerTest, LogsCorrectlyAtEachLevel) {
    OpenConnectV1::Logger::minLogLevel = OpenConnectV1::LogLevel::Info;

    OpenConnectV1::Logger::error("Test error message");
    OpenConnectV1::Logger::info("Test info message");
    OpenConnectV1::Logger::debug("Test debug message"); // Should NOT be logged

    std::string output = buffer.str();

    // Verify ERROR and INFO messages appear, DEBUG does not
    EXPECT_NE(output.find("[ERROR   ] Test error message"), std::string::npos);
    EXPECT_NE(output.find("[INFO    ] Test info message"), std::string::npos);
    EXPECT_EQ(output.find("[DEBUG   ] Test debug message"), std::string::npos);
}

// Test that DEBUG messages appear when the log level is set to DEBUG
TEST_F(LoggerTest, DebugLevelIncludesAllLogs) {
    OpenConnectV1::Logger::minLogLevel = OpenConnectV1::LogLevel::Debug;

    OpenConnectV1::Logger::error("Error message");
    OpenConnectV1::Logger::info("Info message");
    OpenConnectV1::Logger::debug("Debug message"); // Now should be logged

    std::string output = buffer.str();

    // Verify all messages appear
    EXPECT_NE(output.find("[ERROR   ] Error message"), std::string::npos);
    EXPECT_NE(output.find("[INFO    ] Info message"), std::string::npos);
    EXPECT_NE(output.find("[DEBUG   ] Debug message"), std::string::npos);
}

// Test that logs are ignored when the level is set higher
TEST_F(LoggerTest, LogsAreIgnoredIfBelowMinLogLevel) {
    OpenConnectV1::Logger::minLogLevel = OpenConnectV1::LogLevel::Error;

    OpenConnectV1::Logger::error("Error message");  // Should be logged
    OpenConnectV1::Logger::info("Info message");    // Should NOT be logged
    OpenConnectV1::Logger::debug("Debug message");  // Should NOT be logged

    std::string output = buffer.str();

    // Only ERROR should be present
    EXPECT_NE(output.find("[ERROR   ] Error message"), std::string::npos);
    EXPECT_EQ(output.find("[INFO    ] Info message"), std::string::npos);
    EXPECT_EQ(output.find("[DEBUG   ] Debug message"), std::string::npos);
}
