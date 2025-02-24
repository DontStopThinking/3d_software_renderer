#include "light.h"

#include <algorithm>

#include "colorlibrary.h"

// NOTE(sbalse): Init externs.
constinit static Light g_Light = {};

void InitLight(const Vec3 direction)
{
    g_Light.m_Direction = direction;
}

Vec3 GetLightDirection()
{
    return g_Light.m_Direction;
}

u32 LightApplyIntensity(const u32 originalColor, const float percentageFactor)
{
    const float clampedPercentageFactor = std::clamp(percentageFactor, 0.0f, 1.0f);

    // NOTE(sbalse): Use bitwise "&" to extract only the FF part of the color.

    const u32 a = (originalColor & ALPHA_MASK);
    const u32 r = scast<u32>((originalColor & RED_MASK) * clampedPercentageFactor);
    const u32 g = scast<u32>((originalColor & GREEN_MASK) * clampedPercentageFactor);
    const u32 b = scast<u32>((originalColor & BLUE_MASK) * clampedPercentageFactor);

    const u32 result = a | (r & RED_MASK) | (g & GREEN_MASK) | (b & BLUE_MASK);

    return result;
}
