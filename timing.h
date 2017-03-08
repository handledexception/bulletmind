#ifndef H_TIMING
#define H_TIMING

typedef struct {
	double start;
	double freq;
	double milli;
	double sec;
	double micro;
} timing_t;

void timing_init(timing_t *timing);
double timing_getmilli(timing_t *timing);
double timing_getsec(timing_t *timing);
double timing_getmicro(timing_t *timing);

#endif