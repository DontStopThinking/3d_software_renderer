#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <string_view>
#include <array>
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
#include "clipping.h"
#include "triangle.h"

// NOTE(sbalse): "g_" prefix = file static variables.
static constinit bool g_IsRunning = false;
static constinit u32 g_PreviousUpdateTimeMS = 0u; // NOTE(sbalse): Time taken by the previous update in milliseconds.
static constinit float g_DeltaTimeSeconds = 0.0f;

inline constexpr size_t MAX_NUM_TRIANGLES_TO_RENDER = 10000;
static constinit size_t g_NumTrianglesToRender = 0;
static constinit std::array<Triangle, MAX_NUM_TRIANGLES_TO_RENDER> g_TrianglesToRender = {};

// TODO(sbalse): IMGUI
static constinit bool g_Paused = false;
static constinit bool g_PrintFPS = false;
static constinit bool g_DisplayGrid = false;

static constinit Mat4 g_WorldMatrix = {};
static constinit Mat4 g_ProjMatrix = {};
static constinit Mat4 g_ViewMatrix = {};

inline constexpr Vec3 CAMERA_UP_DIRECTION = { 0, 1, 0 };

static void Setup()
{
    SDL_SetRelativeMouseMode(SDL_TRUE);

    // NOTE(sbalse): Init the render mode, triangle culling method and shading method.
    SetCullMethod(CullMethod::Backface);
    SetRenderMethod(RenderMethod::Textured);
    SetShadingMethod(ShadingMethod::FlatShading);

    // NOTE(sbalse): Init the light direction. Z = 1 means light goes from camera into the screen.
    InitLight(Vec3{ .m_Z = 1 });

    // NOTE(sbalse): Init the camera
    InitCamera(Vec3{ 0, 0, 0 }, Vec3{ 0, 0, 1 });

    // NOTE(sbalse): Init the perspective projection matrix.
    const float aspectX = scast<float>(GetWindowWidth()) / scast<float>(GetWindowHeight());
    const float aspectY = scast<float>(GetWindowHeight()) / scast<float>(GetWindowWidth());
    constexpr float FOV_Y_RADIANS = M_PI / 3.0f; // NOTE(sbalse): Vertical FOV. Same as 180 / 3 or 60 deg.
    const float fovXRadians = std::atanf(std::tanf(FOV_Y_RADIANS / 2.0f) * aspectX) * 2.0f;
    constexpr float Z_NEAR = 0.3f;
    constexpr float Z_FAR = 20.0f;
    g_ProjMatrix = Mat4MakePerspective(FOV_Y_RADIANS, aspectY, Z_NEAR, Z_FAR);

    // NOTE(sbalse): Initialize clipping frustum planes with a point and a normal.
    InitFrustumPlanes(fovXRadians, FOV_Y_RADIANS, Z_NEAR, Z_FAR);

    LoadMesh("assets/runway.obj", "assets/runway.png", Vec3{ 1, 1, 1 }, Vec3{ 0, -1.5, +23 }, Vec3{});
    LoadMesh("assets/f22.obj", "assets/f22.png", Vec3{ 1, 1, 1 }, Vec3{ 0, -1.3, +5 }, Vec3{ 0, -M_PI / 2, 0 });
    LoadMesh("assets/efa.obj", "assets/efa.png", Vec3{ 1, 1, 1 }, Vec3{ -2, -1.3, +9 }, Vec3{ 0, -M_PI / 2, 0 });
    LoadMesh("assets/f117.obj", "assets/f117.png", Vec3{ 1, 1, 1 }, Vec3{ +2, -1.3, +9 }, Vec3{ 0, -M_PI / 2, 0 });
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
                TakeScreenshot("screenshot");
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
                if (GetRenderBufferMethod() == RenderBufferMethod::ColorBuffer)
                {
                    SetRenderBufferMethod(RenderBufferMethod::ZBuffer);
                    LOG_INFO("Change RenderBufferMethod to \"ZBuffer\"");
                }
                else if (GetRenderBufferMethod() == RenderBufferMethod::ZBuffer)
                {
                    SetRenderBufferMethod(RenderBufferMethod::ColorBuffer);
                    LOG_INFO("Change RenderBufferMethod to \"ColorBuffer\"");
                }
            }
            // NOTE(sbalse): 1 to draw wireframe and vertices.
            else if (event.key.keysym.sym == SDLK_1)
            {
                SetRenderMethod(RenderMethod::WireVertex);
                LOG_INFO("Set render method to \"WireVertex\".");
            }
            // NOTE(sbalse): 2 to display ONLY wireframe.
            else if (event.key.keysym.sym == SDLK_2)
            {
                SetRenderMethod(RenderMethod::Wire);
                LOG_INFO("Set render method to \"Wire\".");
            }
            // NOTE(sbalse): 3 to draw filled triangles.
            else if (event.key.keysym.sym == SDLK_3)
            {
                SetRenderMethod(RenderMethod::FillTriangle);
                LOG_INFO("Set render method to \"FillTriangle\".");
            }
            // NOTE(sbalse): 4 to display both filled and wireframe.
            else if (event.key.keysym.sym == SDLK_4)
            {
                SetRenderMethod(RenderMethod::FillTriangleWire);
                LOG_INFO("Set render method to \"FillTriangleWire\".");
            }
            // NOTE(sbalse): 5 to enable flat shading.
            else if (event.key.keysym.sym == SDLK_5)
            {
                SetShadingMethod(ShadingMethod::FlatShading);
                LOG_INFO("Set shading method to \"FlatShading\".");
            }
            // NOTE(sbalse): 6 to disable shading.
            else if (event.key.keysym.sym == SDLK_6)
            {
                SetShadingMethod(ShadingMethod::None);
                LOG_INFO("Set shading method to \"None\".");
            }
            // NOTE(sbalse): 7 to display textured.
            else if (event.key.keysym.sym == SDLK_7)
            {
                SetRenderMethod(RenderMethod::Textured);
                LOG_INFO("Set render method to \"Textured\".");
            }
            // NOTE(sbalse): 8 to display textured and wired.
            else if (event.key.keysym.sym == SDLK_8)
            {
                SetRenderMethod(RenderMethod::WireTextured);
                LOG_INFO("Set render method to \"WireTextured\".");
            }
            // NOTE(sbalse): c to toggle backface culling.
            else if (event.key.keysym.sym == SDLK_c)
            {
                if (GetCullMethod() == CullMethod::Backface)
                {
                    SetCullMethod(CullMethod::None);
                    LOG_INFO("Set cull method to \"None\".");
                }
                else
                {
                    SetCullMethod(CullMethod::Backface);
                    LOG_INFO("Set cull method to \"Backface\".");
                }
            }
        } break;
        }
    }

    const u8* keystate = SDL_GetKeyboardState(nullptr);
    Vec3 cameraPos = GetCameraPosition();
    Vec3 cameraDir = GetCameraDirection();
    // Obtain camera's right vector by doing cross product of current facing direction with the
    // world's UP vector.
    Vec3 cameraRight = Vec3Cross(cameraDir, Vec3{ 0, 1, 0 });
    Vec3Normalize(&cameraRight);

    if (keystate[SDL_SCANCODE_W])
    {
        cameraPos = Vec3Add(cameraPos, Vec3Mul(cameraDir, 5.0f * g_DeltaTimeSeconds));
    }
    else if (keystate[SDL_SCANCODE_S])
    {
        cameraPos = Vec3Add(cameraPos, Vec3Mul(cameraDir, -5.0f * g_DeltaTimeSeconds));
    }

    if (keystate[SDL_SCANCODE_A])
    {
        cameraPos = Vec3Add(cameraPos, Vec3Mul(cameraRight, 5.0f * g_DeltaTimeSeconds));
    }
    else if (keystate[SDL_SCANCODE_D])
    {
        cameraPos = Vec3Add(cameraPos, Vec3Mul(cameraRight, -5.0f * g_DeltaTimeSeconds));
    }

    if (keystate[SDL_SCANCODE_Q])
    {
        cameraPos.m_Y += 5.0f * g_DeltaTimeSeconds;
    }
    else if (keystate[SDL_SCANCODE_E])
    {
        cameraPos.m_Y -= 5.0f * g_DeltaTimeSeconds;
    }

    UpdateCameraPosition(cameraPos);

    int mouseX, mouseY;
    SDL_GetRelativeMouseState(&mouseX, &mouseY);

    constexpr float cameraSensitivity = 0.002f;
    RotateCameraYaw(mouseX * cameraSensitivity);
    RotateCameraPitch(mouseY * cameraSensitivity);
}

// NOTE(sbalse): Process the graphics pipeline stages for all the mesh triangles.
//
// Model space   <-- original mesh vertices
// |
// |---> World space   <-- multiply by world matrix
//      |
//      |---> Camera space   <-- multiply by view matrix
//          |
//          |---> Clipping      <-- clip against the 6 frustum planes
//                |
//                |---> Projection   <-- multiply by projection matrix
//                      |
//                      |---> Image space  <-- apply perspective divide
//                            |
//                            |---> Screen space   <-- ready to render
//
static void ProcessGraphicsPipelineStages(const Mesh* const mesh)
{
    // NOTE(sbalse): Create scale, translation, and rotation matrices that will be
    // multiplied with our mesh vertices.
    const Mat4 scaleMatrix = Mat4MakeScale(
        mesh->m_Scale.m_X,
        mesh->m_Scale.m_Y,
        mesh->m_Scale.m_Z
    );
    const Mat4 translationMatrix = Mat4MakeTranslation(
        mesh->m_Translation.m_X,
        mesh->m_Translation.m_Y,
        mesh->m_Translation.m_Z
    );
    const Mat4 rotationMatrixX = Mat4MakeRotationX(mesh->m_Rotation.m_X);
    const Mat4 rotationMatrixY = Mat4MakeRotationY(mesh->m_Rotation.m_Y);
    const Mat4 rotationMatrixZ = Mat4MakeRotationZ(mesh->m_Rotation.m_Z);

    // NOTE(sbalse): Create a "World Matrix" combining scale, rotation and translation
    // matrices of the mesh.
    g_WorldMatrix = MAT4_IDENTITY;
    g_WorldMatrix = Mat4MulMat4(scaleMatrix, g_WorldMatrix);
    g_WorldMatrix = Mat4MulMat4(rotationMatrixX, g_WorldMatrix);
    g_WorldMatrix = Mat4MulMat4(rotationMatrixY, g_WorldMatrix);
    g_WorldMatrix = Mat4MulMat4(rotationMatrixZ, g_WorldMatrix);
    g_WorldMatrix = Mat4MulMat4(translationMatrix, g_WorldMatrix);

    // NOTE(sbalse): Update camera look at target to create a view matrix
    const Vec3 cameraTarget = UpdateCameraAndGetLookAtTarget();

    // NOTE(sbalse): Create the view matrix.
    g_ViewMatrix = Mat4LookAt(GetCameraPosition(), cameraTarget, CAMERA_UP_DIRECTION);

    // NOTE(sbalse): Loop all faces of our mesh.
    for (const Face& meshFace : mesh->m_Faces)
    {
        // NOTE(sbalse): The 3 vertices that make up a triangle of a face.
        const Vec3 faceVertices[3] =
        {
            mesh->m_Vertices[meshFace.m_A],
            mesh->m_Vertices[meshFace.m_B],
            mesh->m_Vertices[meshFace.m_C],
        };

        Vec4 transformedVertices[3] = {};

        // NOTE(sbalse): Loop over vertices of the face.
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

        const Vec3 faceNormal = GetTriangleNormal(transformedVertices);

        // NOTE(sbalse): Do backface culling.
        if (GetCullMethod() == CullMethod::Backface)
        {
            // NOTE(sbalse): Calculate the triangle face normal.

            // NOTE(sbalse): Find the vector between a point in the triangle and the camera origin.
            // We directly use ORIGIN here [i.e. (0, 0, 0)] since our camera is always meant to be the
            // origin of our world.
            const Vec3 cameraRay = Vec3Sub(ORIGIN, Vec3FromVec4(transformedVertices[0]));

            // NOTE(sbalse): Use dot product to calculate how aligned the face normal is with the camera
            // ray.
            const float cameraAlignmentWithFaceNormal = Vec3Dot(faceNormal, cameraRay);

            // NOTE(sbalse): If the normal is not visible to the camera then don't draw this face. AKA,
            // cull this face.
            if (cameraAlignmentWithFaceNormal < 0)
            {
                continue;
            }
        }

        // NOTE(sbalse): Do clipping before projecting the vertices.
        // NOTE(sbalse): Create a polygon to be clipped from the original transformed vertices.
        Polygon polygon = CreatePolygonFromTriangle(
            Vec3FromVec4(transformedVertices[0]),
            Vec3FromVec4(transformedVertices[1]),
            Vec3FromVec4(transformedVertices[2]),
            meshFace.m_AUV,
            meshFace.m_BUV,
            meshFace.m_CUV
        );

        // NOTE(sbalse): Clip the polygon and get a new polygon with potentially new vertices.
        ClipPolygon(&polygon);

        Triangle trianglesAfterClipping[MAX_NUM_POLYGON_TRIANGLES] = {};
        int numTrianglesAfterClipping = 0;

        // NOTE(sbalse): After clipping, we need to break the clipping polygon back into triangles.
        TrianglesFromPolygon(&polygon, trianglesAfterClipping, &numTrianglesAfterClipping);

        // NOTE(sbalse): Loop all the assembled triangles after clipping.
        for (int t = 0; t < numTrianglesAfterClipping; t++)
        {
            const Triangle triangleAfterClipping = trianglesAfterClipping[t];

            Vec4 projectedPoints[3] = {};

            // NOTE(sbalse): Project vertices of the face.
            for (int vertexIndex = 0; vertexIndex < 3; vertexIndex++)
            {
                // NOTE(sbalse): Project the current vertex.
                projectedPoints[vertexIndex] = Mat4MulVec4Project(
                    g_ProjMatrix,
                    triangleAfterClipping.m_Points[vertexIndex]);

                // NOTE(sbalse): Invert the y values to account for our flipped y axis.
                projectedPoints[vertexIndex].m_Y *= -1;

                // NOTE(sbalse): Transform into screen space.
                projectedPoints[vertexIndex].m_X *= (GetWindowWidth() / 2.0f);
                projectedPoints[vertexIndex].m_Y *= (GetWindowHeight() / 2.0f);

                // NOTE(sbalse): Translate the projected point to the middle of the screen.
                projectedPoints[vertexIndex].m_X += (GetWindowWidth() / 2.0f);
                projectedPoints[vertexIndex].m_Y += (GetWindowHeight() / 2.0f);
            }

            u32 triangleColor = meshFace.m_Color;

            if (GetShadingMethod() == ShadingMethod::FlatShading)
            {
                // NOTE(sbalse): Calculate the shade intensity based on how aligned is the face normal and
                // the inverse of the light ray.
                const float lightIntensityFactor = -Vec3Dot(faceNormal, GetLightDirection());

                // NOTE(sbalse): Calculate triangle color based on the light angle.
                triangleColor = LightApplyIntensity(meshFace.m_Color, lightIntensityFactor);
            }

            // NOTE(sbalse): Finally create the final triangle which we will render.
            const Triangle triangleToRender =
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
                    { triangleAfterClipping.m_TexCoords[0] },
                    { triangleAfterClipping.m_TexCoords[1] },
                    { triangleAfterClipping.m_TexCoords[2] },
                },
                .m_Color = triangleColor,
                .m_Texture = mesh->m_Texture,
            };

            if (g_NumTrianglesToRender < MAX_NUM_TRIANGLES_TO_RENDER)
            {
                // NOTE(sbalse): Save the projected triangle in the array of triangles to render.
                g_TrianglesToRender[g_NumTrianglesToRender++] = triangleToRender;
            }
        }
    }
}

static void Update()
{
    const u32 currentUpdateTimeMS = SDL_GetTicks();
    const u32 frameDuration = currentUpdateTimeMS - g_PreviousUpdateTimeMS;

    g_DeltaTimeSeconds = frameDuration / 1000.0f;
    g_PreviousUpdateTimeMS = currentUpdateTimeMS;

    const int timeToWait = scast<int>(FRAME_TARGET_TIME_MS) - frameDuration;
    if (timeToWait > 0 && timeToWait <= FRAME_TARGET_TIME_MS)
    {
        SDL_Delay(timeToWait);
    }

    g_NumTrianglesToRender = 0;

    const int numOfMeshes = GetNumOfMeshes();

    // NOTE(sbalse): Loop over all the meshes in our scene.
    for (int meshIndex = 0; meshIndex < numOfMeshes; meshIndex++)
    {
        const Mesh* const currentMesh = GetMesh(meshIndex);

        if (!g_Paused)
        {
            // NOTE(sbalse): Will need to remove the const from currentMesh before uncommenting the following code.
            // currentMesh->m_Rotation.m_X += 0.6f * g_DeltaTimeSeconds;
            // currentMesh->m_Rotation.m_Y += 0.6f * g_DeltaTimeSeconds;
            // currentMesh->m_Rotation.m_Z += 0.6f * g_DeltaTimeSeconds;

            // currentMesh->m_Scale.m_X += 0.2 * g_DeltaTimeSeconds;
            // currentMesh->m_Scale.m_Y += 0.2 * g_DeltaTimeSeconds;
            // currentMesh->m_Scale.m_Z += 0.2 * g_DeltaTimeSeconds;

            // currentMesh->m_Translation.m_X += 0.6 * g_DeltaTimeSeconds;
        }

        ProcessGraphicsPipelineStages(currentMesh);
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
    for (int currentTriangleIndex = 0;
        currentTriangleIndex < g_NumTrianglesToRender;
        currentTriangleIndex++)
    {
        const Triangle& currentTriangle = g_TrianglesToRender[currentTriangleIndex];

        const RenderMethod currentRenderMethod = GetRenderMethod();
        if (currentRenderMethod == RenderMethod::FillTriangle
            || currentRenderMethod == RenderMethod::FillTriangleWire)
        {
            // NOTE(sbalse): Draw mesh face triangles.
            DrawFilledTriangle(
                // NOTE(sbalse): vertex A.
                scast<int>(currentTriangle.m_Points[0].m_X),
                scast<int>(currentTriangle.m_Points[0].m_Y),
                currentTriangle.m_Points[0].m_Z,
                currentTriangle.m_Points[0].m_W,
                // NOTE(sbalse): vertex B.
                scast<int>(currentTriangle.m_Points[1].m_X),
                scast<int>(currentTriangle.m_Points[1].m_Y),
                currentTriangle.m_Points[1].m_Z,
                currentTriangle.m_Points[1].m_W,
                // NOTE(sbalse): vertex C.
                scast<int>(currentTriangle.m_Points[2].m_X),
                scast<int>(currentTriangle.m_Points[2].m_Y),
                currentTriangle.m_Points[2].m_Z,
                currentTriangle.m_Points[2].m_W,
                // NOTE(sbalse): The color.
                currentTriangle.m_Color);
        }

        if (currentRenderMethod == RenderMethod::WireVertex)
        {
            // NOTE(sbalse): Draw the cube corner vertices.
            DrawRectangle(
                scast<int>(currentTriangle.m_Points[0].m_X - 3),
                scast<int>(currentTriangle.m_Points[0].m_Y - 3),
                6,
                6,
                RED);
            DrawRectangle(
                scast<int>(currentTriangle.m_Points[1].m_X - 3),
                scast<int>(currentTriangle.m_Points[1].m_Y - 3),
                6,
                6,
                RED);
            DrawRectangle(
                scast<int>(currentTriangle.m_Points[2].m_X - 3),
                scast<int>(currentTriangle.m_Points[2].m_Y - 3),
                6,
                6,
                RED);
        }

        // NOTE(sbalse): Draw textured triangle.
        if (currentRenderMethod == RenderMethod::Textured
            || currentRenderMethod == RenderMethod::WireTextured)
        {
            DrawTexturedTriangle(
                // NOTE(sbalse): vertex A.
                scast<int>(currentTriangle.m_Points[0].m_X),
                scast<int>(currentTriangle.m_Points[0].m_Y),
                currentTriangle.m_Points[0].m_Z,
                currentTriangle.m_Points[0].m_W,
                currentTriangle.m_TexCoords[0].m_U,
                currentTriangle.m_TexCoords[0].m_V,
                // NOTE(sbalse): vertex B.
                scast<int>(currentTriangle.m_Points[1].m_X),
                scast<int>(currentTriangle.m_Points[1].m_Y),
                currentTriangle.m_Points[1].m_Z,
                currentTriangle.m_Points[1].m_W,
                currentTriangle.m_TexCoords[1].m_U,
                currentTriangle.m_TexCoords[1].m_V,
                // NOTE(sbalse): vertex C.
                scast<int>(currentTriangle.m_Points[2].m_X),
                scast<int>(currentTriangle.m_Points[2].m_Y),
                currentTriangle.m_Points[2].m_Z,
                currentTriangle.m_Points[2].m_W,
                currentTriangle.m_TexCoords[2].m_U,
                currentTriangle.m_TexCoords[2].m_V,
                // NOTE(sbalse): The texture.
                currentTriangle.m_Texture);
        }

        if (currentRenderMethod == RenderMethod::Wire
            || currentRenderMethod == RenderMethod::WireVertex
            || currentRenderMethod == RenderMethod::FillTriangleWire
            || currentRenderMethod == RenderMethod::WireTextured)
        {
            // NOTE(sbalse): Draw mesh wireframe triangles.
            DrawTriangle(
                scast<int>(currentTriangle.m_Points[0].m_X),
                scast<int>(currentTriangle.m_Points[0].m_Y),
                scast<int>(currentTriangle.m_Points[1].m_X),
                scast<int>(currentTriangle.m_Points[1].m_Y),
                scast<int>(currentTriangle.m_Points[2].m_X),
                scast<int>(currentTriangle.m_Points[2].m_Y),
                WHITE);
        }
    }

    // NOTE(sbalse): Clear the list of triangles to render every frame loop.
    g_TrianglesToRender = {};

    const RenderBufferMethod currentRenderBufferMethod = GetRenderBufferMethod();
    if (currentRenderBufferMethod == RenderBufferMethod::ColorBuffer)
    {
        RenderColorBuffer();
    }
    else if (currentRenderBufferMethod == RenderBufferMethod::ZBuffer)
    {
        RenderZBuffer();
    }
}

// NOTE(sbalse): Free the memory that was dynamically allocated by the program.
static void FreeResources()
{
    FreeMeshes();
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
