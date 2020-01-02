#define _CRT_SECURE_NO_WARNINGS 1

#include "imgfile.h"
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

static struct img_file* bitmap_font = NULL;
static SDL_Texture *font_tex = NULL;

bool font_init(SDL_Renderer *ren, const char *path)
{
    bitmap_font = (img_file_t *)malloc(sizeof(img_file_t));
    if (!img_file_init(path, &bitmap_font)) {
        printf("font_init: img_file_init failed loading font: %s\n", path);
        return false;
    }

    font_tex = SDL_CreateTexture(
        (SDL_Renderer *)ren,
        SDL_PIXELFORMAT_BGR24,
        SDL_TEXTUREACCESS_STATIC,
        bitmap_font->width,
        bitmap_font->height
    );

    if (font_tex == NULL) {
        printf("font_init: failed to create texture for font!\n");
        return false;
    }

    SDL_UpdateTexture(
        font_tex,
        NULL,
        bitmap_font->data,
        bitmap_font->stride
    );

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
            SDL_RenderCopy((SDL_Renderer *)ren, font_tex, &src, &dst);
            x += FONT_PX * scale;
        }
        c++;
    }
}

void font_shutdown()
{
    if (font_tex) {
        SDL_DestroyTexture(font_tex);
        font_tex = NULL;
    }
    if (bitmap_font) {
        img_file_shutdown(bitmap_font);
        bitmap_font = NULL;
    }
    printf("font_shutdown: OK!\n");
    return;
}