#include "timing.h"
#include <Windows.h>

static LARGE_INTEGER clock_freq;
static double engine_start;

static inline uint64_t get_clock_freq(void)
{
	QueryPerformanceFrequency(&clock_freq);
	return clock_freq.QuadPart;
}

double timing_getsec(void)
{
	LARGE_INTEGER current_time;
	double time_val;

	QueryPerformanceCounter(&current_time);
	time_val = (double)current_time.QuadPart;
	time_val /= (double)get_clock_freq();

	return time_val;
}

double timing_getmillisec(void)
{
	LARGE_INTEGER current_time;
	double time_val;

	QueryPerformanceCounter(&current_time);
	time_val = (double)current_time.QuadPart;
	time_val *= 1000LL;
	time_val /= (double)get_clock_freq();	
	
	return time_val;
}

uint32_t timing_millisec_uint32(void)
{
	LARGE_INTEGER current_time;
	uint64_t time_val;

	QueryPerformanceCounter(&current_time);
	time_val = current_time.QuadPart;
	time_val *= 1000;
	time_val /= get_clock_freq();

	return (uint32_t)time_val;
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

void timing_init(void)
{
	engine_start = timing_getsec();
}

double timing_enginetime(void)
{
	return timing_getsec() - engine_start;	
}