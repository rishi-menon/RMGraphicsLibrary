#include <RMGraphics.h>

unsigned int imageId;

void EgOnStart()
{
    //To Log 
    int var1 = 5;
    float var2 = 9.5f;
    RMG_LOG_ERROR ("OnStartFunction error with variables {0}: {1}", var1, var2);
    RMG_LOG_WARN ("OnStartFunction warn with variables {0}: {1}", var1, var2);

    //Load a texture from 
    imageId = rmg::Texture::LoadTexture ("Assets/Images/bg.png");
    rmg::Font* defaultFont = rmg::Font::LoadFont ("Assets/Fonts/Quicksand/Quicksand Regular 400.ttf", 250);
    rmg::Renderer::SetDefaultFont (defaultFont);
}

void EgOnUpdate()
{
    float posx =  10;

    //Draw a Quad
    rmg::Renderer::DrawQuadCol_BottomLeft ({0, 0, 0}, {2000, 100}, rmg::Color::yellow);
    
    //Draw text: Left Aligned
    rmg::Renderer::DrawText ("Lorem ipsum dolor\nsit amet, ullamco laboris consectetur\nadipiscing elit", {posx, 800, 0}, 40.0f, rmg::Color::white, rmg::FontStyleDefault);

    //Draw text: Right Aligned
    rmg::Renderer::DrawText ("Lorem ipsum dolor\nsit amet, ullamco laboris consectetur\nadipiscing elit", {rmg::Input::GetScreenWidth() - 10, 500, 0}, 40.0f, rmg::Color::red, rmg::FontStyle(rmg::FontAlign::Right, 1.3));

    //Draw text: Center Aligned
    rmg::Renderer::DrawText ("Lorem ipsum dolor\nsit amet, ullamco laboris consectetur\nadipiscing elit", {rmg::Input::GetScreenWidth()/2.0f, 300, 0}, 40.0f, rmg::Color::yellow, rmg::FontStyle(rmg::FontAlign::Middle, 1.5));
    

    //Check to see if a key was pressed
    if (rmg::Input::GetKey (rmg::Keycode::Q))
    {
        rmg::Renderer::DrawQuadCol_BottomLeft ({0, 600, 0}, {2000, 100}, rmg::Color::white);
    }
}