#include "clipping.h"

#include <cmath>

#include "common.h"
#include "vector.h"

constexpr u32 NUM_PLANES = FrustumPlane_Count;
static constinit Plane fs_FrustumPlanes[NUM_PLANES] = {};

static void ClipPolygonAgainstPlane(Polygon* const polygon, const FrustumPlane plane)
{
    if (polygon->m_NumVertices <= 0)
    {
        return;
    }

    const Vec3 planePoint = fs_FrustumPlanes[plane].m_Point;
    const Vec3 planeNormal = fs_FrustumPlanes[plane].m_Normal;

    // NOTE(sbalse): Declare a static array of inside vertices and texture coordinates of those vertices that
    // will be our final destination polygon (returned in-place using the `polygon` param).
    Vec3 insideVertices[MAX_NUM_POLYGON_VERTICES] = {};
    Tex2 insideTexCoords[MAX_NUM_POLYGON_VERTICES] = {};
    int numInsideVertices = 0;

    // NOTE(sbalse): Start the current vertex with the first polygon vertex and the previous with the last
    // polygon vertex.
    const Vec3* currentVertex = &polygon->m_Vertices[0];
    const Vec3* previousVertex = &polygon->m_Vertices[polygon->m_NumVertices - 1];

    // NOTE(sbalse): Start the current vertex texture coordinate with the first polygon vertex's texture
    // coordinate and the previous with the last polygon vertex.
    const Tex2* currentTexCoord = &polygon->m_TexCoords[0];
    const Tex2* previousTexCoord = &polygon->m_TexCoords[polygon->m_NumVertices - 1];

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
            // this by using linear interpolation formula: I = Q1 + t(Q2 - Q1).
            const float t = previousDP / (previousDP - currentDP);

            // NOTE(sbalse): Calculate the intersection vertex using linear interpolation.
            const Vec3 intersectionVertex =
            {
                .m_X = std::lerp(previousVertex->m_X, currentVertex->m_X, t),
                .m_Y = std::lerp(previousVertex->m_Y, currentVertex->m_Y, t),
                .m_Z = std::lerp(previousVertex->m_Z, currentVertex->m_Z, t),
            };

            // NOTE(sbalse): Similar to the vertex, also do linear interpolation of the texture coordinate.
            const Tex2 interpolatedTexCoord =
            {
                .m_U = std::lerp(previousTexCoord->m_U, currentTexCoord->m_U, t),
                .m_V = std::lerp(previousTexCoord->m_V, currentTexCoord->m_V, t),
            };

            // NOTE(sbalse): Insert the intersection Vertex and Texture coordinate to the list of
            // "inside vertices".
            insideVertices[numInsideVertices] = intersectionVertex;
            insideTexCoords[numInsideVertices] = interpolatedTexCoord;
            numInsideVertices++;
        }

        // NOTE(sbalse): If the current dot product is positive meaning that the current vertex is *inside*
        // the plane.
        if (currentDP > 0)
        {
            // NOTE(sbalse): Insert the current vertex to the list of "inside vertices".
            insideVertices[numInsideVertices] = *currentVertex;
            insideTexCoords[numInsideVertices] = *currentTexCoord;
            numInsideVertices++;
        }

        previousDP = currentDP;
        previousVertex = currentVertex;
        previousTexCoord = currentTexCoord;
        currentVertex++;
        currentTexCoord++;
    }

    // NOTE(sbalse): Copy the list of inside vertices into the polygon parameter.
    for (int i = 0; i < numInsideVertices; i++)
    {
        polygon->m_Vertices[i] = insideVertices[i];
        polygon->m_TexCoords[i] = insideTexCoords[i];
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
void InitFrustumPlanes(const float fovX, const float fovY, const float znear, const float zfar)
{
    const float cosHalfFovX = std::cosf(fovX / 2.0f);
    const float sinHalfFovX = std::sinf(fovX / 2.0f);
    const float cosHalfFovY = std::cosf(fovY / 2.0f);
    const float sinHalfFovY = std::sinf(fovY / 2.0f);

    // NOTE(sbalse): Use X FOV for the left and right planes.
    fs_FrustumPlanes[FrustumPlane_Left] =
    {
        .m_Point = ORIGIN,
        .m_Normal = { .m_X = cosHalfFovX, .m_Y = 0, .m_Z = sinHalfFovX },
    };
    fs_FrustumPlanes[FrustumPlane_Right] =
    {
        .m_Point = ORIGIN,
        .m_Normal = { .m_X = -cosHalfFovX, .m_Y = 0, .m_Z = sinHalfFovX },
    };
    // NOTE(sbalse): Use Y FOV for the top and bottom planes.
    fs_FrustumPlanes[FrustumPlane_Top] =
    {
        .m_Point = ORIGIN,
        .m_Normal = { .m_X = 0, .m_Y = -cosHalfFovY, .m_Z = sinHalfFovY },
    };
    fs_FrustumPlanes[FrustumPlane_Bottom] =
    {
        .m_Point = ORIGIN,
        .m_Normal = { .m_X = 0, .m_Y = cosHalfFovY, .m_Z = sinHalfFovY },
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

Polygon CreatePolygonFromTriangle(
    const Vec3 v0,
    const Vec3 v1,
    const Vec3 v2,
    const Tex2 t0,
    const Tex2 t1,
    const Tex2 t2
)
{
    const Polygon result =
    {
        .m_Vertices = { v0, v1, v2 },
        .m_NumVertices = 3, // NOTE(sbalse): Triangle.
        .m_TexCoords = { t0, t1, t2 },
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

        triangles[i].m_TexCoords[0] = polygon->m_TexCoords[index0];
        triangles[i].m_TexCoords[1] = polygon->m_TexCoords[index1];
        triangles[i].m_TexCoords[2] = polygon->m_TexCoords[index2];
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
