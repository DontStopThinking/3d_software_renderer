#include "mesh.h"

#include <cstdio>
#include <array>

#include "log.h"
#include "vector.h"

extern constinit Mesh g_Mesh =
{
    .m_Scale = { 1.0f, 1.0f, 1.0f },
};

void LoadCubeMeshData()
{
    LOG_INFO("Creating cube mesh...");

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

    LOG_INFO("Successfully created cube mesh.");
}

void LoadObjFileData(const std::string_view fileName)
{
    std::FILE* file = nullptr;
    fopen_s(&file, fileName.data(), "r");

    LOG_INFO("Loading obj file: %s...", fileName.data());

    if (!file)
    {
        LOG_ERROR("Failed to open obj file: %s.", fileName.data());
        return;
    }

    std::array<char, 1024> line = {};

    constexpr int maxNumberOfCharactersToRead = static_cast<int>(line.size());

    while (std::fgets(line.data(), maxNumberOfCharactersToRead, file))
    {
        // NOTE(sbalse): Read vertex information.
        if (std::strncmp(line.data(), "v ", 2) == 0)
        {
            Vec3 vertex = {};
            sscanf_s(line.data(), "v %f %f %f", &vertex.m_X, &vertex.m_Y, &vertex.m_Z);
            g_Mesh.m_Vertices.emplace_back(vertex);
        }
        // NOTE(sbalse): Read face information.
        if (std::strncmp(line.data(), "f ", 2) == 0)
        {
            int vertexIndices[3] = {};
            int textureIndices[3] = {};
            int normalIndices[3] = {};
            sscanf_s(
                line.data(),
                "f %d/%d/%d %d/%d/%d %d/%d/%d",
                &vertexIndices[0], &textureIndices[0], &normalIndices[0],
                &vertexIndices[1], &textureIndices[1], &normalIndices[1],
                &vertexIndices[2], &textureIndices[2], &normalIndices[2]);

            const Face face =
            {
                .m_A = vertexIndices[0],
                .m_B = vertexIndices[1],
                .m_C = vertexIndices[2],
                .m_Color = WHITE,
            };

            g_Mesh.m_Faces.emplace_back(face);
        }
    }

    LOG_INFO("Successfully loaded obj file: %s.", fileName.data());

    std::fclose(file);
}
