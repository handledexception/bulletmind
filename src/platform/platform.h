/*
 * Copyright (c) 2013 Hugh Bailey <obs.jim@gmail.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#pragma once

#include "core/c99defs.h"
#include "core/types.h"
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
