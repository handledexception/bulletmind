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

static inline u64 sec_to_msec_u64(f64 sec)
{
	return (u64)(sec * 1000.0);
}

static inline u64 sec_to_nsec_u64(f64 sec)
{
	return (u64)(sec * 1e+9);
}

static inline f64 nsec_to_sec_f64(const u64 nanos)
{
	return (f64)((f64)nanos / (f64)1000000000ULL);
}

static inline f64 nsec_to_msec_f64(const u64 nanos)
{
	return (f64)((f64)nanos / (f64)1000000);
}

static inline f64 nsec_to_usec_f64(const u64 nanos)
{
	return (f64)((f64)nanos / (f64)1000);
}

static inline u32 nsec_to_msec_u32(const u64 nanos)
{
	return (u32)(nanos / 1000000);
}
