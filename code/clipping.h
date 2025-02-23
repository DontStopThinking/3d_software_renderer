#pragma once
#include "common.h"
#include "vector.h"
#include "triangle.h"
#include "texture.h"

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
inline constexpr u32 MAX_NUM_POLYGON_TRIANGLES = 10; // NOTE(sbalse): Max triangles that a clipped polygon contains.

struct Polygon
{
    Vec3 m_Vertices[MAX_NUM_POLYGON_VERTICES]; // NOTE(sbalse): The vertices of the polygon.
    u32 m_NumVertices; // NOTE(sbalse): No. of vertices (less than or equal to MAX_NUM_POLYGON_VERTICES).
    Tex2 m_TexCoords[MAX_NUM_POLYGON_VERTICES]; // NOTE(sbalse): The polygon texture UV coordinates.
};

// NOTE(sbalse): Initialize the frustum planes that are used for clipping.
// fov = fov of view that we're using.
// znear = value of the Z-Near plane.
// zfar = value of the Z-Far plane.
void InitFrustumPlanes(const float fovX, const float fovY, const float znear, const float zfar);
Polygon CreatePolygonFromTriangle(
    const Vec3 v0,
    const Vec3 v1,
    const Vec3 v2,
    const Tex2 t0,
    const Tex2 t1,
    const Tex2 t2
);
void TrianglesFromPolygon(
    const Polygon* const polygon,
    Triangle triangles[],
    int* numTriangles
);
void ClipPolygon(Polygon* const polygon);
