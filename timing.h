#ifndef H_TIMING
#define H_TIMING

#include "c99defs.h"

typedef struct {
	double original;
	double start;
	double freq;
	double milli;
	double sec;
	double micro;
} timing_t;

double timing_getmillisec(void);
uint64_t timing_getnanosec(void);

#endif