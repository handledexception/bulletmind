#include "performance.h"

#if defined(BM_WINDOWS)
#include <Windows.h>

static LARGE_INTEGER clock_freq;

static inline uint64_t get_clock_freq(void) {
    QueryPerformanceFrequency(&clock_freq);
    return clock_freq.QuadPart;
}

uint64_t perf_microseconds(void) {
    LARGE_INTEGER qpc_value;
    qpc_value.QuadPart = 0LL;

    QueryPerformanceCounter(&qpc_value);
    qpc_value.QuadPart *= 1000000LL;
    qpc_value.QuadPart /= get_clock_freq();

    return qpc_value.QuadPart;
}

double perf_seconds(void) {
    LARGE_INTEGER qpc_value;
    qpc_value.QuadPart = 0LL;

    QueryPerformanceCounter(&qpc_value);

    return (double)((double)(qpc_value.QuadPart) / (double)(get_clock_freq()));
}

#endif // BM_WINDOWS

double perf_milliseconds(void) {
    return (double)((double)(perf_microseconds()) / 1000.0);
}
