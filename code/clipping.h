#pragma once
#include "vector.h"

enum FrustPlane
{
    FrustumPlane_Left,
    FrustumPlane_Right,
    FrustumPlane_Top,
    FrustumPlane_Bottom,
    FrustumPlane_Near,
    FrustumPlane_Far,
    FrustumPlane_Count,
};

struct Plane
{
    Vec3 m_Point; // NOTE(sbalse): A point in the plane.
    Vec3 m_Normal; // NOTE(sbalse): The normal of the point.
};

// NOTE(sbalse): Initialize the frustum planes that are used for clipping.
// fov = fov of view that we're using.
// znear = value of the Z-Near plane.
// zfar = value of the Z-Far plane.
void InitFrustumPlanes(const float fov, const float znear, const float zfar);
