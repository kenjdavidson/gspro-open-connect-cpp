#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <iostream>
#include <cstdarg>
#include <cstdio>

namespace OpenConnectV1 {
    enum class LogLevel {
        Error,
        Info,
        Debug
    };
}

namespace OpenConnectV1 {
    class Logger {
    public:
        // Static member definition of LogLevel
        static LogLevel minLogLevel;

        static void error(const char* message, ...);
        static void info(const char* message, ...);
        static void debug(const char* message, ...);

    private:
        static void log(LogLevel level, const char* message, va_list args);
        static const char* logLevelToString(LogLevel level);
        static LogLevel stringToLogLevel(const char* level);
    };
}

#endif
