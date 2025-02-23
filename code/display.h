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

bool InitializeWindow(const std::string_view windowTitle);
void DestroyWindow();

int GetWindowWidth();
int GetWindowHeight();

void DrawGrid();
void DrawPixel(const int x, const int y, const u32 color);
void DrawRectangle(
    const int x,
    const int y,
    const int width,
    const int height,
    const u32 color
);
void DrawLine(const int x0, const int y0, const int x1, const int y1, const u32 color);
void TakeScreenshot(const std::string_view fileNamePrefix);

void RenderColorBuffer();
void RenderZBuffer();
void ClearColorBuffer(const u32 color);
void ClearZBuffer();

float GetZBufferAt(const int x, const int y);
// NOTE(sbalse): Normalized Z-Buffer is the array that's actually used for depth-testing.
void UpdateNormalizedZBufferAt(const int x, const int y, const float value);
// NOTE(sbalse): Displayable Z-Buffer is the array that contains the grayscale value used to display
// and visualize the z-buffer.
void UpdateDisplayableZBufferAt(const int x, const int y, const u32 value);

CullMethod GetCullMethod();
void SetCullMethod(const CullMethod newCullMethod);
RenderMethod GetRenderMethod();
void SetRenderMethod(const RenderMethod newRenderMethod);
ShadingMethod GetShadingMethod();
void SetShadingMethod(const ShadingMethod newShadingMethod);
RenderBufferMethod GetRenderBufferMethod();
void SetRenderBufferMethod(const RenderBufferMethod newRenderBufferMethod);
