#include "rmg_Random.h"
#include "rmg_Random_int.h"
#include <random>
#include <chrono>

namespace rmg {
//////////////////////////////////////////////////////
////                  Math                        ////
//////////////////////////////////////////////////////
namespace Math
{
    float Lerp (float x, float y, float t)
    {
        return x + (y-x)*t;
    }
    float Clamp (float min, float max, float value)
    {
        if (value < min)        return min;
        else if (value > max)   return max;
        else                    return value;
    }
    float ClampLeft (float min, float value)
    {
        if (value < min)        return min;
        else                    return value;
    }
    float ClampRight (float max, float value)
    {
        if (value > max)   return max;
        else               return value;
    }
    float Clamp01(float value)
    {
        if (value < 0.0f)        return 0.0f;
        else if (value > 1.0f)   return 1.0f;
        else                     return value;
    }
    
}


//////////////////////////////////////////////////////
////                Random                        ////
//////////////////////////////////////////////////////
namespace Random
{
    static std::default_random_engine generator;
	static std::uniform_real_distribution<double> distReal(0.0f, 1.0f);
    void Init()
    {
        std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();
		long long value = std::chrono::time_point_cast <std::chrono::milliseconds> (timeNow).time_since_epoch().count();
		generator.seed(static_cast<unsigned int>(value));

    }
    float Get()
    {
        return distReal(generator);
    }
    float GetRange (float min, float max)
    {
        return min + (max-min) * distReal(generator);
    }
    int GetRange (int min, int max)
    {
        int num = (int)(min + (max-min+1) * distReal(generator));
        return (num > max) ? max : num;
    }
}

//////////////////////////////////////////////////////
////                Color                         ////
//////////////////////////////////////////////////////
Color Color::white (255, 255, 255, 255);
Color Color::black (0, 0, 0, 255);
Color Color::red (230, 26, 26, 255);
Color Color::green (43, 198, 23, 255);
Color Color::blue (16, 0, 187, 255);
Color Color::pink (246, 100, 252, 255);
Color Color::purple (124, 68, 255, 255);  
Color Color::yellow (240, 240, 20, 255);
Color Color::orange (240, 133, 20, 255); 
Color Color::DarkGrey (40, 40, 40, 255); 

rmg::vec4 Color::ToPercent () const
{
    rmg::vec4 color;
    color.r = (float)r / 255.0f;
    color.g = (float)g / 255.0f;
    color.b = (float)b / 255.0f;
    color.a = (float)a / 255.0f;
    return color;
}
}