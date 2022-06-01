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

// #define LOG_ALLOCATIONS
#define TRACK_MEMORY
#ifdef TRACK_MEMORY
static s32 live_alloc_count = 0;
static s32 total_alloc_count = 0;
static s32 live_bytes_allocated = 0;
static s32 total_bytes_allocated = 0;
static s32 total_free_count = 0;
static s32 total_bytes_freed = 0;
static s32 min_bytes_allocated = 0;
static s32 max_bytes_allocated = 0;
#endif

// extern from memory.h
size_t mem_arena_allocated_bytes = 0;
u8* mem_arena_backing_buffer = NULL;
arena_t mem_arena = {NULL, 0, 0, 0};

static void recalculate_allocs(size_t size)
{
#ifdef LOG_ALLOCATIONS
	logger(LOG_DEBUG, "MEM ALLOC %zu", size);
#endif
#ifdef TRACK_MEMORY
	os_atomic_inc_s32(&live_alloc_count);
	os_atomic_inc_s32(&total_alloc_count);
	os_atomic_set_s32(&total_bytes_allocated, size + total_bytes_allocated);
	size_t new_size = live_bytes_allocated + size;
	os_atomic_set_s32(&live_bytes_allocated, new_size);
	if (new_size > max_bytes_allocated) {
		os_atomic_set_s32(&max_bytes_allocated,
				   new_size);
	}
	if (min_bytes_allocated == 0) {
		os_atomic_set_s32(&min_bytes_allocated,
				   live_bytes_allocated);
	}
#ifdef LOG_ALLOCATIONS
	logger(LOG_DEBUG, "[allocs] allocated %zu bytes in %zu allocations", live_bytes_allocated, live_alloc_count);
#endif
#endif
}

static void recalculate_frees(size_t size)
{
#ifdef LOG_ALLOCS
	logger(LOG_DEBUG, "MEM FREE %zu", size);
#endif
#ifdef TRACK_MEMORY
	os_atomic_dec_s32(&live_alloc_count);
	os_atomic_inc_s32(&total_free_count);
	os_atomic_set_s32(&total_bytes_freed, total_bytes_freed + (s32)size);
	size_t new_size = live_bytes_allocated - size;
	os_atomic_set_s32(&live_bytes_allocated, new_size);
	if (live_bytes_allocated <= min_bytes_allocated) {
		os_atomic_set_s32(&min_bytes_allocated,
				   live_bytes_allocated);
	}
#ifdef LOG_ALLOCATIONS
	logger(LOG_DEBUG, "[frees] allocated %zu bytes in %zu allocations", live_bytes_allocated, live_alloc_count);
#endif
#endif
}

void* mem_alloc(size_t size, const char* func)
{
	void* ptr = NULL;
#ifdef TRACK_MEMORY
	// set size before ptr for later recall
	s32 alloc_size = size + sizeof(s32);
	ptr = allocator.malloc(alloc_size);
	*(s32*)(ptr) = alloc_size;
	*(u8**)(&ptr) += sizeof(s32);
	recalculate_allocs(alloc_size);
#else
	ptr = allocator.malloc(size);
#endif
	return ptr;
}

void* mem_realloc(void* ptr, size_t size)
{
	void* new_ptr = ptr;
#ifdef TRACK_MEMORY
	u8* curr_hdr = *(u8**)(&ptr) - sizeof(s32);
	s32 curr_size = *(s32*)(curr_hdr);
	s32 new_size = curr_size + size + sizeof(s32);
	new_ptr = allocator.malloc(new_size);
	recalculate_allocs(new_size);
	u8* new_hdr = *(u8**)(&new_ptr);
	*(s32*)(new_hdr) = new_size;
	*(u8**)(&new_ptr) += sizeof(s32);
	memcpy(new_ptr, ptr, curr_size);
	mem_free(ptr);
	recalculate_frees(curr_size);
#else
	allocator.realloc(new_ptr, size);
#endif
	return new_ptr;
}

void mem_free(void* ptr)
{
	if (ptr) {
#ifdef TRACK_MEMORY
		// retrieve size of allocation from ptr header
		u8* p = (u8*)ptr;
		s32* hdr = (s32*)(p)-1;
		s32 alloc_size = *hdr;
		s32 obj_size = alloc_size - sizeof(s32);
		p -= sizeof(s32);
		allocator.free(p);
		recalculate_frees(alloc_size);
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

int mem_report_leaks()
{
#ifdef TRACK_MEMORY
	int num_leaks = total_alloc_count - total_free_count;
	logger(LOG_INFO, "[Memory Leak Report]\n"\
		"Total Allocated: %d bytes (%d allocations)\n"
		"Total Freed: %d bytes (%d frees)\n"
		"Live Allocated: %d bytes (%d allocations)\n"
		"Max Allocated: %d bytes | Min Allocated: %d bytes\n"
		"Memory Leaks: %d",
		total_bytes_allocated, total_alloc_count,
		total_bytes_freed, total_free_count,
		live_bytes_allocated, live_alloc_count,
		max_bytes_allocated, min_bytes_allocated,
		num_leaks);
	assert(total_alloc_count == total_free_count);
	assert(total_bytes_allocated == total_bytes_freed);
	return num_leaks;
#endif
	return 0;
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
