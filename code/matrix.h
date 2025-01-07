#pragma once
#include "vector.h"

struct Mat4
{
    float m_Values[4][4];
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

Mat4 Mat4MakeScale(const float sx, const float sy, const float sz);
Vec4 Mat4MulVec4(const Mat4 m, const Vec4 v);
