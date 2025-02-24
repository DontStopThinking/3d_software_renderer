#pragma once
#include "common.h"
#include "vector.h"

struct Light
{
    Vec3 m_Direction;
};

void InitLight(const Vec3 direction);
Vec3 GetLightDirection();
u32 LightApplyIntensity(const u32 originalColor, const float percentageFactor);

// TODO(sbalse): Smooth shading using Gouraud Shading.
