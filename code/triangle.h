#pragma once
#include "vector.h"
#include "common.h"
#include "texture.h"

struct Face
{
    // NOTE(sbalse): Vertex indices.
    int m_A;
    int m_B;
    int m_C;

    // NOTE(sbalse): The face color.
    u32 m_Color;

    // NOTE(sbalse): Texture UV coordinates.
    Tex2 m_AUV;
    Tex2 m_BUV;
    Tex2 m_CUV;
};

struct Triangle
{
    Vec2 m_Points[3];
    Tex2 m_TexCoords[3];
    u32 m_Color;
    float m_AvgDepth; // NOTE(sbalse): Depth of the triangle, used to sort the face.
};

void DrawTriangle(
    const int x0, const int y0, const int x1,
    const int y1, const int x2, const int y2,
    const u32 color
);

void DrawFilledTriangle(
    int x0,
    int y0,
    int x1,
    int y1,
    int x2,
    int y2,
    const u32 color
);

void DrawTexturedTriangle(
    int x0, int y0, float u0, float v0,
    int x1, int y1, float u1, float v1,
    int x2, int y2, float u2, float v2,
    const u32* const texture
);
