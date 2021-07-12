#pragma once

#include <stdlib.h>
#include <time.h>

inline int gen_random(int lower, int upper, int iterations)
{
	int result = 0;
	// srand(time(0));
	for (int i = lower; i < iterations; i++) {
		result = (rand() % (upper - lower + 1) + lower);
	}

	return result;
}
