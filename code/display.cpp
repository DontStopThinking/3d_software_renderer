#include "display.h"

#include <cstdio>
#include <cstdlib>

constinit SDL_Window* g_Window = nullptr;
constinit SDL_Renderer* g_Renderer = nullptr;
constinit ColorBuffer g_ColorBuffer = {};
constinit int g_WindowWidth = 800;
constinit int g_WindowHeight = 600;

bool InitializeWindow(const std::string_view windowTitle)
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        std::fprintf(stderr, "ERROR initializing SDL.\n");
        return false;
    }

    // NOTE(sbalse): Create an SDL window.
    SDL_Window* g_Window = SDL_CreateWindow(
        windowTitle.data(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        g_WindowWidth,
        g_WindowHeight,
        0
    );
    if (!g_Window)
    {
        std::fprintf(stderr, "ERROR creating SDL window.\n");
        return false;
    }

    // NOTE(sbalse): Create a SDL renderer.
    g_Renderer = SDL_CreateRenderer(g_Window, -1, 0);
    if (!g_Renderer)
    {
        fprintf(stderr, "ERROR creating SDL renderer.\n");
        return false;
    }

    return true;
}

void RenderColorBuffer()
{
    SDL_UpdateTexture(
        g_ColorBuffer.m_Texture,
        nullptr,
        g_ColorBuffer.m_Buffer,
        static_cast<int>(g_WindowWidth * sizeof(u32)));
    SDL_RenderCopy(
        g_Renderer,
        g_ColorBuffer.m_Texture,
        nullptr,
        nullptr);
}

// NOTE(sbalse): Clear our custom color buffer to the given color.
void ClearColorBuffer(u32 color)
{
    for (int row = 0; row < g_WindowHeight; row++)
    {
        for (int col = 0; col < g_WindowWidth; col++)
        {
            const int pixelIndex = (g_WindowWidth * row) + col;
            g_ColorBuffer.m_Buffer[pixelIndex] = color;
        }
    }
}

void DrawGrid()
{
    for (int y = 0; y < g_WindowHeight; y++)
    {
        for (int x = 0; x < g_WindowWidth; x++)
        {
            if (x % 10 == 0 || y % 10 == 0)
            {
                g_ColorBuffer.m_Buffer[(g_WindowWidth * y) + x] = 0xFF333333;
            }
        }
    }
}

void DrawPixel(int x, int y, u32 color)
{
    if (x < g_WindowWidth && y < g_WindowHeight)
    {
        int position = (g_WindowWidth * y) + x;
        g_ColorBuffer.m_Buffer[position] = color;
    }
}

void DrawRectangle(int x, int y, int width, int height, u32 color)
{
    for (int yy = y; yy < height; yy++)
    {
        for (int xx = x; xx < width; xx++)
        {
            g_ColorBuffer.m_Buffer[(g_WindowWidth * yy) + xx] = color;
        }
    }
}

void DestroyWindow()
{
    std::free(g_ColorBuffer.m_Buffer);
    g_ColorBuffer = {}; // NOTE(sbalse): Reset color buffer

    SDL_DestroyRenderer(g_Renderer);
    SDL_DestroyWindow(g_Window);
    SDL_Quit();
}
