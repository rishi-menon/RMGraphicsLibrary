#pragma once
namespace RMGraphics {
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
}