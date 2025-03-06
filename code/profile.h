#pragma once

#if PROFILER_ENABLED
#define USE_TRACY
#endif PROFILER_ENABLED

#if defined(USE_TRACY)
#define TRACY_ENABLE
#include <tracy/Tracy.hpp>

#define PROFILE_FRAME() FrameMark
#define PROFILE_EVENT() ZoneScoped
#define PROFILE_EVENT_SCOPED_BEGIN(var, name) ZoneNamedN(var, name, true)

#else

#define PROFILE_FRAME()
#define PROFILE_EVENT()
#define PROFILE_EVENT_SCOPED_BEGIN(var, name)

#endif // USE_TRACY
