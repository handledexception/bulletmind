/*
 * Portions of this code:
 *
 * Copyright (c) 2013 Hugh Bailey <obs.jim@gmail.com>
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

#include "platform/platform.h"

#include <unistd.h>

void os_sleep_ms(const u32 duration)
{
	usleep(duration * 1000);
}

bool os_path_exists(const char* path)
{
	return access(path, F_OK) == 0;
}

long os_atomic_inc_long(volatile long* val)
{
	return __atomic_add_fetch(val, 1, __ATOMIC_SEQ_CST);
}

long os_atomic_dec_long(volatile long* val)
{
	return __atomic_sub_fetch(val, 1, __ATOMIC_SEQ_CST);
}

void os_atomic_store_long(volatile long* ptr, long val)
{
	__atomic_store_n(ptr, val, __ATOMIC_SEQ_CST);
}

long os_atomic_set_long(volatile long* ptr, long val)
{
	return __atomic_exchange_n(ptr, val, __ATOMIC_SEQ_CST);
}

long os_atomic_exchange_long(volatile long* ptr, long val)
{
	return os_atomic_set_long(ptr, val);
}

long os_atomic_load_long(const volatile long* ptr)
{
	return __atomic_load_n(ptr, __ATOMIC_SEQ_CST);
}

bool os_atomic_compare_swap_long(volatile long* val, long old_val, long new_val)
{
	return __atomic_compare_exchange_n(val, &old_val, new_val, false,
					   __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
}

bool os_atomic_compare_exchange_long(volatile long* val, long* old_val,
				     long new_val)
{
	return __atomic_compare_exchange_n(val, old_val, new_val, false,
					   __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
}

void os_atomic_store_bool(volatile bool* ptr, bool val)
{
	__atomic_store_n(ptr, val, __ATOMIC_SEQ_CST);
}

bool os_atomic_set_bool(volatile bool* ptr, bool val)
{
	return __atomic_exchange_n(ptr, val, __ATOMIC_SEQ_CST);
}

bool os_atomic_exchange_bool(volatile bool* ptr, bool val)
{
	return os_atomic_set_bool(ptr, val);
}

bool os_atomic_load_bool(const volatile bool* ptr)
{
	return __atomic_load_n(ptr, __ATOMIC_SEQ_CST);
}
