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
                DrawPixel(x, y, (x % 2 == 0 && y % 2 == 0) ? MAGENTA : BLACK);
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
                // TODO(sbalse): Draw our pixel with the color that comes from the texture.
                DrawPixel(x, y, (x % 2 == 0 && y % 2 == 0) ? MAGENTA : BLACK);
            }
        }
    }
}
