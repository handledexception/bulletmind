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

#include "core/mem_align.h"

#include <assert.h>
#include <stdlib.h>

#if defined(BM_WINDOWS)
#define ALIGNED_MALLOC 1
#endif

bool is_power_of_two(uintptr_t x)
{
	return (x & (x - 1)) == 0;
}

uintptr_t align_forward(uintptr_t ptr, size_t align)
{
	uintptr_t p, a, modulo;

	assert(is_power_of_two(align));
	if (!is_power_of_two(align))
		return 0;

	p = ptr;
	a = (uintptr_t)align;
	modulo = p & (a - 1);

	if (modulo != 0)
		p += a - modulo;

	return p;
}

//https://codeyarns.github.io/tech/2017-02-28-aligned-memory-allocation.html
void* aligned_malloc(size_t size, size_t alignment)
{
#ifdef ALIGNED_MALLOC
	return _aligned_malloc(size, alignment);
#else
	const size_t alloc_size = size + (alignment - 1) + sizeof(void*);
	void* block = malloc(alloc_size);
	uintptr_t ptr = align_forward((uintptr_t)block, alignment);
	return (void*)ptr;
#endif
}

void aligned_free(void* ptr)
{
#ifdef ALIGNED_MALLOC
	_aligned_free(ptr);
#else
	void* p1 =
		((void**)ptr)[-1]; // get the pointer to the buffer we allocated
	free(p1);
#endif
}
