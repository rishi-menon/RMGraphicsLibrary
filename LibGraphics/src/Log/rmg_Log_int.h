#pragma once
#include "rmg_Log.h"

namespace RMGraphics {
namespace Log {
    extern std::shared_ptr<spdlog::logger> g_CoreLogger;
    void Init();
}
}

//Internal logging systems
#define RMG_LOG_ITRACE(...)            ::RMGraphics::Log::g_CoreLogger->trace (__VA_ARGS__)
#define RMG_LOG_IINFO(...)             ::RMGraphics::Log::g_CoreLogger->info (__VA_ARGS__)
#define RMG_LOG_IWARN(...)             ::RMGraphics::Log::g_CoreLogger->warn (__VA_ARGS__)
#define RMG_LOG_IERROR(...)            ::RMGraphics::Log::g_CoreLogger->error (__VA_ARGS__)
#define RMG_LOG_IFATAL(...)            ::RMGraphics::Log::g_CoreLogger->critical (__VA_ARGS__)

#define IASSERT(x, ...)              do { \
                                        if (!(x)) { \
                                            RMG_LOG_IERROR(__VA_ARGS__); \
                                            assert (false); \
                                        } \
                                    } while(0)
