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

#pragma once

#include "core/types.h"

#include "math/types.h"

typedef struct engine_s engine_t;

#define FOREVER 0.0
#define BASIC_BULLET_LIFETIME 5.f
#define PLAYER_ENTITY_INDEX 0
#define SATELLITE_ENTITY_INDEX 1

#define MAX_ENTITIES 1024 // small max ents for debugging
#define MASK_ENTITIES (MAX_ENTITIES - 1)

#define MAX_ENTITY_CAPS 32

typedef enum {
	kEntityMover = 1 << 0,
	kEntityShooter = 1 << 1,
	kEntityDestroyable = 1 << 2,
	kEntityCollider = 1 << 3,
	kEntitySatellite = 1 << 4,
	kEntityBullet = 1 << 5,
	kEntityPlayer = 1 << 6,
	kEntityRenderable = 1 << 7,
	kEntityEnemy = 1 << 9,
	kEntityCamera = 1 << 10,
	kEntityTile = 1 << 11,
} entity_caps_t;

typedef enum {
	kSatelliteStationary = 1 << 0,
	kSatelliteOrbitCW = 1 << 1,
	kSatelliteOrbitCCW = 1 << 2,
	kSatelliteTargetNearestEnemy = 1 << 3,
	kSatelliteTargetNearestFriendly = 1 << 4,
	kSatelliteCollectItems = 1 << 5
} satellite_flags_t;

typedef struct entity_s {
	s32 index;
	char name[TEMP_STRING_MAX];
	entity_caps_t caps;
	s32 flags;

	struct vec3f origin; // top-left
	struct vec3f velocity;
	struct vec4f vertex_color;
	f32 capsule_radius;
	f32 capsule_height;
	struct bounds bbox;
	// struct bounds bounding;

	vec2f_t org;  // entity centerpoint
	vec2f_t vel;  // entity velocity
	vec2i_t size; // entity width and height in pixels
	// rect_t rect;       // entity bounding box
	vec2f_t mouse_org; // mouse click origin
	rgba_t color;      // entity rect color (if no sprite)
	f32 angle;         // entity angle

	f64 timestamp; // engine timestamp in seconds
	f64 lifetime;  // entity lifetime in seconds
} entity_t;

extern s32 gActiveEntities;
extern s32 gLastEntity;

bool ent_init(entity_t** ent_list, const s32 num_ents);
void ent_refresh(engine_t* eng, const f64 dt);
void ent_refresh_movers(engine_t* eng, entity_t* e, f64 dt);
void ent_refresh_colliders(engine_t* eng, entity_t* e, f64 dt);
void ent_refresh_emitters(engine_t* eng, entity_t* e, f64 dt);
void ent_refresh_renderables(engine_t* eng, entity_t* e, f64 dt);
void ent_shutdown(entity_t* ent_list);

entity_t* ent_new(entity_t* ent_list);
entity_t* ent_by_name(entity_t* ent_list, const char* name);
entity_t* ent_by_index(entity_t* ent_list, const s32 idx);

entity_t* ent_spawn_v2(entity_t* ents, const char* name, vec3f_t* org,
		       bounds_t* bb, s32 caps, f64 lifetime);

entity_t* ent_spawn(entity_t* ent_list, const char* name, const vec2f_t org,
		    const vec2i_t size, const rgba_t* color, const s32 caps,
		    const f64 lifetime);
void ent_despawn(entity_t* ent_list, entity_t* ent);

void ent_lifetime_update(entity_t* e);
void ent_center_rect(entity_t* e);

void ent_set_name(entity_t* e, const char* name);

void ent_add_caps(entity_t* e, const entity_caps_t caps);
void ent_remove_caps(entity_t* e, const entity_caps_t caps);
bool ent_has_caps(entity_t* e, const entity_caps_t caps);
bool ent_has_no_caps(entity_t* e);

void ent_set_pos(entity_t* e, const vec2f_t org);
void ent_set_vel(entity_t* e, const vec2f_t vel, f32 ang);
// void ent_set_rect(entity_t* e, const rect_t* r);
void ent_set_mouse_org(entity_t* e, const vec2f_t m_org);
void ent_euler_move(entity_t* e, const vec2f_t accel, const f32 friction,
		    const f64 dt);

bool ent_spawn_player_and_satellite(entity_t* ent_list, s32 cam_width,
				    s32 cam_height);
bool ent_spawn_enemy(entity_t* ent_list, s32 cam_width, s32 cam_height);
void ent_move_player(entity_t* player, engine_t* eng, const f64 dt);
void ent_move_satellite(entity_t* satellite, entity_t* player, engine_t* eng,
			const f64 dt);
void ent_move_bullet(entity_t* bullet, engine_t* eng, const f64 dt);
void ent_move_enemy(entity_t* enemy, entity_t* player, engine_t* eng, f64 dt);
