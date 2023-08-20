#pragma once

#define ANKER_PROFILE_ENALBED 0

#if ANKER_PROFILE_ENALBED

#define TRACY_CALLSTACK 16
#include <tracy/Tracy.hpp>

// Tracks the current scope.
#define ANKER_PROFILE_ZONE() ZoneScoped

// Tracks the current scope, with a given name (static const char*).
#define ANKER_PROFILE_ZONE_N(name) ZoneScopedN(name)

// Tracks the current scope, with a given string (copied).
#define ANKER_PROFILE_ZONE_T(text) \
	ZoneScoped; \
	ZoneText(text.data(), text.size())

#define ANKER_PROFILE_FRAME_MARK() FrameMark

#define ANKER_PROFILE_ALLOC(ptr, size) TracyAlloc(ptr, size)
#define ANKER_PROFILE_FREE(ptr) TracyFree(ptr)

#else

#define ANKER_PROFILE_ZONE(name)
#define ANKER_PROFILE_ZONE_N(name)
#define ANKER_PROFILE_ZONE_T(text)
#define ANKER_PROFILE_FRAME_MARK()
#define ANKER_PROFILE_ALLOC(ptr, size)
#define ANKER_PROFILE_FREE(ptr)

#endif
