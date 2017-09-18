/* 		Bulletmind
		(c) 2017 Paul Hindt
		v0.1.030817a
*/

//#include "bitwise.h"
#include "command.h"
#include "entity.h"
#include "input.h"
#include "main.h"
#include "system.h"
#include "timing.h"
#include "vector.h"

#include <stdio.h>
#include <string.h>

double engine_time = 0.0;

int main(int argc, char *argv[])
{		
	engine_t *engine = (engine_t *)malloc(sizeof(engine_t));
	if (engine == NULL) { return -1; }	
	
	if (sys_init(engine) == 0) {
		engine->state = ES_PLAY;
	}
	else {
		printf("sys_init failed!\n");
		return -1;
	}
	
	// main loop	
	while(engine->state != ES_QUIT) {		
		double frame_start = timing_getmillisec();

		switch(engine->state) {
			case ES_STARTUP:
				break;
			case ES_PLAY:
				sys_refresh();
				
				SDL_SetRenderDrawColor(engine->renderer, 0x00, 0x00, 0x00, 0xFF);
				SDL_RenderClear(engine->renderer);
				
				ent_refresh(engine->renderer);			
		
				if (cmd_getstate(CMD_QUIT) == true) { engine->state = ES_QUIT; }
				break;
			case ES_QUIT:
				break;
		}
		
		SDL_RenderPresent(engine->renderer);
		
		engine_time = timing_getmillisec() - frame_start;
		engine_lockfps(engine_time, TARGET_FPS);
		engine->frame_count++;
	}

	sys_shutdown(engine);	

	free(engine);
	engine = NULL;
	return 0;
}