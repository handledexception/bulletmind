#pragma once

#include <stdlib.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

static inline int random(int lower, int upper)
{
	int div = (upper + 1 - lower) + lower;
	if (div == 0)
		return rand();
	return rand() % div;
}

#ifdef __cplusplus
}
#endif
