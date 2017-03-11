#include "command.h"
#include "main.h"
#include "input.h"
#include "system.h"
#include "timing.h"

#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>



uint8_t sys_init(engine_t *eng)
{	
	timing_init(&eng->timing);

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
	
	printf("sys_init OK [%fms]\n", timing_interval_ms(&eng->timing));
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
		}		
	}
}

void sys_shutdown(engine_t *eng)
{
	cmd_shutdown();
	in_shutdown();
	
	SDL_DestroyRenderer(eng->renderer);
	SDL_DestroyWindow(eng->window);
	eng->renderer = NULL;
	eng->window = NULL;
	SDL_Quit();
}