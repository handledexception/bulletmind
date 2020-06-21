#ifndef _H_TIMING
#define _H_TIMING

#include "c99defs.h"

double timing_getsec(void);
double timing_getmillisec(void);
uint32_t timing_millisec_uint32(void);
uint64_t timing_getnanosec(void);

void timing_init(void);
double timing_enginetime(void);

#endif