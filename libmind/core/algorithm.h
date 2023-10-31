#pragma once

#include "core/types.h"

#undef min
#undef max

#if _WIN32 && !defined(NOMINMAX)
#define NOMINMAX
#endif

static inline u32 min(u32 a, u32 b)
{
	return (a < b) ? a : b;
}

static inline u32 max(u32 a, u32 b)
{
	return (a > b) ? a : b;
}
