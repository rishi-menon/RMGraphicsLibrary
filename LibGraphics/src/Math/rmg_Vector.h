#pragma once
#include "Log/rmg_log.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtx/compatibility.hpp>

namespace RMGraphics {

#if 0
class Vector2;
class Vector3;
class Vector4;

typedef float VectorType;

class Vector2 {
public:
    Vector2() = default;
    Vector2(const Vector2&) = default;
    Vector2(VectorType x, VectorType y) : x(x), y(y) {}
     
    //For logging
    friend std::ostream& operator<< (std::ostream& os, const Vector2& vec);
    
    //Operators
    friend Vector2 operator+ (const Vector2& v1, const Vector2& v2);
    friend Vector2 operator- (const Vector2& v1, const Vector2& v2);
    friend Vector2 operator* (const Vector2& v1, const Vector2& v2);

    friend Vector2 operator* (const Vector2& v1, VectorType scaler);
    friend Vector2 operator* (VectorType scaler, const Vector2& v1);
    
    Vector2 operator- ();

    //Affects the Original object
    Vector2& operator+= (const Vector2& v1);
    Vector2& operator+= (VectorType scaler);
    Vector2& operator-= (const Vector2& v1);
    Vector2& operator-= (VectorType scaler);

    Vector2& operator*= (const Vector2& v1);
    Vector2& operator*= (VectorType scaler);
    Vector2& operator/= (VectorType scaler);

    VectorType x,y;
};

class Vector3 {
public:
    Vector3() = default;
    Vector3(const Vector3&) = default;
    Vector3(VectorType x, VectorType y, VectorType z) : x(x), y(y), z(z) {}
    
    VectorType x,y,z;
};

class Vector4 {
public:
    Vector4() = default;
    Vector4(const Vector4&) = default;
    Vector4(VectorType x, VectorType y, VectorType z, VectorType w) : x(x), y(y), z(z), w(w) {}

    VectorType x,y,z,w;
};
#else
typedef glm::vec2 vec2;
typedef glm::vec3 vec3;
typedef glm::vec4 vec4;

#endif
}