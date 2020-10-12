#include "rmg_Log_int.h"
#include <spdlog/sinks/stdout_color_sinks.h>

//This cpp file is not added to the makefile project and will NOT BE COMPILED
namespace RMGraphics {
namespace Log {
    std::shared_ptr<spdlog::logger> g_CoreLogger;
    std::shared_ptr<spdlog::logger> g_ClientLogger;
    void Init()
    {
        g_CoreLogger = ::spdlog::stdout_color_mt("Core");
		g_CoreLogger->set_level(spdlog::level::trace);
		g_CoreLogger->set_pattern("%^[%T] %n: %v%$");

        g_ClientLogger = ::spdlog::stdout_color_mt("Client");
		g_ClientLogger->set_level(spdlog::level::trace);
		g_ClientLogger->set_pattern("%^%v%$");
    }
    void SetPattern (const char* const strPattern /* = "%^%v%$"*/ )
    {
        g_ClientLogger->set_pattern(strPattern);
    }

    void SetLogLevel (::RMGraphics::LogLevel level)
    {
        
        g_ClientLogger->set_level(spdlog::level::level_enum ((int)(level)));
    }
    void SetInternalLogLevel(::RMGraphics::LogLevel level)
    {
        g_CoreLogger->set_level(spdlog::level::level_enum ((int)(level)));
    }
}
}