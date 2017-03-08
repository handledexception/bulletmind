#include "timing.h"

#include <Windows.h>

void timing_init(timing_t *timing)
{
	LARGE_INTEGER qpf, qpc;
	
	QueryPerformanceFrequency(&qpf);
	timing->freq = qpf.QuadPart;
	
	QueryPerformanceCounter(&qpc);
	timing->start = qpc.QuadPart;
	
	timing->milli = 0.0;
	timing->sec = 0.0;
	timing->micro = 0.0;
}

double timing_getmilli(timing_t *timing)
{
	double msec = 0.0;
	if (timing != NULL) {
		LARGE_INTEGER qpc;
		QueryPerformanceCounter(&qpc);
		msec = (qpc.QuadPart - timing->start) / (timing->freq / 1000.0);
		timing->start = qpc.QuadPart;
	}	
	return msec;
}

double timing_getsec(timing_t *timing)
{
	double sec = 0.0;
	if (timing != NULL) {
		LARGE_INTEGER qpc;
		QueryPerformanceCounter(&qpc);
		sec = (qpc.QuadPart - timing->start) / (timing->freq);
		timing->start = qpc.QuadPart;
	}
	return sec;
}

double timing_getmicro(timing_t *timing)
{
	double micro = 0.0;	
	if (timing != NULL) {
		LARGE_INTEGER qpc;
		QueryPerformanceCounter(&qpc);
		micro = (qpc.QuadPart - timing->start) / (timing->freq / 1000000.0);
		timing->start = qpc.QuadPart;
	}	
	return micro;
}