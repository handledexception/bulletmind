#ifndef _H_FONT
#define _H_FONT

#include "c99defs.h"

bool font_init(void *ren, const char *path);
void font_print(void *ren, int32_t x, int32_t y, float scale, const char *str, ...);
void font_shutdown();

#endif