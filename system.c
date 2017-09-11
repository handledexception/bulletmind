#include "main.h"
#include "system.h"
#include "input.h"
#include "command.h"
#include "entity.h"
//#include "timing.h"

#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>

uint8_t sys_init(engine_t *eng)
{	
	eng->state = ES_STARTUP;
	double init_start = timing_getmillisec();
	eng->frame_count = 0;

	SDL_Init(SDL_INIT_EVERYTHING);
	eng->window = SDL_CreateWindow
	(
		"SDLGame", 
		SDL_WINDOWPOS_CENTERED, 
		SDL_WINDOWPOS_CENTERED,
		WINDOW_WIDTH, WINDOW_HEIGHT,
		SDL_WINDOW_SHOWN
	);
	
	if(!eng->window) {
		printf("sys_window: %s\n", SDL_GetError());
		return -1;
	}
	
	eng->renderer = SDL_CreateRenderer(eng->window, -1, SDL_RENDERER_ACCELERATED);
	if (!eng->renderer) {
		printf("sys_renderer: %s\n", SDL_GetError());
		return -1;
	}

	in_init();
	cmd_init();
	ent_init();
	printf("sys_init OK [%fms]\n", timing_getmillisec() - init_start);
	return 0;
}

void sys_refresh()
{
	SDL_Event e;
	
	while (SDL_PollEvent(&e)) {
		switch(e.type) {
			case SDL_KEYDOWN:
				in_setkeystate(e.key.keysym.scancode, e.key.state);
				break;
			case SDL_KEYUP:
				in_setkeystate(e.key.keysym.scancode, e.key.state);
				break;
			case SDL_MOUSEBUTTONDOWN:
				//printf("%d\n", SDL_BUTTON(e.button.button));
				in_setmousebuttonstate((e.button.button), e.button.state);
				break;
			case SDL_MOUSEBUTTONUP:
				in_setmousebuttonstate((e.button.button), e.button.state);
				break;			
		}		
	}
	
	if (cmd_getstate(CMD_PLAYER_UP) == true) { printf("sys_refresh - CMD_PLAYER_UP triggered!\n"); }
	if (cmd_getstate(CMD_PLAYER_DOWN) == true) { printf("sys_refresh - CMD_PLAYER_DOWN triggered!\n"); }
	if (cmd_getstate(CMD_PLAYER_LEFT) == true) { printf("sys_refresh - CMD_PLAYER_LEFT triggered!\n"); }
	if (cmd_getstate(CMD_PLAYER_RIGHT) == true) { printf("sys_refresh - CMD_PLAYER_RIGHT triggered!\n"); }
	if (cmd_getstate(CMD_PLAYER_PRIMARY_FIRE) == true) { printf("sys_refresh - CMD_PLAYER_PRIMARY_FIRE triggered!\n"); }
	if (cmd_getstate(CMD_PLAYER_ALTERNATE_FIRE) == true) { printf("sys_refresh - CMD_PLAYER_ALTERNATE_FIRE triggered!\n"); }
}

void sys_shutdown(engine_t *eng)
{
	ent_shutdown();
	cmd_shutdown();
	in_shutdown();

	SDL_DestroyRenderer(eng->renderer);
	SDL_DestroyWindow(eng->window);
	eng->renderer = NULL;
	eng->window = NULL;
	SDL_Quit();
	printf("sys_shutdown OK\n\nGoodbye!\n");
}