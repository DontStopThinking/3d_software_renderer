#include "triangle.h"

#include "log.h"
#include "display.h"

#define SWAP(Type, v1, v2) \
do \
{ \
    Type temp = v2; \
    v2 = v1; \
    v1 = temp; \
} while (0)

/*
NOTE(sbalse): Draw a filled triangle with a flat bottom:
         (x0, y0)
           / \
          /   \
         /     \
        /       \
       /         \
(x1, y1)--------(x2, y2)
*/

static void FillFlatBottomTriangle(
    const int x0,
    const int y0,
    const int x1,
    const int y1,
    const int x2,
    const int y2,
    const u32 color
)
{
    // NOTE(sbalse): Find the slopes of the two triangle legs.

    /* NOTE(sbalse) : Normally, slope is calculated with d(y) / d(x) which gives the how much y
     * increases with an increase in x, However, we are filling out triangle horizontally (scanline
     * -by-scanline), so here we calculate with inverse of that: d(x) / d(y) which gives us how much
     * much x should change per change in y. */
    const float inverseSlope1 = (x1 - x0) / static_cast<float>(y1 - y0);
    const float inverseSlope2 = (x2 - x0) / static_cast<float>(y2 - y0);

    float xStart = static_cast<float>(x0);
    float xEnd = static_cast<float>(x0);

    // Loop all scan lines from top to bottom.
    for (int y = y0; y <= y2; y++)
    {
        DrawLine(static_cast<int>(xStart), y, static_cast<int>(xEnd), y, color);
        xStart += inverseSlope1;
        xEnd += inverseSlope2;
    }
}

/*
NOTE(sbalse): Draw a filled triangle with a flat top:
   (x0, y0)------(x1, y1)
       \          \
         \         \
           \       \
             \      \
               \     \
                 \    \
                   \   \
                     \ \
                       \
                      (x2, y2)
*/
static void FillFlatTopTriangle(
    const int x0,
    const int y0,
    const int x1,
    const int y1,
    const int x2,
    const int y2,
    const u32 color
)
{
    // NOTE(sbalse): Find the slopes of the two triangle legs.
    const float inverseSlope1 = (x2 - x0) / static_cast<float>(y2 - y0);
    const float inverseSlope2 = (x2 - x1) / static_cast<float>(y2 - y1);

    float xStart = static_cast<float>(x2);
    float xEnd = static_cast<float>(x2);

    // Loop all scan lines from bottom to top.
    for (int y = y2; y >= y0; y--)
    {
        DrawLine(static_cast<int>(xStart), y, static_cast<int>(xEnd), y, color);
        xStart -= inverseSlope1;
        xEnd -= inverseSlope2;
    }
}

/*
NOTE(sbalse): Return the barycentric weights alpha, beta and gamma for point P inside triangle ABC.

         B
        /|\
       / |\
      /  | \
     /  (p) \
    / /    \ \
   A----------C
*/
static Vec3 BarycentricWeights(const Vec2 a, const Vec2 b, const Vec2 c, const Vec2 p)
{
    // NOTE(sbalse): Find the vectors between the vertices ABC and point P.
    const Vec2 ac = Vec2Sub(c, a);
    const Vec2 ab = Vec2Sub(b, a);
    const Vec2 ap = Vec2Sub(p, a);
    const Vec2 pc = Vec2Sub(c, p);
    const Vec2 pb = Vec2Sub(b, p);

    // NOTE(sbalse): Compute the area of the full parallelogram formed by vectors AC and AB using 2D
    // cross product.
    const float areaParallelogramABC = (ac.m_X * ab.m_Y - ac.m_Y * ab.m_X); // || AC x AB ||

    // NOTE(sbalse): Alpha is the area of the small parallelogram PBC divided by the area of the full
    // parallelogram ABC.
    const float alpha = (pc.m_X * pb.m_Y - pc.m_Y * pb.m_X) / areaParallelogramABC;

    // NOTE(sbalse): Beta is the area of the small parallelogram APC divided by the area of the full
    // parallelogram ABC.
    const float beta = (ac.m_X * ap.m_Y - ac.m_Y * ap.m_X) / areaParallelogramABC;

    // NOTE(sbalse): Weight gamma is easily found since barycentric coordinates always add up to 1.
    const float gamma = 1 - alpha - beta;

    const Vec3 result = { alpha, beta, gamma };
    return result;
}

void DrawTriangle(
    const int x0,
    const int y0,
    const int x1,
    const int y1,
    const int x2,
    const int y2,
    const u32 color)
{
    DrawLine(x0, y0, x1, y1, color);
    DrawLine(x1, y1, x2, y2, color);
    DrawLine(x2, y2, x0, y0, color);
}


/*
NOTE(sbalse): Draw a filled triangle with the flat-top/flat-bottom method.
          (x0, y0)
           / \
          /   \
         /     \
        /       \
       /         \
(x1, y1)--------(Mx, My)
       \          \
         \        \
           \       \
             \      \
               \     \
                 \    \
                   \  \
                     \ \
                       \
                      (x2, y2)
*/
void DrawFilledTriangle(
    int x0,
    int y0,
    int x1,
    int y1,
    int x2,
    int y2,
    const u32 color
)
{
    // NOTE(sbalse): Sort vertices by y-coordinate ascending (y0 < y1 < y2);
    if (y0 > y1)
    {
        SWAP(int, x0, x1);
        SWAP(int, y0, y1);
    }
    if (y1 > y2)
    {
        SWAP(int, x1, x2);
        SWAP(int, y1, y2);
    }
    if (y0 > y1)
    {
        SWAP(int, x0, x1);
        SWAP(int, y0, y1);
    }

    if (y1 == y2)
    {
        // NOTE(sbalse): If y1 == y2 then that means there's no bottom triangle, only the upper
        // triangle exists. In that case, just draw the upper triangle.
        FillFlatBottomTriangle(x0, y0, x1, y1, x2, y2, color);
    }
    else if (y0 == y1)
    {
        // NOTE(sbalse): if y0 == y1, then that means there's no upper triangle, only the bottom
        // triangle exists. In that case, just draw the bottom triangle.
        FillFlatTopTriangle(x0, y0, x1, y1, x2, y2, color);
    }
    else // NOTE(sbalse): Normal triangle that has both the upper and bottom part.
    {
        // NOTE(sbalse): Calculate the new vertex (Mx, My) using triangle similarity.
        const int my = y1;
        const int mx = static_cast<int>((((x2 - x0) * (y1 - y0)) / static_cast<float>(y2 - y0)) + x0);

        FillFlatBottomTriangle(x0, y0, x1, y1, mx, my, color);

        FillFlatTopTriangle(x1, y1, mx, my, x2, y2, color);
    }
}

/*
NOTE(sbalse): Draw a textured triangle with the flat-top/flat-bottom method.
          (v0)
           / \
          /   \
         /     \
        /       \
       /         \
   (v1)-----------\
       \          \
         \        \
           \       \
             \      \
               \     \
                 \    \
                   \  \
                     \ \
                       \
                      (v2)
*/
void DrawTexturedTriangle(
    int x0, int y0, float u0, float v0,
    int x1, int y1, float u1, float v1,
    int x2, int y2, float u2, float v2,
    const u32* const texture
)
{
    // NOTE(sbalse): Loop all the pixels of the triangle to render them based on the color that
    // comes from the texture.

    // NOTE(sbalse): Sort vertices by y-coordinate ascending (y0 < y1 < y2);
    if (y0 > y1)
    {
        SWAP(int, x0, x1);
        SWAP(int, y0, y1);
        SWAP(float, u0, u1);
        SWAP(float, v0, v1);
    }
    if (y1 > y2)
    {
        SWAP(int, x1, x2);
        SWAP(int, y1, y2);
        SWAP(float, u1, u2);
        SWAP(float, v1, v2);
    }
    if (y0 > y1)
    {
        SWAP(int, x0, x1);
        SWAP(int, y0, y1);
        SWAP(float, u0, u1);
        SWAP(float, v0, v1);
    }

    // NOTE(sbalse): Create vectors for the 3 triangle vertices.
    const Vec2 pointA = { static_cast<float>(x0), static_cast<float>(y0) };
    const Vec2 pointB = { static_cast<float>(x1), static_cast<float>(y1) };
    const Vec2 pointC = { static_cast<float>(x2), static_cast<float>(y2) };

    /////////// NOTE(sbalse): Render the upper part of the triangle (flat-bottom). ////////////////

    // NOTE(sbalse): Use inverse slopes since we want to know how much our x changes with y (instead
    // of the other way around).
    float invSlope1 = (y1 - y0 != 0) ? (static_cast<float>(x1 - x0) / std::abs(y1 - y0)) : 0.0f;
    float invSlope2 = (y2 - y0 != 0) ? (static_cast<float>(x2 - x0) / std::abs(y2 - y0)) : 0.0f;

    if (y1 - y0 != 0)
    {
        for (int y = y0; y <= y1; y++)
        {
            /* NOTE(sbalse) : Explaination of this formula :
            (y - y1) is the entire range from start to end of y. Then we multiply by the slope to
            get the correct scaling factor to get where we want to be in the range. The first "x1 +"
            is just to offset the range of values by the start value of x0. */
            int xStart = static_cast<int>(x1 + (y - y1) * invSlope1);
            int xEnd = static_cast<int>(x0 + (y - y0) * invSlope2);

            if (xEnd < xStart)
            {
                // NOTE(sbalse): Swap if xStart is to the right of xEnd.
                SWAP(int, xStart, xEnd);
            }

            for (int x = xStart; x < xEnd; x++)
            {
                // TODO(sbalse): Draw our pixel with the color that comes from the texture.
                DrawTexel(x, y, pointA, pointB, pointC, u0, v0, u1, v1, u2, v2, texture);
            }
        }
    }

    /////////// NOTE(sbalse): Render the bottom part of the triangle (flat-top). /////////////////

    invSlope1 = (y2 - y1 != 0) ? (static_cast<float>(x2 - x1) / std::abs(y2 - y1)) : 0.0f;
    invSlope2 = (y2 - y0 != 0) ? (static_cast<float>(x2 - x0) / std::abs(y2 - y0)) : 0.0f;

    if (y2 - y1 != 0)
    {
        for (int y = y1; y <= y2; y++)
        {
            int xStart = static_cast<int>(x1 + (y - y1) * invSlope1);
            int xEnd = static_cast<int>(x2 + (y - y2) * invSlope2);

            if (xEnd < xStart)
            {
                // NOTE(sbalse): Swap if xStart is to the right of xEnd.
                SWAP(int, xStart, xEnd);
            }

            for (int x = xStart; x < xEnd; x++)
            {
                DrawTexel(x, y, pointA, pointB, pointC, u0, v0, u1, v1, u2, v2, texture);
            }
        }
    }
}

void DrawTexel(
    const int x,
    const int y,
    const Vec2 pointA,
    const Vec2 pointB,
    const Vec2 pointC,
    const float u0,
    const float v0,
    const float u1,
    const float v1,
    const float u2,
    const float v2,
    const u32* const texture
)
{
    const Vec2 pointP = { static_cast<float>(x), static_cast<float>(y) };
    const Vec3 weights = BarycentricWeights(pointA, pointB, pointC, pointP);

    const float alpha = weights.m_X;
    const float beta = weights.m_Y;
    const float gamma = weights.m_Z;

    // NOTE(sbalse): Calculating barycentric coordinates gives us the interpolated U and V values.
    const float interpolatedU = (u0 * alpha) + (u1 * beta) + (u2 * gamma);
    const float interpolatedV = (v0 * alpha) + (v1 * beta) + (v2 * gamma);

    // NOTE(sbalse): The interpolated value will be between 0 and 1 so we need to multiply it by the
    // texture width and height to map the UV coordinates to the full texture width and height.
    const int texelX = std::abs(static_cast<int>(interpolatedU * g_TextureWidth) % g_TextureWidth);
    const int texelY = std::abs(static_cast<int>(interpolatedV * g_TextureHeight) % g_TextureHeight);

    // NOTE(sbalse): Draw the corresponding color from our texture.
    const u32 color = texture[(g_TextureWidth * texelY) + texelX];
    DrawPixel(x, y, color);
}
