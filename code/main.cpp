#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <SDL.h>

constinit bool g_IsRunning = false;

constinit SDL_Window* g_Window = nullptr;
constinit SDL_Renderer* g_Renderer = nullptr;

struct ColorBuffer
{
    uint32_t* m_Buffer;
    size_t m_Size;
} g_ColorBuffer;
constinit SDL_Texture* g_ColorBufferTexture = nullptr;

constinit int g_WindowWidth = 800;
constinit int g_WindowHeight = 600;

static bool InitializeWindow()
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        std::fprintf(stderr, "ERROR initializing SDL.\n");
        return false;
    }

    // Create an SDL window.
    SDL_Window* g_Window = SDL_CreateWindow(
        nullptr,
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

    // Create a SDL renderer.
    g_Renderer = SDL_CreateRenderer(g_Window, -1, 0);
    if (!g_Renderer)
    {
        fprintf(stderr, "ERROR creating SDL renderer.\n");
        return false;
    }

    return true;
}

static void Setup()
{
    size_t size = sizeof(uint32_t) * g_WindowWidth * g_WindowHeight;
    g_ColorBuffer.m_Buffer = (uint32_t*)std::malloc(size);
    g_ColorBuffer.m_Size = size;

    g_ColorBufferTexture = SDL_CreateTexture(
        g_Renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        g_WindowWidth,
        g_WindowHeight);
}

static void ProcessInput()
{
    SDL_Event event = {};

    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
        {
            g_IsRunning = false;
        } break;

        case SDL_KEYDOWN:
        {
            if (event.key.keysym.sym == SDLK_ESCAPE)
            {
                g_IsRunning = false;
            }
        } break;
        }
    }
}

static void Update()
{
}

static void RenderColorBuffer()
{
    SDL_UpdateTexture(
        g_ColorBufferTexture,
        nullptr,
        g_ColorBuffer.m_Buffer,
        (int)(g_WindowWidth * sizeof(uint32_t)));
    SDL_RenderCopy(
        g_Renderer,
        g_ColorBufferTexture,
        nullptr,
        nullptr);
}

// Clear our custom color buffer to the given color.
static void ClearColorBuffer(uint32_t color)
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

static void DrawGrid()
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

static void DrawRectangle(int x, int y, int width, int height, uint32_t color)
{
    for (int yy = y; yy < height; yy++)
    {
        for (int xx = x; xx < width; xx++)
        {
            g_ColorBuffer.m_Buffer[(g_WindowWidth * yy) + xx] = color;
        }
    }
}

static void Render()
{
    SDL_SetRenderDrawColor(g_Renderer, 0, 0, 0, 255);
    SDL_RenderClear(g_Renderer);

    ClearColorBuffer(0xFF000000);

    DrawGrid();

    DrawRectangle(100, 100, 300, 150, 0xFFFF00FF);

    RenderColorBuffer();

    SDL_RenderPresent(g_Renderer);
}

static void DestroyWindow()
{
    std::free(g_ColorBuffer.m_Buffer);
    g_ColorBuffer = {};

    SDL_DestroyRenderer(g_Renderer);
    SDL_DestroyWindow(g_Window);
    SDL_Quit();
}

int main(int argc, char* argv[])
{
    g_IsRunning = InitializeWindow();

    Setup();

    while (g_IsRunning)
    {
        ProcessInput();

        Update();

        Render();
    }

    DestroyWindow();

    return 0;
}
