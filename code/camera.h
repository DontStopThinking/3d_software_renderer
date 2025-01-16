#pragma once
#include "vector.h"

struct Camera
{
    Vec3 m_Position;
    Vec3 m_Direction; // NOTE(sbalse): Direction the camera is looking at.
};

extern constinit Camera g_Camera;
