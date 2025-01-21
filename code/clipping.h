#pragma once
#include "common.h"
#include "vector.h"
#include "triangle.h"

enum FrustumPlane
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

inline constexpr u32 MAX_NUM_POLYGON_VERTICES = 10;
inline constexpr u32 MAX_NUM_POLYGON_TRIANGLES = 10; // NOTE(sbalse): Max triangles a clipped polygon contains.

struct Polygon
{
    Vec3 m_Vertices[MAX_NUM_POLYGON_VERTICES];
    u32 m_NumVertices;
};

// NOTE(sbalse): Initialize the frustum planes that are used for clipping.
// fov = fov of view that we're using.
// znear = value of the Z-Near plane.
// zfar = value of the Z-Far plane.
void InitFrustumPlanes(const float fov, const float znear, const float zfar);
Polygon CreatePolygonFromTriangle(const Vec3 v0, const Vec3 v1, const Vec3 v2);
void TrianglesFromPolygon(
    const Polygon* const polygon,
    Triangle triangles[],
    int* numTriangles);
void ClipPolygon(Polygon* const polygon);
