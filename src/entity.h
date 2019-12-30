#ifndef H_ENTITY
#define H_ENTITY

#include "bitflags.h"
#include "c99defs.h"
#include "main.h"
#include "vector.h"

#define FOREVER 0.0

#define MAX_ENTITIES 32 // small max ents for debugging
#define MASK_ENTITIES (MAX_ENTITIES - 1)
#define MAX_ENTITY_CAPS 64

#define PLAYER_ENTITY_INDEX 0

#define PLAYER_CAPS (1 << PLAYER | 1 << MOVER | 1 << COLLIDER | 1 << SHOOTER)
#define SATELLITE_CAPS (1 << SATELLITE | 1 << MOVER | 1 << SHOOTER | 1 << COLLIDER)
#define BULLET_CAPS (1 << BULLET | 1 << MOVER | 1 << COLLIDER)

typedef enum {
    MOVER = 1,
    SHOOTER = 2,
    DESTROYABLE = 3,
    COLLIDER = 4,
    SATELLITE = 5,
    BULLET = 6,
    PLAYER = 7
} entity_caps_t;

typedef struct {
    int32_t index;
    int8_t name[TEMP_STRING_MAX];
    int32_t caps;

    vec2f_t org;
    vec2f_t size;
    vec2f_t vel;
    vec2f_t mouse_org; // click origin

    float angle;
    rectf_t bbox;

    double timestamp;
    double lifetime;
} entity_t;

extern int32_t active_ents;
extern int32_t last_entity;

bool ent_init(entity_t** ent_list, int32_t num_ents);
void ent_refresh(entity_t* ent_list, double dt);
void ent_shutdown(entity_t** ent_list);

entity_t* ent_new(entity_t* ent_list);
void ent_free(entity_t* e);
entity_t* ent_by_index(entity_t* ent_list, int32_t idx);

entity_t* ent_spawn(entity_t* ent_list, const char* name, double lifetime, vec2f_t* org, vec2f_t* size, int32_t caps);
void ent_lifetime_update(entity_t* e);
void ent_bbox_update(entity_t* e);

void ent_add_caps(entity_t* e, entity_caps_t caps);
void ent_remove_caps(entity_t* e, entity_caps_t caps);
void ent_set_caps(entity_t* e, int32_t cap_flags);
bool ent_has_caps(entity_t* e, entity_caps_t caps);
bool ent_has_no_caps(entity_t* e);

void ent_set_pos(entity_t* e, vec2f_t* org);
void ent_set_vel(entity_t* e, vec2f_t *vel, float ang);
void ent_set_bbox(entity_t* e, rectf_t* bbox);
void ent_set_mouse_org(entity_t* e, vec2f_t* morg);
void ent_euler_move(entity_t* e, vec2f_t* accel, float friction, double dt);

bool ent_spawn_player(entity_t* ent_list);

#endif