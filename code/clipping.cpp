#include "clipping.h"

#include <cmath>

#include "common.h"

constexpr u32 NUM_PLANES = FrustumPlane_Count;
static constinit Plane fs_FrustumPlanes[NUM_PLANES] = {};

/*
Frustum planes are defines by a point and a normal vector.

Near plane   : P=(0, 0, znear), N = (0, 0, 1)
Far plane    : P=(0, 0, zfar), N = (0, 0, -1)
Top plane    : P=(0, 0, 0), N=(0, -cos(fov/2), sin(fov/2))
Bottom plane : P=(0, 0, 0), N=(0, cos(fov/2), sin(fov/2))
Left plane   : P=(0, 0, 0), N=(cos(fov/2), 0, sin(fov/2))
Right plane   : P=(0, 0, 0), N=(-cos(fov/2), 0, sin(fov/2))

           /|\
         /  | |
       /\   | |
     /      | |
  P*|-->  <-|*|   ----> +z-axis
     \      | |
       \/   | |
         \  | |
           \|/
*/
void InitFrustumPlanes(const float fov, const float znear, const float zfar)
{
    const float cosHalfFov = std::cosf(fov / 2.0f);
    const float sinHalfFov = std::sinf(fov / 2.0f);

    fs_FrustumPlanes[FrustumPlane_Left] =
    {
        .m_Point = ORIGIN,
        .m_Normal = { .m_X = cosHalfFov, .m_Y = 0, .m_Z = sinHalfFov },
    };
    fs_FrustumPlanes[FrustumPlane_Right] =
    {
        .m_Point = ORIGIN,
        .m_Normal = { .m_X = -cosHalfFov, .m_Y = 0, .m_Z = sinHalfFov },
    };
    fs_FrustumPlanes[FrustumPlane_Top] =
    {
        .m_Point = ORIGIN,
        .m_Normal = { .m_X = 0, .m_Y = -cosHalfFov, .m_Z = sinHalfFov },
    };
    fs_FrustumPlanes[FrustumPlane_Bottom] =
    {
        .m_Point = ORIGIN,
        .m_Normal = { .m_X = 0, .m_Y = cosHalfFov, .m_Z = sinHalfFov },
    };
    fs_FrustumPlanes[FrustumPlane_Near] =
    {
        .m_Point = { .m_X = 0, .m_Y = 0, .m_Z = znear },
        .m_Normal = { .m_X = 0, .m_Y = 0, .m_Z = 1 },
    };
    fs_FrustumPlanes[FrustumPlane_Far] =
    {
        .m_Point = { .m_X = 0, .m_Y = 0, .m_Z = zfar },
        .m_Normal = { .m_X = 0, .m_Y = 0, .m_Z = -1 },
    };
}
