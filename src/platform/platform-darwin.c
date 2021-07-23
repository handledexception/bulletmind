#include "platform/platform.h"

#include <time.h>

u64 os_get_time_ns(void)
{
    return clock_gettime_nsec_np(CLOCK_UPTIME_RAW);
}
