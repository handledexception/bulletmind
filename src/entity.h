#ifndef H_ENTITY
#define H_ENTITY

#include "c99defs.h"
#include "vector.h"
#include "system.h"

#define FOREVER 0.0

#define MAX_ENTITIES 128 // small max ents for debugging
#define MASK_ENTITIES (MAX_ENTITIES - 1)
#define MAX_ENTITY_CAPS 64

typedef enum {
    MOVER = 4,
    SHOOTER = 16,
    DESTROYABLE = 32,
    COLLIDER = 64,
    SATELLITE = 128,
    BULLET = 256,
    PLAYER = 512
} entity_caps;

typedef struct {
    int8_t name[128];
    int32_t caps;

    vec2f_t org;
    vec2f_t vel;
    vec2f_t mouse_org; // click origin

    float angle;
    recti32_t bbox;

    double timestamp;
    double lifetime;
} entity_t;

extern entity_t *array_ents;

bool ent_init();
int32_t ent_new();
int32_t ent_spawn(const char *name, float lifetime, vec2f_t *org, entity_caps caps);
void ent_eulermove(entity_t *e, vec2f_t *accel, float friction, double dt);
void ent_lifetime_update(entity_t *e);
void ent_refresh(engine_t *engine, double dt);
int32_t ent_setcaps(int32_t ent, entity_caps caps);
int32_t ent_removecaps(int32_t ent, entity_caps caps);
bool ent_hascaps(int32_t ent, entity_caps caps);
void ent_free(entity_t *e);

void ent_setpos(int32_t ent, vec2f_t *org);
void ent_setvel(int32_t ent, vec2f_t *vel, float ang);
void ent_setmouseorg(int32_t ent, vec2f_t *morg);
void ent_setbbox(int32_t ent, recti32_t *bbox);
entity_t *ent_byindex(int32_t idx);
void ent_shutdown();

#endif