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

#include "core/buffer.h"
#include "core/memory.h"

buffer_t* buffer_new(size_t size)
{
	buffer_t* buf = (buffer_t*)BM_ALLOC(sizeof(*buf));
	buf->size = size;
	buf->capacity = size;
	buf->pos = 0;
	if (size > 0) {
		buf->data = (u8*)BM_ALLOC(size);
		memset(buf->data, 0, size);
	} else {
		buf->data = NULL;
	}
	return buf;
}

buffer_t* buffer_aligned_new(size_t size, size_t alignment)
{
	const size_t aligned_size = size + (alignment - 1) + sizeof(void*);
	buffer_t* buf = (buffer_t*)malloc(sizeof(buffer_t));

	buf->data = aligned_malloc(size, alignment);

	memset(buf->data, 0, aligned_size);

	buf->size = aligned_size;

	aligned_free(buf->data);

	return buf;
}

void buffer_free(buffer_t* buf)
{
	if (buf) {
		if (buf->data) {
			BM_FREE(buf->data);
			buf->data = NULL;
		}
		BM_FREE(buf);
		buf = NULL;
	}
}

void buffer_copy_data(buffer_t* buf, void* data, size_t size)
{
	if (!buf)
		return;

	size_t write_size = buf->pos + size;
	if (write_size >= buf->capacity) {
		size_t new_capacity = buf->capacity * 2;
		while (new_capacity <= write_size)
			new_capacity *= 2;
		buffer_resize(buf, new_capacity);
	}

	memcpy(buf->data + buf->pos, data, size);
	buf->size += size;
	buf->pos += size;
}

size_t buffer_resize(buffer_t* buf, size_t size)
{
	if (!buf)
		return 0;
	u8* data = (u8*)BM_REALLOC(buf->data, size);
	if (data == NULL)
		return 0;
	buf->data = data;
	buf->capacity = size;
	return size;
}

void buffer_seek(buffer_t* buf, size_t size, enum seek_dir seek)
{
	if (!buf)
		return;
	if (seek == SEEK_TO_START)
		buf->pos = 0;
	else if (seek == SEEK_TO_END)
		buf->pos = buf->size;
	else if (seek == SEEK_FORWARD)
		buf->pos += size;
	else if (seek == SEEK_REVERSE)
		buf->pos -= size;
}
