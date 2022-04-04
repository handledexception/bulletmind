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

#include "font.h"
#include "engine.h"

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include <SDL.h>

#define ASCII_BASE 32
#define ASCII_NULL 127
#define FONT_CEL_SIZE_PX 7
#define FONT_NUM_COLS 16
#define FONT_NUM_ROWS 6

void font_print(engine_t* eng, s32 x, s32 y, f32 scale, const char* str, ...)
{
	va_list args;
	char text[TEMP_STRING_MAX];
	s32 c = 0;
	s32 fx, tu, tv;

	if (str == NULL)
		return;
	if (*str == '\0')
		return;

	va_start(args, str);
	vsprintf(text, str, args);
	va_end(args);

	while (text[c] != '\0') {
		if (text[c] >= ASCII_BASE && text[c] < ASCII_NULL) {
			fx = text[c] - ASCII_BASE;
			tu = (s32)((f32)(fx % FONT_NUM_COLS) *
				   FONT_CEL_SIZE_PX);
			tv = (s32)((f32)(fx / FONT_NUM_COLS) *
				   FONT_CEL_SIZE_PX);
			SDL_Rect src = {tu, tv, FONT_CEL_SIZE_PX,
					FONT_CEL_SIZE_PX};
			SDL_Rect dst = {x, y, (int)(FONT_CEL_SIZE_PX * scale),
					(int)(FONT_CEL_SIZE_PX * scale)};
			// SDL_RenderCopy((SDL_Renderer*)eng->renderer,
			// 	       eng->font.sprite->texture, &src, &dst);
			x += (s32)((f32)FONT_CEL_SIZE_PX * scale);
		}
		c++;
	}
}
