#pragma once
#include <vector>
#include <string_view>

#include "common.h"
#include "vector.h"
#include "triangle.h"
#include "display.h"

inline constexpr u8 NUM_CUBE_VERTICES = 8;
inline constexpr Vec3 CUBE_VERTICES[NUM_CUBE_VERTICES] =
{
    { .m_X = -1, .m_Y = -1, .m_Z = 1 }, // 1
    { .m_X = 1, .m_Y = -1, .m_Z = 1 }, // 2
    { .m_X = -1, .m_Y = 1, .m_Z = 1 }, // 3
    { .m_X = 1, .m_Y = 1, .m_Z = 1 }, // 4
    { .m_X = -1, .m_Y = 1, .m_Z = -1 }, // 5
    { .m_X = 1, .m_Y = 1, .m_Z = -1 }, // 6
    { .m_X = -1, .m_Y = -1, .m_Z = -1 }, // 7
    { .m_X = 1, .m_Y = -1, .m_Z = -1 }, // 8
};

inline constexpr u8 NUM_CUBE_FACES = 6 * 2; // NOTE(sbalse): 6 cube faces, 2 triangles per face.
inline constexpr Face CUBE_FACES[NUM_CUBE_FACES] =
{
    // Front
    { .m_A = 1, .m_B = 2, .m_C = 3, .m_Color = RED, .m_AUV = { 0, 0 }, .m_BUV = { 0, 1 }, .m_CUV = { 1, 1 }, },
    { .m_A = 3, .m_B = 2, .m_C = 4, .m_Color = RED, .m_AUV = { 0, 0 }, .m_BUV = { 1, 1 }, .m_CUV = { 1, 0 }, },

    // Right
    { .m_A = 3, .m_B = 4, .m_C = 5, .m_Color = LIGHT_GREEN, .m_AUV = { 0, 0 }, .m_BUV = { 0, 1 }, .m_CUV = { 1, 1 }, },
    { .m_A = 5, .m_B = 4, .m_C = 6, .m_Color = LIGHT_GREEN, .m_AUV = { 0, 0 }, .m_BUV = { 1, 1 }, .m_CUV = { 1, 0 }, },

    // Back
    { .m_A = 5, .m_B = 6, .m_C = 7, .m_Color = BLUE, .m_AUV = { 0, 0 }, .m_BUV = { 0, 1 }, .m_CUV = { 1, 1 }, },
    { .m_A = 7, .m_B = 6, .m_C = 8, .m_Color = BLUE, .m_AUV = { 0, 0 }, .m_BUV = { 1, 1 }, .m_CUV = { 1, 0 }, },

    // Left
    { .m_A = 7, .m_B = 8, .m_C = 1, .m_Color = YELLOW, .m_AUV = { 0, 0 }, .m_BUV = { 0, 1 }, .m_CUV = { 1, 1 }, },
    { .m_A = 1, .m_B = 8, .m_C = 2, .m_Color = YELLOW, .m_AUV = { 0, 0 }, .m_BUV = { 1, 1 }, .m_CUV = { 1, 0 }, },

    // Top
    { .m_A = 2, .m_B = 8, .m_C = 4, .m_Color = MAGENTA, .m_AUV = { 0, 0 }, .m_BUV = { 0, 1 }, .m_CUV = { 1, 1 }, },
    { .m_A = 4, .m_B = 8, .m_C = 6, .m_Color = MAGENTA, .m_AUV = { 0, 0 }, .m_BUV = { 1, 1 }, .m_CUV = { 1, 0 }, },

    // Bottom
    { .m_A = 7, .m_B = 1, .m_C = 5, .m_Color = CYAN, .m_AUV = { 0, 0 }, .m_BUV = { 0, 1 }, .m_CUV = { 1, 1 }, },
    { .m_A = 5, .m_B = 1, .m_C = 3, .m_Color = CYAN, .m_AUV = { 0, 0 }, .m_BUV = { 1, 1 }, .m_CUV = { 1, 0 }, },
};

// NOTE(sbalse): A struct for dynamic sized meshes. Contains an array of vertices, faces and the
// rotation of the mesh.
struct Mesh
{
    std::vector<Vec3> m_Vertices; // NOTE(sbalse): The mesh vertices.
    std::vector<Face> m_Faces; // NOTE(sbalse): The mesh faces.
    Vec3 m_Rotation; // NOTE(sbalse): Rotation of the mesh using Euler angles.
    Vec3 m_Scale; // NOTE(sbalse): Scale with x, y and z values.
    Vec3 m_Translation; // NOTE(sbalse): Translation with x, y and z values.
};

extern constinit Mesh g_Mesh;

void LoadCubeMeshData();
void LoadObjFileData(const std::string_view fileName);
