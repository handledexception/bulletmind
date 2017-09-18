#ifndef H_SYSTEM
#define H_SYSTEM

#include "c99defs.h"
#include "timing.h"
#include <stdio.h>
#include <SDL.h>

#define ARRAY_SIZE(x) sizeof(x) / sizeof(x[0])
#define TARGET_FPS 60
#define TARGET_FRAMETIME(f) 1/(f) * 1000

typedef enum {
	ES_STARTUP,
	ES_PLAY,
	ES_QUIT
} engine_state_t;

typedef struct {
	SDL_Window *window;
	SDL_Renderer *renderer;
	engine_state_t state;
    //double engine_time;
    uint32_t frame_count;
} engine_t;

inline void engine_lockfps(double frame_time, double fps)
{		
	double lock_time = frame_time;
	if (lock_time < (double)TARGET_FRAMETIME(fps)) {
		while (lock_time < (double)TARGET_FRAMETIME(fps)) {
			lock_time += 0.0001;
		}
	}
	//printf("frame time: %fms\n", lock_time);
}

uint8_t sys_init(engine_t *eng);
void sys_refresh();
void sys_shutdown(engine_t *eng);

#endif