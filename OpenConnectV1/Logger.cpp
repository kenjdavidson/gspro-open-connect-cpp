#include <cstdarg>
#include <iomanip>

#include "Logger.h"

namespace OpenConnectV1 {
    // Define the static member variable
    LogLevel Logger::minLogLevel = LogLevel::Info;

    // LogLevel helper functions
    const char* Logger::logLevelToString(LogLevel level) {
        switch (level) {
        case LogLevel::Error: return "ERROR";
        case LogLevel::Info: return "INFO";
        case LogLevel::Debug: return "DEBUG";
        default: return "UNKNOWN";
        }
    }

    LogLevel Logger::stringToLogLevel(const char* level) {
        if (strcmp(level, "Error") == 0) return LogLevel::Error;
        if (strcmp(level, "Info") == 0) return LogLevel::Info;
        if (strcmp(level, "Debug") == 0) return LogLevel::Debug;
        return LogLevel::Info;  // Default if unrecognized
    }

    // Implement logging functions
    void Logger::log(LogLevel level, const char* message, va_list args) {
        if (level > minLogLevel) return;

        constexpr size_t BUFFER_SIZE = 1024; 
        char buffer[BUFFER_SIZE];

        vsnprintf(buffer, BUFFER_SIZE, message, args);

        const char* levelStr = logLevelToString(level);

        std::cout << "[" << std::left << std::setw(8) << levelStr << "] " << buffer << std::endl;
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
