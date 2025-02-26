#include "pch.h"

#include "../OpenConnectV1/Logger.h"
#include <gtest/gtest.h>
#include <sstream>

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

TEST_F(LoggerTest, LogsCorrectlyAtEachLevel) {
    OpenConnectV1::Logger::minLogLevel = OpenConnectV1::LogLevel::Info;

    OpenConnectV1::Logger::error("Test error message");
    OpenConnectV1::Logger::info("Test info message");
    OpenConnectV1::Logger::debug("Test debug message");

    std::string output = buffer.str();

    EXPECT_NE(output.find("[ERROR   ] Test error message"), std::string::npos);
    EXPECT_NE(output.find("[INFO    ] Test info message"), std::string::npos);
    EXPECT_EQ(output.find("[DEBUG   ] Test debug message"), std::string::npos);
}

TEST_F(LoggerTest, DebugLevelIncludesAllLogs) {
    OpenConnectV1::Logger::minLogLevel = OpenConnectV1::LogLevel::Debug;

    OpenConnectV1::Logger::error("Error message");
    OpenConnectV1::Logger::info("Info message");
    OpenConnectV1::Logger::debug("Debug message");

    std::string output = buffer.str();

    // Verify all messages appear
    EXPECT_NE(output.find("[ERROR   ] Error message"), std::string::npos);
    EXPECT_NE(output.find("[INFO    ] Info message"), std::string::npos);
    EXPECT_NE(output.find("[DEBUG   ] Debug message"), std::string::npos);
}

// Test that logs are ignored when the level is set higher
TEST_F(LoggerTest, LogsAreIgnoredIfBelowMinLogLevel) {
    OpenConnectV1::Logger::minLogLevel = OpenConnectV1::LogLevel::Error;

    OpenConnectV1::Logger::error("Error message");
    OpenConnectV1::Logger::info("Info message");
    OpenConnectV1::Logger::debug("Debug message");

    std::string output = buffer.str();

    EXPECT_NE(output.find("[ERROR   ] Error message"), std::string::npos);
    EXPECT_EQ(output.find("[INFO    ] Info message"), std::string::npos);
    EXPECT_EQ(output.find("[DEBUG   ] Debug message"), std::string::npos);
}

TEST_F(LoggerTest, HandlesStringParameterCorrectly) {
    std::string testString = "This is a test";
    OpenConnectV1::Logger::info("String: %s", testString.c_str());

    std::string logOutput = buffer.str();

    EXPECT_NE(logOutput.find("[INFO   ] String: This is a test"), std::string::npos);
}