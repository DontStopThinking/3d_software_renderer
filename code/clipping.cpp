#include "clipping.h"

#include <cmath>

#include "common.h"
#include "vector.h"

constexpr u32 NUM_PLANES = FrustumPlane_Count;
static constinit Plane fs_FrustumPlanes[NUM_PLANES] = {};

static void ClipPolygonAgainstPlane(Polygon* const polygon, const FrustumPlane plane)
{
    const Vec3 planePoint = fs_FrustumPlanes[plane].m_Point;
    const Vec3 planeNormal = fs_FrustumPlanes[plane].m_Normal;

    // NOTE(sbalse): Declare a static array of inside vertices that will be our final destination polygon
    // (returned in-place in the `polygon` param).
    Vec3 insideVertices[MAX_NUM_POLYGON_VERTICES] = {};
    int numInsideVertices = 0;

    // NOTE(sbalse): Start the current vertex with the first polygon vertex and the previous with the last
    // polygon vertex.
    const Vec3* currentVertex = &polygon->m_Vertices[0];
    const Vec3* previousVertex = &polygon->m_Vertices[polygon->m_NumVertices - 1];

    // NOTE(sbalse): DP = dot product.
    // This is the dot product of the plane normal with the vector between the plane point and the current
    // vertex.
    float currentDP = 0.0f;
    float previousDP = Vec3Dot(Vec3Sub(*previousVertex, planePoint), planeNormal);

    // NOTE(sbalse): Loop all the polygon vertices while the current is different than the last one.
    while (currentVertex != &polygon->m_Vertices[polygon->m_NumVertices])
    {
        currentDP = Vec3Dot(Vec3Sub(*currentVertex, planePoint), planeNormal);

        // NOTE(sbalse): If we changed from inside to outside, or vice versa (multiplication of both numbers
        // will be negative only if they are different signs).
        if (currentDP * previousDP < 0)
        {
            // NOTE(sbalse): We need to calculate the intersection point of the line with the plane. We do
            // that using the interpolation formula: I = Q1 + t(Q2 - Q1).
            const float t = previousDP / (previousDP - currentDP);

            // NOTE(sbalse): Calculate the intersection point using linear interpolation formula.
            Vec3 intersectionPoint = Vec3Sub(*currentVertex, *previousVertex);
            intersectionPoint = Vec3Mul(intersectionPoint, t);
            intersectionPoint = Vec3Add(intersectionPoint, *previousVertex);

            // NOTE(sbalse): Insert the intersection point to the list of "inside vertices".
            insideVertices[numInsideVertices] = intersectionPoint;
            numInsideVertices++;
        }

        // NOTE(sbalse): If the DP is positive then that means the current vertex is *inside* the plane.
        if (currentDP > 0)
        {
            // NOTE(sbalse): Insert the current vertex to the list of "inside vertices".
            insideVertices[numInsideVertices] = *currentVertex;
            numInsideVertices++;
        }

        previousDP = currentDP;
        previousVertex = currentVertex;
        currentVertex++;
    }

    // NOTE(sbalse): Copy the list of inside vertices into the polygon parameter.
    for (int i = 0; i < numInsideVertices; i++)
    {
        polygon->m_Vertices[i] = insideVertices[i];
    }
    polygon->m_NumVertices = numInsideVertices;
}

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

Polygon CreatePolygonFromTriangle(const Vec3 v0, const Vec3 v1, const Vec3 v2)
{
    const Polygon result =
    {
        .m_Vertices = { v0, v1, v2 },
        .m_NumVertices = 3, // NOTE(sbalse): Triangle.
    };

    return result;
}

void TrianglesFromPolygon(
    const Polygon* const polygon,
    Triangle triangles[],
    int* numTriangles)
{
    *numTriangles = polygon->m_NumVertices - 2;
    for (int i = 0; i < *numTriangles; i++)
    {
        // NOTE(sbalse): Create three vertices of the destination triangle.
        const int index0 = 0;
        const int index1 = i + 1;
        const int index2 = i + 2;

        triangles[i].m_Points[0] = Vec4FromVec3(polygon->m_Vertices[index0]);
        triangles[i].m_Points[1] = Vec4FromVec3(polygon->m_Vertices[index1]);
        triangles[i].m_Points[2] = Vec4FromVec3(polygon->m_Vertices[index2]);
    }
}

void ClipPolygon(Polygon* const polygon)
{
    ClipPolygonAgainstPlane(polygon, FrustumPlane_Left);
    ClipPolygonAgainstPlane(polygon, FrustumPlane_Right);
    ClipPolygonAgainstPlane(polygon, FrustumPlane_Top);
    ClipPolygonAgainstPlane(polygon, FrustumPlane_Bottom);
    ClipPolygonAgainstPlane(polygon, FrustumPlane_Near);
    ClipPolygonAgainstPlane(polygon, FrustumPlane_Far);
}
