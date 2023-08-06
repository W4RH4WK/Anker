#pragma once

#include "anker_log.hpp"

#define ANKER_ENABLE_CHECKS 1
#define ANKER_ENABLE_CHECKS_INBOUNDS 1

#if ANKER_ENABLE_CHECKS
#define ANKER_CHECK(condition) \
	do { \
		if (!(condition)) { \
			ANKER_FATAL("Check failed: " #condition); \
		} \
	} while (0)
#else
#define ANKER_CHECK(condition)
#endif

#if ANKER_ENABLE_CHECKS_INBOUNDS
#define ANKER_CHECK_INBOUNDS(index, size) \
	ANKER_CHECK(static_cast<::std::size_t>(index) < static_cast<::std::size_t>(size))
#else
#define ANKER_CHECK_INBOUNDS(index, size)
#endif
