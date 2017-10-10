/* 		Bulletmind
		(c) 2017 Paul Hindt
		v0.1.030817a
*/


#include "command.h"
#include "entity.h"
#include "imgfile.h"
#include "input.h"
#include "main.h"
#include "system.h"
#include "timing.h"
#include "vector.h"

#include <stdio.h>
#include <string.h>

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

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

	imgfile_t img;	
	imgfile_init(".\\assets\\7px_font.tga", &img);
	//imgfile_init(".\\assets\\colors.tga", &img);		
	SDL_Texture *tgatex = SDL_CreateTexture(engine->renderer, SDL_PIXELFORMAT_BGR24, SDL_TEXTUREACCESS_STATIC, img.width, img.height);	
	SDL_UpdateTexture(tgatex, NULL, img.data, img.stride);

	
	double dt = 0.0;
	recti32_t scr = { 0, 0, engine->cx, engine->cy };
	
	// main loop	
	while(engine->state != ES_QUIT) {		
		double frame_start = timing_getmillisec();		

		switch(engine->state) {
			case ES_STARTUP:
				break;
			case ES_PLAY:		

				SDL_RenderClear(engine->renderer);			
				SDL_SetRenderDrawColor(engine->renderer, 0x00, 0x00, 0x00, 0xFF);

				SDL_RenderCopy(engine->renderer, tgatex, NULL, NULL);				
		
				sys_refresh();				
				ent_refresh(engine->renderer, dt, &scr);
				
				if (cmd_getstate(CMD_QUIT) == true) { engine->state = ES_QUIT; }
				break;
			case ES_QUIT:
				break;
		}
		
		dt = timing_getmillisec() - frame_start;
		//engine_lockfps(engine_time, TARGET_FPS);
		
		SDL_RenderPresent(engine->renderer);
		engine->frame_count++;		
	}

	sys_shutdown(engine);	

	free(engine);
	engine = NULL;
	return 0;
}