#pragma once
#include <SDL.h>

#include "common.h"

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

bool InitializeWindow();
void DrawGrid();
void DrawPixel(int x, int y, u32 color);
void DrawRectangle(int x, int y, int width, int height, u32 color);
void RenderColorBuffer();
void ClearColorBuffer(u32 color);
void DestroyWindow();
