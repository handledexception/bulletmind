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

#ifdef BM_WINDOWS
#include <emmintrin.h>
#endif

#define ALIGN_SIZE(size, align) size = (((size) + (align - 1)) & (~(align - 1)))

static struct memory_allocator allocator = {malloc, realloc, free};

// #define LOG_ALLOCATIONS

#ifdef TRACK_MEMORY
struct memory_stats {
	s32 live_alloc_count;
	s32 total_alloc_count;
	s32 live_bytes_allocated;
	s32 total_bytes_allocated;
	s32 total_free_count;
	s32 total_bytes_freed;
	s32 min_bytes_allocated;
	s32 max_bytes_allocated;
};
static struct memory_stats mem_stats = {0};
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
	os_atomic_inc_s32(&mem_stats.live_alloc_count);
	os_atomic_inc_s32(&mem_stats.total_alloc_count);
	os_atomic_set_s32(&mem_stats.total_bytes_allocated,
			  (s32)size + mem_stats.total_bytes_allocated);

	size_t new_size = mem_stats.live_bytes_allocated + size;
	os_atomic_set_s32(&mem_stats.live_bytes_allocated, (s32)new_size);

	if (mem_stats.live_bytes_allocated > mem_stats.max_bytes_allocated) {
		os_atomic_set_s32(&mem_stats.max_bytes_allocated,
				  (s32)mem_stats.live_bytes_allocated);
	}

	if (mem_stats.live_bytes_allocated < mem_stats.min_bytes_allocated ||
	    mem_stats.min_bytes_allocated == 0) {
		os_atomic_set_s32(&mem_stats.min_bytes_allocated,
				  (s32)mem_stats.live_bytes_allocated);
	}
#ifdef LOG_ALLOCATIONS
	logger(LOG_DEBUG, "[allocs] allocated %zu bytes in %zu allocations",
	       live_bytes_allocated, live_alloc_count);
#endif
#endif
}

static void recalculate_frees(size_t size)
{
#ifdef LOG_ALLOCATIONS
	logger(LOG_DEBUG, "MEM FREE %zu", size);
#endif
#ifdef TRACK_MEMORY
	os_atomic_dec_s32(&mem_stats.live_alloc_count);
	os_atomic_inc_s32(&mem_stats.total_free_count);
	os_atomic_set_s32(&mem_stats.total_bytes_freed,
			  mem_stats.total_bytes_freed + (s32)size);

	size_t new_size = mem_stats.live_bytes_allocated - size;
	os_atomic_set_s32(&mem_stats.live_bytes_allocated, (s32)new_size);
#ifdef LOG_ALLOCATIONS
	logger(LOG_DEBUG, "[frees] allocated %zu bytes in %zu allocations",
	       live_bytes_allocated, live_alloc_count);
#endif
#endif
}

void* mem_alloc(size_t size)
{
	void* ptr = NULL;
#ifdef TRACK_MEMORY
	// set size before ptr for later recall
	size_t alloc_size = size + sizeof(size_t);
	ptr = allocator.malloc(alloc_size);
	*(size_t*)(ptr) = alloc_size;
	*(u8**)(&ptr) += sizeof(size_t);
	recalculate_allocs(alloc_size);
#else
	ptr = allocator.malloc(size);
#endif
	return ptr;
}

void* mem_realloc(void* ptr, size_t size)
{
	if (ptr) {
#ifdef TRACK_MEMORY
		// rewind to retrieve size from header, then forward again to get data
		size_t* p = (size_t*)(ptr)-1;
		size_t alloc_size = *p;
		size_t obj_size = alloc_size - sizeof(size_t);
		p++;
		// calculate new size
		u8* new_p = BM_ALLOC(size);
		size_t copy_size = size;
		if (copy_size > obj_size)
			copy_size = obj_size;
		memcpy(new_p, p, copy_size);
		BM_FREE(p);
		ptr = (void*)new_p;
	} else {
		ptr = BM_ALLOC(size);
	}
#else
		ptr = allocator.realloc(new_ptr, size);
#endif
	return ptr;
}

void mem_free(void* ptr)
{
	if (ptr) {
#ifdef TRACK_MEMORY
		// retrieve size of allocation from ptr header
		u8* p = (u8*)ptr;
		size_t* hdr = (size_t*)(p)-1;
		size_t alloc_size = *hdr;
		size_t obj_size = alloc_size - sizeof(size_t);
		p -= sizeof(size_t);
		allocator.free(p);
		recalculate_frees(alloc_size);
#else
			allocator.free(ptr);
#endif
	}
}

void* mem_dupe(const void* ptr, size_t size)
{
	void* out = mem_alloc(size);
	if (size)
		memcpy(out, ptr, size);

	return out;
}

void mem_copy(void* dst, const void* src, size_t size)
{
	u8* to = (u8*)dst;
	u8* from = (u8*)src;
	while (size--)
		*to++ = *from++;
}

#ifdef BM_WINDOWS
void mem_copy_sse2(void* dst, void* src, size_t size)
{
	__m128i* from = (__m128i*)src;
	__m128i* to = (__m128i*)dst;
	size_t index = 0;
	while (size) {
		__m128i x = _mm_load_si128(&from[index]);
		_mm_stream_si128(&to[index], x);
		size -= 16;
		index++;
	}
}
#endif

int mem_report_leaks()
{
#ifdef TRACK_MEMORY
	int num_leaks =
		mem_stats.total_alloc_count - mem_stats.total_free_count;
	logger(LOG_INFO,
	       "[Memory Leak Report]\n"
	       "Total Allocated: %d bytes (%d allocations)\n"
	       "Total Freed: %d bytes (%d frees)\n"
	       "Live Allocated: %d bytes (%d allocations)\n"
	       "Max Allocated: %d bytes | Min Allocated: %d bytes\n"
	       "Memory Leaks: %d",
	       mem_stats.total_bytes_allocated, mem_stats.total_alloc_count,
	       mem_stats.total_bytes_freed, mem_stats.total_free_count,
	       mem_stats.live_bytes_allocated, mem_stats.live_alloc_count,
	       mem_stats.max_bytes_allocated, mem_stats.min_bytes_allocated,
	       num_leaks);
	assert(mem_stats.total_alloc_count == mem_stats.total_free_count);
	assert(mem_stats.total_bytes_allocated == mem_stats.total_bytes_freed);
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

int mem_base_alignment()
{
	return BASE_ALIGNMENT;
}
