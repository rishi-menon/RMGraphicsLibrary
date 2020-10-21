#pragma once
#include "Math/rmg_Math.h"
#include "Texture/rmg_Font.h"

namespace rmg {

namespace Renderer {
    
    void DrawQuadCol (const rmg::vec3& pos, const rmg::vec2& size, const Color& color);
    void DrawQuadCol_BottomLeft (const rmg::vec3& pos, const rmg::vec2& size, const Color& color);

    void DrawQuadTex (const rmg::vec3& pos, const rmg::vec2& size, const Color& color, unsigned int textureId);
    void DrawQuadTex_BottomLeft (const rmg::vec3& pos, const rmg::vec2& size, const Color& color, unsigned int textureId);

    void DrawText_Left (const char* strText, const rmg::vec3& posOriginal, float pixelSize, const Color& color, Font* pFont = nullptr);

    bool SetDefaultFont (const char* strFontPath, int nSizeY);
}
}