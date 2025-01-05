#pragma once

struct Vec2
{
    float m_X;
    float m_Y;
};

struct Vec3
{
    float m_X;
    float m_Y;
    float m_Z;
};

// TODO(sbalse): Since I'm using C++, could use operator overloading for vector operations?

/******** NOTE(sbalse): Vector 2D functions. ***********/
float Vec2Length(const Vec2 value);
Vec2 Vec2Add(const Vec2 v1, const Vec2 v2);
Vec2 Vec2Sub(const Vec2 v1, const Vec2 v2);
Vec2 Vec2Mul(const Vec2 v, const float factor);
Vec2 Vec2Div(const Vec2 v, const float factor);
float Vec2Dot(const Vec2 v1, const Vec2 v2);

/******** NOTE(sbalse): Vector 3D functions. ***********/
float Vec3Length(const Vec3 value);
Vec3 Vec3Add(const Vec3 v1, const Vec3 v2);
Vec3 Vec3Sub(const Vec3 v1, const Vec3 v2);
Vec3 Vec3Mul(const Vec3 v, const float factor);
Vec3 Vec3Div(const Vec3 v, const float factor);
Vec3 Vec3Cross(const Vec3 v1, const Vec3 v2);
float Vec3Dot(const Vec3 v1, const Vec3 v2);

Vec3 Vec3RotateX(const Vec3 value, const float angle);
Vec3 Vec3RotateY(const Vec3 value, const float angle);
Vec3 Vec3RotateZ(const Vec3 value, const float angle);
