#pragma once
#include "vector.h"

struct Camera
{
    Vec3 m_Position;
    Vec3 m_Direction; // NOTE(sbalse): Direction the camera is looking at.
    Vec3 m_ForwardVelocity; // NOTE(sbalse): Forward/backwards movement velocity.
    float m_Yaw; // NOTE(sbalse): Side rotation.
};

extern constinit Camera g_Camera;
