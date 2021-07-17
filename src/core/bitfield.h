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
