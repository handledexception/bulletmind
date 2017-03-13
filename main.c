/* 		Bulletmind
		(c) 2017 Paul Hindt
		v0.1.030817a
*/

#include "bitwise.h"
#include "command.h"
#include "input.h"
#include "main.h"
#include "system.h"
#include "timing.h"
#include "vector.h"
#include <stdio.h>


void drawrect_centered(SDL_Renderer *rend, int32_t x, int32_t y, int32_t w, int32_t h, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{		
	SDL_Rect rec = { 
		x - (w / 2), 
		y - (w / 2), 
		w, h 
	};
	SDL_SetRenderDrawColor(rend, r, g, b, a);	
	SDL_RenderFillRect(rend, &rec);
}

#define MAX_ENTITY_CAPS 64

typedef enum {
	MOVER,
	SHOOTER,
	DESTROYABLE,
	COLLIDER,
	PLAYSOUND	
} entity_caps;

typedef struct {
	int32_t index;
	vec2f pos;
	vec2f heading;
	SDL_Rect rect;
	entity_caps caps[MAX_ENTITY_CAPS];
} entity_t;

int main(int argc, char *argv[])
{	
	engine_t *engine = (engine_t *)malloc(sizeof(engine_t));
	if (engine == NULL) { return -1; }	

	engine->state = ES_STARTUP;
	sys_init(engine);		
	// main loop
	engine->state = ES_PLAY;	
	while(engine->state != ES_QUIT) {
		double frame_start = timing_getmillisec();
		switch(engine->state) {
			case ES_STARTUP:
				break;
			case ES_PLAY:
				sys_refresh();
				
				SDL_SetRenderDrawColor(engine->renderer, 0x00, 0x00, 0x00, 0xFF);
				SDL_RenderClear(engine->renderer);
				
				drawrect_centered(engine->renderer, (WINDOW_WIDTH / 2), (WINDOW_HEIGHT / 2), 20, 20, 0xff, 0x00, 0x00, 0xff);
		
				if (cmd_getstate(CMD_QUIT) == true) { engine->state = ES_QUIT; }
				break;
			case ES_QUIT:
				break;
		}
		double frame_time = timing_getmillisec() - frame_start;
		engine_lockfps(frame_time, TARGET_FPS);
		SDL_RenderPresent(engine->renderer);	
	}
	sys_shutdown(engine);	
	free(engine);
	engine = NULL;
	return 0;
}