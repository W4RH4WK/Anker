#pragma once

#include "anker_log.hpp"

// ANKER_CHECK is similar to ANKER_ASSERT, but instead of terminating the
// program we log and return from the current function. ret can be omitted for
// functions without return values (void).
#define ANKER_CHECK(condition, ret) \
	do { \
		if (!(condition)) { \
			ANKER_ERROR("Check failed: " #condition); \
			return ret; \
		} \
	} while (0)
