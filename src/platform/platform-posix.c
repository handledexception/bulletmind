#include "platform/platform.h"

#include <unistd.h>

void os_sleep_ms(const u32 duration)
{
    usleep(duration * 1000);
}

bool os_file_exists(const char* path)
{
    return access(path, F_OK) == 0;
}
