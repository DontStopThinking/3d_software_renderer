#include "display.h"

#include <ctime>
#include <cmath>

#include "log.h"

static constinit SDL_Window* g_Window = nullptr;
static constinit SDL_Renderer* g_Renderer = nullptr;
static constinit ColorBuffer g_ColorBuffer = {};
static constinit ZBuffer g_ZBuffer = {};
static constinit CullMethod g_CullMethod = {};
static constinit RenderMethod g_RenderMethod = {};
static constinit ShadingMethod g_ShadingMethod = {};
static constinit RenderBufferMethod g_RenderBufferMethod = {};

static constinit int g_WindowWidth = 1024;
static constinit int g_WindowHeight = 720;

static void UpdateColorBufferAt(const int x, const int y, const u32 color)
{
    if (x < 0 || x >= g_WindowWidth || y < 0 || y >= g_WindowHeight)
    {
        return;
    }

    const int position = (g_WindowWidth * y) + x;
    if (position > g_ColorBuffer.m_Size)
    {
        return;
    }

    g_ColorBuffer.m_Buffer[position] = color;
}

int GetWindowWidth()
{
    return g_WindowWidth;
}

int GetWindowHeight()
{
    return g_WindowHeight;
}

bool InitializeWindow(Arena* const persistentArena, const char* const windowTitle)
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
        windowTitle,
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
    g_Renderer = SDL_CreateRenderer(g_Window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!g_Renderer)
    {
        LOG_ERROR("Failed to create an SDL renderer.");
        return false;
    }

    // NOTE(sbalse): Allocate the color buffer.
    const size_t colorBufferSize = g_WindowWidth * g_WindowHeight;
    g_ColorBuffer.m_Buffer = PushArray(persistentArena, u32, colorBufferSize);
    g_ColorBuffer.m_Size = colorBufferSize;

    g_ColorBuffer.m_Texture = SDL_CreateTexture(
        g_Renderer,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_STREAMING,
        g_WindowWidth,
        g_WindowHeight);

    // NOTE(sbalse): Allocate the z buffer.
    const size_t zBufferUNormSize = g_WindowWidth * g_WindowHeight;
    g_ZBuffer.m_BufferUNorm = PushArray(persistentArena, float, zBufferUNormSize);
    g_ZBuffer.m_BufferUNormSize = zBufferUNormSize;

    const size_t zBufferUIntSize = g_WindowWidth * g_WindowHeight;
    g_ZBuffer.m_BufferUInt = PushArray(persistentArena, u32, zBufferUIntSize);
    g_ZBuffer.m_BufferUIntSize = zBufferUIntSize;

    g_ZBuffer.m_Texture = SDL_CreateTexture(
        g_Renderer,
        SDL_PIXELFORMAT_RGB888,
        SDL_TEXTUREACCESS_STREAMING,
        g_WindowWidth,
        g_WindowHeight
    );

    LOG_INFO("Successfully initialized window.");

    return true;
}

void RenderColorBuffer()
{
    SDL_UpdateTexture(
        g_ColorBuffer.m_Texture,
        nullptr,
        g_ColorBuffer.m_Buffer,
        scast<int>(g_WindowWidth * sizeof(u32))
    );
    SDL_RenderCopy(
        g_Renderer,
        g_ColorBuffer.m_Texture,
        nullptr,
        nullptr
    );
    SDL_RenderPresent(g_Renderer);
}

void RenderZBuffer()
{
    SDL_UpdateTexture(
        g_ZBuffer.m_Texture,
        nullptr,
        g_ZBuffer.m_BufferUInt,
        scast<int>(g_WindowWidth * sizeof(u32))
    );
    SDL_RenderCopy(
        g_Renderer,
        g_ZBuffer.m_Texture,
        nullptr,
        nullptr
    );
    SDL_RenderPresent(g_Renderer);
}

CullMethod GetCullMethod()
{
    return g_CullMethod;
}

void SetCullMethod(const CullMethod newCullMethod)
{
    g_CullMethod = newCullMethod;
}

void SetRenderMethod(const RenderMethod newRenderMethod)
{
    g_RenderMethod = newRenderMethod;
}

ShadingMethod GetShadingMethod()
{
    return g_ShadingMethod;
}

RenderMethod GetRenderMethod()
{
    return g_RenderMethod;
}

void SetShadingMethod(const ShadingMethod newShadingMethod)
{
    g_ShadingMethod = newShadingMethod;
}

RenderBufferMethod GetRenderBufferMethod()
{
    return g_RenderBufferMethod;
}

void SetRenderBufferMethod(const RenderBufferMethod newRenderBufferMethod)
{
    g_RenderBufferMethod = newRenderBufferMethod;
}

// NOTE(sbalse): Clear our custom color buffer to the given color.
void ClearColorBuffer(const u32 color)
{
    const size_t size = g_ColorBuffer.m_Size;
    for (int i = 0; i < size; i++)
    {
        g_ColorBuffer.m_Buffer[i] = color;
    }
}

void ClearZBuffer()
{
    const size_t size = g_ZBuffer.m_BufferUNormSize;
    for (int i = 0; i < size; i++)
    {
        // NOTE(sbalse): Clear z-buffer to "1". "0" is the near plane and "1" is the far plane.
        // So z-buffer being 1 after clearing means that it is infinitely far away right by
        // default.
        g_ZBuffer.m_BufferUNorm[i] = 1.0;

        // NOTE(sbalse): We represent the far plane with WHITE color. This means, objects that
        // are closer to the camera appear darker and objects far away or no geometry appears as
        // white in the z-buffer visualization.
        g_ZBuffer.m_BufferUInt[i] = WHITE;
    }
}

float GetZBufferAt(const int x, const int y)
{
    if (x < 0 || x >= g_WindowWidth || y < 0 || y > g_WindowHeight)
    {
        return 1.0f;
    }

    const size_t position = scast<size_t>(g_WindowWidth * y) + x;
    if (position >= g_ZBuffer.m_BufferUNormSize)
    {
        return 1.0f;
    }

    return g_ZBuffer.m_BufferUNorm[position];
}

void UpdateNormalizedZBufferAt(const int x, const int y, const float value)
{
    if (x < 0 || x >= g_WindowWidth || y < 0 || y > g_WindowHeight)
    {
        return;
    }

    const size_t position = scast<size_t>(g_WindowWidth * y) + x;
    if (position >= g_ZBuffer.m_BufferUIntSize)
    {
        return;
    }

    g_ZBuffer.m_BufferUNorm[position] = value;
}

void UpdateDisplayableZBufferAt(const int x, const int y, const u32 value)
{
    if (x < 0 || x >= g_WindowWidth || y < 0 || y > g_WindowHeight)
    {
        return;
    }

    const size_t position = scast<size_t>(g_WindowWidth * y) + x;
    if (position >= g_ZBuffer.m_BufferUIntSize)
    {
        return;
    }

    g_ZBuffer.m_BufferUInt[position] = value;
}

void DrawGrid()
{
    for (int y = 0; y < g_WindowHeight; y++)
    {
        for (int x = 0; x < g_WindowWidth; x++)
        {
            if (x % 10 == 0 || y % 10 == 0)
            {
                UpdateColorBufferAt(x, y, GRAY);
            }
        }
    }
}

void DrawPixel(const int x, const int y, const u32 color)
{
    UpdateColorBufferAt(x, y, color);
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

void TakeScreenshot(const char* const fileNamePrefix)
{
    LOG_INFO("Taking screenshot...");

    // NOTE(sbalse): Get current Unix timestamp.
    const u64 currentUnixTime = scast<u64>(std::time(nullptr));

    char fileNameWithTimestamp[128] = {};
    sprintf_s(fileNameWithTimestamp, "%s-%llu.bmp", fileNamePrefix, currentUnixTime);

    int rendererWidth, rendererHeight;
    SDL_GetRendererOutputSize(g_Renderer, &rendererWidth, &rendererHeight);

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
        g_Renderer,
        nullptr,
        SDL_PIXELFORMAT_ARGB8888,
        screenshotSurface->pixels,
        screenshotSurface->pitch
    );
    SDL_SaveBMP(screenshotSurface, fileNameWithTimestamp);
    SDL_FreeSurface(screenshotSurface);

    LOG_INFO("Saved screenshot %s.", fileNameWithTimestamp);
}

void DestroyWindow()
{
    SDL_DestroyTexture(g_ColorBuffer.m_Texture);
    g_ColorBuffer = {};

    SDL_DestroyTexture(g_ZBuffer.m_Texture);
    g_ZBuffer = {};

    SDL_DestroyRenderer(g_Renderer);
    SDL_DestroyWindow(g_Window);
    SDL_Quit();
}
