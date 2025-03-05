#include "camera.h"

#include <cmath>

#include "matrix.h"

inline constexpr double M_PI_2 = 1.57079632679489661923;   // pi/2

constinit static Camera g_Camera = {};

void InitCamera(const Vec3 position, const Vec3 direction)
{
    g_Camera.m_Position = position;
    g_Camera.m_Direction = direction;
}

Vec3 GetCameraPosition()
{
    return g_Camera.m_Position;
}

Vec3 GetCameraDirection()
{
    return g_Camera.m_Direction;
}

Vec3 GetCameraForwardVelocity()
{
    return g_Camera.m_ForwardVelocity;
}

void UpdateCameraPosition(const Vec3 position)
{
    g_Camera.m_Position = position;
}

void UpdateCameraDirection(const Vec3 direction)
{
    g_Camera.m_Direction = direction;
}

void UpdateCameraForwardVelocity(const Vec3 forwardVelocity)
{
    g_Camera.m_ForwardVelocity = forwardVelocity;
}

float GetCameraYaw()
{
    return g_Camera.m_Yaw;
}

float GetCameraPitch()
{
    return g_Camera.m_Pitch;
}

void RotateCameraYaw(const float angle)
{
    g_Camera.m_Yaw += angle;
}

void RotateCameraPitch(const float angle)
{
    g_Camera.m_Pitch += angle;

    if (g_Camera.m_Pitch > M_PI_2)
    {
        g_Camera.m_Pitch = M_PI_2;
    }

    if (g_Camera.m_Pitch < -M_PI_2)
    {
        g_Camera.m_Pitch = -M_PI_2;
    }
}

Vec3 UpdateCameraAndGetLookAtTarget()
{
    // Initialize the target looking at the positive z-axis.
    Vec3 target = { 0, 0, 1 };

    const Mat4 cameraYawRotation = Mat4MakeRotationY(g_Camera.m_Yaw);
    const Mat4 cameraPitchRotation = Mat4MakeRotationX(g_Camera.m_Pitch);

    // Create camera rotation matrix based on yaw and pitch.
    Mat4 cameraRotation = MAT4_IDENTITY;
    cameraRotation = Mat4MulMat4(cameraPitchRotation, cameraRotation);
    cameraRotation = Mat4MulMat4(cameraYawRotation, cameraRotation);

    // Update camera direction based on the rotation.
    const Vec4 cameraDirection = Mat4MulVec4(cameraRotation, Vec4FromVec3(target));
    g_Camera.m_Direction = Vec3FromVec4(cameraDirection);

    // Offset the camera position in the direction where the camera is pointing at.
    target = Vec3Add(g_Camera.m_Position, g_Camera.m_Direction);

    return target;
}
