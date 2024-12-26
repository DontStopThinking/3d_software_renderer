#include <cstdio>
#include <cstdlib>
#include <SDL.h>

#include "common.h"
#include "display.h"

constinit bool g_IsRunning = false;

static void Setup()
{
    size_t size = sizeof(u32) * g_WindowWidth * g_WindowHeight;
    // NOTE(sbalse): Allocate the color buffer.
    g_ColorBuffer.m_Buffer = reinterpret_cast<u32*>(std::malloc(size));
    g_ColorBuffer.m_Size = size;

    g_ColorBuffer.m_Texture = SDL_CreateTexture(
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
            // NOTE(sbalse): ESCAPE to quit
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
