#include <cstdio>
#include <cstdlib>
#include <SDL.h>

#include "common.h"
#include "vector.h"
#include "display.h"

constinit bool g_IsRunning = false;

// NOTE(sbalse): Declare an array of vectors/points
constexpr int NUM_POINTS = 9 * 9 * 9;
constinit Vec3 g_CubePoints[NUM_POINTS] = {}; // NOTE(sbalse): A cube

constinit Vec2 g_ProjectedPoints[NUM_POINTS] = {};

constexpr Vec3 g_CameraPosition = { .m_X = 0, .m_Y = 0, .m_Z = -5 };

constexpr int FOV_FACTOR = 640;

static void Setup()
{
    const size_t size = sizeof(u32) * g_WindowWidth * g_WindowHeight;
    // NOTE(sbalse): Allocate the color buffer.
    g_ColorBuffer.m_Buffer = reinterpret_cast<u32*>(std::malloc(size));
    g_ColorBuffer.m_Size = size;

    g_ColorBuffer.m_Texture = SDL_CreateTexture(
        g_Renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        g_WindowWidth,
        g_WindowHeight);

    int pointIndex = 0;
    // NOTE(sbalse): Start loading our array of vectors. From -1 to 1.
    for (float x = -1; x <= 1; x += 0.25) // NOTE(sbalse): Increment by 0.25 as 2 / 9 ~= 0.25
    {
        for (float y = -1; y <= 1; y += 0.25)
        {
            for (float z = -1; z <= 1; z += 0.25)
            {
                const Vec3 point = { .m_X = x, .m_Y = y, .m_Z = z };
                g_CubePoints[pointIndex] = point;
                ++pointIndex;
            }
        }
    }
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
            // NOTE(sbalse): ESCAPE to quit.
            if (event.key.keysym.sym == SDLK_ESCAPE)
            {
                g_IsRunning = false;
            }
            // NOTE(sbalse): F9 to take a screenshot.
            else if (event.key.keysym.sym == SDLK_F9)
            {
                TakeScreenshot(g_Renderer, "screenshot");
            }
        } break;
        }
    }
}

// NOTE(sbalse): Take a 3D vector and return a 2D point.
static Vec2 Project(const Vec3 point)
{
    const Vec2 projectedPoint =
    {
        // NOTE(sbalse): Divide by Z, so the further away something is the smaller it appears.
        // And conversely, the closer something is, the bigger it appears.
        .m_X = (point.m_X * FOV_FACTOR) / point.m_Z,
        .m_Y = (point.m_Y * FOV_FACTOR) / point.m_Z,
    };

    return projectedPoint;
}

static void Update()
{
    for (int i = 0; i < NUM_POINTS; i++)
    {
        Vec3 point = g_CubePoints[i];

        // NOTE(sbalse): Move the points away from the camera.
        point.m_Z -= g_CameraPosition.m_Z;

        // NOTE(sbalse): Project the current point
        const Vec2 projectedPoint = Project(point);

        // NOTE(sbalse): Save the projected 2D vector in the array of projected points.
        g_ProjectedPoints[i] = projectedPoint;
    }
}

static void Render()
{
    ClearColorBuffer(BLACK);

    //DrawGrid();

    // NOTE(sbalse): Loop all projected points and render them.
    for (int i = 0; i < NUM_POINTS; i++)
    {
        const Vec2 projectedPoint = g_ProjectedPoints[i];

        // NOTE(sbalse): Move the projected point to the center of our window.
        const int translatedX = static_cast<int>(projectedPoint.m_X) + (g_WindowWidth / 2);
        const int translatedY = static_cast<int>(projectedPoint.m_Y) + (g_WindowHeight / 2);
        DrawRectangle(translatedX, translatedY, 4, 4, YELLOW);
    }

    RenderColorBuffer();

    SDL_RenderPresent(g_Renderer);
}

int main(int argc, char* argv[])
{
    g_IsRunning = InitializeWindow("3D Renderer");

    if (!g_IsRunning)
    {
        return EXIT_FAILURE;
    }

    Setup();

    while (g_IsRunning)
    {
        ProcessInput();

        Update();

        Render();
    }

    DestroyWindow();

    return EXIT_SUCCESS;
}
