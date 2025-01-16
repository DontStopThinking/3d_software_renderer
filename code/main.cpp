#include <cstdio>
#include <cstdlib>
#include <string_view>
#include <vector>
#include <algorithm>
#include <SDL.h>
extern "C"
{
    #include <upng.h>
}

#include "log.h"
#include "common.h"
#include "vector.h"
#include "matrix.h"
#include "display.h"
#include "mesh.h"
#include "light.h"
#include "texture.h"
#include "camera.h"

constinit bool g_IsRunning = false;
constinit u32 g_PreviousUpdateTimeMS = 0u; // NOTE(sbalse): Time taken by the previous update in milliseconds.
constinit float g_DeltaTimeSeconds = 0.0f;

constinit std::vector<Triangle> g_TrianglesToRender;

// TODO(sbalse): IMGUI
constinit bool g_Paused = false;
constinit bool g_PrintFPS = false;
constinit bool g_DisplayGrid = false;

constinit Mat4 g_WorldMatrix = {};
constinit Mat4 g_ProjMatrix = {};
constinit Mat4 g_ViewMatrix = {};

constexpr Vec3 CAMERA_TARGET = { 0, 0, 4 };
constexpr Vec3 CAMERA_UP_DIRECTION = { 0, 1, 0 };

static void Setup()
{
    // NOTE(sbalse): Init the render mode, triangle culling method and shading method.
    g_CullMethod = CullMethod::Backface;
    g_RenderMethod = RenderMethod::Textured;
    g_ShadingMethod = ShadingMethod::FlatShading;

    // NOTE(sbalse): Allocate the color buffer.
    const size_t colorBufferSize = sizeof(u32) * g_WindowWidth * g_WindowHeight;
    g_ColorBuffer.m_Buffer = reinterpret_cast<u32*>(std::malloc(colorBufferSize));
    g_ColorBuffer.m_Size = colorBufferSize;

    g_ColorBuffer.m_Texture = SDL_CreateTexture(
        g_Renderer,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_STREAMING,
        g_WindowWidth,
        g_WindowHeight);

    // NOTE(sbalse): Allocate the z buffer.
    const size_t zBufferUNormSize = sizeof(float) * g_WindowWidth * g_WindowHeight;
    g_ZBuffer.m_BufferUNorm = reinterpret_cast<float*>(std::malloc(zBufferUNormSize));
    g_ZBuffer.m_BufferUNormSize = zBufferUNormSize;

    const size_t zBufferUIntSize = sizeof(u32) * g_WindowWidth * g_WindowHeight;
    g_ZBuffer.m_BufferUInt = reinterpret_cast<u32*>(std::malloc(zBufferUIntSize));
    g_ZBuffer.m_BufferUIntSize = zBufferUIntSize;

    g_ZBuffer.m_Texture = SDL_CreateTexture(
        g_Renderer,
        SDL_PIXELFORMAT_RGB888,
        SDL_TEXTUREACCESS_STREAMING,
        g_WindowWidth,
        g_WindowHeight
    );

    // NOTE(sbalse): Init the perspective projection matrix.
    constexpr float fovRadians = M_PI / 3.0f; // NOTE(sbalse): Same as 180 / 3 or 60 deg.
    const float aspect = static_cast<float>(g_WindowHeight) / g_WindowWidth;
    constexpr float znear = 0.1f;
    constexpr float zfar = 100.0f;
    g_ProjMatrix = Mat4MakePerspective(fovRadians, aspect, znear, zfar);

    // NOTE(sbalse): Load the cube values in the mesh data structure.
    // LoadCubeMeshData();

    LoadObjFileData("assets/efa.obj");
    LoadPNGTextureData("assets/efa.png");

    g_TrianglesToRender.reserve(g_Mesh.m_Faces.size());
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
            // NOTE(sbalse): p to pause mesh rotation.
            else if (event.key.keysym.sym == SDLK_p)
            {
                g_Paused = !g_Paused;
                if (g_Paused)
                {
                    LOG_INFO("Paused: true.");
                }
                else
                {
                    LOG_INFO("Paused: false.");
                }
            }
            // NOTE(sbalse): f to toggle printing of FPS.
            else if (event.key.keysym.sym == SDLK_f)
            {
                g_PrintFPS = !g_PrintFPS;
                if (g_PrintFPS)
                {
                    LOG_INFO("Printing FPS.");
                }
                else
                {
                    LOG_INFO("Stopped printing of FPS.");
                }
            }
            else if (event.key.keysym.sym == SDLK_g)
            {
                g_DisplayGrid = !g_DisplayGrid;
                if (g_DisplayGrid)
                {
                    LOG_INFO("Displaying grid.");
                }
                else
                {
                    LOG_INFO("Stopping display of grid.");
                }
            }
            else if (event.key.keysym.sym == SDLK_z)
            {
                if (g_RenderBufferMethod == RenderBufferMethod::ColorBuffer)
                {
                    g_RenderBufferMethod = RenderBufferMethod::ZBuffer;
                    LOG_INFO("Change RenderBufferMethod to \"ZBuffer\"");
                }
                else if (g_RenderBufferMethod == RenderBufferMethod::ZBuffer)
                {
                    g_RenderBufferMethod = RenderBufferMethod::ColorBuffer;
                    LOG_INFO("Change RenderBufferMethod to \"ColorBuffer\"");
                }
            }
            // NOTE(sbalse): 1 to draw wireframe and vertices.
            else if (event.key.keysym.sym == SDLK_1)
            {
                g_RenderMethod = RenderMethod::WireVertex;
                LOG_INFO("Set render method to \"WireVertex\".");
            }
            // NOTE(sbalse): 2 to display ONLY wireframe.
            else if (event.key.keysym.sym == SDLK_2)
            {
                g_RenderMethod = RenderMethod::Wire;
                LOG_INFO("Set render method to \"Wire\".");
            }
            // NOTE(sbalse): 3 to draw filled triangles.
            else if (event.key.keysym.sym == SDLK_3)
            {
                g_RenderMethod = RenderMethod::FillTriangle;
                LOG_INFO("Set render method to \"FillTriangle\".");
            }
            // NOTE(sbalse): 4 to display both filled and wireframe.
            else if (event.key.keysym.sym == SDLK_4)
            {
                g_RenderMethod = RenderMethod::FillTriangleWire;
                LOG_INFO("Set render method to \"FillTriangleWire\".");
            }
            // NOTE(sbalse): 5 to enable flat shading.
            else if (event.key.keysym.sym == SDLK_5)
            {
                g_ShadingMethod = ShadingMethod::FlatShading;
                LOG_INFO("Set shading method to \"FlatShading\".");
            }
            // NOTE(sbalse): 6 to disable shading.
            else if (event.key.keysym.sym == SDLK_6)
            {
                g_ShadingMethod = ShadingMethod::None;
                LOG_INFO("Set shading method to \"None\".");
            }
            // NOTE(sbalse): 7 to display textured.
            else if (event.key.keysym.sym == SDLK_7)
            {
                g_RenderMethod = RenderMethod::Textured;
                LOG_INFO("Set render method to \"Textured\".");
            }
            // NOTE(sbalse): 8 to display textured and wired.
            else if (event.key.keysym.sym == SDLK_8)
            {
                g_RenderMethod = RenderMethod::WireTextured;
                LOG_INFO("Set render method to \"WireTextured\".");
            }
            // NOTE(sbalse): c to enable backface culling.
            else if (event.key.keysym.sym == SDLK_c)
            {
                g_CullMethod = CullMethod::Backface;
                LOG_INFO("Set cull method to \"Backface\".");
            }
            // NOTE(sbalse): d to disable backface culling.
            else if (event.key.keysym.sym == SDLK_d)
            {
                g_CullMethod = CullMethod::None;
                LOG_INFO("Set cull method to \"None\".");
            }
        } break;
        }
    }
}

static void Update()
{
    const u32 currentUpdateTimeMS = SDL_GetTicks();
    const u32 frameDuration = currentUpdateTimeMS - g_PreviousUpdateTimeMS;

    g_DeltaTimeSeconds = frameDuration / 1000.0f;
    g_PreviousUpdateTimeMS = currentUpdateTimeMS;

    const int timeToWait = static_cast<int>(FRAME_TARGET_TIME_MS) - frameDuration;
    if (timeToWait > 0 && timeToWait <= FRAME_TARGET_TIME_MS)
    {
        SDL_Delay(timeToWait);
    }

    if (!g_Paused)
    {
        g_Mesh.m_Rotation.m_X += 0.6f * g_DeltaTimeSeconds;
        g_Mesh.m_Rotation.m_Y += 0.6f * g_DeltaTimeSeconds;
        g_Mesh.m_Rotation.m_Z += 0.6f * g_DeltaTimeSeconds;

        // g_Mesh.m_Scale.m_X += 0.2 * g_DeltaTimeSeconds;
        // g_Mesh.m_Scale.m_Y += 0.2 * g_DeltaTimeSeconds;
        // g_Mesh.m_Scale.m_Z += 0.2 * g_DeltaTimeSeconds;

        // g_Mesh.m_Translation.m_X += 0.6 * g_DeltaTimeSeconds;

        // NOTE(sbalse): Move away from the camera.
        g_Mesh.m_Translation.m_Z = 5.0f;

        // NOTE(sbalse): Change the camera position per animation frame.
        // g_Camera.m_Position.m_X += 0.6f * g_DeltaTimeSeconds;
        // g_Camera.m_Position.m_Y += 0.6f * g_DeltaTimeSeconds;
    }

    // NOTE(sbalse): Create scale, translation, and rotation matrices that will be
    // multiplied with our mesh vertices.
    const Mat4 scaleMatrix = Mat4MakeScale(
        g_Mesh.m_Scale.m_X,
        g_Mesh.m_Scale.m_Y,
        g_Mesh.m_Scale.m_Z);
    const Mat4 translationMatrix = Mat4MakeTranslation(
        g_Mesh.m_Translation.m_X,
        g_Mesh.m_Translation.m_Y,
        g_Mesh.m_Translation.m_Z);
    const Mat4 rotationMatrixX = Mat4MakeRotationX(g_Mesh.m_Rotation.m_X);
    const Mat4 rotationMatrixY = Mat4MakeRotationY(g_Mesh.m_Rotation.m_Y);
    const Mat4 rotationMatrixZ = Mat4MakeRotationZ(g_Mesh.m_Rotation.m_Z);

    // NOTE(sbalse): Create a "World Matrix" combining scale, rotation and translation
    // matrices of the mesh.
    g_WorldMatrix = MAT4_IDENTITY;
    g_WorldMatrix = Mat4MulMat4(scaleMatrix, g_WorldMatrix);
    g_WorldMatrix = Mat4MulMat4(rotationMatrixX, g_WorldMatrix);
    g_WorldMatrix = Mat4MulMat4(rotationMatrixY, g_WorldMatrix);
    g_WorldMatrix = Mat4MulMat4(rotationMatrixZ, g_WorldMatrix);
    g_WorldMatrix = Mat4MulMat4(translationMatrix, g_WorldMatrix);

    // NOTE(sbalse): Create the camera view matrix looking at a hardcoded target point.
    g_ViewMatrix = Mat4LookAt(g_Camera.m_Position, CAMERA_TARGET, CAMERA_UP_DIRECTION);

    // NOTE(sbalse): Loop all faces of our mesh.
    for (const Face& meshFace : g_Mesh.m_Faces)
    {
        // NOTE(sbalse): The 3 vertices that make up a triangle of a face.
        const Vec3 faceVertices[3] =
        {
            g_Mesh.m_Vertices[meshFace.m_A],
            g_Mesh.m_Vertices[meshFace.m_B],
            g_Mesh.m_Vertices[meshFace.m_C],
        };

        Vec4 transformedVertices[3] = {};

        // NOTE(sbalse): Transform vertices of the face.
        for (int vertexIndex = 0; vertexIndex < 3; vertexIndex++)
        {
            Vec4 transformedVertex = Vec4FromVec3(faceVertices[vertexIndex]);

            // NOTE(sbalse): Transform our vertex by multiplying it with our world matrix.
            transformedVertex = Mat4MulVec4(g_WorldMatrix, transformedVertex);

            // NOTE(sbalse): Multiply the view matrix by the original vector to transform
            // our scene to camera space.
            transformedVertex = Mat4MulVec4(g_ViewMatrix, transformedVertex);

            // NOTE(sbalse): Save the transformed vertex.
            transformedVertices[vertexIndex] = transformedVertex;
        }

        const Vec3 vectorA = Vec3FromVec4(transformedVertices[0]); /*   A   */
        const Vec3 vectorB = Vec3FromVec4(transformedVertices[1]); /*  / \  */
        const Vec3 vectorC = Vec3FromVec4(transformedVertices[2]); /* C---B */

        Vec3 vectorAToB = Vec3Sub(vectorB, vectorA); // NOTE(sbalse): Get vector A to B.
        Vec3Normalize(&vectorAToB);
        Vec3 vectorAToC = Vec3Sub(vectorC, vectorA); // NOTE(sbalse): Get vector A to C.
        Vec3Normalize(&vectorAToC);

        // NOTE(sbalse): Get the face normal using cross-product.
        // NOTE(sbalse): We're using LEFT-HANDED co-ordinate system, so cross product should be
        // calculated using (AB, AC). In right-handed system, it would have been using (AC, AB);
        Vec3 normal = Vec3Cross(vectorAToB, vectorAToC);
        Vec3Normalize(&normal);

        // NOTE(sbalse): Find the vector between a point in the triangle and the camera origin.
        // We directly use ORIGIN here [i.e. (0, 0, 0)] since our camera is always meant to be the
        // origin of our world.
        const Vec3 cameraRay = Vec3Sub(ORIGIN, vectorA);

        // NOTE(sbalse): Use dot product to calculate how aligned the face normal is with the camera
        // ray.
        const float cameraAlignmentWithFaceNormal = Vec3Dot(normal, cameraRay);

        // NOTE(sbalse): Do backface culling.
        if (g_CullMethod == CullMethod::Backface)
        {
            // NOTE(sbalse): If the normal is not visible to the camera then don't draw this face. AKA,
            // cull this face.
            if (cameraAlignmentWithFaceNormal < 0)
            {
                continue;
            }
        }

        Vec4 projectedPoints[3] = {};

        // NOTE(sbalse): Project vertices of the face.
        for (int vertexIndex = 0; vertexIndex < 3; vertexIndex++)
        {
            // NOTE(sbalse): Project the current vertex.
            projectedPoints[vertexIndex] = Mat4MulVec4Project(
                g_ProjMatrix,
                transformedVertices[vertexIndex]);

            // NOTE(sbalse): Invert the y values to account for our flipped y axis.
            projectedPoints[vertexIndex].m_Y *= -1;

            // NOTE(sbalse): Scale into the view.
            projectedPoints[vertexIndex].m_X *= (g_WindowWidth / 2.0f);
            projectedPoints[vertexIndex].m_Y *= (g_WindowHeight / 2.0f);

            // NOTE(sbalse): Translate the projected point to the middle of the screen.
            projectedPoints[vertexIndex].m_X += (g_WindowWidth / 2.0f);
            projectedPoints[vertexIndex].m_Y += (g_WindowHeight / 2.0f);
        }

        u32 triangleColor = meshFace.m_Color;

        if (g_ShadingMethod == ShadingMethod::FlatShading)
        {
            // NOTE(sbalse): Calculate the shade intensity based on how aligned is the face normal and
            // the inverse of the light ray.
            const float lightIntensityFactor = -Vec3Dot(normal, g_Light.m_Direction);

            // NOTE(sbalse): Calculate triangle color based on the light angle.
            triangleColor = LightApplyIntensity(meshFace.m_Color, lightIntensityFactor);
        }

        const Triangle projectedTriangle =
        {
            .m_Points =
            {
                {
                    projectedPoints[0].m_X,
                    projectedPoints[0].m_Y,
                    projectedPoints[0].m_Z,
                    projectedPoints[0].m_W
                },
                {
                    projectedPoints[1].m_X,
                    projectedPoints[1].m_Y,
                    projectedPoints[1].m_Z,
                    projectedPoints[1].m_W
                },
                {
                    projectedPoints[2].m_X,
                    projectedPoints[2].m_Y,
                    projectedPoints[2].m_Z,
                    projectedPoints[2].m_W
                },
            },
            .m_TexCoords =
            {
                { meshFace.m_AUV },
                { meshFace.m_BUV },
                { meshFace.m_CUV },
            },
            .m_Color = triangleColor,
        };

        // NOTE(sbalse): Save the projected triangle in the array of triangles to render.
        g_TrianglesToRender.emplace_back(projectedTriangle);
    }
}

static void Render()
{
    ClearColorBuffer(BLACK);
    ClearZBuffer();

    if (g_DisplayGrid)
    {
        DrawGrid();
    }

    // NOTE(sbalse): Loop all projected triangles and render them.
    for (const Triangle& currentTriangle : g_TrianglesToRender)
    {
        if (g_RenderMethod == RenderMethod::FillTriangle
            || g_RenderMethod == RenderMethod::FillTriangleWire)
        {
            // NOTE(sbalse): Draw mesh face triangles.
            DrawFilledTriangle(
                // NOTE(sbalse): vertex A.
                static_cast<int>(currentTriangle.m_Points[0].m_X),
                static_cast<int>(currentTriangle.m_Points[0].m_Y),
                currentTriangle.m_Points[0].m_Z,
                currentTriangle.m_Points[0].m_W,
                // NOTE(sbalse): vertex B.
                static_cast<int>(currentTriangle.m_Points[1].m_X),
                static_cast<int>(currentTriangle.m_Points[1].m_Y),
                currentTriangle.m_Points[1].m_Z,
                currentTriangle.m_Points[1].m_W,
                // NOTE(sbalse): vertex C.
                static_cast<int>(currentTriangle.m_Points[2].m_X),
                static_cast<int>(currentTriangle.m_Points[2].m_Y),
                currentTriangle.m_Points[2].m_Z,
                currentTriangle.m_Points[2].m_W,
                // NOTE(sbalse): The color.
                currentTriangle.m_Color);
        }

        if (g_RenderMethod == RenderMethod::WireVertex)
        {
            // NOTE(sbalse): Draw the cube corner vertices.
            DrawRectangle(
                static_cast<int>(currentTriangle.m_Points[0].m_X - 3),
                static_cast<int>(currentTriangle.m_Points[0].m_Y - 3),
                6,
                6,
                RED);
            DrawRectangle(
                static_cast<int>(currentTriangle.m_Points[1].m_X - 3),
                static_cast<int>(currentTriangle.m_Points[1].m_Y - 3),
                6,
                6,
                RED);
            DrawRectangle(
                static_cast<int>(currentTriangle.m_Points[2].m_X - 3),
                static_cast<int>(currentTriangle.m_Points[2].m_Y - 3),
                6,
                6,
                RED);
        }

        // NOTE(sbalse): Draw textured triangle.
        if (g_RenderMethod == RenderMethod::Textured
            || g_RenderMethod == RenderMethod::WireTextured)
        {
            DrawTexturedTriangle(
                // NOTE(sbalse): vertex A.
                static_cast<int>(currentTriangle.m_Points[0].m_X),
                static_cast<int>(currentTriangle.m_Points[0].m_Y),
                currentTriangle.m_Points[0].m_Z,
                currentTriangle.m_Points[0].m_W,
                currentTriangle.m_TexCoords[0].m_U,
                currentTriangle.m_TexCoords[0].m_V,
                // NOTE(sbalse): vertex B.
                static_cast<int>(currentTriangle.m_Points[1].m_X),
                static_cast<int>(currentTriangle.m_Points[1].m_Y),
                currentTriangle.m_Points[1].m_Z,
                currentTriangle.m_Points[1].m_W,
                currentTriangle.m_TexCoords[1].m_U,
                currentTriangle.m_TexCoords[1].m_V,
                // NOTE(sbalse): vertex C.
                static_cast<int>(currentTriangle.m_Points[2].m_X),
                static_cast<int>(currentTriangle.m_Points[2].m_Y),
                currentTriangle.m_Points[2].m_Z,
                currentTriangle.m_Points[2].m_W,
                currentTriangle.m_TexCoords[2].m_U,
                currentTriangle.m_TexCoords[2].m_V,
                // NOTE(sbalse): The texture.
                g_MeshTexture);
        }

        if (g_RenderMethod == RenderMethod::Wire
            || g_RenderMethod == RenderMethod::WireVertex
            || g_RenderMethod == RenderMethod::FillTriangleWire
            || g_RenderMethod == RenderMethod::WireTextured)
        {
            // NOTE(sbalse): Draw mesh wireframe triangles.
            DrawTriangle(
                static_cast<int>(currentTriangle.m_Points[0].m_X),
                static_cast<int>(currentTriangle.m_Points[0].m_Y),
                static_cast<int>(currentTriangle.m_Points[1].m_X),
                static_cast<int>(currentTriangle.m_Points[1].m_Y),
                static_cast<int>(currentTriangle.m_Points[2].m_X),
                static_cast<int>(currentTriangle.m_Points[2].m_Y),
                WHITE);
        }
    }

    // NOTE(sbalse): Clear the list of triangles to render every frame loop.
    g_TrianglesToRender.clear();

    if (g_RenderBufferMethod == RenderBufferMethod::ColorBuffer)
    {
        RenderColorBuffer();
    }
    else if (g_RenderBufferMethod == RenderBufferMethod::ZBuffer)
    {
        RenderZBuffer();
    }

    SDL_RenderPresent(g_Renderer);
}

// NOTE(sbalse): Free the memory that was dynamically allocated by the program.
static void FreeResources()
{
    std::free(g_ColorBuffer.m_Buffer);
    SDL_DestroyTexture(g_ColorBuffer.m_Texture);
    g_ColorBuffer = {};

    std::free(g_ZBuffer.m_BufferUNorm);
    std::free(g_ZBuffer.m_BufferUInt);
    SDL_DestroyTexture(g_ZBuffer.m_Texture);
    g_ZBuffer = {};

    upng_free(g_PNGTexture);
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

    u32 prevTime = SDL_GetTicks();
    u32 printTime = prevTime;
    int numFrames = 0;

    while (g_IsRunning)
    {
        const u32 currTime = SDL_GetTicks();

        ProcessInput();

        Update();

        Render();

        // NOTE(sbalse): Print the FPS and average frame time.
        if (g_PrintFPS)
        {
            numFrames += 1;
            if (currTime - printTime >= 1000) // NOTE(sbalse): If at least 1 second has elapsed.
            {
                const int timeElapsed = currTime - printTime;
                const int avgFrameTime = timeElapsed / numFrames;
                LOG_INFO("fps: %3d, avg frame time: %3d ms", numFrames, avgFrameTime);
                numFrames = 0;
                printTime = currTime;
            }
            prevTime = currTime;
        }
    }

    LOG_INFO("Shutting down...");

    DestroyWindow();
    FreeResources();

    return EXIT_SUCCESS;
}
