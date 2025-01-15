#pragma once
#include <string_view>
#include <SDL.h>

#include "common.h"
#include "colorlibrary.h"

// NOTE(sbalse): We will update and render these many frames in 1 second.
inline constexpr u32 FPS = 30;
// NOTE(sbalse): How many milliseconds each frame should take. Using 1000 as FPS is in seconds and
// this value is in milliseconds.
inline constexpr float FRAME_TARGET_TIME_MS = (1000.0f / FPS);

struct ColorBuffer
{
    u32* m_Buffer;
    size_t m_Size;
    SDL_Texture* m_Texture;
};

struct ZBuffer
{
    float* m_BufferUNorm; // NOTE(sbalse): Color values are between 0.0f and 1.0f in this.
    size_t m_BufferUNormSize;
    u32* m_BufferUInt; // NOTE(sbalse): Color values are between 0 to 255 in this.
    size_t m_BufferUIntSize;
    SDL_Texture* m_Texture;
};

enum class CullMethod
{
    None,
    Backface,
};

enum class RenderMethod
{
    Wire,
    WireVertex,
    FillTriangle,
    FillTriangleWire,
    Textured,
    WireTextured,
};

enum class RenderBufferMethod
{
    ColorBuffer,
    ZBuffer,
};

enum class ShadingMethod
{
    None,
    FlatShading
};

// NOTE(sbalse): extern as these will be initialized in display.cpp
extern constinit SDL_Window* g_Window;
extern constinit SDL_Renderer* g_Renderer;
extern constinit ColorBuffer g_ColorBuffer;
extern constinit ZBuffer g_ZBuffer;

extern constinit int g_WindowWidth;
extern constinit int g_WindowHeight;

extern constinit CullMethod g_CullMethod;
extern constinit RenderMethod g_RenderMethod;
extern constinit ShadingMethod g_ShadingMethod;
extern constinit RenderBufferMethod g_RenderBufferMethod;

bool InitializeWindow(const std::string_view windowTitle);
void DrawGrid();
void DrawPixel(const int x, const int y, const u32 color);
void DrawRectangle(
    const int x,
    const int y,
    const int width,
    const int height,
    const u32 color);
void DrawLine(const int x0, const int y0, const int x1, const int y1, const u32 color);
void TakeScreenshot(SDL_Renderer* renderer, const std::string_view fileNamePrefix);
void RenderColorBuffer();
void RenderZBuffer();
void ClearColorBuffer(const u32 color);
void ClearZBuffer();
void DestroyWindow();
