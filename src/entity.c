#define _CRT_SECURE_NO_WARNINGS 1

#include "command.h"
#include "entity.h"
#include "font.h"
#include "main.h"
#include "system.h"
#include "timing.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <SDL.h>

entity_t *array_ents; // extern
size_t sz_arrayents = sizeof(entity_t) * MAX_ENTITIES;

int last_entity = 0;
int32_t mousex, mousey; // extern

void drawrect_centered(SDL_Renderer *rend, int32_t x, int32_t y, int32_t w, int32_t h, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	recti32_t rec = {
		x - (w / 2),
		y - (w / 2),
		w,
		h
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

	entity_t *player = ent_byindex(last_entity);
	vec2f_t epos;
	epos.x = player->org.x + 150;
	epos.y = player->org.y + 75;
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
			//printf("ent_new: found empty slot %d for entity\n", edx);
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
	if (name != NULL && strlen(name) <= 256+1) { strcpy((char*)e->name, name); }
	e->caps |= caps;
	e->org = *org;
	e->angle = 0.f;
	e->bbox = bounding;
	//e->org.x += (float)(e->bbox.w / 2);
	//e->org.y += (float)(e->bbox.h / 2);
	e->timestamp = timing_enginetime();
	e->lifetime = e->timestamp + lifetime;
	printf("ent_spawn: (%f) \"%s\" with caps %d\n", e->timestamp, e->name, caps);
	return ent;
}

void ent_eulermove(entity_t *e, vec2f_t *accel, float friction, double dt)
{
	vec2f_t delta = { 0 };

	vec2f_scale(accel, dt);
	vec2f_addequ(&e->vel, accel);
	vec2f_friction(&e->vel, friction);
	vec2f_equ(&delta, &e->vel);
	vec2f_scale(&delta, dt);
	vec2f_addequ(&e->org, &delta);
}

void ent_lifetime_update(entity_t *e)
{
	// kill entities that have a fixed lifetime
	if (e->lifetime > 0.0 && (timing_enginetime() >= e->lifetime)) {
		printf("Entity %s lifetime expired\n", e->name);
		//memset(e, 0, sizeof(entity_t));
		e->caps = 0;
	}
}

void ent_refresh(engine_t *engine, double dt)
{
	vec2f_t p_accel = { 0 };
	float p_speed = 800.f;
	float p_weap_fire_rate = 0.150f;
	static double p_shoot_time = 0.0;
	static bool p_shooting = false;

	// Player entity movement
	if (cmd_getstate(CMD_PLAYER_SPEED) == true) { p_speed *= 1.5f; }
	if (cmd_getstate(CMD_PLAYER_UP) == true) { p_accel.y = -p_speed; }
	if (cmd_getstate(CMD_PLAYER_DOWN) == true) { p_accel.y = p_speed; }
	if (cmd_getstate(CMD_PLAYER_LEFT) == true) { p_accel.x = -p_speed; }
	if (cmd_getstate(CMD_PLAYER_RIGHT) == true) { p_accel.x = p_speed; }

	// Player entity shooting
	if (cmd_getstate(CMD_PLAYER_PRIMARY_FIRE) == true) {
		if (p_shooting == false) {
			p_shooting = true;
		}
	}
	else if (cmd_getstate(CMD_PLAYER_PRIMARY_FIRE) == false) {
		p_shooting = false;
	}
	if (cmd_getstate(CMD_PLAYER_ALTERNATE_FIRE) == true) {
		printf("sys_refresh - CMD_PLAYER_ALTERNATE_FIRE triggered!\n");
	}

	//todo(paulh): decide whether to use pointers or indices to deal w/ entities in functions
	for (int32_t edx = 0; edx <= MAX_ENTITIES; edx++) {
		entity_t *e = ent_byindex(edx);

		if (e) {
			if (e->caps != 0)
				ent_lifetime_update(e);
		}

		// PLAYER
		if (ent_hascaps(edx, PLAYER) == true) {
			// player shooting
			if (p_shooting && timing_getsec() >= p_shoot_time) {
				p_shoot_time = timing_getsec() + p_weap_fire_rate;
				entity_t *player = ent_byindex(0);
				vec2f_t bullet_org = player->org;
				vec2f_t mouse = { mousex, mousey };

				int32_t bullet = ent_spawn("basic_bullet", 5.0, &bullet_org, BULLET | MOVER | COLLIDER);
				recti32_t bbox = { 0, 0, 12, 12 };
				ent_setbbox(bullet, &bbox);
				ent_setmouseorg(bullet, &mouse);
			}

			// euler
			ent_eulermove(e, &p_accel, 0.035, dt);

			// screen bounds checking
			if (e->org.x > (float)engine->scr_bounds.w) { e->org.x = (float)engine->scr_bounds.w; }
			if (e->org.y > (float)engine->scr_bounds.h) { e->org.y = (float)engine->scr_bounds.h; }
			if (e->org.x < (float)engine->scr_bounds.x) { e->org.x = (float)engine->scr_bounds.x; }
			if (e->org.y < (float)engine->scr_bounds.y) { e->org.y = (float)engine->scr_bounds.y; }

			drawrect_centered(engine->renderer, (float)e->org.x, (float)e->org.y, e->bbox.w, e->bbox.h, 0xff, 0x00, 0x00, 0xff);
		}

		// BULLETS
		if (ent_hascaps(edx, BULLET) == true) {
				static vec2f_t dist = { 0 };
			if (e->vel.x == 0.0 && e->vel.y == 0.0) {

				// vector between entity mouse origin and entity origin
				vec2f_sub(&dist, &e->mouse_org, &e->org);
				vec2f_norm(&dist);
				vec2f_scale(&dist, 50000.f);

				// reflection: r = d-2(d*n)n where d*nd*n is the dot product, and nn must be normalized.
			}
			ent_eulermove(e, &dist, 0.0, dt);
			//vec2f_addequ(&e->org, &e->vel);

			drawrect_centered(engine->renderer, (float)e->org.x, (float)e->org.y, e->bbox.w, e->bbox.h, 0xc0, 0xa0, 0xdd, 0xff);
		}

		// SATELLITE
		if (ent_hascaps(edx, SATELLITE) == true) {
			vec2f_t dist = { 0 };

			entity_t *player = ent_byindex(0);
			vec2f_sub(&dist, &player->org, &e->org);
			vec2f_norm(&dist);
			vec2f_scale(&dist, 800.f);
			ent_eulermove(e, &dist, 0.05, dt);

			drawrect_centered(engine->renderer, (float)e->org.x, (float)e->org.y, e->bbox.w, e->bbox.h, 0x00, 0xff, 0x00, 0xff);
		}

		//if (ent_hascaps(edx, COLLIDER) == true) {
		//	for (int32_t c = 0; c < MAX_ENTITIES; c++) {
		//		if (c == edx) break;
		//		if (ent_hascaps(c, COLLIDER) == true) {
		//			entity_t *coll = ent_byindex(c);
		//			float ex = e->org.x + e->bbox.w;
		//			float ey = e->org.y + e->bbox.h;
		//			if (ex >= coll->org.x) {
		//				e->org.x = coll->org.x - e->bbox.w;
		//			}
		//			if (ey > coll->org.y) {
		//				e->org.y = coll->org.y - e->bbox.h;
		//			}
		//		}
		//	}
		//}
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

void ent_setpos(int32_t ent, vec2f_t *org)
{
	entity_t *e = &array_ents[ent];
	if (e != NULL && org != NULL) {
		e->org = *org;
	}
}

void ent_setvel(int32_t ent, vec2f_t *vel, float ang)
{
	entity_t *e = &array_ents[ent];
	if (e != NULL && vel != NULL) {
		e->vel = *vel;
		e->angle = atan(vec2f_dot(&e->org, &e->vel));
	}
}

void ent_setbbox(int32_t ent, recti32_t *bbox)
{
	entity_t *e = ent_byindex(ent);
	e->bbox = *bbox;
	e->org.x -= e->bbox.w / 2;
	e->org.y -= e->bbox.h / 2 ;
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