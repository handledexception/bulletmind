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

#ifdef __cplusplus
extern "C" {
#endif

struct memory_allocator {
	void* (*malloc)(size_t);
	void* (*realloc)(void*, size_t);
	void (*free)(void*);
};

BM_EXPORT void* bm_malloc(size_t size);
BM_EXPORT void* bm_realloc(void* ptr, size_t size);
BM_EXPORT void bm_free(void* ptr);

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

extern size_t arena_allocated_bytes;
extern u8* mem_arena_backing_buffer;
extern arena_t mem_arena;

BM_EXPORT void arena_init(arena_t* arena, void* backing_buffer, size_t sz_backing);
BM_EXPORT void arena_free_all(arena_t* arena);
BM_EXPORT void* arena_alloc(arena_t* arena, size_t size, size_t align);

#define bm_arena_alloc(_arena, _sz) arena_alloc(_arena, _sz, DEFAULT_ALIGNMENT)

#ifdef __cplusplus
}
#endif

#endif
