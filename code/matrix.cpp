#include "matrix.h"

#include <cmath>

#include "log.h"

Vec4 Mat4MulVec4(const Mat4 m, const Vec4 v)
{
    Vec4 result = {};

    result.m_X =
        m.m_Values[0][0] * v.m_X
        + m.m_Values[0][1] * v.m_Y
        + m.m_Values[0][2] * v.m_Z
        + m.m_Values[0][3] * v.m_W;
    result.m_Y =
        m.m_Values[1][0] * v.m_X
        + m.m_Values[1][1] * v.m_Y
        + m.m_Values[1][2] * v.m_Z
        + m.m_Values[1][3] * v.m_W;
    result.m_Z =
        m.m_Values[2][0] * v.m_X
        + m.m_Values[2][1] * v.m_Y
        + m.m_Values[2][2] * v.m_Z
        + m.m_Values[2][3] * v.m_W;
    result.m_W =
        m.m_Values[3][0] * v.m_X
        + m.m_Values[3][1] * v.m_Y
        + m.m_Values[3][2] * v.m_Z
        + m.m_Values[3][3] * v.m_W;

    return result;
}

Mat4 Mat4MulMat4(const Mat4 m1, const Mat4 m2)
{
    Mat4 result = {};
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            result.m_Values[i][j] =
                m1.m_Values[i][0] * m2.m_Values[0][j]
                + m1.m_Values[i][1] * m2.m_Values[1][j]
                + m1.m_Values[i][2] * m2.m_Values[2][j]
                + m1.m_Values[i][3] * m2.m_Values[3][j];
        }
    }
    return result;
}

Mat4 Mat4MakeScale(const float sx, const float sy, const float sz)
{
    /*
        | sx 0  0  0 |
        | 0  sy 0  0 |
        | 0  0  sz 0 |
        | 0  0  0  1 |
    */
    Mat4 result = MAT4_IDENTITY;
    result.m_Values[0][0] = sx;
    result.m_Values[1][1] = sy;
    result.m_Values[2][2] = sz;
    return result;
}

Mat4 Mat4MakeTranslation(const float tx, const float ty, const float tz)
{
    /*
        | 1 0 0 tx |
        | 0 1 0 ty |
        | 0 0 1 tz |
        | 0 0 0  1 |
    */
    Mat4 result = MAT4_IDENTITY;
    result.m_Values[0][3] = tx;
    result.m_Values[1][3] = ty;
    result.m_Values[2][3] = tz;
    return result;
}

Mat4 Mat4MakeRotationX(const float angle)
{
    const float c = std::cosf(angle);
    const float s = std::sinf(angle);
    /*
        | 1  0  0 0 |
        | 0  c -s 0 |
        | 0  s  c 0 |
        | 0  0  0 1 |
    */
    Mat4 result = MAT4_IDENTITY;
    result.m_Values[1][1] = c;
    result.m_Values[1][2] = -s;
    result.m_Values[2][1] = s;
    result.m_Values[2][2] = c;
    return result;
}

Mat4 Mat4MakeRotationY(const float angle)
{
    const float c = std::cosf(angle);
    const float s = std::sinf(angle);
    /*
        |  c  0  s 0 |
        |  0  1  0 0 |
        | -s  0  c 0 |
        |  0  0  0 1 |
    */
    Mat4 result = MAT4_IDENTITY;
    result.m_Values[0][0] = c;
    result.m_Values[0][2] = s;
    result.m_Values[2][0] = -s;
    result.m_Values[2][2] = c;
    return result;
}

Mat4 Mat4MakeRotationZ(const float angle)
{
    const float c = std::cosf(angle);
    const float s = std::sinf(angle);
    /*
        | c -s  0 0 |
        | s  c  0 0 |
        | 0  0  1 0 |
        | 0  0  0 1 |
    */
    Mat4 result = MAT4_IDENTITY;
    result.m_Values[0][0] = c;
    result.m_Values[0][1] = -s;
    result.m_Values[1][0] = s;
    result.m_Values[1][1] = c;
    return result;
}

Mat3 Mat3MakeTranslation(
    const float x,
    const float y,
    const float z,
    const float tx,
    const float ty,
    const float tz)
{
    /*
        | [(tx / x) + 1]     0                  0 |
        |    0            [(ty / y) + 1]        0  |
        |    0               0            [(tz / z) + 1] |
    */
    if (x == 0 || y == 0 || z == 0)
    {
        LOG_INFO("Zero! (x: %.2f, y: %.2f, z: %.2f)", x, y, z);
    }

    Mat3 result = MAT3_IDENTITY;
    result.m_Values[0][0] = (tx / x) + 1;
    result.m_Values[1][1] = (ty / y) + 1;
    result.m_Values[2][2] = (tz / z) + 1;
    return result;
}

Vec3 Mat3MulVec3(const Mat3 m, const Vec3 v)
{
    Vec3 result = {};
    result.m_X = m.m_Values[0][0] * v.m_X + m.m_Values[0][1] * v.m_Y + m.m_Values[0][2] * v.m_Z;
    result.m_Y = m.m_Values[1][0] * v.m_X + m.m_Values[1][1] * v.m_Y + m.m_Values[1][2] * v.m_Z;
    result.m_Z = m.m_Values[2][0] * v.m_X + m.m_Values[2][1] * v.m_Y + m.m_Values[2][2] * v.m_Z;
    return result;
}
