#include "mesh.h"

#include <cstdio>
#include <array>
#include <vector>
#include <cassert>

#include "log.h"
#include "vector.h"
#include "colorlibrary.h"
#include "triangle.h"

inline constexpr int MAX_NUM_MESHES = 10;
static Mesh g_Meshes[MAX_NUM_MESHES] = {};
constinit static int g_MeshCount = 0;

static void LoadMeshObjData(const std::string_view objFileName, Mesh* const outMesh)
{
    assert(outMesh != nullptr);

    std::FILE* file = nullptr;
    fopen_s(&file, objFileName.data(), "r");

    LOG_INFO("Loading obj file: %s...", objFileName.data());

    if (!file)
    {
        LOG_ERROR("Failed to open obj file: %s.", objFileName.data());
        return;
    }

    std::array<char, 1024> line = {};

    constexpr int maxNumberOfCharactersToRead = scast<int>(line.size());

    std::vector<Tex2> texcoords;

    while (std::fgets(line.data(), maxNumberOfCharactersToRead, file))
    {
        // NOTE(sbalse): Read vertex information.
        if (std::strncmp(line.data(), "v ", 2) == 0)
        {
            Vec3 vertex = {};
            sscanf_s(line.data(), "v %f %f %f", &vertex.m_X, &vertex.m_Y, &vertex.m_Z);
            outMesh->m_Vertices.emplace_back(vertex);
        }

        // NOTE(sbalse): Texture coordinate information.
        if (std::strncmp(line.data(), "vt ", 3) == 0)
        {
            Tex2 texcoord = {};
            sscanf_s(line.data(), "vt %f %f", &texcoord.m_U, &texcoord.m_V);
            texcoords.emplace_back(texcoord);
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
                // NOTE(sbalse): Need to do -1 since numbers start at 1 in OBJ files.
                .m_A = vertexIndices[0] - 1,
                .m_B = vertexIndices[1] - 1,
                .m_C = vertexIndices[2] - 1,
                .m_AUV = texcoords[textureIndices[0] - 1],
                .m_BUV = texcoords[textureIndices[1] - 1],
                .m_CUV = texcoords[textureIndices[2] - 1],
                .m_Color = WHITE,
            };

            outMesh->m_Faces.emplace_back(face);
        }
    }

    LOG_INFO("Successfully loaded obj file: %s.", objFileName.data());

    std::fclose(file);
}

static void LoadMeshPNGData(const std::string_view fileName, Mesh* const outMesh)
{
    assert(outMesh != nullptr);

    upng_t* pngImage = upng_new_from_file(fileName.data());
    if (pngImage == nullptr)
    {
        LOG_ERROR("Failed to load PNG file: %s", fileName.data());
        return;
    }

    upng_decode(pngImage);

    const upng_error error = upng_get_error(pngImage);
    if (error != UPNG_EOK)
    {
        const u32 errorLine = upng_get_error_line(pngImage);
        LOG_ERROR(
            "Error while decoding PNG. Error enum value = %d, Line = errorLine",
            error,
            errorLine);
        return;
    }

    outMesh->m_Texture = pngImage;
}

void LoadMesh(
    const std::string_view objFileName,
    const std::string_view pngFileName,
    const Vec3 translation,
    const Vec3 scale,
    const Vec3 rotation
)
{
    // Load the OBJ
    LoadMeshObjData(objFileName, &g_Meshes[g_MeshCount]);

    // Load the PNG
    LoadMeshPNGData(pngFileName, &g_Meshes[g_MeshCount]);

    // Init the scale, translation and rotation
    g_Meshes[g_MeshCount].m_Scale = scale;
    g_Meshes[g_MeshCount].m_Translation = translation;
    g_Meshes[g_MeshCount].m_Rotation = rotation;

    // Add the new mesh to the array of meshes
    g_MeshCount++;
}

int GetNumOfMeshes()
{
    return g_MeshCount;
}

Mesh* GetMesh(const int meshIndex)
{
    assert(meshIndex < g_MeshCount && "ERROR: Mesh index is greater than total number of available meshes.");

    return &g_Meshes[meshIndex];
}

void FreeMeshes()
{
    for (int i = 0; i < g_MeshCount; i++)
    {
        // Free mesh texture.
        upng_free(g_Meshes[i].m_Texture);
    }
}
