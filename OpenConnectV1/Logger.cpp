#include "Logger.h"
#include <cstdarg>

namespace OpenConnectV1 {
    // Define the static member variable
    LogLevel Logger::minLogLevel = LogLevel::Info;  // Default value

    // LogLevel helper functions
    const char* Logger::logLevelToString(LogLevel level) {
        switch (level) {
        case LogLevel::Error: return "Error";
        case LogLevel::Info: return "Info";
        case LogLevel::Debug: return "Debug";
        default: return "Unknown";
        }
    }

    LogLevel Logger::stringToLogLevel(const char* level) {
        if (strcmp(level, "Error") == 0) return LogLevel::Error;
        if (strcmp(level, "Info") == 0) return LogLevel::Info;
        if (strcmp(level, "Debug") == 0) return LogLevel::Debug;
        return LogLevel::Info;  // Default if unrecognized
    }

    // Implement logging functions
    void Logger::log(LogLevel level, const char* message, ...) {
        if (level > minLogLevel) return;

        va_list args;
        va_start(args, message);

        const char* levelStr = logLevelToString(level);
        printf("[%s] ", levelStr);
        vprintf(message, args);
        printf("\n");

        va_end(args);
    }

    void Logger::error(const char* message, ...) {
        va_list args;
        va_start(args, message);
        log(LogLevel::Error, message, args);
        va_end(args);
    }

    void Logger::info(const char* message, ...) {
        va_list args;
        va_start(args, message);
        log(LogLevel::Info, message, args);
        va_end(args);
    }

    void Logger::debug(const char* message, ...) {
        va_list args;
        va_start(args, message);
        log(LogLevel::Debug, message, args);
        va_end(args);
    }
}
