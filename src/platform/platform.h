#pragma once

#include "c99defs.h"
#include "types.h"
#include "platform/export.h"

#ifdef __cplusplus
extern "C"
#endif

#if defined(BM_WINDOWS)
#define BM_MAX_PATH 256
#elif defined(BM_MACOS)
#define BM_MAX_PATH 1024
#elif defined(BM_LINUX)
#define BM_MAX_PATH 4096
#endif

BM_PLATFORM_EXPORT size_t os_utf8_to_wcs(const char *str, size_t len, wchar_t *dst,
		      size_t dst_size);
BM_PLATFORM_EXPORT size_t os_utf8_to_wcs_ptr(const char *str, size_t len, wchar_t **pstr);

BM_PLATFORM_EXPORT void os_sleep_ms(const u32 duration);
BM_PLATFORM_EXPORT u64 os_get_time_ns(void);
BM_PLATFORM_EXPORT f64 os_get_time_sec(void);
BM_PLATFORM_EXPORT f64 os_get_time_msec(void);

BM_PLATFORM_EXPORT bool os_file_exists(const char* path);
