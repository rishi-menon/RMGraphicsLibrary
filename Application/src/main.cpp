#include <RMGraphics.h>

#include "Examples.h"

int main(int argc, const char* argv[])
{
    if (!rmg::Initialise(argc, argv))
    {
        RMG_LOG_ERROR ("Failed to initialise RMGraphics...");
        return 0;
    }
    rmg::Log::SetInternalLogLevel (rmg::LogLevel::warn);
    rmg::Log::SetLogLevel (rmg::LogLevel::trace);

    rmg::ivec2 size (1600, 1200);
    rmg::Application* myApp = new rmg::Application();
    if (!myApp->Initialise(size))
    {
        return 0;
    }
   
    EgOnStart();

    while (myApp->IsRunning())
    {
        myApp->ClearScreen();

        //Now you can finally start drawing
        
        EgOnUpdate();
        
        myApp->UpdateScreen();
        rmg::Sleep(2);
    }

    myApp->Cleanup();
    rmg::Cleanup();
    delete myApp;

    RMG_LOG_TRACE ("Terminating...");
    return 0;
}