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
#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

extern u32 pack_version(u8 maj, u8 min, u8 rev);
extern void version_string(const u32 version, char* ver_str);

/*
 * Example:
 * switch(enum_val) {
 *     ENUM_CASE_RETURN_STR(enum_val);
 * }
 */
#define ENUM_CASE_RETURN_STR(enum_name) \
	case (enum_name):               \
		return #enum_name;

#ifdef __cplusplus
}
#endif
