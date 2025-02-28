#pragma once
#include <vector>
#include <string_view>

extern "C"
{
    #include <upng.h>
}

#include "vector.h"
#include "triangle.h"

// NOTE(sbalse): A struct for dynamic sized meshes. Contains an array of vertices, faces and the
// rotation of the mesh.
struct Mesh
{
    std::vector<Vec3> m_Vertices; // NOTE(sbalse): The mesh vertices.
    std::vector<Face> m_Faces; // NOTE(sbalse): The mesh faces.
    upng_t* m_Texture; // NOTE(sbalse): The mesh's PNG texture.
    Vec3 m_Rotation; // NOTE(sbalse): Rotation of the mesh using x, y and z Euler angles.
    Vec3 m_Scale; // NOTE(sbalse): Scale with x, y and z values.
    Vec3 m_Translation; // NOTE(sbalse): Translation with x, y and z values.
};

void LoadMesh(
    const std::string_view objFileName,
    const std::string_view pngFileName,
    const Vec3 translation,
    const Vec3 scale,
    const Vec3 rotation
);
int GetNumOfMeshes();
Mesh* GetMesh(const int meshIndex);
void FreeMeshes();
