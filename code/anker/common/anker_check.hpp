#pragma once

#include <anker/common/anker_log.hpp>
#include <anker/common/anker_macros.hpp>

// ANKER_CHECK is similar to assertions, but instead of terminating the program
// it logs and bails from the current function. There are 2 overloads, one with
// a return value argument, and one without (for void functions). ANKER_CHECK is
// always enabled.
#define ANKER_CHECK(...) ANKER_EXPAND(ANKER_CHECK_OVERLOAD(__VA_ARGS__, ANKER_CHECK2, ANKER_CHECK1)(__VA_ARGS__))
#define ANKER_CHECK_OVERLOAD(_1, _2, NAME, ...) NAME

#define ANKER_CHECK1(condition) \
	do { \
		if (!(condition)) [[unlikely]] { \
			ANKER_ERROR("{}", "Check failed: " #condition); \
			return; \
		} \
	} while (0)

#define ANKER_CHECK2(condition, ret) \
	do { \
		if (!(condition)) [[unlikely]] { \
			ANKER_ERROR("{}", "Check failed: " #condition); \
			return ret; \
		} \
	} while (0)

#define ANKER_CHECK_SCENE_NODE_INVARIANT_ENABLED 1
