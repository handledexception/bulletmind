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
#include "core/vector.h"

#include <sys/types.h>
#include <wchar.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct os_stack_frame {
	u64 address;
	u32 line_num;
	char func_name[BM_MAX_PATH];
	char module_name[BM_MAX_PATH];
	char file_name[BM_MAX_PATH];
} os_stack_frame_t;

typedef struct os_callstack {
	size_t frame_count;
	VECTOR(struct os_stack_frame) frames;
} os_callstack_t;

BM_EXPORT os_callstack_t* os_callstack_new();
BM_EXPORT void os_callstack_init(os_callstack_t* cs);
BM_EXPORT void os_callstack_free(os_callstack_t* cs);
BM_EXPORT bool os_callstack_read(os_callstack_t* cs);
/*
	// Example:
	os_callstack_t cs;
	os_callstack_init(&cs);
	os_callstack_read(&cs);
	char tmp[MAX_PATH];
	for (size_t i = 0; i < cs.frames.num_elems; i++) {
		struct os_stack_frame* f = &cs.frames.elems[i];
		printf("%s!%s Line %d %s\n",
			path_get_basename(f->module_name),
			f->func_name,
			f->line_num,
			path_get_basename(f->file_name));
	}
	vec_free(cs.frames);
*/

// file i/o & utf8
#ifdef _WIN32
BM_EXPORT int os_stat(const char* file, struct stat* st);
#else
#define os_stat stat
#endif
BM_EXPORT FILE* os_wfopen(const wchar_t* path, const char* mode);
BM_EXPORT FILE* os_fopen(const char* path, const char* mode);
BM_EXPORT s64 os_fgetsize(FILE* file);
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

// timing
BM_EXPORT void os_sleep_ms(const u32 duration);
BM_EXPORT u64 os_get_time_ns(void);
BM_EXPORT f64 os_get_time_sec(void);
BM_EXPORT f64 os_get_time_msec(void);

// dynamic library
BM_EXPORT void* os_dlopen(const char* path);
BM_EXPORT void* os_dlsym(void* module, const char* func);
BM_EXPORT void os_dlclose(void* module);

// path
BM_EXPORT bool os_path_exists(const char* path);

BM_EXPORT const char* os_architecture_string(void);
BM_EXPORT const char* os_compiler_string(void);
BM_EXPORT const char* os_platform_string(void);
BM_EXPORT const char* os_bits_string(void);

// 32-bit atomics
BM_EXPORT s32 os_atomic_inc_s32(volatile s32* ptr);
BM_EXPORT s32 os_atomic_dec_s32(volatile s32* ptr);
BM_EXPORT s32 os_atomic_set_s32(volatile s32* ptr, s32 val);
BM_EXPORT s32 os_atomic_get_s32(volatile s32* ptr);
BM_EXPORT s32 os_atomic_exchange_s32(volatile s32* ptr, s32 val);
BM_EXPORT bool os_atomic_compare_swap_s32(volatile s32* ptr, s32 old_val,
					  s32 new_val);
BM_EXPORT bool os_atomic_compare_exchange_s32(volatile s32* ptr, s32* old_ptr,
					      s32 new_val);
// 64-bit atomics
#if defined(BM_BITS_64)
BM_EXPORT s64 os_atomic_inc_s64(volatile s64* ptr);
BM_EXPORT s64 os_atomic_dec_s64(volatile s64* ptr);
BM_EXPORT s64 os_atomic_set_s64(volatile s64* ptr, s64 val);
BM_EXPORT s64 os_atomic_get_s64(volatile s64* ptr);
BM_EXPORT s64 os_atomic_exchange_s64(volatile s64* ptr, s64 val);
BM_EXPORT bool os_atomic_compare_swap_s64(volatile s64* ptr, s64 old_val,
					  s64 new_val);
BM_EXPORT bool os_atomic_compare_exchange_s64(volatile s64* ptr, s64* old_ptr,
					      s64 new_val);
#endif

#ifdef __cplusplus
}
#endif
