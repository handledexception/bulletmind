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
#include "core/mem_arena.h"
#include "core/mem_align.h"

#include <assert.h>

size_t arena_allocated_bytes = 0;
u8 arena_buf[ARENA_TOTAL_BYTES];
arena_t g_mem_arena = {NULL, 0, 0, 0};

void arena_init(arena_t *arena, void *backing_buffer, size_t sz_backing)
{
	arena->buffer = (u8 *)backing_buffer;
	arena->sz_buffer = sz_backing;
	arena->curr_offset = 0;
	arena->prev_offset = 0;
}

void arena_free_all(arena_t *arena)
{
	arena->curr_offset = 0;
	arena->prev_offset = 0;
}

void *arena_alloc(arena_t *arena, size_t size, size_t align)
{
	uintptr_t curr_ptr =
		(uintptr_t)arena->buffer + (uintptr_t)arena->curr_offset;
	uintptr_t offset = align_forward(curr_ptr, align);
	if (offset == 0)
		return NULL;

	offset -= (uintptr_t)arena->buffer;

	if (offset + size <= arena->sz_buffer) {
		void *ptr = &arena->buffer[offset];
		arena->prev_offset = offset;
		arena->curr_offset = offset + size;
		memset(ptr, 0, size);

		arena_allocated_bytes = arena->curr_offset;

#if defined(BM_DEBUG)
		// logger(LOG_DEBUG, "arena_alloc - this: %zu bytes | used: %zu bytes | remain: %zu bytes | arena size: %zu bytes\n",
		//        arena->curr_offset - arena->prev_offset,
		//        arena_allocated_bytes,
		//        ARENA_TOTAL_BYTES - arena_allocated_bytes,
		//        ARENA_TOTAL_BYTES);
#endif
		return ptr;
	} else {
		logger(LOG_ERROR, "Out of arena memory!\n");
	}

	return NULL;
}
