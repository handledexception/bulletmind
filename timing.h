#ifndef H_TIMING
#define H_TIMING

typedef struct {
	double original;
	double start;
	double freq;
	double milli;
	double sec;
	double micro;
} timing_t;

void timing_init(timing_t *timing);

/* 
these functions return the time elapsed since the last call
or since the first call to timing_init
*/
double timing_interval_ms(timing_t *timing);
double timing_interval_sec(timing_t *timing);
double timing_interval_micro(timing_t *timing);

#endif