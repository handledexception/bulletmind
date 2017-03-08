#include "input.h"
#include "system.h"

#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>

SDL_Window *sys_window = 0;
SDL_Renderer *sys_renderer = 0;

uint8_t sys_init()
{	
	SDL_Init(SDL_INIT_EVERYTHING);
	sys_window = SDL_CreateWindow
	(
		"SDLGame", 
		SDL_WINDOWPOS_CENTERED, 
		SDL_WINDOWPOS_CENTERED,
		960, 540,
		SDL_WINDOW_SHOWN
	);
	
	if(!sys_window) {
		printf("sys_window: %s\n", SDL_GetError());
		return -1;
	}
	
	sys_renderer = SDL_CreateRenderer(sys_window, -1, SDL_RENDERER_ACCELERATED);
	if (!sys_renderer) {
		printf("sys_renderer: %s\n", SDL_GetError());
		return -1;
	}
	printf("sys_init OK\n");
	in_init();	
	
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

void sys_shutdown()
{
	in_shutdown();
	
	SDL_DestroyWindow(sys_window);
	SDL_DestroyRenderer(sys_renderer);
	sys_window = NULL;
	sys_renderer = NULL;
	SDL_Quit();
}