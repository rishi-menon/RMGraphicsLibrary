#pragma once
#include <string>

namespace rmg {
namespace Texture
{
    bool Init();
    void Cleanup();
    unsigned int LoadTextureBuffer (unsigned char buffer[], int nSizeX, int nSizeY);

    const std::string& GetCWD();
}
}