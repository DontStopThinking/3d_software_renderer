#include "vector.h"

#include <cmath>

/******** NOTE(sbalse): Implementation of vector 2 functions. ***********/
float Vec2Length(const Vec2 value)
{
    return std::sqrtf(value.m_X * value.m_X + value.m_Y * value.m_Y);
}

Vec2 Vec2Add(const Vec2 v1, const Vec2 v2)
{
    const Vec2 result =
    {
        .m_X = v1.m_X + v2.m_X,
        .m_Y = v1.m_Y + v2.m_Y,
    };
    return result;
}

Vec2 Vec2Sub(const Vec2 v1, const Vec2 v2)
{
    const Vec2 result =
    {
        .m_X = v1.m_X - v2.m_X,
        .m_Y = v1.m_Y - v2.m_Y,
    };
    return result;
}

Vec2 Vec2Mul(const Vec2 v, const float factor)
{
    const Vec2 result =
    {
        .m_X = v.m_X * factor,
        .m_Y = v.m_Y * factor,
    };
    return result;
}

Vec2 Vec2Div(const Vec2 v, const float factor)
{
    const Vec2 result =
    {
        .m_X = v.m_X / factor,
        .m_Y = v.m_Y / factor,
    };
    return result;
}

float Vec2Dot(const Vec2 v1, const Vec2 v2)
{
    return (v1.m_X * v2.m_X) + (v1.m_Y * v2.m_Y);
}

void Vec2Normalize(Vec2* const v)
{
    const float magnitude = Vec2Length(*v);
    *v = Vec2Div(*v, magnitude);
}

/******** NOTE(sbalse): Implementation of vector 3 functions. ***********/
float Vec3Length(const Vec3 value)
{
    return std::sqrtf(value.m_X * value.m_X + value.m_Y * value.m_Y + value.m_Z * value.m_Z);
}

Vec3 Vec3Add(const Vec3 v1, const Vec3 v2)
{
    const Vec3 result =
    {
        .m_X = v1.m_X + v2.m_X,
        .m_Y = v1.m_Y + v2.m_Y,
        .m_Z = v1.m_Z + v2.m_Z,
    };
    return result;
}

Vec3 Vec3Sub(const Vec3 v1, const Vec3 v2)
{
    const Vec3 result =
    {
        .m_X = v1.m_X - v2.m_X,
        .m_Y = v1.m_Y - v2.m_Y,
        .m_Z = v1.m_Z - v2.m_Z,
    };
    return result;
}

Vec3 Vec3Mul(const Vec3 v, const float factor)
{
    const Vec3 result =
    {
        .m_X = v.m_X * factor,
        .m_Y = v.m_Y * factor,
        .m_Z = v.m_Z * factor,
    };
    return result;
}

Vec3 Vec3Div(const Vec3 v, const float factor)
{
    const Vec3 result =
    {
        .m_X = v.m_X / factor,
        .m_Y = v.m_Y / factor,
        .m_Z = v.m_Z / factor,
    };
    return result;
}

Vec3 Vec3Cross(const Vec3 v1, const Vec3 v2)
{
    const Vec3 result =
    {
        .m_X = v1.m_Y * v2.m_Z - v1.m_Z * v2.m_Y,
        .m_Y = v1.m_Z * v2.m_X - v1.m_X * v2.m_Z,
        .m_Z = v1.m_X * v2.m_Y - v1.m_Y * v2.m_X,
    };
    return result;
}

float Vec3Dot(const Vec3 v1, const Vec3 v2)
{
    return (v1.m_X * v2.m_X) + (v1.m_Y * v2.m_Y) + (v1.m_Z * v2.m_Z);
}

void Vec3Normalize(Vec3* const v)
{
    const float magnitude = Vec3Length(*v);
    *v = Vec3Div(*v, magnitude);
}

Vec3 Vec3RotateX(const Vec3 value, const float angle)
{
    const Vec3 result =
    {
        .m_X = value.m_X,
        .m_Y = value.m_Y * std::cosf(angle) - value.m_Z * std::sinf(angle),
        .m_Z = value.m_Y * std::sinf(angle) + value.m_Z * std::cosf(angle)
    };

    return result;
}

Vec3 Vec3RotateY(const Vec3 value, const float angle)
{
    const Vec3 result =
    {
        .m_X = value.m_X * std::cosf(angle) - value.m_Z * std::sinf(angle),
        .m_Y = value.m_Y,
        .m_Z = value.m_X * std::sinf(angle) + value.m_Z * std::cosf(angle)
    };

    return result;
}

Vec3 Vec3RotateZ(const Vec3 value, const float angle)
{
    const Vec3 result =
    {
        .m_X = value.m_X * std::cosf(angle) - value.m_Y * std::sinf(angle),
        .m_Y = value.m_X * std::sinf(angle) + value.m_Y * std::cosf(angle),
        .m_Z = value.m_Z
    };

    return result;
}

/******** NOTE(sbalse): Implementation of vector conversion functions. ***********/
Vec4 Vec4FromVec3(const Vec3 v)
{
    const Vec4 result =
    {
        .m_X = v.m_X,
        .m_Y = v.m_Y,
        .m_Z = v.m_Z,
        .m_W = 1.0f,
    };

    return result;
}

Vec3 Vec3FromVec4(const Vec4 v)
{
    const Vec3 result =
    {
        .m_X = v.m_X,
        .m_Y = v.m_Y,
        .m_Z = v.m_Z,
    };

    return result;
}
