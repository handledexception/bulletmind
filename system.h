#ifndef H_SYSTEM
#define H_SYSTEM

#include "c99defs.h"

#define WINDOW_WIDTH 960
#define WINDOW_HEIGHT 540
#define WINDOW_WIDTH_HALF WINDOW_WIDTH/2
#define WINDOW_HEIGHT_HALF WINDOW_HEIGHT/2

uint8_t sys_init(engine_t *eng);
void sys_refresh();
void sys_shutdown(engine_t *eng);

#endif