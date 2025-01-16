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

// TODO(sbalse): Use operator overloading for matrix math.

Vec4 Mat4MulVec4(const Mat4 m, const Vec4 v);
Mat4 Mat4MulMat4(const Mat4 m1, const Mat4 m2);
Mat4 Mat4MakeScale(const float sx, const float sy, const float sz);
Mat4 Mat4MakeTranslation(const float tx, const float ty, const float tz);
Mat4 Mat4MakeRotationX(const float angle);
Mat4 Mat4MakeRotationY(const float angle);
Mat4 Mat4MakeRotationZ(const float angle);
Mat4 Mat4MakePerspective(const float fov, const float aspect, const float znear, const float zfar);
// NOTE(sbalse): Multiply the projection matrix `matProj` with the Vector `v` and also
// perform perspective divide on the result.
Vec4 Mat4MulVec4Project(const Mat4 matProj, const Vec4 v);
// TODO(sbalse): Orthographic projection.

// NOTE(sbalse):
// eye = Camera position.
// target = What should the camera look at.
// up = What is "up" for the camera.
Mat4 Mat4LookAt(const Vec3 eye, const Vec3 target, const Vec3 up);
