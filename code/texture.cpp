#include "texture.h"

#include "log.h"

// NOTE(sbalse): Init externs.
constinit int g_TextureWidth = 0;
constinit int g_TextureHeight = 0;
constinit const u32* g_MeshTexture = nullptr;
constinit upng_t* g_PNGTexture = nullptr;

void LoadPNGTextureData(const std::string_view fileName)
{
    g_PNGTexture = upng_new_from_file(fileName.data());
    if (g_PNGTexture == nullptr)
    {
        LOG_ERROR("Failed to load PNG file: %s", fileName.data());
        return;
    }

    upng_decode(g_PNGTexture);

    const upng_error error = upng_get_error(g_PNGTexture);
    if (error != UPNG_EOK)
    {
        const u32 errorLine = upng_get_error_line(g_PNGTexture);
        LOG_ERROR(
            "Error while decoding PNG. Error enum value = %d, Line = errorLine",
            error,
            errorLine);
        return;
    }

    g_MeshTexture = reinterpret_cast<const u32*>(upng_get_buffer(g_PNGTexture));
    g_TextureWidth = upng_get_width(g_PNGTexture);
    g_TextureHeight = upng_get_height(g_PNGTexture);
}
