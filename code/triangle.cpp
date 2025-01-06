#include "triangle.h"

#include "display.h"

static void IntSwap(int* a, int* b)
{
    const int tmp = *a;
    *a = *b;
    *b = tmp;
}

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
                      \\
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
                      \\
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
        IntSwap(&y0, &y1);
        IntSwap(&x0, &x1);
    }
    if (y1 > y2)
    {
        IntSwap(&y1, &y2);
        IntSwap(&x1, &x2);
    }
    if (y0 > y1)
    {
        IntSwap(&y0, &y1);
        IntSwap(&x0, &x1);
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
