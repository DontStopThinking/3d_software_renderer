#include "display.h"

#include <cstdio>
#include <cstdlib>
#include <chrono>

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

    // NOTE(sbalse): Get index of the monitor where the mouse pointer currently is. We will use it to
    // make the window start on that monitor.
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

void DrawTriangle(
    const int x0,
    const int y0,
    const int x1,
    const int y1,
    const int x2,
    const int y2,
    const u32 color)
{
    DrawLine(x0, y0, x1, y1, color);
    DrawLine(x1, y1, x2, y2, color);
    DrawLine(x2, y2, x0, y0, color);
}

void DrawLine(const int x0, const int y0, const int x1, const int y1, const u32 color)
{
    const int deltaX = x1 - x0;
    const int deltaY = y1 - y0;

    const int longestSideLength = std::max(std::abs(deltaX), std::abs(deltaY));

    // NOTE(sbalse): Find how much we should increment in both X and Y each step.
    const float xInc = deltaX / static_cast<float>(longestSideLength);
    const float yInc = deltaY / static_cast<float>(longestSideLength);

    float currentX = static_cast<float>(x0);
    float currentY = static_cast<float>(y0);

    for (int i = 0; i <= longestSideLength; i++)
    {
        DrawPixel(std::lround(currentX), std::lround(currentY), color);
        currentX += xInc;
        currentY += yInc;
    }
}

void TakeScreenshot(SDL_Renderer* renderer, const std::string_view fileNamePrefix)
{
    std::fprintf(stdout, "INFO: Taking screenshot\n");

    // NOTE(sbalse): Get current Unix timestamp.
    using namespace std::chrono;
    const i64 currentUnixTime = duration_cast<seconds>(system_clock::now().time_since_epoch()).count();

    char fileNameWithTimestamp[128] = {};
    sprintf_s(fileNameWithTimestamp, "%s-%lld.bmp", fileNamePrefix.data(), currentUnixTime);

    int rendererWidth, rendererHeight;
    SDL_GetRendererOutputSize(renderer, &rendererWidth, &rendererHeight);

    SDL_Surface* screenshotSurface = SDL_CreateRGBSurface(
        0,
        rendererWidth,
        rendererHeight,
        32,
        0x00FF0000,
        0x0000FF00,
        0x000000FF,
        0xFF000000
    );
    SDL_RenderReadPixels(
        renderer,
        nullptr,
        SDL_PIXELFORMAT_ARGB8888,
        screenshotSurface->pixels,
        screenshotSurface->pitch);
    SDL_SaveBMP(screenshotSurface, fileNameWithTimestamp);
    SDL_FreeSurface(screenshotSurface);

    std::fprintf(stdout, "INFO: Saved screenshot %s\n", fileNameWithTimestamp);
}

void DestroyWindow()
{
    SDL_DestroyRenderer(g_Renderer);
    SDL_DestroyWindow(g_Window);
    SDL_Quit();
}
