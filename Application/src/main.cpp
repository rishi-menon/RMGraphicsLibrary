#include <RMGraphics/RMGraphics.h>

int main(int argc, const char* argv[])
{
    if (!rmg::Initialise())
    {
        RMG_LOG_ERROR ("Failed to initialise RMGraphics...");
        return 0;
    }
    rmg::Log::SetInternalLogLevel (rmg::LogLevel::trace);
    rmg::Log::SetLogLevel (rmg::LogLevel::trace);

    rmg::ivec2 size (1600, 1200);
    rmg::Application* myApp = new rmg::Application();
    if (!myApp->Initialise(size))
    {
        return 0;
    }

    while (myApp->IsRunning())
    {
        myApp->ClearScreen();
        rmg::Keycode key = rmg::Keycode::A;
        if (rmg::Input::GetKey(key))
        {
            RMG_LOG_INFO ("Key A");
        }
        if (rmg::Input::GetKeyDown(key))
        {
            RMG_LOG_WARN ("Key A was just pressed");
        }
        if (rmg::Input::GetKeyUp(key))
        {
            RMG_LOG_WARN ("Key A was just released");
        }

        myApp->UpdateScreen();
        rmg::Sleep(2);
    }

    myApp->Cleanup();
    delete myApp;

    RMG_LOG_TRACE ("Terminating...");
    rmg::Cleanup();
    return 0;
}