/*
 * Portions of this code adapted or borrowed from Open Broadcaster (OBS).
 *
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

#include "core/export.h"
#include "core/types.h"

#include <sys/types.h>
#include <wchar.h>

#ifdef __cplusplus
extern "C" {
#endif

BM_EXPORT FILE* os_wfopen(const wchar_t* path, const char* mode);
BM_EXPORT FILE* os_fopen(const char* path, const char* mode);
BM_EXPORT s64 os_fgetsize(FILE* file);

#ifdef _WIN32
BM_EXPORT int os_stat(const char* file, struct stat* st);
#else
#define os_stat stat
#endif

BM_EXPORT int os_fseek_s64(FILE* file, s64 offset, int origin);
BM_EXPORT s64 os_ftell_s64(FILE* file);

BM_EXPORT size_t os_fread_utf8(FILE* file, char** pstr);
BM_EXPORT char* os_quick_read_utf8_file(const char* path);

BM_EXPORT s64 os_get_file_size(const char* path);

BM_EXPORT size_t os_utf8_to_wcs(const char* str, size_t len, wchar_t* dst,
				    size_t dst_size);
BM_EXPORT size_t os_utf8_to_wcs_ptr(const char* str, size_t len,
					wchar_t** pstr);
BM_EXPORT size_t os_wcs_to_utf8(const wchar_t* str, size_t len, char* dst,
				    size_t dst_size);
BM_EXPORT size_t os_wcs_to_utf8_ptr(const wchar_t* str, size_t len,
					char** pstr);

BM_EXPORT void os_sleep_ms(const u32 duration);
BM_EXPORT u64 os_get_time_ns(void);
BM_EXPORT f64 os_get_time_sec(void);
BM_EXPORT f64 os_get_time_msec(void);

BM_EXPORT void* os_dlopen(const char* path);
BM_EXPORT void* os_dlsym(void* module, const char* func);
BM_EXPORT void os_dlclose(void* module);

BM_EXPORT bool os_file_exists(const char* path);

BM_EXPORT long os_atomic_inc_long(volatile long* val);
BM_EXPORT long os_atomic_dec_long(volatile long* val);
BM_EXPORT long os_atomic_set_long(volatile long *ptr, long val);
BM_EXPORT long os_atomic_exchange_long(volatile long *ptr, long val);

#ifdef __cplusplus
}
#endif
