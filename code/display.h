#pragma once
#include <string_view>
#include <SDL.h>

#include "common.h"

// NOTE(sbalse): Colors
constexpr u32 BLACK = 0xFF000000;
constexpr u32 YELLOW = 0xFFFFFF00;
constexpr u32 MAGENTA = 0xFFFF00FF;
constexpr u32 GRAY = 0xFF333333;

// NOTE(sbalse): We will update and render these many frames in 1 second.
constexpr u32 FPS = 30;
// NOTE(sbalse): How many milliseconds each frame should take. Using 1000 as FPS is in seconds and this value
// is in milliseconds.
constexpr float FRAME_TARGET_TIME_MS = (1000.0f / FPS);

struct ColorBuffer
{
    u32* m_Buffer;
    size_t m_Size;
    SDL_Texture* m_Texture;
};

// NOTE(sbalse): extern as these will be initialized in display.cpp
extern constinit SDL_Window* g_Window;
extern constinit SDL_Renderer* g_Renderer;
extern constinit ColorBuffer g_ColorBuffer;
extern constinit int g_WindowWidth;
extern constinit int g_WindowHeight;

bool InitializeWindow(const std::string_view windowTitle);
void DrawGrid();
void DrawPixel(const int x, const int y, const u32 color);
void DrawRectangle(
    const int x,
    const int y,
    const int width,
    const int height,
    const u32 color);
void TakeScreenshot(SDL_Renderer* renderer, const std::string_view fileNamePrefix);
void RenderColorBuffer();
void ClearColorBuffer(const u32 color);
void DestroyWindow();
