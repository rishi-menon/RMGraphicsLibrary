#include <spdlog/spdlog.h>
#include <RMGraphics/RMGraphics.h>
#include <chrono>
#include <thread>
int main()
{
    if (!RMGraphics::Initialise())
    {
        RMG_LOG_ERROR ("Failed to initialise RMGraphics...");
        return 0;
    }
    RMGraphics::Log::SetInternalLogLevel (RMGraphics::LogLevel::trace);
    RMGraphics::Log::SetLogLevel (RMGraphics::LogLevel::trace);
    RMG_LOG_TRACE ("Initialised at {0}", RMGraphics::Time::GetTime());

    RMGraphics::ivec2 size (1600, 1200);
    RMGraphics::Application* myApp = new RMGraphics::Application();
    if (!myApp->Initialise(size))
    {
        return false;
    }

    std::chrono::milliseconds chronoRegularSleep(2);

    while (myApp->IsRunning())
    {
        myApp->ClearScreen();
        RMG_LOG_TRACE ("Time: {0}", RMGraphics::Time::GetTime());

        myApp->UpdateScreen();
        std::this_thread::sleep_for (chronoRegularSleep);
    }

    myApp->Cleanup();
    delete myApp;

    RMG_LOG_TRACE ("Terminating...");
    RMGraphics::Cleanup();
    return 0;
}