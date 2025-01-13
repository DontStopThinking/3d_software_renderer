#pragma once
#include <string_view>
extern "C"
{
    #include <upng.h>
}

#include "common.h"

// NOTE(sbalse): A texture type.
struct Tex2
{
    float m_U;
    float m_V;
};

extern constinit int g_TextureWidth;
extern constinit int g_TextureHeight;

extern constinit const u32* g_MeshTexture;
extern constinit upng_t* g_PNGTexture;

void LoadPNGTextureData(const std::string_view fileName);
