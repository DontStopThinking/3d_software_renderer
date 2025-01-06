#pragma once
#include "vector.h"
#include "common.h"

struct Face
{
    int m_A;
    int m_B;
    int m_C;
    u32 m_Color;
};

struct Triangle
{
    Vec2 m_Points[3];
    u32 m_Color;
    float m_AvgDepth; // NOTE(sbalse): Depth of the triangle, used to sort the face.
};

void DrawFilledTriangle(
    int x0,
    int y0,
    int x1,
    int y1,
    int x2,
    int y2,
    const u32 color
);
