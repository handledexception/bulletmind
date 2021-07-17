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

#include "core/binary.h"

bool bin_stream_init(stream_t** stream, u8* data, const size_t size)
{
	*stream = NULL;
	*stream = (stream_t*)malloc(sizeof(stream_t));
	if (*stream == NULL) {
		free(*stream);
		return false;
	}

	stream_t* ptr = *stream;
	ptr->data = data;
	ptr->size = size;
	ptr->position = size;

	return true;
}

void bin_stream_shutdown(stream_t* stream)
{
	if (stream)
		free((void*)stream);
}

bool bin_stream_seek(stream_t* stream, const seek_origin_t origin,
		     const i32 offset)
{
	if (!stream)
		return false;

	if (origin == SEEK_ORIGIN_BEGIN) {
		if (offset <= stream->size && offset >= 0)
			stream->position = offset;
		else
			return false;
	} else if (origin == SEEK_ORIGIN_CURRENT) {
		const size_t new_pos = (size_t)((i32)stream->position + offset);
		if (new_pos <= stream->size && new_pos >= 0)
			stream->position = new_pos;
		else
			return false;
	} else if (origin == SEEK_ORIGIN_END) {
		const size_t new_pos = stream->size + offset;
		if (new_pos <= stream->size && new_pos >= 0)
			stream->position = new_pos;
		else
			return false;
	}

	return true;
}

bool bin_stream_write(stream_t* stream, u8* data, const size_t size,
		      size_t* bytes_written)
{
	if (!stream)
		return false;
	if (size > stream->size)
		return false;

	u8* data_ptr = &stream->data[stream->position];
	if (!data_ptr)
		return false;

	const size_t new_pos = stream->position + size;
	if (new_pos > stream->size)
		return false;

	memcpy((void*)data_ptr, (const void*)data, size);

	stream->position = new_pos;

	*bytes_written = size;

	return true;
}
