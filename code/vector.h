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

Vec3 Vec3RotateX(const Vec3 value, const float angle);
Vec3 Vec3RotateY(const Vec3 value, const float angle);
Vec3 Vec3RotateZ(const Vec3 value, const float angle);
