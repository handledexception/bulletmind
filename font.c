#include "font.h"
#include "imgfile.h"

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

static imgfile_t *bitmap_font = NULL;
static SDL_Texture *fontex = NULL;

bool font_init(void *ren, const char *path)
{
	bitmap_font = (imgfile_t *)malloc(sizeof(imgfile_t));
	bool loaded = imgfile_init(path, bitmap_font);
	if (loaded == false) {
		printf("font_init: imgfile_init failed loading font: %s\n", path);
		return false;
	}

	fontex = SDL_CreateTexture(
							(SDL_Renderer *)ren, 
							SDL_PIXELFORMAT_BGR24, 
							SDL_TEXTUREACCESS_STATIC, 
							bitmap_font->width, 
							bitmap_font->height);
	if (fontex == NULL) {
		printf("font_init: failed to create texture for font!\n");
		return false;
	}

	SDL_UpdateTexture(fontex, NULL, bitmap_font->data, bitmap_font->stride);
	return true;
}

void font_print(void *ren, int32_t x, int32_t y, float scale, const char *str, ...)
{
	va_list args;
	char text[256];
	int32_t c = 0;	
	int32_t fx, tu, tv;	
	
	if (*str == '\0') {
		return;
	}
	
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
			SDL_RenderCopy((SDL_Renderer *)ren, fontex, &src, &dst);
			x += FONT_PX * scale;
		}		
		c++;
	}
}

void font_shutdown()
{
	if (fontex) {
		SDL_DestroyTexture(fontex);
		fontex = NULL;
	}
	if (bitmap_font) {
		imgfile_shutdown(bitmap_font);
		bitmap_font = NULL;
	}
	printf("font_shutdown: OK!\n");
	return;
}