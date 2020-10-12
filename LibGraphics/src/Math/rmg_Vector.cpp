#include "rmg_Vector.h"

namespace RMGraphics {

////////////////////////////////////////////////////
//                   Vector2                      //
////////////////////////////////////////////////////
#if 0
std::ostream& operator<< (std::ostream& os, const Vector2& vec)
{
    os << "x: "<< vec.x << " y: " << vec.y;
    return os;
}

Vector2 operator+ (const Vector2& v1, const Vector2& v2)
{
    return Vector2(v1.x + v2.x, v1.y + v2.y);
}
Vector2 operator- (const Vector2& v1, const Vector2& v2)
{
    return Vector2(v1.x - v2.x, v1.y - v2.y);

}
Vector2 operator* (const Vector2& v1, const Vector2& v2)
{
    return Vector2(v1.x * v2.x, v1.y * v2.y);
}

Vector2 operator* (const Vector2& v1, VectorType scaler)
{
    return Vector2(v1.x * scaler, v1.y * scaler);
}
Vector2 operator* (VectorType scaler, const Vector2& v1)
{
    return Vector2(v1.x * scaler, v1.y * scaler);
}
    
Vector2 Vector2::operator- ()
{
    return Vector2(-x, -y);
}

Vector2& Vector2::operator+= (const Vector2& v1)
{
    x += v1.x;
    y += v1.y;
    return *this;
}
Vector2& Vector2::operator+= (VectorType scaler)
{
    x += scaler;
    y += scaler;
    return *this;
}
Vector2& Vector2::operator-= (const Vector2& v1)
{
    x -= v1.x;
    y -= v1.y;
    return *this;
}
Vector2& Vector2::operator-= (VectorType scaler)
{
    x -= scaler;
    y -= scaler;
    return *this;
}
Vector2& Vector2::operator*= (const Vector2& v1)
{
    x *= v1.x;
    y *= v1.y;
    return *this;
}
Vector2& Vector2::operator*= (VectorType scaler)
{
    x *= scaler;
    y *= scaler;
    return *this;
}
Vector2& Vector2::operator/= (VectorType scaler)
{
    if (scaler != (VectorType)(0)) {
        x /= scaler;
        y /= scaler;
    }
    return *this;
}

////////////////////////////////////////////////////
//                   Vector3                      //
////////////////////////////////////////////////////

////////////////////////////////////////////////////
//                   Vector4                      //
////////////////////////////////////////////////////
#else

#endif
}