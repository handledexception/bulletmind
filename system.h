#ifndef H_SYSTEM
#define H_SYSTEM

#include "c99defs.h"
#include "timing.h"
#include "vector.h"
#include <stdio.h>
#include <SDL.h>

#define ARRAY_SIZE(x) sizeof(x) / sizeof(x[0])
#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

typedef enum {
	ES_STARTUP,
	ES_PLAY,
	ES_QUIT
} engine_state_t;

typedef struct {
	SDL_Window *window;
	SDL_Renderer *renderer;
	engine_state_t state;
	int32_t scr_width, scr_height;
	recti32_t scr_bounds;
	double target_frametime;
	int32_t frame_count;
} engine_t;

extern int32_t mousex, mousey;

uint8_t sys_init(engine_t *eng);
void sys_refresh();
void sys_shutdown(engine_t *eng);

#endif