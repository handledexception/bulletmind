#define _CRT_SECURE_NO_WARNINGS 1

#include "sprite.h"
#include "font.h"
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

static img_file_t* bitmap_font = NULL;

bool font_init(SDL_Renderer *ren, const char *path)
{
    if (!img_file_load(path, &bitmap_font)) {
        printf("font_init: img_file_load failed loading font: %s\n", path);
        return false;
    }

    img_file_create_texture(ren, bitmap_font);

    return true;
}

void font_print(SDL_Renderer* ren, int32_t x, int32_t y, float scale, const char *str, ...)
{
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

    while (text[c] != '\0') {
        if (text[c] >= FONT_BASE && text[c] < FONT_NULL) {
            fx = text[c] - FONT_BASE;
            tu = (float)(fx % FONT_COLS) * FONT_PX;
            tv = (float)(fx / FONT_COLS) * FONT_PX;
            SDL_Rect src = { tu, tv, FONT_PX, FONT_PX };
            SDL_Rect dst = { x, y, FONT_PX * scale, FONT_PX * scale };
            SDL_RenderCopy((SDL_Renderer *)ren, bitmap_font->texture, &src, &dst);
            x += FONT_PX * scale;
        }
        c++;
    }
}

void font_shutdown()
{
    if (bitmap_font) {
        img_file_shutdown(bitmap_font);
        bitmap_font = NULL;
    }
    printf("font_shutdown: OK!\n");
    return;
}