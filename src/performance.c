#include "performance.h"

#define SDL_PERF

#if defined (SDL_PERF)
#include <SDL.h>
#elif defined(BM_WINDOWS)
#include <Windows.h>
#endif

static inline uint64_t get_clock_freq(void) {
#if defined(SDL_PERF)
    const uint64_t freq = SDL_GetPerformanceFrequency();
#elif defined(BM_WINDOWS)
    LARGE_INTEGER clock_freq = { 0 };
    QueryPerformanceFrequency(&clock_freq);
    const uint64_t freq = clock_freq.QuadPart;
#endif
    return freq;
}

double perf_seconds(void) {
#if defined(SDL_PERF)
    const double seconds = (double)SDL_GetPerformanceCounter() /
        (double)get_clock_freq();
#elif defined(BM_WINDOWS)
    LARGE_INTEGER qpc_value = { 0 };
    qpc_value.QuadPart = 0LL;

    QueryPerformanceCounter(&qpc_value);

    const double seconds = (double)((double)(qpc_value.QuadPart) / (double)(get_clock_freq()));
#endif
    return seconds;
}

uint64_t perf_microseconds(void) {
    return (uint64_t)(perf_seconds() * 1000000ULL);
}

double perf_milliseconds(void) {
    return (double)((double)(perf_microseconds()) / 1000.0);
}
