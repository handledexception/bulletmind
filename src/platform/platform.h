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

#include "core/export.h"
#include "core/types.h"

#include <sys/types.h>
#include <wchar.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	BM_ARCH_X86,
	BM_ARCH_AMD64,
	BM_ARCH_ARM32,
	BM_ARCH_ARM64,
	BM_ARCH_PPC32,
	BM_ARCH_PPC64,
	BM_ARCH_UNKNOWN
} architecture_t;

typedef enum {
	BM_COMPILER_GCC,
	BM_COMPILER_CLANG,
	BM_COMPILER_MSVC,
	BM_COMPILER_UNKNOWN
} compiler_t;

typedef enum {
	BM_PLATFORM_WINDOWS,
	BM_PLATFORM_DARWIN,
	BM_PLATFORM_LINUX,
	BM_PLATFORM_UNKNOWN
} platform_t;

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

BM_EXPORT bool os_path_exists(const char* path);

BM_EXPORT long os_atomic_inc_long(volatile long* val);
BM_EXPORT long os_atomic_dec_long(volatile long* val);
BM_EXPORT long os_atomic_set_long(volatile long* ptr, long val);
BM_EXPORT long os_atomic_exchange_long(volatile long* ptr, long val);

BM_EXPORT architecture_t os_get_architecture(void);
BM_EXPORT const char* os_architecture_to_string(architecture_t arch);
BM_EXPORT compiler_t os_get_compiler(void);
BM_EXPORT const char* os_compiler_to_string(compiler_t comp);
BM_EXPORT platform_t os_get_platform(void);
BM_EXPORT const char* os_platform_to_string(platform_t plat);

#ifdef __cplusplus
}
#endif
