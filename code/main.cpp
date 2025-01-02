#include <cstdio>
#include <cstdlib>
#include <string_view>
#include <vector>
#include <SDL.h>

#include "common.h"
#include "vector.h"
#include "display.h"
#include "mesh.h"

constinit bool g_IsRunning = false;
constinit u32 g_PreviousFrameTimeMS = 0u; // NOTE(sbalse): Time taken by the previous frame in milliseconds.

constexpr int FOV_FACTOR = 640;
constexpr Vec3 CAMERA_POSITION = { .m_X = 0, .m_Y = 0, .m_Z = -5 };

constinit std::vector<Triangle> g_TrianglesToRender;

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

    // NOTE(sbalse): Load the cube values in the mesh data structure.
    LoadCubeMeshData();
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
        // NOTE(sbalse): Divide by Z, so the further away something is, the smaller it appears.
        // And conversely, the closer something is, the bigger it appears.
        .m_X = (point.m_X * FOV_FACTOR) / point.m_Z,
        .m_Y = (point.m_Y * FOV_FACTOR) / point.m_Z,
    };

    return projectedPoint;
}

static void Update()
{
    const u32 currentFrameTimeMS = SDL_GetTicks();
    const u32 frameDuration = currentFrameTimeMS - g_PreviousFrameTimeMS;
    g_PreviousFrameTimeMS = currentFrameTimeMS;

    const int timeToWait = static_cast<int>(FRAME_TARGET_TIME_MS) - frameDuration;
    if (timeToWait > 0 && timeToWait <= FRAME_TARGET_TIME_MS)
    {
        SDL_Delay(timeToWait);
    }

    g_Mesh.m_Rotation.m_X += 0.01f;
    g_Mesh.m_Rotation.m_Y += 0.01f;
    g_Mesh.m_Rotation.m_Z += 0.01f;

    // NOTE(sbalse): Loop all triangle faces of our mesh.
    for (const Face& meshFace : g_Mesh.m_Faces)
    {
        // NOTE(sbalse): The 3 vertices that make up a triangle of a face.
        const Vec3 faceVertices[3] =
        {
            // NOTE(sbalse): Need to subtract -1 from face since our faces are 1-indexed and C arrays are 0-indexed.
            CUBE_VERTICES[meshFace.m_A - 1],
            CUBE_VERTICES[meshFace.m_B - 1],
            CUBE_VERTICES[meshFace.m_C - 1]
        };

        Triangle projectedTriangle = {};

        // NOTE(sbalse): Loop all three vertices of this current face and apply transformations
        for (int vertexIndex = 0; vertexIndex < 3; vertexIndex++)
        {
            Vec3 transformedVertex = faceVertices[vertexIndex];

            // NOTE(sbalse): Rotate the mesh along its X, Y and Z axes.
            transformedVertex = Vec3RotateX(transformedVertex, g_Mesh.m_Rotation.m_X);
            transformedVertex = Vec3RotateY(transformedVertex, g_Mesh.m_Rotation.m_Y);
            transformedVertex = Vec3RotateZ(transformedVertex, g_Mesh.m_Rotation.m_Z);

            // NOTE(sbalse): Translate the vertex away from the camera.
            transformedVertex.m_Z -= CAMERA_POSITION.m_Z;

            // NOTE(sbalse): Project the current vertex.
            Vec2 projectedPoint = Project(transformedVertex);

            // NOTE(sbalse): Scale and translate the projected point to the middle of the screen.
            projectedPoint.m_X += (g_WindowWidth / 2.0f);
            projectedPoint.m_Y += (g_WindowHeight / 2.0f);

            projectedTriangle.m_Points[vertexIndex] = projectedPoint;
        }

        // NOTE(sbalse): Save the projected triangle in the array of triangles to render.
        // g_TrianglesToRender[faceIndex] = projectedTriangle;
        g_TrianglesToRender.emplace_back(projectedTriangle);
    }
}

static void Render()
{
    ClearColorBuffer(BLACK);

    // DrawGrid();

    // NOTE(sbalse): Loop all projected triangles and render them.
    for (const Triangle& currentTriangle : g_TrianglesToRender)
    {
        // NOTE(sbalse): Draw the cube corner vertices.
        DrawRectangle(
            static_cast<int>(currentTriangle.m_Points[0].m_X),
            static_cast<int>(currentTriangle.m_Points[0].m_Y),
            3,
            3,
            YELLOW);
        DrawRectangle(
            static_cast<int>(currentTriangle.m_Points[1].m_X),
            static_cast<int>(currentTriangle.m_Points[1].m_Y),
            3,
            3,
            YELLOW);
        DrawRectangle(
            static_cast<int>(currentTriangle.m_Points[2].m_X),
            static_cast<int>(currentTriangle.m_Points[2].m_Y),
            3,
            3,
            YELLOW);

        // NOTE(sbalse): Draw cube faces using triangles.
        DrawTriangle(
            static_cast<int>(currentTriangle.m_Points[0].m_X),
            static_cast<int>(currentTriangle.m_Points[0].m_Y),
            static_cast<int>(currentTriangle.m_Points[1].m_X),
            static_cast<int>(currentTriangle.m_Points[1].m_Y),
            static_cast<int>(currentTriangle.m_Points[2].m_X),
            static_cast<int>(currentTriangle.m_Points[2].m_Y),
            GREEN);
    }

    // NOTE(sbalse): Clear the list of triangles to render every frame loop.
    g_TrianglesToRender.clear();

    RenderColorBuffer();

    SDL_RenderPresent(g_Renderer);
}

// NOTE(sbalse): Free the memory that was dynamically allocated by the program.
static void FreeResources()
{
    std::free(g_ColorBuffer.m_Buffer);
    g_ColorBuffer.m_Size = 0;
    SDL_DestroyTexture(g_ColorBuffer.m_Texture);
}

int main(int argc, char* argv[])
{
#if _DEBUG
    const std::string_view windowTitle = "3D Renderer [DEBUG]";
#else
    const std::string_view windowTitle = "3D Renderer [RELEASE]";
#endif // _DEBUG

    g_IsRunning = InitializeWindow(windowTitle);

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
    FreeResources();

    return EXIT_SUCCESS;
}
