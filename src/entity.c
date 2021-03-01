#if defined(BM_WINDOWS)
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS 1
#endif
#endif

#include "command.h"
#include "entity.h"
#include "font.h"
#include "input.h"
#include "math-defs.h"
#include "memarena.h"
#include "render.h"
#include "resource.h"
#include "utils.h"

#include "platform/platform.h"
#include "time/time_convert.h"

#include <SDL.h>

#define FOREVER 0.0
#define BASIC_kEntityBullet_LIFETIME 10.f
#define kEntityPlayer_ENTITY_INDEX 0
#define kEntitySatellite_ENTITY_INDEX 1

static const i32 kPlayerCaps = (
	1 << kEntityPlayer | 1 << kEntityMover |
	1 << kEntityCollider |	1 << kEntityShooter |
	1 << kEntityRenderable
);

static const i32 kSatelliteCaps = (
	1 << kEntitySatellite | 1 << kEntityMover |
	1 << kEntityShooter | 1 << kEntityCollider |
	1 << kEntityRenderable
);

static const i32 kBulletCaps = (
	1 << kEntityBullet | 1 << kEntityMover |
	1 << kEntityCollider | 1 << kEntityRenderable
);

i32 gActiveEntities = 0; // extern
i32 gLastEntity = 0; // extern

static const f32 kBulletSpeedMultiplier = 24000.f;

bool ent_init(entity_t** ent_list, const i32 num_ents)
{
	if (ent_list == NULL)
		return false;

	const size_t sz_ent_list = sizeof(entity_t) * num_ents;
	*ent_list = (entity_t*)arena_alloc(&g_mem_arena, sz_ent_list, DEFAULT_ALIGNMENT);
	memset(*ent_list, 0, sz_ent_list);

	printf("ent_init OK\n");

	return true;
}

void ent_refresh(engine_t* eng, const f64 dt)
{
	if (eng == NULL)
		return;

	entity_t* ent_list = eng->ent_list;
	vec2f_t mouse_pos = { 0.f, 0.f };
	mouse_pos.x = (f32)eng->inputs->mouse.window_pos.x;
	mouse_pos.y = (f32)eng->inputs->mouse.window_pos.y;

	gActiveEntities = 0;
	for (i32 edx = 0; edx < MAX_ENTITIES; edx++) {
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
			if (!strcmp(e->name, "satellite")) {
				entity_t* player = ent_by_index(ent_list, kEntityPlayer_ENTITY_INDEX);
				ent_move_satellite(e, player, eng, dt);
			}
			if (!strcmp(e->name, "bullet"))
				ent_move_bullet(e, eng, dt);
		}

		if (ent_has_caps(e, kEntityShooter)) {
			if (!strcmp(e->name, "player")) {
				static bool p_shooting = false;

				// Player entity shooting
				if (cmd_get_state(eng->inputs, kCommandPlayerPrimaryFire) == true) {
					if (p_shooting == false) {
						p_shooting = true;
					}
				} else if (cmd_get_state(eng->inputs, kCommandPlayerPrimaryFire) == false) {
					p_shooting = false;
				}
				if (cmd_get_state(eng->inputs, kCommandPlayerAltFire) == true) {
					printf("eng_refresh - CMD_kEntityPlayer_ALTERNATE_FIRE triggered!\n");
				}

				// player shooting
				f32 p_weap_fire_rate = 0.100f;
				static f64 p_shoot_time = 0.0;
				if (p_shooting && os_get_time_sec() >= p_shoot_time) {
					p_shoot_time = os_get_time_sec() + p_weap_fire_rate;
					entity_t* player =
						ent_by_index(ent_list, kEntityPlayer_ENTITY_INDEX);
					vec2f_t bullet_org = player->org;
					// vec2f_t mouse = {mouse_pos.x, mouse_pos.y};
					const vec2i_t bullet_size = {8, 8};
					const rgba_t bullet_color = {0xf5, 0xa4, 0x42, 0xff};
					entity_t* bullet = ent_spawn(ent_list, "bullet", bullet_org,
								     bullet_size, &bullet_color,
								     kBulletCaps,
								     (f64)BASIC_kEntityBullet_LIFETIME);
					ent_set_mouse_org(bullet, mouse_pos);
				}
			}
		}

		if (ent_has_caps(e, kEntityRenderable)) {
			if (!strcmp(e->name, "player")) {
				game_resource_t* resource = eng_get_resource(engine, "player");
				sprite_sheet_t* sprite_sheet = (sprite_sheet_t*)resource->data;

				//TODO: implement frame timing (hold frame for X milliseconds before incrementing)
				//TODO: scale frames iteration by player movement speed (simulate running)

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

				// printf("frame scale %f, %f, %f | ", frame_scale, vel_tmp.x, vel_tmp.y);

				draw_sprite_sheet(engine->renderer, sprite_sheet, &e->bbox,
						  frame_scale, e->angle, flip);
			}
			if (!strcmp(e->name, "satellite")) {
				draw_rect_solid(engine->renderer, (f32)e->bbox.x, (f32)e->bbox.y,
						e->size.x, e->size.y, e->color);
			}
			if (!strcmp(e->name, "bullet")) {
				//TODO(paulh): Need a game_resource_t method for get_resource_by_name
				game_resource_t* resource = eng_get_resource(engine, "bullet");
				sprite_t* sprite = (sprite_t*)resource->data;
				SDL_Rect dst = {e->bbox.x, e->bbox.y, sprite->surface->clip_rect.w,
						sprite->surface->clip_rect.h};

				// calculate angle of rotation between mouse and bullet origins
				if (e->angle == 0.f) {
					vec2f_t mouse_to_bullet = {0.f, 0.f};
					vec2f_sub(&mouse_to_bullet, e->mouse_org, e->org);
					vec2f_norm(&mouse_to_bullet, mouse_to_bullet);
					e->angle = RAD_TO_DEG(
						atan2f(mouse_to_bullet.y, mouse_to_bullet.x));
				}
				SDL_RenderCopyEx(engine->renderer, sprite->texture, NULL, &dst,
						 e->angle, NULL, SDL_FLIP_NONE);
			}
		}

		// if (ent_has_caps(e, kEntityCollider)) {
		// 	for (i32 c = 0; c < MAX_ENTITIES; c++) {
		//         entity_t* collider = ent_by_index(ent_list, c);
		// 		if (e == collider) break; // don't check against self
		// 		if (ent_has_caps(collider, kEntityCollider)) {
		// 			f32 ex = e->org.x + e->bbox.w;
		// 			f32 ey = e->org.y + e->bbox.h;
		// 			if (ex >= collider->org.x) {
		// 				e->org.x = collider->org.x - e->bbox.w;
		// 			}
		// 			if (ey > collider->org.y) {
		// 				e->org.y = collider->org.y - e->bbox.h;
		// 			}
		// 		}
		// 	}
		// }
	}
}

void ent_shutdown(entity_t* ent_list)
{
	printf("ent_shutdown OK\n");
}

entity_t* ent_new(entity_t* ent_list)
{
	entity_t* e = NULL;

	// search entity list for first slot with no caps set
	for (i32 edx = 0; edx < MAX_ENTITIES; edx++) {
		e = ent_by_index(ent_list, edx);
		if (e == NULL) {
			printf("Entity slot %d is NULL!\n", edx);
			continue;
		}

		if (ent_has_no_caps(e)) {
			printf("ent_new: found empty slot %d for entity\n", edx);
			memset(e, 0, sizeof(entity_t));
			e->index = edx;
			break;
		}
	}

	return e;
}

entity_t* ent_by_index(entity_t* ent_list, const i32 idx)
{
	entity_t* e = &ent_list[idx];
	if (e == NULL) {
		printf("ent_by_index - NULL entity at index %d\n", idx);
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

entity_t* ent_spawn(entity_t* ent_list, const char* name, const vec2f_t org, const vec2i_t size,
		    const rgba_t* color, const i32 caps, f64 lifetime)
{
	entity_t* e = ent_new(ent_list);
	if (e != NULL) {
		const vec2i_t half_size = {(i32)(size.x / 2), (i32)(size.y / 2)};
		const rect_t bounding = {
			(i32)org.x - half_size.x,
			(i32)org.y - half_size.y,
			(i32)org.x + half_size.x,
			(i32)org.y + half_size.y,
		};

		ent_set_name(e, name);
		ent_set_caps(e, caps);

		e->org = org;
		e->size = size;
		e->color = *color;
		e->angle = 0.f;
		e->bbox = bounding;
		e->timestamp = eng_get_time();

		if (!lifetime)
			e->lifetime = lifetime;
		else
			e->lifetime = e->timestamp + lifetime;

		printf("ent_spawn: (%f) \"%s\" with caps %d\n", e->timestamp, e->name, caps);
	} else
		printf("ent_spawn: no slots found to spawn entity %s\n", name);

	return e;
}

void ent_lifetime_update(entity_t* e)
{
	// kill entities that have a fixed lifetime
	if (e->lifetime > 0.0 && (eng_get_time() >= e->lifetime)) {
		printf("Entity %s lifetime expired\n", e->name);
		e->caps = 0;
	}
}

void ent_bbox_update(entity_t* e)
{
	vec2f_t half_size = {0.f, 0.f};
	vec2f_set(&half_size, (f32)(e->size.x) * 0.5f, (f32)(e->size.y) * 0.5f);
	e->bbox.x = e->org.x - half_size.x;
	e->bbox.y = e->org.y - half_size.y;
}

void ent_set_name(entity_t* e, const char* name)
{
	if (name != NULL && strlen(name) <= ENT_NAME_MAX + 1)
		strcpy((char*)e->name, name);
}

void ent_add_caps(entity_t* e, const entity_caps_t caps)
{
	SET_FLAG(e->caps, caps);
}

void ent_remove_caps(entity_t* e, const entity_caps_t caps)
{
	CLEAR_FLAG(e->caps, caps);
}

void ent_set_caps(entity_t* e, const i32 cap_flags)
{
	e->caps = cap_flags;
}

bool ent_has_caps(entity_t* e, const entity_caps_t caps)
{
	return IS_FLAG_SET(e->caps, caps) != 0;
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

void ent_set_bbox(entity_t* e, const rect_t* bbox)
{
	e->bbox = *bbox;
	const vec2i_t bbox_half_size = {e->bbox.w / 2, e->bbox.h / 2};
	e->org.x -= (f32)bbox_half_size.x;
	e->org.y -= (f32)bbox_half_size.y;
}

void ent_set_mouse_org(entity_t* e, const vec2f_t m_org)
{
	vec2f_copy(&e->mouse_org, m_org);
}

void ent_euler_move(entity_t* e, const vec2f_t accel, const f32 friction, const f64 dt)
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

bool ent_spawn_player_and_satellite(entity_t* ent_list)
{
	// center on screen
	vec2f_t player_org = {(f32)(CAMERA_WIDTH * 0.5f), (f32)(CAMERA_HEIGHT * 0.5f)};
	vec2i_t player_size = {16, 16};
	rgba_t player_color = {0x0, 0x0, 0xff, 0xff};

	entity_t* player = ent_spawn(ent_list, "player", player_org, player_size, &player_color,
				     kPlayerCaps, FOREVER);
	if (player == NULL) {
		printf("ent_init - failed to initialize player entity!\n");
		return false;
	}

	// spawn satellite
	vec2f_t sat_org = {0.f, 0.f};
	vec2i_t sat_size = {16, 16};
	rgba_t sat_color = {0x90, 0xf5, 0x42, 0xff};
	f32 sat_offset = 512.f;
	vec2f_set(&sat_org, player_org.x + sat_offset, player_org.y + sat_offset);
	entity_t* satellite = ent_spawn(ent_list, "satellite", sat_org, sat_size, &sat_color,
					kSatelliteCaps, FOREVER);
	if (satellite == NULL) {
		printf("ent_init - failed to initialize satellite entity!\n");
		return false;
	}

	return true;
}

void ent_move_player(entity_t* player, engine_t* eng, f64 dt)
{
	// Player entity movement
	vec2f_t p_accel = {0};
	f32 p_speed = 800.f;
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

	ent_euler_move(player, p_accel, 0.035, dt);
	// screen bounds checking
	if (player->org.x > (f32)eng->scr_bounds.w) {
		player->org.x = (f32)eng->scr_bounds.w;
	}
	if (player->org.y > (f32)eng->scr_bounds.h) {
		player->org.y = (f32)eng->scr_bounds.h;
	}
	if (player->org.x < (f32)eng->scr_bounds.x) {
		player->org.x = (f32)eng->scr_bounds.x;
	}
	if (player->org.y < (f32)eng->scr_bounds.y) {
		player->org.y = (f32)eng->scr_bounds.y;
	}
	ent_bbox_update(player);
}

void ent_move_satellite(entity_t* satellite, entity_t* player, engine_t* eng, f64 dt)
{
	static f32 sat_speed = 750.f;
	vec2f_t dist = {0.f, 0.f};
	vec2f_sub(&dist, player->org, satellite->org);
	vec2f_norm(&dist, dist);

	const f32 orbit_dist = 48.f;
	const f32 orbit_thresh = 64.f;
	const bool is_orbiting = (fabsf(dist.x) < orbit_thresh || fabsf(dist.y) < orbit_thresh);

	static f32 orbit_angle = 0.f;
	if (is_orbiting) {
		sat_speed = 350.f;
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
		sat_speed = 750.f;
		vec2f_mulf(&dist, dist, sat_speed);
	}

	ent_euler_move(satellite, dist, 0.05f, dt);
	ent_bbox_update(satellite);
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
	ent_bbox_update(bullet);
}

#ifdef _CRT_SECURE_NO_WARNINGS
#undef _CRT_SECURE_NO_WARNINGS
#endif
