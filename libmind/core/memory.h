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

#ifndef H_BM_MEMORY
#define H_BM_MEMORY

#include "core/types.h"
#include "core/export.h"
#include "core/logger.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BASE_ALIGNMENT 32

#define BM_TRACK_MEMORY
// #define BM_LOG_MEMORY

struct memory_allocator {
	void* (*malloc)(size_t);
	void* (*realloc)(void*, size_t);
	void (*free)(void*);
};

BM_EXPORT int mem_base_alignment();

BM_EXPORT void* mem_alloc(size_t size);
BM_EXPORT void* mem_realloc(void* ptr, size_t size);
BM_EXPORT void mem_free(void* ptr);
BM_EXPORT void* mem_dupe(const void* ptr, size_t size);
BM_EXPORT void mem_copy(void* dst, const void* src, size_t size);
BM_EXPORT void mem_copy_sse2(void* dst, void* src, size_t size);
BM_EXPORT void mem_log_usage();
BM_EXPORT int mem_report_leaks();

BM_EXPORT bool is_power_of_two(uintptr_t x);
BM_EXPORT uintptr_t align_forward(uintptr_t ptr, size_t align);
BM_EXPORT void* aligned_malloc(size_t size, size_t alignment);
BM_EXPORT void aligned_free(void* ptr);

#define BM_FUNC_SIG __func__
#if defined(_MSC_VER)
#undef BM_FUNC_SIG
#define BM_FUNC_SIG __FUNCSIG__
#elif defined(__GNUC__)
#undef BM_FUNC_SIG
#define BM_FUNC_SIG __FUNCTION__
#endif

#if !defined(BM_ALLOC) && defined(BM_DEBUG) && defined(BM_TRACK_MEMORY)
	#ifdef BM_LOG_MEMORY
		#define BM_ALLOC(sz) \
			mem_alloc(sz); \
			printf("mem_alloc: %s (%zu bytes)\n", BM_FUNC_SIG, (size_t)sz);
	#else
		#define BM_ALLOC(sz) mem_alloc(sz)
	#endif
#else
	#define BM_ALLOC(sz) mem_alloc(sz)
#endif

#if !defined(BM_FREE) && defined(BM_DEBUG) && defined(BM_TRACK_MEMORY)
#ifdef BM_LOG_MEMORY
	#define BM_FREE(p) \
		do { \
			size_t sz = 0; \
			sz = *((size_t*)(p)-1) - sizeof(size_t); \
			mem_free(p); \
			printf("mem_free: %s (%zu bytes)\n", BM_FUNC_SIG, (size_t)sz); \
		} while (0);
#else
	#define BM_FREE(p) \
		do { \
			size_t sz = 0; \
			sz = *((size_t*)(p)-1) - sizeof(size_t); \
			mem_free(p); \
		} while (0);
#endif
#else
	#define BM_FREE(p) mem_free(p)
#endif

#if !defined(BM_REALLOC) && defined(BM_DEBUG) && defined(BM_TRACK_MEMORY)
#ifdef BM_LOG_MEMORY
	#define BM_REALLOC(p, sz) \
		mem_realloc(p, sz); \
		printf("mem_realloc: %s (%zu bytes)\n", BM_FUNC_SIG, (size_t)sz);
#else
	#define BM_REALLOC(p, sz) mem_realloc(p, sz)
#endif
#else
	#define BM_REALLOC(p, sz) mem_realloc(p, sz)
#endif

// Basic linear allocator
// https://www.gingerbill.org/article/2019/02/08/memory-allocation-strategies-002/

#ifndef DEFAULT_ALIGNMENT
#define DEFAULT_ALIGNMENT (2 * sizeof(void*))
#endif

#define ARENA_TOTAL_BYTES 16777216 // 16MiB

typedef struct arena_s {
	u8* buffer;
	size_t sz_buffer;
	size_t prev_offset;
	size_t curr_offset;
} arena_t;

extern size_t mem_arena_allocated_bytes;
extern u8* mem_arena_backing_buffer;
extern arena_t mem_arena;

BM_EXPORT void mem_arena_init(arena_t* arena, void* backing_buffer,
			      size_t sz_backing);
BM_EXPORT void mem_arena_free(arena_t* arena);
BM_EXPORT void* mem_arena_alloc(arena_t* arena, size_t size, size_t align);

#define bm_mem_arena_alloc(_arena, _sz) \
	mem_arena_alloc(_arena, _sz, DEFAULT_ALIGNMENT)

#ifdef __cplusplus
}
#endif

#endif
