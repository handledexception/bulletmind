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

#include "gui/key_state.h"

#include "core/types.h"

void gui_key_up_win32(int key, struct key_state* state)
{
    if (key < 0 || key > 0xfe)
        return;
    u32* ptr = (u32*)state;
    u32 bf = 1u << (key & 0x1f);
    ptr[(key >> 5)] |= bf;
}

void gui_key_down_win32(int key, struct key_state* state)
{
    if (key < 0 || key > 0xfe)
        return;
    u32* ptr = (u32*)state;
    u32 bf = 1u << (key & 0x1f);
    ptr[(key >> 5)] &= ~bf;
}
