#pragma once
#include "vector.h"

struct Mat4
{
    float m_Values[4][4];
};

struct Mat3
{
    float m_Values[3][3];
};

// NOTE(sbalse): A 4x4 identity matrix.
constexpr Mat4 MAT4_IDENTITY =
{
    .m_Values =
    {
        { 1, 0, 0, 0 },
        { 0, 1, 0, 0 },
        { 0, 0, 1, 0 },
        { 0, 0, 0, 1 },
    }
};

// NOTE(sbalse): A 3x3 identity matrix.
constexpr Mat3 MAT3_IDENTITY =
{
    .m_Values =
    {
        { 1, 0, 0 },
        { 0, 1, 0 },
        { 0, 0, 1 },
    }
};

// TODO(sbalse): Use operator overloading for matrix math.

Vec4 Mat4MulVec4(const Mat4 m, const Vec4 v);
Mat4 Mat4MulMat4(const Mat4 m1, const Mat4 m2);
Mat4 Mat4MakeScale(const float sx, const float sy, const float sz);
Mat4 Mat4MakeTranslation(const float tx, const float ty, const float tz);
Mat4 Mat4MakeRotationX(const float angle);
Mat4 Mat4MakeRotationY(const float angle);
Mat4 Mat4MakeRotationZ(const float angle);

Mat3 Mat3MakeTranslation(
    const float x,
    const float y,
    const float z,
    const float tx,
    const float ty,
    const float tz);

Vec3 Mat3MulVec3(const Mat3 m, const Vec3 v);
