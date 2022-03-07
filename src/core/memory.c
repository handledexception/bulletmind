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
#include "core/logger.h"
#include "core/memory.h"
#include "core/mem_align.h"
#include "platform/platform.h"

#include <assert.h>
#include <malloc.h>

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
		os_atomic_set_long((long*)&max_bytes_allocated, (long)bytes_allocated);
	if (bytes_allocated <= min_bytes_allocated || min_bytes_allocated == 0)
		os_atomic_set_long((long*)&min_bytes_allocated, (long)bytes_allocated);
#else
	(void)new_size;
#endif
}

void* mem_alloc(size_t size)
{
	void* ptr = NULL;
#ifdef BM_TRACK_MEMORY_USAGE
	// set size before ptr for later recall
	size_t alloc_size = size + sizeof(size_t);
	ptr = allocator.malloc(alloc_size);
	*(size_t*)(ptr) = alloc_size;
	*(u8**)(&ptr) += sizeof(size_t);
	recalculate_usage(bytes_allocated + alloc_size);
	os_atomic_inc_long((long*)&alloc_count);
#else
	ptr = allocator.malloc(size);
#endif
	return ptr;
}

void* mem_realloc(void* ptr, size_t size)
{
	if (!ptr)
		os_atomic_inc_long((long*)&alloc_count);
#ifdef BM_TRACK_MEMORY_USAGE
		ptr = mem_alloc(sizeof(size_t));
		u8* p = (u8*)ptr;
		size_t* hdr = (size_t*)(p) - 1;
		size_t sz_hdr = *hdr;
#endif
		allocator.realloc(ptr, size + sz_hdr);
#ifdef BM_TRACK_MEMORY_USAGE
		size_t realloc_size = sz_hdr - size;
		size_t new_bytes_allocated = bytes_allocated + realloc_size;
		recalculate_usage(new_bytes_allocated);
#endif
}

void mem_free(void* ptr)
{
	if (ptr) {
#ifdef BM_TRACK_MEMORY_USAGE
		// retrieve size of allocation from ptr header
		u8* p = (u8*)ptr;
		size_t* hdr = (size_t*)(p) - 1;
		size_t alloc_size = *hdr;
		size_t obj_size = alloc_size - sizeof(size_t);
		p -= sizeof(size_t);
		allocator.free(p);
		recalculate_usage(bytes_allocated - alloc_size);
		os_atomic_dec_long((long*)&alloc_count);
#else
		allocator.free(ptr);
#endif
	}
}

static void log_memory_usage()
{
#ifdef BM_TRACK_MEMORY_USAGE
	logger(LOG_INFO,  "Current: %zu | Max: %zu | Min: %zu | Objects: %zu",
		bytes_allocated, max_bytes_allocated, min_bytes_allocated, alloc_count);
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
		logger(LOG_ERROR,  "Out of arena memory!\n");
	}

	return NULL;
}
