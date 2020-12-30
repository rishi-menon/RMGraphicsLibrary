#pragma once
#include "rmg_Log.h"

namespace rmg {
namespace Log {
    extern std::shared_ptr<spdlog::logger> g_CoreLogger;
    void Init();

    void rmg_glClearError();
    bool rmg_glCheckError();
}
}

//Internal logging systems
#define RMG_LOG_ITRACE(...)            ::rmg::Log::g_CoreLogger->trace (__VA_ARGS__)
#define RMG_LOG_IINFO(...)             ::rmg::Log::g_CoreLogger->info (__VA_ARGS__)
#define RMG_LOG_IWARN(...)             ::rmg::Log::g_CoreLogger->warn (__VA_ARGS__)
#define RMG_LOG_IERROR(...)            ::rmg::Log::g_CoreLogger->error (__VA_ARGS__)
#define RMG_LOG_IFATAL(...)            ::rmg::Log::g_CoreLogger->critical (__VA_ARGS__)

#define IASSERT(x, ...)              do { \
                                        if (!(x)) { \
                                            RMG_LOG_IERROR(__VA_ARGS__); \
                                            RMG_BREAK (); \
                                        } \
                                    } while(0)


#define rmg_glcall(x)                   do { \
                                            ::rmg::Log::rmg_glClearError(); \
                                            x ; \
                                            if (::rmg::Log::rmg_glCheckError()) \
                                            { \
                                                RMG_LOG_IERROR ("RMG Error: OpenGL error at line {0}: {1}", __LINE__, #x); \
                                            } \
                                        } while (0)


