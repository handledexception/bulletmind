#include "timing.h"

#include <Windows.h>

void timing_init(timing_t *timing)
{
	if (timing != NULL) {
		LARGE_INTEGER qpf, qpc;

		QueryPerformanceFrequency(&qpf);
		timing->freq = (double)qpf.QuadPart;

		QueryPerformanceCounter(&qpc);		
		timing->original = timing->start = (double)qpc.QuadPart;
		timing->milli = 0.0;
		timing->sec = 0.0;
		timing->micro = 0.0;
	}
}

double timing_interval_ms(timing_t *timing)
{
	double msec = 0.0;
	if (timing != NULL) {
		LARGE_INTEGER qpc;
		QueryPerformanceCounter(&qpc);		
		msec = (qpc.QuadPart - timing->start) / (timing->freq / 1000.0);
		timing->start = (double)qpc.QuadPart;
	}
	return msec;
}

double timing_interval_sec(timing_t *timing)
{
	double sec = 0.0;
	if (timing != NULL) {
		LARGE_INTEGER qpc;
		QueryPerformanceCounter(&qpc);
		sec = (qpc.QuadPart - timing->start) / (timing->freq);
		timing->start = (double)qpc.QuadPart;
	}
	return sec;
}

double timing_interval_micro(timing_t *timing)
{
	double micro = 0.0;	
	if (timing != NULL) {
		LARGE_INTEGER qpc;
		QueryPerformanceCounter(&qpc);
		micro = (qpc.QuadPart - timing->start) / (timing->freq / 1000000.0);
		timing->start = (double)qpc.QuadPart;
	}	
	return micro;
}