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

#include "core/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SET_FLAG(flags, bit_idx) flags |= (1 << bit_idx)
#define CLEAR_FLAG(flags, bit_idx) flags &= ~(1 << bit_idx)
#define TOGGLE_FLAG(flags, bit_idx) flags ^= (1 << bit_idx)
#define IS_FLAG_SET(flags, bit_idx) (flags & (1 << bit_idx))

inline const char* byte_to_bin(u8 x)
{
	static char b[9];
	b[0] = '\0';

	u8 z;
	char* p = b;
	for (z = 128; z > 0; z >>= 1) {
		u8 result = (x & z);
		// printf("(%d & %d) = %d\n", x, z, result);
		*p++ = result ? '1' : '0';
		/* if ((x & z) > 0) {
            *p++ = '1';
        } else {
            *p++ = '0';
        } */
	}

	return b;
}

inline const u64 bit_round(u64 val)
{
	if ((val & (val - 1)) == 0)
		return val;
	while (val & (val - 1))
		val &= val - 1;
	return val << 1;
}

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
		     const s32 offset);
bool bin_stream_write(stream_t* stream, u8* data, const size_t size,
		      size_t* bytes_written);
// void bin_write_s8(stream_t* stream, const s8 value, const seek_origin_t origin);
// void binary_writer_write_s16(stream_t* stream, const s16 value);
// void binary_writer_write_s32(stream_t* stream, const s32 value);
// void binary_writer_write_s64(stream_t* stream, const s64 value);

#ifdef __cplusplus
}
#endif
