#pragma once
#include <memory>
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#include <cassert>

namespace rmg {
enum class LogLevel : char 
{
    trace = 0,
    info = 2,
    warn,
    error,
    fatal,
    off,
};

namespace Log {
    extern std::shared_ptr<spdlog::logger> g_ClientLogger;
    void SetPattern (const char* const strPattern="%^%v%$");

    void SetLogLevel (::rmg::LogLevel level);
    void SetInternalLogLevel(::rmg::LogLevel level);
}
}

#define RMG_LOG_TRACE(...)            ::rmg::Log::g_ClientLogger->trace (__VA_ARGS__)
#define RMG_LOG_INFO(...)             ::rmg::Log::g_ClientLogger->info (__VA_ARGS__)
#define RMG_LOG_WARN(...)             ::rmg::Log::g_ClientLogger->warn (__VA_ARGS__)
#define RMG_LOG_ERROR(...)            ::rmg::Log::g_ClientLogger->error (__VA_ARGS__)
#define RMG_LOG_FATAL(...)            ::rmg::Log::g_ClientLogger->critical (__VA_ARGS__)

#define ASSERT(x, ...)              do { \
                                        if (!(x)) { \
                                            RMG_LOG_ERROR(__VA_ARGS__); \
                                            assert (false); \
                                        } \
                                    } while(0)
