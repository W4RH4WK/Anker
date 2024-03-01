#pragma once

#define ANKER_EXPAND(x) x

#define ANKER_STR(x) #x
#define ANKER_XSTR(x) ANKER_STR(x)

#define ANKER_CONCAT(x, y) x##y
#define ANKER_XCONCAT(x, y) ANKER_CONCAT(x, y)

#define ANKER_COUNTER __COUNTER__
#define ANKER_TEMPORARY(prefix) ANKER_XCONCAT(prefix, ANKER_COUNTER)

#define ANKER_ARRAYSIZE(x) (sizeof(x) / sizeof(0 [x]))

// DEBUG_BREAK should trigger the debugger (if one is present) and terminate the
// application otherwise.
#ifdef _MSC_VER
#define ANKER_DEBUG_BREAK() \
	do { \
		if (IsDebuggerPresent()) { \
			__debugbreak(); \
		} else { \
			::std::abort(); \
		} \
	} while (0)
#elif __GNUC__
#define ANKER_DEBUG_BREAK() __builtin_trap()
#elif __clang__
#define ANKER_DEBUG_BREAK() __builtin_debugtrap()
#else
#define ANKER_DEBUG_BREAK() ::std::abort()
#endif
