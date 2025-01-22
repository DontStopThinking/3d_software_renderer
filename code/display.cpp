#include "display.h"

#include <cstdlib>
#include <ctime>
#include <cmath>

#include "log.h"

constinit SDL_Window* g_Window = nullptr;
constinit SDL_Renderer* g_Renderer = nullptr;
constinit ColorBuffer g_ColorBuffer = {};
constinit ZBuffer g_ZBuffer = {};
constinit CullMethod g_CullMethod = {};
constinit RenderMethod g_RenderMethod = {};
constinit ShadingMethod g_ShadingMethod = {};
constinit RenderBufferMethod g_RenderBufferMethod = {};

constinit int g_WindowWidth = 1024;
constinit int g_WindowHeight = 720;

bool InitializeWindow(const std::string_view windowTitle)
{
    LOG_INFO("Initializing window...");

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        LOG_ERROR("Failed to initialize SDL.");
        return false;
    }

    // NOTE(sbalse): Get index of the monitor where the mouse pointer currently is. We will use it
    // to open our window on that monitor.
    SDL_Point mousePos = {};
    SDL_GetGlobalMouseState(&mousePos.x, &mousePos.y);
    int displayIndex = SDL_GetPointDisplayIndex(&mousePos);

    // NOTE(sbalse): Create an SDL window.
    g_Window = SDL_CreateWindow(
        windowTitle.data(),
        SDL_WINDOWPOS_CENTERED_DISPLAY(displayIndex),
        SDL_WINDOWPOS_CENTERED_DISPLAY(displayIndex),
        g_WindowWidth,
        g_WindowHeight,
        SDL_WINDOW_RESIZABLE
    );
    if (!g_Window)
    {
        LOG_ERROR("Failed to create an SDL window.");
        return false;
    }

    // NOTE(sbalse): Create a SDL renderer.
    g_Renderer = SDL_CreateRenderer(g_Window, -1, 0);
    if (!g_Renderer)
    {
        LOG_ERROR("Failed to create an SDL renderer.");
        return false;
    }

    LOG_INFO("Successfully initialized window.");

    return true;
}

void RenderColorBuffer()
{
    SDL_UpdateTexture(
        g_ColorBuffer.m_Texture,
        nullptr,
        g_ColorBuffer.m_Buffer,
        scast<int>(g_WindowWidth * sizeof(u32)));
    SDL_RenderCopy(
        g_Renderer,
        g_ColorBuffer.m_Texture,
        nullptr,
        nullptr);
}

void RenderZBuffer()
{
    SDL_UpdateTexture(
        g_ZBuffer.m_Texture,
        nullptr,
        g_ZBuffer.m_BufferUInt,
        scast<int>(g_WindowWidth * sizeof(u32)));
    SDL_RenderCopy(
        g_Renderer,
        g_ZBuffer.m_Texture,
        nullptr,
        nullptr);
}

// NOTE(sbalse): Clear our custom color buffer to the given color.
void ClearColorBuffer(const u32 color)
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

void ClearZBuffer()
{
    for (int row = 0; row < g_WindowHeight; row++)
    {
        for (int col = 0; col < g_WindowWidth; col++)
        {
            const int pixelIndex = (g_WindowWidth * row) + col;
            // NOTE(sbalse): Clear z-buffer to "1". "0" is the near plane and "1" is the far plane.
            // So z-buffer being 1 after clearing means that it is infinitely far away right by
            // default.
            g_ZBuffer.m_BufferUNorm[pixelIndex] = 1.0;

            // NOTE(sbalse): We represent the far plane with WHITE color. This means, objects that
            // are closer to the camera appear darker and objects far away or no geometry appears as
            // white in the z-buffer visualization.
            g_ZBuffer.m_BufferUInt[pixelIndex] = WHITE;
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
                g_ColorBuffer.m_Buffer[(g_WindowWidth * y) + x] = GRAY;
            }
        }
    }
}

void DrawPixel(const int x, const int y, const u32 color)
{
    if (x >= 0 && y >= 0 && x < g_WindowWidth && y < g_WindowHeight)
    {
        int position = (g_WindowWidth * y) + x;
        g_ColorBuffer.m_Buffer[position] = color;
    }
}

void DrawRectangle(
    const int x,
    const int y,
    const int width,
    const int height,
    const u32 color)
{
    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j++)
        {
            const int currentX = x + i;
            const int currentY = y + j;
            DrawPixel(currentX, currentY, color);
        }
    }
}

void DrawLine(const int x0, const int y0, const int x1, const int y1, const u32 color)
{
    const int deltaX = x1 - x0;
    const int deltaY = y1 - y0;

    const int longestSideLength = std::max(std::abs(deltaX), std::abs(deltaY));

    // NOTE(sbalse): Find how much we should increment in both X and Y each step.
    const float xInc = deltaX / scast<float>(longestSideLength);
    const float yInc = deltaY / scast<float>(longestSideLength);

    float currentX = scast<float>(x0);
    float currentY = scast<float>(y0);

    for (int i = 0; i <= longestSideLength; i++)
    {
        DrawPixel(std::lround(currentX), std::lround(currentY), color);
        currentX += xInc;
        currentY += yInc;
    }
}

void TakeScreenshot(SDL_Renderer* renderer, const std::string_view fileNamePrefix)
{
    LOG_INFO("Taking screenshot...");

    // NOTE(sbalse): Get current Unix timestamp.
    const u64 currentUnixTime = scast<u64>(std::time(nullptr));

    char fileNameWithTimestamp[128] = {};
    sprintf_s(fileNameWithTimestamp, "%s-%llu.bmp", fileNamePrefix.data(), currentUnixTime);

    int rendererWidth, rendererHeight;
    SDL_GetRendererOutputSize(renderer, &rendererWidth, &rendererHeight);

    SDL_Surface* screenshotSurface = SDL_CreateRGBSurface(
        0,
        rendererWidth,
        rendererHeight,
        32,
        RED_MASK,
        GREEN_MASK,
        BLUE_MASK,
        ALPHA_MASK
    );
    SDL_RenderReadPixels(
        renderer,
        nullptr,
        SDL_PIXELFORMAT_ARGB8888,
        screenshotSurface->pixels,
        screenshotSurface->pitch);
    SDL_SaveBMP(screenshotSurface, fileNameWithTimestamp);
    SDL_FreeSurface(screenshotSurface);

    LOG_INFO("Saved screenshot %s.", fileNameWithTimestamp);
}

void DestroyWindow()
{
    SDL_DestroyRenderer(g_Renderer);
    SDL_DestroyWindow(g_Window);
    SDL_Quit();
}
