#pragma once
#include "vector.h"

struct Camera
{
    Vec3 m_Position;
    Vec3 m_Direction; // NOTE(sbalse): Direction the camera is looking at.
    Vec3 m_ForwardVelocity; // NOTE(sbalse): Forward/backwards movement velocity.
    float m_Yaw; // NOTE(sbalse): Side rotation.
    float m_Pitch; // NOTE(sbalse): Forward rotation.
};

void InitCamera(const Vec3 position, const Vec3 direction);

Vec3 GetCameraPosition();
Vec3 GetCameraDirection();
Vec3 GetCameraForwardVelocity();

void UpdateCameraPosition(const Vec3 position);
void UpdateCameraDirection(const Vec3 direction);
void UpdateCameraForwardVelocity(const Vec3 forwardVelocity);

float GetCameraYaw();
float GetCameraPitch();

void RotateCameraYaw(const float angle);
void RotateCameraPitch(const float angle);

// Gets the camera's look at target based on its position and direction.
Vec3 UpdateCameraAndGetLookAtTarget();
