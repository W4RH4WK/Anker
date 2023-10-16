#pragma once

#include "anker_log.hpp"
#include "anker_type_utils.hpp"

#define ANKER_ASSERT_ENABLED 1
#define ANKER_ASSERT_INBOUNDS_ENABLED 1
#define ANKER_ASSERT_NUM_CAST_ENABLED 1

#if ANKER_ASSERT_ENABLED
#define ANKER_ASSERT(condition) \
	do { \
		if (!(condition)) { \
			ANKER_FATAL("Check failed: " #condition); \
		} \
	} while (0)
#else
#define ANKER_ASSERT(condition)
#endif

#if ANKER_ASSERT_INBOUNDS_ENABLED
#define ANKER_ASSERT_INBOUNDS(index, size) \
	ANKER_ASSERT(static_cast<::Anker::usize>(index) < static_cast<::Anker::usize>(size))
#else
#define ANKER_ASSERT_INBOUNDS(index, size)
#endif

#if ANKER_ASSERT_NUM_CAST_ENABLED
#define ANKER_ASSERT_NUM_CAST(toType, value) \
	ANKER_ASSERT(::std::numeric_limits<toType>::min() <= value && value <= ::std::numeric_limits<toType>::max())
#else
#define ANKER_ASSERT_NUM_CAST(toType, value)
#endif
