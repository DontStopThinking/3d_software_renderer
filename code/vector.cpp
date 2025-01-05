#include "vector.h"

#include <cmath>

/******** NOTE(sbalse): Implementation of vector 2D functions. ***********/
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

/******** NOTE(sbalse): Implementation of vector 3D functions. ***********/
float Vec3Length(const Vec3 value)
{
    return std::sqrtf(value.m_X * value.m_X + value.m_Y * value.m_Y + value.m_Z + value.m_Z);
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
