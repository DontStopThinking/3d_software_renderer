#pragma once
#include "common.h"
#include "vector.h"
#include "triangle.h"

constexpr u8 NUM_MESH_VERTICES = 8;
constexpr Vec3 MESH_VERTICES[NUM_MESH_VERTICES] =
{
    { .m_X = -1, .m_Y = -1, .m_Z = -1 }, // 1
    { .m_X = -1, .m_Y = 1, .m_Z = -1 }, // 2
    { .m_X = 1, .m_Y = 1, .m_Z = -1 }, // 3
    { .m_X = 1, .m_Y = -1, .m_Z = -1 }, // 4
    { .m_X = 1, .m_Y = 1, .m_Z = 1 }, // 5
    { .m_X = 1, .m_Y = -1, .m_Z = 1 }, // 6
    { .m_X = -1, .m_Y = 1, .m_Z = 1 }, // 7
    { .m_X = -1, .m_Y = -1, .m_Z = 1 }, // 8
};

constexpr u8 NUM_MESH_FACES = 6 * 2; // NOTE(sbalse): 6 cube faces, 2 triangles per face.
constexpr Face MESH_FACES[NUM_MESH_FACES] =
{
    // Front
    { .m_A = 1, .m_B = 2, .m_C = 3 },
    { .m_A = 1, .m_B = 3, .m_C = 4 },

    // Right
    { .m_A = 4, .m_B = 3, .m_C = 5 },
    { .m_A = 4, .m_B = 5, .m_C = 6 },

    // Back
    { .m_A = 6, .m_B = 5, .m_C = 7 },
    { .m_A = 6, .m_B = 7, .m_C = 8 },

    // Left
    { .m_A = 8, .m_B = 7, .m_C = 2 },
    { .m_A = 8, .m_B = 2, .m_C = 1 },

    // Top
    { .m_A = 2, .m_B = 7, .m_C = 5 },
    { .m_A = 2, .m_B = 5, .m_C = 3 },

    // Bottom
    { .m_A = 6, .m_B = 6, .m_C = 1 },
    { .m_A = 6, .m_B = 1, .m_C = 4 },
};
