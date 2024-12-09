#include <cstdio>
#include <SDL.h>

constinit SDL_Window* g_Window = nullptr;
constinit SDL_Renderer* g_Renderer = nullptr;
constinit bool g_IsRunning = false;

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
        800,
        600,
        SDL_WINDOW_RESIZABLE
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

static void Render()
{
    SDL_SetRenderDrawColor(g_Renderer, 255, 0, 0, 255);
    SDL_RenderClear(g_Renderer);

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

    return 0;
}
