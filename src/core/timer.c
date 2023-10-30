#include "timer.h"
#include "time_convert.h"
#include "platform/platform.h"

void timer_start(timer_t* t)
{
    if (t != NULL) {
        t->start_time = os_get_time_ns();
        t->stop_time  = 0;
        t->running = true;
    }
}

void timer_stop(timer_t* t)
{
    if (t != NULL) {
        t->stop_time = os_get_time_ns();
        t->elapsed = t->stop_time - t->start_time;
        t->running = false;
    }
}

u64 timer_elapsed(timer_t* t, enum timer_precision precision)
{
    if (t == NULL)
        return 0;

    u64 elapsed = t->elapsed;
    if (t->running)
        elapsed = os_get_time_ns();
    switch (precision) {
    case TIMER_PRECISION_MILLISECONDS:
        return (u64)nsec_to_msec_f64(elapsed);
    case TIMER_PRECISION_MICROSECONDS:
        return (u64)nsec_to_usec_f64(elapsed);
    case TIMER_PRECISION_NANOSECONDS:
        return elapsed;
    default:
    case TIMER_PRECISION_SECONDS:
        return (u64)nsec_to_sec_f64(elapsed);
    }
    return 0;
}
