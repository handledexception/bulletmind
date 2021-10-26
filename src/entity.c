/*
 * Copyright (c) 2021 Paul Hindt
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "audio.h"
#include "command.h"
#include "entity.h"
#include "font.h"
#include "input.h"
#include "render.h"
#include "resource.h"

#include "core/logger.h"
#include "core/mem_arena.h"
#include "core/random.h"
#include "core/rect.h"
#include "core/time_convert.h"
#include "core/utils.h"

#include "math/utils.h"

#include "platform/platform.h"

#include <SDL.h>

static const s32 kPlayerCaps = (kEntityPlayer | kEntityMover | kEntityCollider |
				kEntityShooter | kEntityRenderable);

static const s32 kSatelliteCaps =
	(kEntitySatellite | kEntityMover | kEntityShooter | kEntityCollider |
	 kEntityRenderable);

static const s32 kBulletCaps =
	(kEntityBullet | kEntityMover | kEntityCollider | kEntityRenderable);

static const s32 kEnemyCaps =
	(kEntityEnemy | kEntityMover | kEntityCollider | kEntityRenderable);

s32 gActiveEntities = 0; // extern
s32 gLastEntity = 0;     // extern

static const f32 kGravity = 9.8f;

static const f32 kBulletSpeedMultiplier = 24000.f;

bool ent_init(entity_t** ent_list, const s32 num_ents)
{
	if (ent_list == NULL)
		return false;

	const size_t sz_ent_list = sizeof(entity_t) * num_ents;
	*ent_list = (entity_t*)arena_alloc(&g_mem_arena, sz_ent_list,
					   DEFAULT_ALIGNMENT);
	memset(*ent_list, 0, sz_ent_list);

	logger(LOG_INFO, "ent_init OK\n");

	return true;
}

void ent_refresh(engine_t* eng, const f64 dt)
{
	if (eng == NULL)
		return;

	entity_t* ent_list = eng->ent_list;
	vec2f_t mouse_pos = {0.f, 0.f};
	mouse_pos.x = (f32)eng->inputs->mouse.window_pos.x;
	mouse_pos.y = (f32)eng->inputs->mouse.window_pos.y;

	if (eng->spawn_timer[0] == 0.0)
		eng->spawn_timer[0] = eng_get_time_sec() + 2.0;
	if (eng_get_time_sec() >= eng->spawn_timer[0]) {
		ent_spawn_enemy(ent_list, eng->camera_bounds.w,
				eng->camera_bounds.h);
		eng->spawn_timer[0] = 0.0;
	}

	gActiveEntities = 0;
	for (s32 edx = 0; edx < MAX_ENTITIES; edx++) {
		entity_t* e = ent_by_index(ent_list, edx);

		if (e == NULL)
			continue;

		if (ent_has_no_caps(e))
			continue;
		else {
			gActiveEntities += 1;
			ent_lifetime_update(e);
		}

		if (ent_has_caps(e, kEntityMover)) {
			if (!strcmp(e->name, "player"))
				ent_move_player(e, eng, dt);
			else if (!strcmp(e->name, "satellite")) {
				entity_t* player = ent_by_index(
					ent_list, PLAYER_ENTITY_INDEX);
				ent_move_satellite(e, player, eng, dt);
			} else if (!strcmp(e->name, "bullet")) {
				ent_move_bullet(e, eng, dt);
			}

			if (ent_has_caps(e, kEntityEnemy)) {
				entity_t* player = ent_by_index(
					ent_list, PLAYER_ENTITY_INDEX);
				ent_move_enemy(e, player, eng, dt);
			}
		}

		if (ent_has_caps(e, kEntityShooter)) {
			if (!strcmp(e->name, "player")) {
				static bool p_shooting = false;

				// Player entity shooting
				if (cmd_get_state(eng->inputs,
						  kCommandPlayerPrimaryFire) ==
				    true) {
					if (p_shooting == false) {
						p_shooting = true;
					}
				} else if (cmd_get_state(
						   eng->inputs,
						   kCommandPlayerPrimaryFire) ==
					   false) {
					p_shooting = false;
				}
				if (cmd_get_state(eng->inputs,
						  kCommandPlayerAltFire) ==
				    true) {
					logger(LOG_INFO,
					       "eng_refresh - kCommandPlayerAltFire triggered!\n");
				}

				// player shooting
				f32 p_weap_fire_rate = 0.100f;
				static f64 p_shoot_time = 0.0;
				if (p_shooting &&
				    os_get_time_sec() >= p_shoot_time) {
					p_shoot_time = os_get_time_sec() +
						       p_weap_fire_rate;
					entity_t* player = ent_by_index(
						ent_list, PLAYER_ENTITY_INDEX);
					vec2f_t bullet_org = player->org;
					// vec2f_t mouse = {mouse_pos.x, mouse_pos.y};
					const vec2i_t bullet_size = {8, 8};
					const rgba_t bullet_color = {
						0xf5, 0xa4, 0x42, 0xff};
					entity_t* bullet = ent_spawn(
						ent_list, "bullet", bullet_org,
						bullet_size, &bullet_color,
						kBulletCaps,
						(f64)BASIC_BULLET_LIFETIME);
					ent_set_mouse_org(bullet, mouse_pos);

					eng_play_sound(engine,
						       "snd_primary_fire",
						       DEFAULT_SFX_VOLUME);
				}
			}
		}

		if (ent_has_caps(e, kEntityRenderable)) {
			if (!strcmp(e->name, "player")) {
				game_resource_t* resource =
					eng_get_resource(engine, "player");
				sprite_sheet_t* sprite_sheet =
					(sprite_sheet_t*)resource->data;

				// Flip sprite on X axis depending on mouse pos
				vec2f_t player_to_mouse = {0.f, 0.f};
				vec2f_t pm_temp = {0.f, 0.f};
				vec2f_sub(&pm_temp, e->org, mouse_pos);
				vec2f_norm(&player_to_mouse, pm_temp);
				bool flip = false;
				if (player_to_mouse.x > 0.f)
					flip = true;

				f64 frame_scale = 1.0;
				vec2f_t vel_tmp = {0.f, 0.f};
				vec2f_fabsf(&vel_tmp, e->vel);
				frame_scale = MAX(vel_tmp.x, vel_tmp.y);
				draw_sprite_sheet(engine->renderer,
						  sprite_sheet, &e->org,
						  frame_scale, e->angle, flip);
			} else if (!strcmp(e->name, "satellite")) {
				rect_t sat_rect = {(f32)e->bounds.x,
						   (f32)e->bounds.y, e->size.x,
						   e->size.y};

				draw_rect_solid(engine->renderer, sat_rect,
						&e->color);
			} else if (!strcmp(e->name, "bullet")) {
				//TODO(paulh): Need a game_resource_t method for get_resource_by_name
				game_resource_t* resource =
					eng_get_resource(engine, "bullet");
				sprite_t* sprite = (sprite_t*)resource->data;
				SDL_Rect dst = {e->bounds.x, e->bounds.y,
						sprite->surface->clip_rect.w,
						sprite->surface->clip_rect.h};

				// calculate angle of rotation between mouse and bullet origins
				if (e->angle == 0.f) {
					vec2f_t mouse_to_bullet = {0.f, 0.f};
					vec2f_sub(&mouse_to_bullet,
						  e->mouse_org, e->org);
					vec2f_norm(&mouse_to_bullet,
						   mouse_to_bullet);
					e->angle = RAD_TO_DEG(
						atan2f(mouse_to_bullet.y,
						       mouse_to_bullet.x));
				}
				SDL_RenderCopyEx(engine->renderer,
						 sprite->texture, NULL, &dst,
						 e->angle, NULL, SDL_FLIP_NONE);
			} else {
				rect_t r = {(f32)e->bounds.x, (f32)e->bounds.y,
					    e->size.x, e->size.y};
				draw_rect_solid(engine->renderer, r, &e->color);
			}

			// Draw debug overlays
			if (engine->debug) {
				SDL_SetRenderDrawColor(engine->renderer, 0xff,
						       0xff, 0xff, 0xff);
				f32 rad = radius_of_circle_in_rect(e->bounds);
				draw_circle(engine->renderer, (f32)e->org.x,
					    (f32)e->org.y, rad);
			}
		}

		// if (ent_has_caps(e, kEntityCollider)) {
		// 	for (s32 c = 0; c < MAX_ENTITIES; c++) {
		// 		entity_t* collider = ent_by_index(ent_list, c);
		// 		if (e == collider)
		// 			continue; // don't check against self
		// 		if (ent_has_caps(collider, kEntityCollider)) {
		// 			if (e->bounds.w > collider->bounds.x)
		// 				e->org.x -= e->size.x;
		// 			if (e->bounds.h > collider->bounds.y)
		// 				e->org.y -= e->size.y;
		// 			// if (e->bounds.x < collider->bounds.w)
		// 			// 	e->org.x += e->size.x;
		// 			// if (e->bounds.y < collider->bounds.h)
		// 			// 	e->org.y += e->size.y;
		// 			ent_update_bounds(e);
		// 		}
		// 	}
		// }
	}

	// logger(LOG_INFO, "engine time: %f", eng_get_time_sec());
}

void ent_shutdown(entity_t* ent_list)
{
	logger(LOG_INFO, "ent_shutdown OK\n");
}

entity_t* ent_new(entity_t* ent_list)
{
	entity_t* e = NULL;

	// search entity list for first slot with no caps set
	for (s32 edx = 0; edx < MAX_ENTITIES; edx++) {
		e = ent_by_index(ent_list, edx);
		if (e == NULL) {
			logger(LOG_WARNING, "Entity slot %d is NULL!\n", edx);
			continue;
		}

		if (ent_has_no_caps(e)) {
			logger(LOG_INFO,
			       "ent_new: found empty slot %d for entity\n",
			       edx);
			memset(e, 0, sizeof(entity_t));
			e->index = edx;
			break;
		}
	}

	return e;
}

entity_t* ent_by_index(entity_t* ent_list, const s32 idx)
{
	entity_t* e = &ent_list[idx];
	if (e == NULL) {
		logger(LOG_WARNING, "ent_by_index - NULL entity at index %d\n",
		       idx);
	}

	return e;
}

entity_t* ent_by_name(entity_t* ent_list, const char* name)
{
	entity_t* e = NULL;
	for (size_t edx = 0; edx < MAX_ENTITIES; edx++) {
		e = &ent_list[edx];
		if (!strcmp(e->name, name))
			break;
	}

	return e;
}

entity_t* ent_spawn(entity_t* ent_list, const char* name, const vec2f_t org,
		    const vec2i_t size, const rgba_t* color, const s32 caps,
		    f64 lifetime)
{
	entity_t* e = ent_new(ent_list);
	if (e != NULL) {
		// const vec2i_t half_size = {(s32)(size.x / 2),
		// 			   (s32)(size.y / 2)};
		// const rect_t bounding = {
		// 	(s32)org.x - half_size.x,
		// 	(s32)org.y - half_size.y,
		// 	(s32)org.x + half_size.x,
		// 	(s32)org.y + half_size.y,
		// };

		ent_set_name(e, name);
		ent_set_caps(e, caps);

		e->org = org;
		e->size = size;
		e->color = *color;
		e->angle = 0.f;
		e->timestamp = eng_get_time_sec();

		ent_update_bounds(e);

		if (!lifetime)
			e->lifetime = lifetime;
		else
			e->lifetime = e->timestamp + lifetime;

		logger(LOG_INFO, "ent_spawn: (%f) \"%s\" with caps %d\n",
		       e->timestamp, e->name, caps);
	} else
		logger(LOG_WARNING,
		       "ent_spawn: no slots found to spawn entity %s\n", name);

	return e;
}

void ent_lifetime_update(entity_t* e)
{
	// kill entities that have a fixed lifetime
	if (e->lifetime > 0.0 && (eng_get_time_sec() >= e->lifetime)) {
		logger(LOG_INFO, "Entity %s lifetime expired\n", e->name);
		e->caps = 0;
	}
}

// update the bounding box constraints based on changes in entity origin
void ent_update_bounds(entity_t* e)
{
	vec2i_t half_size = {e->size.x, e->size.y};
	vec2i_div(&half_size, e->size, 2);
	e->bounds.x = (s32)(e->org.x) - half_size.x;
	e->bounds.y = (s32)(e->org.y) - half_size.y;
	e->bounds.w = (s32)(e->org.x) + half_size.x;
	e->bounds.h = (s32)(e->org.y) + half_size.y;
}

void eng_centerpoint(entity_t* e, vec2f_t* p) {}

void ent_set_name(entity_t* e, const char* name)
{
	if (name != NULL && strlen(name) <= TEMP_STRING_MAX + 1)
		strcpy((char*)e->name, name);
}

void ent_add_caps(entity_t* e, const entity_caps_t caps)
{
	e->caps |= caps;
}

void ent_remove_caps(entity_t* e, const entity_caps_t caps)
{
	e->caps &= ~caps;
}

void ent_set_caps(entity_t* e, const s32 cap_flags)
{
	e->caps = cap_flags;
}

bool ent_has_caps(entity_t* e, const entity_caps_t caps)
{
	return e->caps & caps;
}

bool ent_has_no_caps(entity_t* e)
{
	return (e->caps == 0);
}

void ent_set_pos(entity_t* e, const vec2f_t org)
{
	vec2f_copy(&e->org, org);
}

void ent_set_vel(entity_t* e, const vec2f_t vel, const f32 ang)
{
	vec2f_copy(&e->vel, vel);
	e->angle = atan(vec2f_dot(e->org, e->vel));
}

void ent_set_bounds(entity_t* e, const rect_t* bounds)
{
	e->bounds = *bounds;
	const vec2i_t bounds_half_size = {e->bounds.w / 2, e->bounds.h / 2};
	e->org.x -= (f32)bounds_half_size.x;
	e->org.y -= (f32)bounds_half_size.y;
}

void ent_set_mouse_org(entity_t* e, const vec2f_t m_org)
{
	vec2f_copy(&e->mouse_org, m_org);
}

void ent_euler_move(entity_t* e, const vec2f_t accel, const f32 friction,
		    const f64 dt)
{
	vec2f_t delta = {0.f, 0.f};
	vec2f_t accel_scaled = {0.f, 0.f};

	vec2f_mulf(&accel_scaled, accel, dt);
	vec2f_add(&e->vel, e->vel, accel_scaled);
	vec2f_friction(&e->vel, e->vel, friction);
	vec2f_copy(&delta, e->vel);
	vec2f_mulf(&delta, delta, dt);
	vec2f_add(&e->org, e->org, delta);
}

bool ent_spawn_player_and_satellite(entity_t* ent_list, s32 cam_width,
				    s32 cam_height)
{
	// center on screen
	vec2f_t player_org = {(f32)(cam_width * 0.5f),
			      (f32)(cam_height * 0.5f)};
	vec2i_t player_size = {16, 16};
	rgba_t player_color = {0x0, 0x0, 0xff, 0xff};

	entity_t* player = ent_spawn(ent_list, "player", player_org,
				     player_size, &player_color, kPlayerCaps,
				     FOREVER);
	if (player == NULL) {
		logger(LOG_ERROR,
		       "ent_init - failed to initialize player entity!\n");
		return false;
	}

	// spawn satellite
	vec2f_t sat_org = {0.f, 0.f};
	vec2i_t sat_size = {16, 16};
	rgba_t sat_color = {0x90, 0xf5, 0x42, 0xff};
	f32 sat_offset = 512.f;
	vec2f_set(&sat_org, player_org.x + sat_offset,
		  player_org.y + sat_offset);
	entity_t* satellite = ent_spawn(ent_list, "satellite", sat_org,
					sat_size, &sat_color, kSatelliteCaps,
					FOREVER);
	if (satellite == NULL) {
		logger(LOG_ERROR,
		       "ent_init - failed to initialize satellite entity!\n");
		return false;
	}

	return true;
}

bool ent_spawn_enemy(entity_t* ent_list, s32 cam_width, s32 cam_height)
{
	vec2f_t org = {(f32)gen_random(0, cam_width, 1),
		       (f32)gen_random(0, cam_height, 3)};
	vec2i_t size = {16, 16};
	rgba_t color = {0xf0, 0x36, 0x00, 0xff};
	entity_t* enemy = ent_spawn(ent_list, "enemy", org, size, &color,
				    kEnemyCaps, FOREVER);
	if (enemy == NULL) {
		logger(LOG_ERROR, "ent_init - failed to spawn enemy!");
		return false;
	}

	return true;
}

void ent_move_player(entity_t* player, engine_t* eng, f64 dt)
{
	// Player entity movement
	vec2f_t p_accel = {0};
	f32 p_speed = 48.f * kGravity;  // meters/sec
	f32 friction = 0.015625f * 2.f; // 1 meter / 64 pixels
	if (cmd_get_state(eng->inputs, kCommandPlayerSpeed) == true) {
		p_speed *= 2.f;
	}
	if (cmd_get_state(eng->inputs, kCommandPlayerUp) == true) {
		p_accel.y = -p_speed;
	}
	if (cmd_get_state(eng->inputs, kCommandPlayerDown) == true) {
		p_accel.y = p_speed;
	}
	if (cmd_get_state(eng->inputs, kCommandPlayerLeft) == true) {
		p_accel.x = -p_speed;
	}
	if (cmd_get_state(eng->inputs, kCommandPlayerRight) == true) {
		p_accel.x = p_speed;
	}

	ent_euler_move(player, p_accel, friction, dt);

	// screen bounds checking
	if (player->org.x > (f32)eng->camera_bounds.w - 25) {
		player->org.x = (f32)eng->camera_bounds.w - 25;
	}
	if (player->org.y > (f32)eng->camera_bounds.h - 25) {
		player->org.y = (f32)eng->camera_bounds.h - 25;
	}
	if (player->org.x < (f32)eng->camera_bounds.x + 25) {
		player->org.x = (f32)eng->camera_bounds.x + 25;
	}
	if (player->org.y < (f32)eng->camera_bounds.y + 25) {
		player->org.y = (f32)eng->camera_bounds.y + 25;
	}

	ent_update_bounds(player);
}

void ent_move_satellite(entity_t* satellite, entity_t* player, engine_t* eng,
			f64 dt)
{
	static f32 sat_speed = 1000.f;
	vec2f_t dist = {0.f, 0.f};
	vec2f_sub(&dist, player->org, satellite->org);
	vec2f_norm(&dist, dist);

	const f32 orbit_dist = 48.f;
	const f32 orbit_thresh = 64.f;
	const bool is_orbiting =
		(fabsf(dist.x) < orbit_thresh || fabsf(dist.y) < orbit_thresh);

	static f32 orbit_angle = 0.f;
	if (is_orbiting) {
		sat_speed = 450.f;
		vec2f_t orbit_ring = {0.f, 0.f};
		vec2f_t orbit_vec = {0.f, 0.f};
		f32 px = player->org.x;
		f32 py = player->org.y;

		orbit_ring.x = (px + cos(orbit_angle) * orbit_dist);
		orbit_ring.y = (py + sin(orbit_angle) * orbit_dist);
		vec2f_sub(&orbit_vec, player->org, orbit_ring);

		// orbit_angle += DEG_TO_RAD((f32)(dt * 360.f));
		orbit_angle += DEG_TO_RAD(3.0f);
		if (orbit_angle > DEG_TO_RAD(360.f))
			orbit_angle = 0.f;

		vec2f_mulf(&dist, dist, sat_speed);
		vec2f_norm(&orbit_vec, orbit_vec);
		vec2f_mulf(&orbit_vec, orbit_vec, 800.f);
		vec2f_sub(&dist, dist, orbit_vec);

		// u8 r,g,b,a;
		// SDL_GetRenderDrawColor(engine->renderer, &r, &g, &b, &a);
		// SDL_SetRenderDrawColor(engine->renderer, 0xff, 0xaa, 0x00, 0xff);
		// SDL_RenderDrawLine(engine->renderer, player->org.x, player->org.y, satellite->org.x, satellite->org.y);
		// SDL_SetRenderDrawColor(engine->renderer, r, g, b, a);
	} else {
		sat_speed = 1000.f;
		vec2f_mulf(&dist, dist, sat_speed);
	}

	ent_euler_move(satellite, dist, 0.05f, dt);
	ent_update_bounds(satellite);
}

void ent_move_bullet(entity_t* bullet, engine_t* eng, f64 dt)
{
	vec2f_t dist = {0.f, 0.f};
	// vector between entity mouse origin and entity origin
	if (bullet->vel.x == 0.f && bullet->vel.y == 0.f) {
		vec2f_sub(&dist, bullet->mouse_org, bullet->org);
		vec2f_norm(&dist, dist);
		vec2f_mulf(&dist, dist, kBulletSpeedMultiplier);
	}
	// reflection: r = d-2(d*n)n where d*nd*n is the dot product, and nn must be normalized.
	ent_euler_move(bullet, dist, 0.0, dt);
	ent_update_bounds(bullet);
}

void ent_move_enemy(entity_t* enemy, entity_t* player, engine_t* eng, f64 dt)
{
	vec2f_t dist = {0.f, 0.f};

	// if (enemy->vel.x == 0.f && enemy->vel.y == 0.f) {
	vec2f_sub(&dist, player->org, enemy->org);
	vec2f_norm(&dist, dist);
	vec2f_mulf(&dist, dist, 150.f);
	// }
	ent_euler_move(enemy, dist, 0.0, dt);
	ent_update_bounds(enemy);
}
