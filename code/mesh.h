#pragma once
extern "C"
{
    #include <upng.h>
}

#include "arena.h"
#include "vector.h"
#include "triangle.h"

// NOTE(sbalse): A struct for dynamic sized meshes. Contains an array of vertices, faces and the
// rotation of the mesh.
struct Mesh
{
    Vec3 m_Rotation; // NOTE(sbalse): Rotation of the mesh using x, y and z Euler angles.
    Vec3 m_Scale; // NOTE(sbalse): Scale with x, y and z values.
    Vec3 m_Translation; // NOTE(sbalse): Translation with x, y and z values.

    Vec3* m_Vertices; // NOTE(sbalse): The mesh vertices.
    size_t m_VerticesCount;

    Face* m_Faces; // NOTE(sbalse): The mesh faces.
    size_t m_FacesCount;

    upng_t* m_Texture; // NOTE(sbalse): The mesh's PNG texture.
};

void LoadMesh(
    Arena* arena,
    const char* const objFileName,
    const char* const pngFileName,
    const Vec3 translation,
    const Vec3 scale,
    const Vec3 rotation
);
int GetNumOfMeshes();
Mesh* GetMesh(const int meshIndex);
void FreeMeshes();
