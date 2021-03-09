#pragma once

#include "bitfield.h"
#include "c99defs.h"
#include "types.h"
#include "main.h"
#include "vector.h"

typedef struct engine_s engine_t;

#define MAX_ENTITIES 1024 // small max ents for debugging
#define MASK_ENTITIES (MAX_ENTITIES - 1)

#define MAX_ENTITY_CAPS 32

#define ENT_NAME_MAX 4096

typedef enum {
	kEntityMover = 1,
	kEntityShooter = 2,
	kEntityDestroyable = 3,
	kEntityCollider = 4,
	kEntitySatellite = 5,
	kEntityBullet = 6,
	kEntityPlayer = 7,
	kEntityRenderable = 8,
} entity_caps_t;

typedef struct entity_s {
	i32 index;
	char name[ENT_NAME_MAX];
	i32 caps;

	vec2f_t org;
	vec2f_t vel;
	vec2i_t size;
	rect_t bbox;
	vec2f_t mouse_org; // click origin
	rgba_t color;
	f32 angle;

	f64 timestamp;
	f64 lifetime;
} entity_t;

extern i32 gActiveEntities;
extern i32 gLastEntity;

bool ent_init(entity_t **ent_list, const i32 num_ents);
void ent_refresh(engine_t *eng, const f64 dt);
void ent_shutdown(entity_t *ent_list);

entity_t *ent_new(entity_t *ent_list);
entity_t *ent_by_name(entity_t *ent_list, const char *name);
entity_t *ent_by_index(entity_t *ent_list, const i32 idx);

entity_t *ent_spawn(entity_t *ent_list, const char *name, const vec2f_t org,
		    const vec2i_t size, const rgba_t *color, const i32 caps,
		    const f64 lifetime);
void ent_lifetime_update(entity_t *e);
void ent_bbox_update(entity_t *e);

void ent_set_name(entity_t *e, const char *name);

void ent_add_caps(entity_t *e, const entity_caps_t caps);
void ent_remove_caps(entity_t *e, const entity_caps_t caps);
void ent_set_caps(entity_t *e, const i32 cap_flags);
bool ent_has_caps(entity_t *e, const entity_caps_t caps);
bool ent_has_no_caps(entity_t *e);

void ent_set_pos(entity_t *e, const vec2f_t org);
void ent_set_vel(entity_t *e, const vec2f_t vel, f32 ang);
void ent_set_bbox(entity_t *e, const rect_t *bbox);
void ent_set_mouse_org(entity_t *e, const vec2f_t m_org);
void ent_euler_move(entity_t *e, const vec2f_t accel, const f32 friction,
		    const f64 dt);

bool ent_spawn_player_and_satellite(entity_t *ent_list);
void ent_move_player(entity_t *player, engine_t *eng, const f64 dt);

void ent_move_satellite(entity_t *satellite, entity_t *player, engine_t *eng,
			const f64 dt);

void ent_move_bullet(entity_t *bullet, engine_t *eng, const f64 dt);
