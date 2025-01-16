#include "camera.h"

constinit Camera g_Camera =
{
    .m_Position = { 0, 0, 0 },
    .m_Direction = { 0, 0, 1 },
    .m_ForwardVelocity = { 0, 0, 0 },
    .m_Yaw = 0.0f,
};
