#pragma once
namespace rmg {
    class Application;
    extern Application* g_pCurrentApp;

    extern int g_Argc;
    constexpr int g_nMaxArgc = 10;
    extern const char* g_Argv[g_nMaxArgc];  //We most probably wont have more than 10 arguments
}