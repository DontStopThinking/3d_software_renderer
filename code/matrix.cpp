#include "matrix.h"

#include <cmath>

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

Mat4 Mat4MakePerspective(const float fov, const float aspect, const float znear, const float zfar)
{
    /*
        | (h/w)*1/tan(fov/2)             0           0                 0 |
        |                  0  1/tan(fov/2)           0                 0 |
        |                  0             0  zf/(zf-zn) (-zf*zn)/(zf-zn)) |
        |                  0             0           1                 0 |
    */
    Mat4 result = {};
    result.m_Values[0][0] = aspect * (1 / std::tanf(fov / 2));
    result.m_Values[1][1] = 1 / std::tanf(fov / 2);
    result.m_Values[2][2] = zfar / (zfar - znear);
    result.m_Values[3][3] = (-zfar * znear) / (zfar - znear);
    result.m_Values[3][2] = 1.0f;
    return result;
}

Vec4 Mat4MulVec4Project(const Mat4 matProj, const Vec4 v)
{
    // NOTE(sbalse): Multiply the projection matrix by our original vector.
    Vec4 result = Mat4MulVec4(matProj, v);

    // NOTE(sbalse): Perform perspective divide with original z-value that is now stored in w.
    if (result.m_W != 0.0f)
    {
        result.m_X /= result.m_W;
        result.m_Y /= result.m_W;
        result.m_Z /= result.m_W;
    }

    return result;
}

Mat4 Mat4LookAt(const Vec3 eye, const Vec3 target, const Vec3 up)
{
    // NOTE(sbalse): Compute the forward (z), right (x) and up (y) vectors.
    Vec3 z = Vec3Sub(target, eye);
    Vec3Normalize(&z);
    Vec3 x = Vec3Cross(up, z);
    Vec3Normalize(&x);
    Vec3 y = Vec3Cross(z, x);

    // | x.x  x.y  x.z  -dot(x, eye) |
    // | y.x  y.y  y.z  -dot(y, eye) |
    // | z.x  z.y  z.z  -dot(z, eye) |
    // |   0    0    0             1 |
    const Mat4 result = // NOTE(sbalse): The result view matrix.
    {
        .m_Values =
        {
            { x.m_X, x.m_Y, x.m_Z, -Vec3Dot(x, eye) },
            { y.m_X, y.m_Y, y.m_Z, -Vec3Dot(y, eye) },
            { z.m_X, z.m_Y, z.m_Z, -Vec3Dot(z, eye) },
            { 0, 0, 0, 1 },
        },
    };

    return result;
}
