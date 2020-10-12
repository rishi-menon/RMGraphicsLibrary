#pragma once
#include <memory>
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

namespace RMGraphics {
namespace Log {
    extern std::shared_ptr<spdlog::logger> g_CoreLogger;
    void SetPattern (const char* const strPattern="%^%v%$");
}
}

#define RMG_LOG_TRACE(...)            ::RMGraphics::Log::g_CoreLogger->trace (__VA_ARGS__)
#define RMG_LOG_INFO(...)             ::RMGraphics::Log::g_CoreLogger->info (__VA_ARGS__)
#define RMG_LOG_WARN(...)             ::RMGraphics::Log::g_CoreLogger->warn (__VA_ARGS__)
#define RMG_LOG_ERROR(...)            ::RMGraphics::Log::g_CoreLogger->error (__VA_ARGS__)
#define RMG_LOG_FATAL(...)            ::RMGraphics::Log::g_CoreLogger->critical (__VA_ARGS__)