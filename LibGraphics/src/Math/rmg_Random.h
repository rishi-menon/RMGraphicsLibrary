#pragma once
#include "stdint.h"
#include "Math/rmg_Vector.h"

namespace rmg {
namespace Math
{
    float Lerp (float x, float y, float t);
    float Clamp (float min, float max, float value);
    float ClampLeft (float min, float value);
    float ClampRight (float max, float value);
    float Clamp01(float value);
}
namespace Random
{
    //returns a random number from 0 to 1
    float Get();
    float GetRange (float min, float max);
    int GetRange (int min, int max);
}

class Color
{
public:
    static Color white, black, red, green, blue, pink, purple;    
public:
    Color()=default;
    Color(const Color&)=default;
    Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255): r(r), g(g), b(b), a(a) {}

    rmg::vec4 ToPercent () const;

public:
    uint8_t r,g,b,a;
};
}