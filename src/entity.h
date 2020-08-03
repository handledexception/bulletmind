#ifndef _H_ENTITY
#define _H_ENTITY

#include "bitfield.h"
#include "c99defs.h"
#include "main.h"
#include "vector.h"

typedef struct engine_s engine_t;

#define MAX_ENTITIES 1024 // small max ents for debugging
#define MASK_ENTITIES (MAX_ENTITIES - 1)

#define MAX_ENTITY_CAPS 32

#define ENT_NAME_MAX 4096

typedef enum {
    MOVER = 1,
    SHOOTER = 2,
    DESTROYABLE = 3,
    COLLIDER = 4,
    SATELLITE = 5,
    BULLET = 6,
    PLAYER = 7,
    RENDERABLE = 8,
} entity_caps_t;

typedef struct entity_s {
    int32_t index;
    char name[ENT_NAME_MAX];
    int32_t caps;

    vec2f_t org;
    vec2f_t vel;
    vec2i_t size;
    rect_t bbox;
    vec2f_t mouse_org; // click origin
    rgba_t color;
    float angle;

    double timestamp;
    double lifetime;
} entity_t;

extern int32_t active_ents;
extern int32_t last_entity;

bool ent_init(entity_t** ent_list, int32_t num_ents);
void ent_refresh(engine_t* eng, double dt);
void ent_shutdown(entity_t* ent_list);

entity_t* ent_new(entity_t* ent_list);
entity_t* ent_by_name(entity_t* ent_list, const char* name);
entity_t* ent_by_index(entity_t* ent_list, int32_t idx);

entity_t* ent_spawn(entity_t* ent_list, const char* name, double lifetime, vec2f_t* org, vec2i_t* size, rgba_t* color, int32_t caps);
void ent_lifetime_update(entity_t* e);
void ent_bbox_update(entity_t* e);

void ent_set_name(entity_t* e, const char* name);

void ent_add_caps(entity_t* e, entity_caps_t caps);
void ent_remove_caps(entity_t* e, entity_caps_t caps);
void ent_set_caps(entity_t* e, int32_t cap_flags);
bool ent_has_caps(entity_t* e, entity_caps_t caps);
bool ent_has_no_caps(entity_t* e);

void ent_set_pos(entity_t* e, vec2f_t* org);
void ent_set_vel(entity_t* e, vec2f_t *vel, float ang);
void ent_set_bbox(entity_t* e, rect_t* bbox);
void ent_set_mouse_org(entity_t* e, vec2f_t* m_org);
void ent_euler_move(entity_t* e, vec2f_t* accel, float friction, double dt);

bool ent_spawn_player_and_satellite(entity_t* ent_list);
void ent_move_player(entity_t* player, engine_t* engine, double dt);

void ent_move_satellite(entity_t* satellite, entity_t* player, engine_t* engine, double dt);

void ent_move_bullet(entity_t* bullet, engine_t* engine, double dt);

#endif