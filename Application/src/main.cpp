#include <RMGraphics/RMGraphics.h>

int main(int argc, const char* argv[])
{
    if (!rmg::Initialise(argc, argv))
    {
        RMG_LOG_ERROR ("Failed to initialise RMGraphics...");
        return 0;
    }
    rmg::Log::SetInternalLogLevel (rmg::LogLevel::info);
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
        float posx =  10;

        rmg::Renderer::DrawQuadCol_BottomLeft ({0, 0, 0}, {2000, 100}, rmg::Color::red);
        rmg::Renderer::DrawText ("Lorem ipsum dolor\nsit amet, ullamco laboris consectetur\nadipiscing elit", {posx, 800, 0}, 40.0f, rmg::Color::green, rmg::FontStyleDefault, pFont);

        rmg::Renderer::DrawText ("Lorem ipsum dolor\nsit amet, ullamco laboris consectetur\nadipiscing elit", {rmg::Input::GetScreenWidth() - 10, 500, 0}, 40.0f, rmg::Color::red, rmg::FontStyle(rmg::FontAlign::Right, 1.3), pFont);

        rmg::Renderer::DrawText ("Lorem ipsum dolor\nsit amet, ullamco laboris consectetur\nadipiscing elit", {rmg::Input::GetScreenWidth()/2.0f, 300, 0}, 40.0f, rmg::Color::yellow, rmg::FontStyle(rmg::FontAlign::Middle, 1.5), pFont);
        
        myApp->UpdateScreen();
        rmg::Sleep(2);
    }

    myApp->Cleanup();
    delete myApp;

    RMG_LOG_TRACE ("Terminating...");
    rmg::Cleanup();
    return 0;
}