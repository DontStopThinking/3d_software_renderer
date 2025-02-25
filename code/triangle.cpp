#include "triangle.h"

#include "display.h"

#define SWAP(Type, v1, v2) \
do \
{ \
    Type temp = v2; \
    v2 = v1; \
    v1 = temp; \
} while (0)

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

// NOTE(sbalse): Draw a solid pixel at position (x, y) using depth interpolation.
static void DrawTrianglePixel(
    const int x, const int y,
    const Vec4 pointA, const Vec4 pointB, const Vec4 pointC,
    const u32 color)
{
    // NOTE(sbalse): Create three Vec2 to find the interpolation.
    const Vec2 p = { scast<float>(x), scast<float>(y) };
    const Vec2 a = Vec2FromVec4(pointA);
    const Vec2 b = Vec2FromVec4(pointB);
    const Vec2 c = Vec2FromVec4(pointC);

    const Vec3 weights = BarycentricWeights(a, b, c, p);

    const float alpha = weights.m_X;
    const float beta = weights.m_Y;
    const float gamma = weights.m_Z;

    // NOTE(sbalse): Precompute the reciprocal values of W for points A, B and C along with their
    // respective barycentric weights.
    const float reciprocalWAlphaOfPointA = alpha / pointA.m_W;
    const float reciprocalWBetaOfPointB = beta / pointB.m_W;
    const float reciprocalWGammaPointC = gamma / pointC.m_W;

    // NOTE(sbalse): Interpolate the value of 1/w.
    float interpolatedReciprocalW =
        (1.0f / pointA.m_W) * alpha
        + (1.0f / pointB.m_W) * beta
        + (1.0f / pointC.m_W) * gamma;

    // NOTE(sbalse): Adjust 1/w so that pixels that are closer to the camera have smaller values.
    interpolatedReciprocalW = 1.0f - interpolatedReciprocalW;

    // NOTE(sbalse): Only draw the pixel if the depth value is less than the one previously stored
    // in the z-buffer.
    if (interpolatedReciprocalW < GetZBufferAt(x, y))
    {
        DrawPixel(x, y, color);

        UpdateNormalizedZBufferAt(x, y, interpolatedReciprocalW);

        if (GetRenderBufferMethod() == RenderBufferMethod::ZBuffer)
        {
            const u8 grayscale = scast<u8>(interpolatedReciprocalW * 255.0f);
            const u32 zcolor = (grayscale << 16) | (grayscale << 8) | grayscale;
            UpdateDisplayableZBufferAt(x, y, zcolor);
        }
    }
}

// NOTE(sbalse): Draw the the textured pixel at position X and Y using interpolation.
// x, y - Pixels coordinates where the texture is to be drawn.
// pointA, pointB, pointC - vertices of the triangle.
// u0, v0, u1, v1, u2, v2 - The UV coordinate of the texture to draw on the triangle.
// The function will figure out which texel to draw from the given parameters.
static void DrawTriangleTexel(
    const int x,
    const int y,
    const Vec4 pointA,
    const Vec4 pointB,
    const Vec4 pointC,
    const Tex2 aUV,
    const Tex2 bUV,
    const Tex2 cUV,
    const upng_t* const texture
)
{
    const Vec2 p = { scast<float>(x), scast<float>(y) };
    const Vec2 a = Vec2FromVec4(pointA);
    const Vec2 b = Vec2FromVec4(pointB);
    const Vec2 c = Vec2FromVec4(pointC);

    const Vec3 weights = BarycentricWeights(a, b, c, p);

    const float alpha = weights.m_X;
    const float beta = weights.m_Y;
    const float gamma = weights.m_Z;

    // NOTE(sbalse): Calculating barycentric coordinates gives us the interpolated U/w and V/w values.
    // The 1/w factor is what performs perspective correction on our texture. Perspective correction
    // is necessary otherwise the texture will appear distorted.

    // NOTE(sbalse): The formula for interpolated U/w is:
    // (Ua/Wa * alpha) + (Ub/Wb * beta) + (Uc/Wc * gamma).
    // Same for interpolated V.

    // NOTE(sbalse): Precompute the reciprocal values of W for points A, B and C along with their
    // respective barycentric weights.
    const float reciprocalWAlphaOfPointA = alpha / pointA.m_W;
    const float reciprocalWBetaOfPointB = beta / pointB.m_W;
    const float reciprocalWGammaPointC = gamma / pointC.m_W;

    // NOTE(sbalse): Interpolate the reciprocal of W.
    float interpolatedReciprocalW =
        reciprocalWAlphaOfPointA
        + reciprocalWBetaOfPointB
        + reciprocalWGammaPointC;

    // NOTE(sbalse): Interpolate U and V using the precomputed reciprocals of W.
    float interpolatedU =
        (aUV.m_U * reciprocalWAlphaOfPointA)
        + (bUV.m_U * reciprocalWBetaOfPointB)
        + (cUV.m_U * reciprocalWGammaPointC);
    float interpolatedV =
        (aUV.m_V * reciprocalWAlphaOfPointA)
        + (bUV.m_V * reciprocalWBetaOfPointB)
        + (cUV.m_V * reciprocalWGammaPointC);

    // NOTE(sbalse): Convert the interpolated U and V back into screen space by dividing them by
    // the interpolated W.
    interpolatedU /= interpolatedReciprocalW;
    interpolatedV /= interpolatedReciprocalW;

    // Get the mesh texture width and height.
    const u32 textureWidth = upng_get_width(texture);
    const u32 textureHeight = upng_get_height(texture);

    // NOTE(sbalse): The interpolated value will be between 0 and 1 so we need to multiply it by the
    // texture width and height to map the UV coordinates to the full texture width and height.
    const int texelX = std::abs(scast<int>(interpolatedU * textureWidth)) % textureWidth;
    const int texelY = std::abs(scast<int>(interpolatedV * textureHeight)) % textureHeight;

    // NOTE(sbalse): Adjust 1/w so that pixels closer to the camera have smaller values of
    // interpolated reciprocal W.
    interpolatedReciprocalW = 1.0f - interpolatedReciprocalW;

    // NOTE(sbalse): Only draw the pixel if the depth value is less than the one previously stored
    // in the z-buffer.
    if (interpolatedReciprocalW < GetZBufferAt(x, y))
    {
        // Get the buffer of colors from the texture.
        const u32* textureColors = rcast<const u32*>(upng_get_buffer(texture));

        // NOTE(sbalse): Draw the corresponding color from our texture.
        const u32 color = textureColors[(textureWidth * texelY) + texelX];
        DrawPixel(x, y, color);

        // NOTE(sbalse): Update the z-buffer value with the 1/w of this current pixel.
        UpdateNormalizedZBufferAt(x, y, interpolatedReciprocalW);

        if (GetRenderBufferMethod() == RenderBufferMethod::ZBuffer)
        {
            const u8 grayscale = scast<u8>(interpolatedReciprocalW * 255.0f);
            const u32 zcolor = (grayscale << 16) | (grayscale << 8) | grayscale;
            UpdateDisplayableZBufferAt(x, y, zcolor);
        }
    }
}

void DrawTriangle(
    const int x0,
    const int y0,
    const int x1,
    const int y1,
    const int x2,
    const int y2,
    const u32 color
)
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
    int x0, int y0, float z0, float w0,
    int x1, int y1, float z1, float w1,
    int x2, int y2, float z2, float w2,
    const u32 color
)
{
    // NOTE(sbalse): Sort vertices by y-coordinate ascending (y0 < y1 < y2);
    if (y0 > y1)
    {
        SWAP(int, x0, x1);
        SWAP(int, y0, y1);
        SWAP(float, z0, z1);
        SWAP(float, w0, w1);
    }
    if (y1 > y2)
    {
        SWAP(int, x1, x2);
        SWAP(int, y1, y2);
        SWAP(float, z1, z2);
        SWAP(float, w1, w2);
    }
    if (y0 > y1)
    {
        SWAP(int, x0, x1);
        SWAP(int, y0, y1);
        SWAP(float, z0, z1);
        SWAP(float, w0, w1);
    }

    // NOTE(sbalse): Create three vector points after we sort the vertices.
    const Vec4 pointA = { scast<float>(x0), scast<float>(y0), z0, w0 };
    const Vec4 pointB = { scast<float>(x1), scast<float>(y1), z1, w1 };
    const Vec4 pointC = { scast<float>(x2), scast<float>(y2), z2, w2 };

    // NOTE(sbalse): Use inverse slopes since we want to know how much our x changes with y (instead
    // of the other way around).
    float invSlope1 = (y1 - y0 != 0) ? (scast<float>(x1 - x0) / std::abs(y1 - y0)) : 0.0f;
    float invSlope2 = (y2 - y0 != 0) ? (scast<float>(x2 - x0) / std::abs(y2 - y0)) : 0.0f;

    if (y1 - y0 != 0)
    {
        for (int y = y0; y <= y1; y++)
        {
            /* NOTE(sbalse) : Explaination of this formula :
            (y - y1) is the entire range from start to end of y. Then we multiply by the slope to
            get the correct scaling factor to get where we want to be in the range. The first "x1 +"
            is just to offset the range of values by the start value of x0. */
            int xStart = scast<int>(x1 + (y - y1) * invSlope1);
            int xEnd = scast<int>(x0 + (y - y0) * invSlope2);

            if (xEnd < xStart)
            {
                // NOTE(sbalse): Swap if xStart is to the right of xEnd.
                SWAP(int, xStart, xEnd);
            }

            for (int x = xStart; x < xEnd; x++)
            {
                // TODO(sbalse): Draw our pixel with the color that comes from the texture.
                DrawTrianglePixel(x, y, pointA, pointB, pointC, color);
            }
        }
    }

    /////////// NOTE(sbalse): Render the bottom part of the triangle (flat-top). /////////////////

    invSlope1 = (y2 - y1 != 0) ? (scast<float>(x2 - x1) / std::abs(y2 - y1)) : 0.0f;
    invSlope2 = (y2 - y0 != 0) ? (scast<float>(x2 - x0) / std::abs(y2 - y0)) : 0.0f;

    if (y2 - y1 != 0)
    {
        for (int y = y1; y <= y2; y++)
        {
            int xStart = scast<int>(x1 + (y - y1) * invSlope1);
            int xEnd = scast<int>(x2 + (y - y2) * invSlope2);

            if (xEnd < xStart)
            {
                // NOTE(sbalse): Swap if xStart is to the right of xEnd.
                SWAP(int, xStart, xEnd);
            }

            for (int x = xStart; x < xEnd; x++)
            {
                DrawTrianglePixel(x, y, pointA, pointB, pointC, color);
            }
        }
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
    int x0, int y0, float z0, float w0, float u0, float v0,
    int x1, int y1, float z1, float w1, float u1, float v1,
    int x2, int y2, float z2, float w2, float u2, float v2,
    const upng_t* const texture
)
{
    // NOTE(sbalse): Loop all the pixels of the triangle to render them based on the color that
    // comes from the texture.

    // NOTE(sbalse): Sort vertices by y-coordinate ascending (y0 < y1 < y2);
    if (y0 > y1)
    {
        SWAP(int, x0, x1);
        SWAP(int, y0, y1);
        SWAP(float, z0, z1);
        SWAP(float, w0, w1);
        SWAP(float, u0, u1);
        SWAP(float, v0, v1);
    }
    if (y1 > y2)
    {
        SWAP(int, x1, x2);
        SWAP(int, y1, y2);
        SWAP(float, z1, z2);
        SWAP(float, w1, w2);
        SWAP(float, u1, u2);
        SWAP(float, v1, v2);
    }
    if (y0 > y1)
    {
        SWAP(int, x0, x1);
        SWAP(int, y0, y1);
        SWAP(float, z0, z1);
        SWAP(float, w0, w1);
        SWAP(float, u0, u1);
        SWAP(float, v0, v1);
    }

    // NOTE(sbalse): Flip the V component to account for inverted UV-coordinates (V grows downwards).
    v0 = 1.0f - v0;
    v1 = 1.0f - v1;
    v2 = 1.0f - v2;

    // NOTE(sbalse): Create vectors for the 3 triangle vertices.
    const Vec4 pointA = { scast<float>(x0), scast<float>(y0), z0, w0 };
    const Vec4 pointB = { scast<float>(x1), scast<float>(y1), z1, w1 };
    const Vec4 pointC = { scast<float>(x2), scast<float>(y2), z2, w2 };
    const Tex2 aUV = { u0, v0 };
    const Tex2 bUV = { u1, v1 };
    const Tex2 cUV = { u2, v2 };

    /////////// NOTE(sbalse): Render the upper part of the triangle (flat-bottom). ////////////////

    // NOTE(sbalse): Use inverse slopes since we want to know how much our x changes with y (instead
    // of the other way around).
    float invSlope1 = (y1 - y0 != 0) ? (scast<float>(x1 - x0) / std::abs(y1 - y0)) : 0.0f;
    float invSlope2 = (y2 - y0 != 0) ? (scast<float>(x2 - x0) / std::abs(y2 - y0)) : 0.0f;

    if (y1 - y0 != 0)
    {
        for (int y = y0; y <= y1; y++)
        {
            /* NOTE(sbalse) : Explaination of this formula :
            (y - y1) is the entire range from start to end of y. Then we multiply by the slope to
            get the correct scaling factor to get where we want to be in the range. The first "x1 +"
            is just to offset the range of values by the start value of x0. */
            int xStart = scast<int>(x1 + (y - y1) * invSlope1);
            int xEnd = scast<int>(x0 + (y - y0) * invSlope2);

            if (xEnd < xStart)
            {
                // NOTE(sbalse): Swap if xStart is to the right of xEnd.
                SWAP(int, xStart, xEnd);
            }

            for (int x = xStart; x < xEnd; x++)
            {
                // TODO(sbalse): Draw our pixel with the color that comes from the texture.
                DrawTriangleTexel(x, y, pointA, pointB, pointC, aUV, bUV, cUV, texture);
            }
        }
    }

    /////////// NOTE(sbalse): Render the bottom part of the triangle (flat-top). /////////////////

    invSlope1 = (y2 - y1 != 0) ? (scast<float>(x2 - x1) / std::abs(y2 - y1)) : 0.0f;
    invSlope2 = (y2 - y0 != 0) ? (scast<float>(x2 - x0) / std::abs(y2 - y0)) : 0.0f;

    if (y2 - y1 != 0)
    {
        for (int y = y1; y <= y2; y++)
        {
            int xStart = scast<int>(x1 + (y - y1) * invSlope1);
            int xEnd = scast<int>(x2 + (y - y2) * invSlope2);

            if (xEnd < xStart)
            {
                // NOTE(sbalse): Swap if xStart is to the right of xEnd.
                SWAP(int, xStart, xEnd);
            }

            for (int x = xStart; x < xEnd; x++)
            {
                DrawTriangleTexel(x, y, pointA, pointB, pointC, aUV, bUV, cUV, texture);
            }
        }
    }
}
