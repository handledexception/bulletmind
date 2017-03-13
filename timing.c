#include "timing.h"
#include <Windows.h>

static LARGE_INTEGER clock_freq;

static inline uint64_t get_clock_freq(void)
{
	QueryPerformanceFrequency(&clock_freq);
	return clock_freq.QuadPart;
}

double timing_getmillisec(void)
{
	LARGE_INTEGER current_time;
	long long time_val;

	QueryPerformanceCounter(&current_time);
	time_val = current_time.QuadPart;
	time_val *= 1000LL;
	time_val /= get_clock_freq();

	return (double)time_val;
}

uint64_t timing_getnanosec(void)
{
	LARGE_INTEGER current_time;
	double time_val;

	QueryPerformanceCounter(&current_time);
	time_val = (double)current_time.QuadPart;
	time_val *= 1000000000.0;
	time_val /= (double)get_clock_freq();

	return (uint64_t)time_val;
}