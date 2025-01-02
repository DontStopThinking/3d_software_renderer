#include "mesh.h"

void LoadCubeMeshData()
{
    for (int i = 0; i < NUM_CUBE_VERTICES; i++)
    {
        const Vec3 cubeVertex = CUBE_VERTICES[i];
        g_Mesh.m_Vertices.emplace_back(cubeVertex);
    }

    for (int i = 0; i < NUM_CUBE_FACES; i++)
    {
        const Face cubeFace = CUBE_FACES[i];
        g_Mesh.m_Faces.emplace_back(cubeFace);
    }
}
