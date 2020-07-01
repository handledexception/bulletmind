#if defined(BM_WINDOWS)
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS 1
#endif
#endif

#include "sprite.h"
#include "font.h"
#include "resource.h"
#include "main.h"

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include <SDL.h>

#define FONT_BASE 32
#define FONT_NULL 127
#define FONT_PX 7
#define FONT_COLS 16
#define FONT_ROWS 6

void font_print(engine_t* eng, int32_t x, int32_t y, float scale, const char *str, ...) {
    va_list args;
    char text[TEMP_STRING_MAX];
    int32_t c = 0;
    int32_t fx, tu, tv;

    if (str == NULL)
        return;
    if (*str == '\0')
        return;

    va_start(args, str);
    vsprintf(text, str, args);
    va_end(args);

    //TODO(paulh): Need methods like "get_resource_by_name", etc.
    game_resource_t* rsrc = eng->game_resources[0];
    sprite_t* font_sprite = (sprite_t*)(rsrc->data);
    while (text[c] != '\0') {
        if (text[c] >= FONT_BASE && text[c] < FONT_NULL) {
            fx = text[c] - FONT_BASE;
            tu = (float)(fx % FONT_COLS) * FONT_PX;
            tv = (float)(fx / FONT_COLS) * FONT_PX;
            SDL_Rect src = { tu, tv, FONT_PX, FONT_PX };
            SDL_Rect dst = { x, y, FONT_PX * scale, FONT_PX * scale };
            SDL_RenderCopy((SDL_Renderer *)eng->renderer, font_sprite->texture, &src, &dst);
            x += FONT_PX * scale;
        }
        c++;
    }
}

#ifdef _CRT_SECURE_NO_WARNINGS
#undef _CRT_SECURE_NO_WARNINGS
#endif
