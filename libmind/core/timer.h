#pragma once

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

enum timer_precision {
    TIMER_PRECISION_SECONDS,
    TIMER_PRECISION_MILLISECONDS,
    TIMER_PRECISION_MICROSECONDS,
    TIMER_PRECISION_NANOSECONDS
};

typedef struct _timer {
    u64 start_time;
    u64 stop_time;
    u64 elapsed;
    bool running;
} timer_t;

void timer_start(timer_t* t);
void timer_stop(timer_t* t);
void timer_restart(timer_t* t);
u64 timer_elapsed(timer_t* t, enum timer_precision precision);

typedef int (*callback_t)(void* data);
typedef struct timed_callback {
    timer_t timer;
    enum timer_precision precision;
    callback_t cb;
} timed_callback_t;

void timed_callback_init(timed_callback_t* tcb, u64 duration, enum timer_precision precision);
void timed_callback_start(timed_callback_t* tcb);
void timed_callback_stop(timed_callback_t* tcb);

#ifdef __cplusplus
}
#endif
