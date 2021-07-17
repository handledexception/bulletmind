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

#pragma once

#include "core/c99defs.h"
#include "core/types.h"

typedef enum {
	SEEK_ORIGIN_BEGIN,
	SEEK_ORIGIN_CURRENT,
	SEEK_ORIGIN_END,
} seek_origin_t;

typedef struct stream_s {
	u8* data;
	size_t size;
	size_t position;
} stream_t;

bool bin_stream_init(stream_t** stream, u8* data, const size_t size);
void bin_stream_shutdown(stream_t* stream);
bool bin_stream_seek(stream_t* stream, const seek_origin_t origin,
		     const i32 offset);
bool bin_stream_write(stream_t* stream, u8* data, const size_t size,
		      size_t* bytes_written);
// void bin_write_i8(stream_t* stream, const i8 value, const seek_origin_t origin);
// void binary_writer_write_i16(stream_t* stream, const i16 value);
// void binary_writer_write_i32(stream_t* stream, const i32 value);
// void binary_writer_write_i64(stream_t* stream, const i64 value);
