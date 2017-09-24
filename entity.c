#include "command.h"
#include "entity.h"
#include "main.h"
#include "timing.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

entity_t *array_ents;
size_t sz_arrayents = sizeof(entity_t) * MAX_ENTITIES;

double engine_time;
int last_entity = 0;

void drawrect_centered(SDL_Renderer *rend, int32_t x, int32_t y, int32_t w, int32_t h, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{		
	recti32_t rec = { 
		x - (w / 2), 
		y - (w / 2), 
		w, h
	};
	
	SDL_SetRenderDrawColor(rend, r, g, b, a);
	SDL_RenderFillRect(rend, (const SDL_Rect *)&rec);
}

bool ent_init()
{
	array_ents = (entity_t *)malloc(sz_arrayents);
	if (array_ents != NULL) { 
		memset(array_ents, 0, sz_arrayents); 
		for (int32_t i = 0; i < MAX_ENTITIES; i++) {
			array_ents[i].caps = NULL_INDEX;
		}
	}
	
	// spawn player as first entity
	last_entity = ent_spawn(PLAYER | MOVER | SHOOTER | COLLIDER);
	if (last_entity == NULL_INDEX) {
		printf("ent_init - failed initializing player entity!\n");
		return false;
	}
	recti32_t bounding = { 0, 0, 16, 16 };
	vec2f_t pos = { (float)(WINDOW_WIDTH / 2), (float)(WINDOW_HEIGHT / 2) };
	entity_t *player = &array_ents[last_entity];	
	player->id = 999;
	player->bbox = bounding;
	ent_setpos(last_entity, &pos);
	
	printf("ent_init OK\n");
	return true;
}

int32_t ent_new()
{
	static int32_t ent = 0;
	int32_t start = ent;
	entity_t *e = NULL;
	printf("ent_new = %d\n", ent);
	do {
		ent = (ent + 1) & MASK_ENTITIES;
		if (ent == start) {
			printf("ent_new overflow!\n");
			return NULL_INDEX;
		}
	} while (array_ents[ent].caps != NULL_INDEX);

	e = &array_ents[ent];
	memset(e, 0, sizeof(entity_t));
	
	printf("ent_new with index %d\n", ent);

	return ent;
}

int32_t ent_spawn(entity_caps caps)
{
	int32_t ent = ent_new();
	if (ent == NULL_INDEX) { return NULL_INDEX; }
	
	entity_t *e = &array_ents[ent];
	e->caps |= caps;
	e->time_created = engine_time;
	printf("%f - ent_spawn with caps %d\n", e->time_created, caps);
	return ent;
}

void ent_refresh(SDL_Renderer *renderer, double dt)
{
	//printf("ent_refresh - last_entity = %d\n", last_entity);	
	entity_t *e = ent_byindex(1);
	if (e != NULL) {
		if (ent_hascaps(1, PLAYER) == true) {			
			vec2f_t accel = {};			
			// pos = 0.5 * accel * dt^2 + newvel
			if (cmd_getstate(CMD_PLAYER_UP) == true) { accel.y = -1.f; }
			if (cmd_getstate(CMD_PLAYER_DOWN) == true) { accel.y = 1.f; } 
			if (cmd_getstate(CMD_PLAYER_LEFT) == true) { accel.x = -1.f; }
			if (cmd_getstate(CMD_PLAYER_RIGHT) == true) { accel.x = 1.f; }
			/*if (keyboard_state[SDL_SCANCODE_W]) { accel.y = -1.f; }
			if (keyboard_state[SDL_SCANCODE_S]) { accel.y = 1.f; }
			if (keyboard_state[SDL_SCANCODE_A]) { accel.x = -1.f; }
			if (keyboard_state[SDL_SCANCODE_D]) { accel.x = 1.f; }*/
			if (cmd_getstate(CMD_PLAYER_PRIMARY_FIRE) == true) { printf("sys_refresh - CMD_PLAYER_PRIMARY_FIRE triggered!\n"); }
			if (cmd_getstate(CMD_PLAYER_ALTERNATE_FIRE) == true) { printf("sys_refresh - CMD_PLAYER_ALTERNATE_FIRE triggered!\n"); }			
			
			
			vec2f_t old_vel = {};
			vec2f_equ(&old_vel, &e->vel);
			vec2f_scale(&accel, dt);
			vec2f_addequ(&e->vel, &accel);
			vec2f_t avg_vel = {};
			vec2f_add(&avg_vel, &old_vel, &e->vel);
			vec2f_scale(&avg_vel, 0.0005 * dt);						
			
			vec2f_addequ(&e->pos, &avg_vel);
					
			drawrect_centered(renderer, (float)e->pos.x, (float)e->pos.y, e->bbox.w, e->bbox.h, 0xff, 0x00, 0x00, 0xff);						
		}
	}
	/*for (int32_t edx = last_entity; edx < MAX_ENTITIES; edx++) {
		//todo: entity lewp
	}*/
}

int32_t ent_setcaps(int32_t ent, entity_caps caps)
{
	entity_t *e = &array_ents[ent];
	if (e != NULL) {
		e->caps |= caps;
		return ent;
	}

	return NULL_INDEX;
}

int32_t ent_removecaps(int32_t ent, entity_caps caps)
{
	entity_t *e = &array_ents[ent];
	if (e != NULL) {
		e->caps &= ~caps;
		return ent;
	}

	return NULL_INDEX;
}

bool ent_hascaps(int32_t ent, entity_caps caps)
{
	bool found = false;
	entity_t *e = &array_ents[ent];
	if (e != NULL) {
		if ((e->caps & caps) == caps) {
			found = true;
		}
	}

	return found;
}

void ent_free(entity_t *e)
{
	if (e != NULL) {
		free(e);
		e = NULL;
	}
}

void ent_setpos(int32_t ent, vec2f_t *pos)
{	
	entity_t *e = &array_ents[ent];
	if (e != NULL && pos != NULL) {
		e->pos = *pos;
	}
}

void ent_setvel(int32_t ent, vec2f_t *vel, float ang)
{
	entity_t *e = &array_ents[ent];
	if (e != NULL && vel != NULL) {
		e->vel = *vel;
		e->angle = atan(vec2f_dot(&e->pos, &e->vel));
	}
}

entity_t *ent_byindex(int32_t idx)
{
	entity_t *e = &array_ents[idx];
	if (e == NULL) {
		printf("ent_byindex - NULL entity at index %d\n", idx);
	}
	
	return e;
}

void ent_shutdown()
{
	if (array_ents) {
		free(array_ents);
		array_ents = NULL;
	}
	printf("ent_shutdown OK\n");
}