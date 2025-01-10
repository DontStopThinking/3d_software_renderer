#pragma once
#include <array>
#include <cstdio>
#include <ctime>
#include <span>
#include <string_view>

// NOTE(sbalse): Not meant to be used outside of this file.
namespace Internal
{
    enum class LogLevel
    {
        INFO,
        ERROR
    };

    constexpr std::string_view LOG_COLOR_RESET = "\033[0m";
    constexpr std::string_view LOG_COLOR_GREEN = "\033[32m";
    constexpr std::string_view LOG_COLOR_RED = "\033[31m";

    void GetTimestamp(const std::span<char> buffer)
    {
        const std::time_t now = std::time(nullptr);
        std::tm localTime = {};
        localtime_s(&localTime, &now);
        std::strftime(buffer.data(), buffer.size(), "%Y-%m-%d %H:%M:%S", &localTime);
    }

    template<typename... Args>
    void LogMessage(
        const LogLevel level,
        const std::string_view color,
        const std::string_view format,
        const Args... args)
    {
        std::array<char, 32> timestamp = {};
        GetTimestamp(timestamp);

        std::array<char, 1024> message = {};
        if constexpr (sizeof...(args) > 0)
        {
            std::snprintf(message.data(), message.size(), format.data(), args...);
        }
        else
        {
            std::snprintf(message.data(), message.size(), "%s", format.data());
        }

        std::FILE* outStream = nullptr;
        std::string_view logLevelStr;

        switch (level)
        {
        case LogLevel::ERROR:
        {
            outStream = stderr;
            logLevelStr = "ERROR";
        } break;

        case LogLevel::INFO:
        {
            outStream = stdout;
            logLevelStr = "INFO";
        } break;

        default:
        {
            outStream = stdout;
            logLevelStr = "UNKNOWN";
        } break;
        }

        std::fprintf(
            outStream,
            "%s[%s] %s: %s%s\n",
            color.data(),
            logLevelStr.data(),
            timestamp.data(),
            message.data(),
            LOG_COLOR_RESET.data());
    }
} // namespace Internal

// NOTE(sbalse): Macros for logging.
#define LOG_INFO(...) \
do \
{ \
    LogMessage(Internal::LogLevel::INFO, Internal::LOG_COLOR_GREEN, __VA_ARGS__); \
} while (0);

#define LOG_ERROR(...) \
do \
{ \
    LogMessage(Internal::LogLevel::ERROR, Internal::LOG_COLOR_RED, __VA_ARGS__); \
} while (0);
