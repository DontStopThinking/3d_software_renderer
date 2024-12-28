#include "vector.h"

#include <cmath>

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
