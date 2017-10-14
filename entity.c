#define _CRT_SECURE_NO_WARNINGS 1

#include "command.h"
#include "entity.h"
#include "font.h"
#include "main.h"
#include "timing.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <SDL.h>

entity_t *array_ents;
size_t sz_arrayents = sizeof(entity_t) * MAX_ENTITIES;

int last_entity = 0;
int32_t mousex, mousey;

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
			array_ents[i].caps = 0;
		}
	}
	
	// spawn player as first entity
	vec2f_t org = { (float)(WINDOW_WIDTH / 2), (float)(WINDOW_HEIGHT / 2) };
	last_entity = ent_spawn("player", FOREVER, &org, (PLAYER | MOVER | SHOOTER | COLLIDER));
	if (last_entity == NULL_INDEX) {
		printf("ent_init - failed initializing player entity!\n");
		return false;
	}

	entity_t *player = &array_ents[last_entity];	
	vec2f_t epos;
	epos.x = player->pos.x + 150;
	epos.y = player->pos.y + 75;
	last_entity = ent_spawn("satellite", FOREVER, &epos, (SATELLITE | MOVER | SHOOTER | COLLIDER));

	printf("ent_init OK\n");
	return true;
}

int32_t ent_new()
{
	entity_t *e = NULL;
	for (int32_t edx = 0; edx < MAX_ENTITIES; edx++) {
		e = &array_ents[edx];
		if (e->caps == 0) {
			printf("ent_new: found empty slot %d for entity\n", edx);
			memset(e, 0, sizeof(entity_t));
			return edx;
		}
	}	

	return NULL_INDEX;
}

int32_t ent_spawn(const char *name, float lifetime, vec2f_t *org, entity_caps caps)
{
	int32_t ent = ent_new();
	if (ent == NULL_INDEX) { 
		printf("ent_spawn: no slots found to spawn entity %s\n", name);
		return NULL_INDEX; 
	}
	
	const recti32_t bounding = { 0, 0, 16, 16 }; // default rect of entity is 16x16px
	entity_t *e = &array_ents[ent];
	if (name != NULL && strlen(name) <= 256+1) { strcpy(e->name, name); }
	e->caps |= caps;
	e->pos = *org;
	e->angle = 0.f;
	e->bbox = bounding;
	e->timestamp = timing_enginetime();
	e->lifetime = e->timestamp + lifetime;
	printf("ent_spawn: (%f) \"%s\" with caps %d\n", e->timestamp, e->name, caps);
	return ent;
}

void ent_refresh(void *renderer, double dt, recti32_t *screen)
{
	static float p_shoot_time = 0;
	static bool pshooting = false;
	float p_weap_fire_rate = 0.150;

	//todo(paulh): decide whether to use pointers or indices to deal w/ entities in functions
	for (int32_t edx = 0; edx <= MAX_ENTITIES; edx++) {		
		entity_t *e = ent_byindex(edx);
		// kill entities that have a fixed lifetime
		if (e->lifetime > 0.0 && timing_enginetime() >= e->lifetime) {
			//memset(e, 0, sizeof(entity_t));			
			e->caps = 0;
		}

		// PLAYER
		if (ent_hascaps(edx, PLAYER) == true) {			
			float speed = 800.f;
			vec2f_t accel = { 0 };
			vec2f_t avgvel = { 0 };
			vec2f_t oldvel = { 0 };
			vec2f_t delta = { 0 };
			
			// todo(paulh): take this out of the entity loop
			if (cmd_getstate(CMD_PLAYER_SPEED) == true) { speed *= 1.5f; }
			if (cmd_getstate(CMD_PLAYER_UP) == true) { accel.y = -speed; }
			if (cmd_getstate(CMD_PLAYER_DOWN) == true) { accel.y = speed; }
			if (cmd_getstate(CMD_PLAYER_LEFT) == true) { accel.x = -speed; }
			if (cmd_getstate(CMD_PLAYER_RIGHT) == true) { accel.x = speed; }
			if (cmd_getstate(CMD_PLAYER_PRIMARY_FIRE) == true) { 				
				if (pshooting == false) {
					p_shoot_time = (float)timing_getsec() + p_weap_fire_rate;
					pshooting = true;
				}				
			}
			else if (cmd_getstate(CMD_PLAYER_PRIMARY_FIRE) == false) {
				pshooting = false;				
			}
			if (cmd_getstate(CMD_PLAYER_ALTERNATE_FIRE) == true) { 
				printf("sys_refresh - CMD_PLAYER_ALTERNATE_FIRE triggered!\n"); 
			}

			// player shooting
			if (pshooting && timing_getsec() >= p_shoot_time) {
				p_shoot_time = timing_getsec() + p_weap_fire_rate;
				entity_t *player = ent_byindex(0);
				vec2f_t bullet_org = player->pos;
				vec2f_t mouse = { mousex, mousey };
				bullet_org.x += (float)(player->bbox.w / 2);
				bullet_org.y += (float)(player->bbox.h / 2);

				int32_t bullet = ent_spawn("basic_bullet", 5.0, &bullet_org, BULLET | MOVER | COLLIDER);
				recti32_t bbox = { 0, 0, 8, 8 };
				ent_setbbox(bullet, &bbox);
				ent_setmouseorg(bullet, &mouse);
				printf("%d, %d\n", mousex, mousey);
			}

			// euler
			vec2f_scale(&accel, dt);
			vec2f_addequ(&e->vel, &accel);
			vec2f_friction(&e->vel, 0.035);
			vec2f_equ(&delta, &e->vel);
			vec2f_scale(&delta, dt);
			vec2f_addequ(&e->pos, &delta);			

			// screen bounds checking
			if (e->pos.x > (float)screen->w) { e->pos.x = (float)screen->w; }
			if (e->pos.y > (float)screen->h) { e->pos.y = (float)screen->h; }
			if (e->pos.x < (float)screen->x) { e->pos.x = (float)screen->x; }
			if (e->pos.y < (float)screen->y) { e->pos.y = (float)screen->y; }
			
			drawrect_centered((SDL_Renderer *)renderer, (float)e->pos.x, (float)e->pos.y, e->bbox.w, e->bbox.h, 0xff, 0x00, 0x00, 0xff);
		}

		// Bullets
		if (ent_hascaps(edx, BULLET) == true) {
			if (e->vel.x == 0.0 && e->vel.y == 0.0) {
				vec2f_t dist = { 0 };
				static vec2f_t org = { 0 };
				org.x = e->pos.x;
				org.y = e->pos.y;

				vec2f_sub(&dist, &e->mouse_org, &org);
				vec2f_norm(&dist);
				vec2f_scale(&dist, 500.f);
				vec2f_scale(&dist, (float)(0.5 * dt));
				vec2f_equ(&e->vel, &dist);
			}
			vec2f_addequ(&e->pos, &e->vel);

			drawrect_centered((SDL_Renderer *)renderer, (float)e->pos.x, (float)e->pos.y, e->bbox.w, e->bbox.h, 0xc0, 0xa0, 0xdd, 0xff);
		}

		// Player Satellite AKA Option
		if (ent_hascaps(edx, SATELLITE) == true) {
			vec2f_t orig_opt_pos = { 0 };
			vec2f_equ(&orig_opt_pos, &e->pos);

			vec2f_t option_pos = { 0 };
			vec2f_t player_pos = { 0 };
			vec2f_t dist = { 0 };

			entity_t *player = ent_byindex(0);
			vec2f_equ(&option_pos, &e->pos);
			vec2f_equ(&player_pos, &player->pos);
			vec2f_sub(&dist, &player_pos, &option_pos);
			vec2f_norm(&dist);
			vec2f_scale(&dist, 300.f);
			vec2f_scale(&dist, (float)(0.5 * dt));
			//vec2f_add(&option_pos, &orig_opt_pos, &dist)
			vec2f_addequ(&e->pos, &dist);

			/*font_print(renderer, 0, 0, 2.0, "player: %f, %f", player->pos.x, player->pos.y);
			font_print(renderer, 0, 50, 2.0, "satellite: %f, %f", e->pos.x, e->pos.y);
			font_print(renderer, 0, 150, 2.0, "dist: %f, %f", dist.x, dist.y);*/
			drawrect_centered((SDL_Renderer *)renderer, (float)e->pos.x, (float)e->pos.y, e->bbox.w, e->bbox.h, 0x00, 0xff, 0x00, 0xff);
		}
	}
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

void ent_setbbox(int32_t ent, recti32_t *bbox)
{
	entity_t *e = ent_byindex(ent);
	e->bbox = *bbox;
}

void ent_setmouseorg(int32_t ent, vec2f_t *morg)
{
	entity_t *e = ent_byindex(ent);
	e->mouse_org = *morg;
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