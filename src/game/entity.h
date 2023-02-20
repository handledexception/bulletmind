#pragma once

#include "core/export.h"
#include "core/types.h"
#include "math/types.h"

#define MAX_ENTITIES 1024 

#define FOREVER 0.0
#define ENT_INDEX_PLAYER 0

typedef struct entity entity_t;

enum ent_caps {
	ENT_MOVER = 1 << 0,
	ENT_SHOOTER = 1 << 1,
	ENT_DESTROYABLE = 1 << 2,
	ENT_COLLIDER = 1 << 3,
	ENT_SATELLITE = 1 << 4,
	ENT_BULLET = 1 << 5,
	ENT_PLAYER = 1 << 6,
	ENT_RENDERABLE = 1 << 7,
	ENT_ENEMY = 1 << 9,
	ENT_CAMERA = 1 << 10,
};

struct entity {
    s32 index;
    s32 flags;
    char name[TEMP_STRING_MAX];
    enum ent_caps caps;             /* entity capabilities bit flags */
    vec3f_t org;                    /* initial spawn origin */
    vec3f_t pos;                    /* current position */
    vec3f_t vel;                    /* current velocity */
	vec3f_t acc;
    vec4f_t vertex_color;           
    f64 timestamp;
    f64 lifetime;                   /* future time to despawn entity */
};

BM_EXPORT entity_t* ent_init(s32 num_ents);
BM_EXPORT void      ent_refresh(entity_t* ents, f64 app_start_time, f64 dt);
BM_EXPORT void		ent_refresh_movers(entity_t* ent, f64 dt);
BM_EXPORT void      ent_shutdown(entity_t* ents);

BM_EXPORT entity_t* ent_new(entity_t* ents);
BM_EXPORT entity_t* ent_by_name(entity_t* ents, const char* name);
BM_EXPORT entity_t* ent_by_index(entity_t* ents, s32 idx);
BM_EXPORT entity_t* ent_spawn(entity_t* ents, const char* name, vec3f_t org, s32 caps, f64 lifetime);
BM_EXPORT void      ent_despawn(entity_t* ents);
BM_EXPORT void 		ent_update_lifetime(entity_t* ent, f64 app_start_time);
BM_EXPORT void		ent_accel(entity_t* ent, vec3f_t accel, f64 dt);
BM_EXPORT void      ent_euler_move(entity_t* ent, vec3f_t accel, f32 friction, f64 dt);
BM_EXPORT void		ent_set_name(entity_t* ent, const char* name);
BM_EXPORT void      ent_set_pos(entity_t* ent, vec3f_t pos);
BM_EXPORT void      ent_set_vel(entity_t* ent, vec3f_t vel);
BM_EXPORT bool		ent_has_caps(const entity_t* ent, s32 caps);
BM_EXPORT bool      ent_has_no_caps(const entity_t* ent);
