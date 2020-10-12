#include "rmg_Log_int.h"
#include <spdlog/sinks/stdout_color_sinks.h>

//This cpp file is not added to the makefile project and will NOT BE COMPILED
namespace RMGraphics {
namespace Log {
    std::shared_ptr<spdlog::logger> g_CoreLogger;

    void Init()
    {
        g_CoreLogger = ::spdlog::stdout_color_mt("Log");
		g_CoreLogger->set_level(spdlog::level::trace);
		g_CoreLogger->set_pattern("%^%v%$");
    }
    void SetPattern (const char* const strPattern /* = "%^%v%$"*/ )
    {
        g_CoreLogger->set_pattern(strPattern);
    }
}
}