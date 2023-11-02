#pragma once

#include "anker_log.hpp"

// ANKER_CHECK is similar to assertions, but instead of terminating the program
// it logs and bails from the current function. ret can be omitted for functions
// without return values (void). ANKER_CHECK is always enabled.
#define ANKER_CHECK(condition, ret) \
	do { \
		if (!(condition)) { \
			ANKER_ERROR("Check failed: " #condition); \
			return ret; \
		} \
	} while (0)

#define ANKER_CHECK_SCENE_NODE_INVARIANT_ENABLED 1
