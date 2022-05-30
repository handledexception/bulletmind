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
#include "core/algorithm.h"
#include "core/logger.h"
#include "core/memory.h"
#include "core/mem_align.h"
#include "platform/platform.h"

#include <assert.h>
#include <malloc.h>
#include <emmintrin.h>
static struct memory_allocator allocator = {malloc, realloc, free};

#define BM_TRACK_MEMORY_USAGE 1
#ifdef BM_TRACK_MEMORY_USAGE
static uint64_t alloc_count = 0;
static uint64_t bytes_allocated = 0;
static uint64_t min_bytes_allocated = 0;
static uint64_t max_bytes_allocated = 0;
#endif

// extern from memory.h
size_t mem_arena_allocated_bytes = 0;
u8* mem_arena_backing_buffer = NULL;
arena_t mem_arena = {NULL, 0, 0, 0};

static void recalculate_usage(size_t new_size)
{
#ifdef BM_TRACK_MEMORY_USAGE
	os_atomic_set_long((long*)&bytes_allocated, (long)new_size);
	if (bytes_allocated > max_bytes_allocated)
		os_atomic_set_long((long*)&max_bytes_allocated,
				   (long)bytes_allocated);
	if (bytes_allocated <= min_bytes_allocated || min_bytes_allocated == 0)
		os_atomic_set_long((long*)&min_bytes_allocated,
				   (long)bytes_allocated);
	logger(LOG_DEBUG, "memory: allocated %zu bytes in %zu allocations", bytes_allocated, alloc_count);
#else
	(void)new_size;
#endif
}

void* mem_alloc(size_t size)
{
	void* ptr = NULL;
#ifdef BM_TRACK_MEMORY_USAGE
	// set size before ptr for later recall
	u64 alloc_size = size + sizeof(u64);
	ptr = allocator.malloc(alloc_size);
	*(u64*)(ptr) = alloc_size;
	*(u8**)(&ptr) += sizeof(u64);
	os_atomic_inc_long((long*)&alloc_count);
	recalculate_usage(bytes_allocated + alloc_size);
#else
	ptr = allocator.malloc(size);
#endif
	return ptr;
}

void* mem_realloc(void* ptr, size_t size)
{
#ifdef BM_TRACK_MEMORY_USAGE
	if (!ptr)
		os_atomic_inc_long((long*)&alloc_count);
	u8* curr_hdr = *(u8**)(&ptr) - sizeof(u64);
	u64 curr_size = *(u64*)(curr_hdr);
	u64 new_size = curr_size + size + sizeof(u64);
	u8* new_ptr = allocator.malloc(new_size);
	u8* new_hdr = *(u8**)(&new_ptr);
	*(u64*)(new_hdr) = new_size;
	*(u8**)(&new_ptr) += sizeof(u64);
	memcpy(new_ptr, ptr, curr_size);
	mem_free(ptr);
#else
	allocator.realloc(ptr, size);
#endif
#ifdef BM_TRACK_MEMORY_USAGE
	// size_t realloc_size = sz_hdr - size;
	size_t new_bytes_allocated = bytes_allocated + new_size;
	recalculate_usage(new_bytes_allocated);
#endif
	return new_ptr;
}

void mem_free(void* ptr)
{
	if (ptr) {
#ifdef BM_TRACK_MEMORY_USAGE
		// retrieve size of allocation from ptr header
		u8* p = (u8*)ptr;
		u64* hdr = (u64*)(p)-1;
		u64 alloc_size = *hdr;
		u64 obj_size = alloc_size - sizeof(u64);
		p -= sizeof(u64);
		allocator.free(p);
		os_atomic_dec_long((long*)&alloc_count);
		recalculate_usage(bytes_allocated - alloc_size);
#else
		allocator.free(ptr);
#endif
	}
}

void mem_copy(void* dst, void* src, size_t size)
{
	u8* to = (u8*)dst;
	u8* from = (u8*)src;
	while (size--)
		*to++ = *from++;
}

void mem_copy_sse2(void* dst, void* src, size_t size)
{
	__m128i *from = (__m128i *)src;
	__m128i *to = (__m128i *)dst;
	size_t index = 0;
	while(size) {
		__m128i x = _mm_load_si128(&from[index]);
		_mm_stream_si128(&to[index], x);
		size -= 16;
		index++;
	}
}

void mem_log_usage()
{
#ifdef BM_TRACK_MEMORY_USAGE
	logger(LOG_INFO, "Current: %zu | Max: %zu | Min: %zu | Objects: %zu",
	       bytes_allocated, max_bytes_allocated, min_bytes_allocated,
	       alloc_count);
#endif
}

//
// memory arena
//
void mem_arena_init(arena_t* arena, void* backing_buffer, size_t sz_backing)
{
	arena->buffer = (u8*)backing_buffer;
	arena->sz_buffer = sz_backing;
	arena->curr_offset = 0;
	arena->prev_offset = 0;
}

void mem_arena_free(arena_t* arena)
{
	arena->curr_offset = 0;
	arena->prev_offset = 0;
}

void* mem_arena_alloc(arena_t* arena, size_t size, size_t align)
{
	uintptr_t curr_ptr =
		(uintptr_t)arena->buffer + (uintptr_t)arena->curr_offset;
	uintptr_t offset = align_forward(curr_ptr, align);
	if (offset == 0)
		return NULL;

	offset -= (uintptr_t)arena->buffer;

	if (offset + size <= arena->sz_buffer) {
		void* ptr = &arena->buffer[offset];
		arena->prev_offset = offset;
		arena->curr_offset = offset + size;
		memset(ptr, 0, size);

		mem_arena_allocated_bytes = arena->curr_offset;

		logger(LOG_DEBUG,
		       "mem_arena_alloc - this: %zu bytes | used: %zu bytes | remain: %zu bytes | arena size: %zu bytes\n",
		       arena->curr_offset - arena->prev_offset,
		       mem_arena_allocated_bytes,
		       (size_t)ARENA_TOTAL_BYTES - mem_arena_allocated_bytes,
		       (size_t)ARENA_TOTAL_BYTES);
		return ptr;
	} else {
		logger(LOG_ERROR, "Out of arena memory!\n");
	}

	return NULL;
}
