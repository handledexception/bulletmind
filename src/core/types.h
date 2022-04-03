/*
 * Copyright (c) 2021 Paul Hindt
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

#ifndef H_BM_CORE_TYPES
#define H_BM_CORE_TYPES

#ifndef NOMINMAX /* don't define min() and max(). */
#define NOMINMAX
#endif

// #if defined(_MSC_VER)
// #define inline __inline
// #endif

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#ifndef BM_MAX_PATH
#if defined(_WIN32)
#define BM_MAX_PATH 256
#elif defined(__APPLE__)
#define BM_MAX_PATH 1024
#elif defined(__linux__)
#define BM_MAX_PATH 4096
#endif
#endif

#define TEMP_STRING_MAX 4096

typedef char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;
typedef long long s64;
typedef unsigned long long u64;
typedef float f32;
typedef double f64;

typedef enum {
	RESULT_OK = 0,
	RESULT_ERROR = 1,
	RESULT_IO_ERROR = 2,
	RESULT_NOT_FOUND = 3,
	RESULT_NULL = 4,
	RESULT_OOM = 5,
	RESULT_NOT_IMPL = 6,
	RESULT_UNKNOWN
} result;

#define UNUSED_PARAMETER(param) (void)param

#include "core/rect.h"
#include "core/video.h"

#endif
