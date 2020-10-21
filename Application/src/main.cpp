#include <RMGraphics/RMGraphics.h>

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
    //rmg::Renderer::SetDefaultFont ("../")
    unsigned int id = rmg::Texture::LoadTexture ("Assets/Images/bg.png");
    rmg::Font* pFont = rmg::Font::LoadFont ("Assets/Fonts/Quicksand/Quicksand Regular 400.ttf", 250);

    while (myApp->IsRunning())
    {
        myApp->ClearScreen();

        //Now you can finally start drawing
    
        rmg::Renderer::DrawQuadCol_BottomLeft ({0, 0, 0}, {2000, 100}, rmg::Color::red);
        rmg::Renderer::DrawText_Left ("Hi there", {0, 100, 0}, 100.0f, rmg::Color::green, pFont);

        // rmg::Renderer::DrawQuadCol_BottomLeft ({0, 200, 0}, {2000, 100}, rmg::Color::red);
        // rmg::Renderer::DrawText_Left ("klmnopqrstuvwxyz", {0, 200, 0}, 100.0f, rmg::Color::white, pFont);
        myApp->UpdateScreen();
        rmg::Sleep(2);
    }

    myApp->Cleanup();
    delete myApp;

    RMG_LOG_TRACE ("Terminating...");
    rmg::Cleanup();
    return 0;
}