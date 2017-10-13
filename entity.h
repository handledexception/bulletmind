#ifndef H_ENTITY
#define H_ENTITY

#include "c99defs.h"
#include "vector.h"

#define FOREVER 0.f

#define MAX_ENTITIES 32 // small max ents for debugging
#define MASK_ENTITIES (MAX_ENTITIES - 1)
#define MAX_ENTITY_CAPS 64

typedef enum {
	MOVER = 4,
	SHOOTER = 16,
	DESTROYABLE = 32,
	COLLIDER = 64,
	SATELLITE = 128,
	ENEMY = 256,
	PLAYER = 512
} entity_caps;

typedef struct {
	int8_t name[128];
	int32_t caps;
		
	vec2f_t pos;
	vec2f_t vel;
	float angle;
	recti32_t bbox;
		
	double timestamp;
	double lifetime;
} entity_t;

bool ent_init();
int32_t ent_new();
int32_t ent_spawn(const char *name, float lifetime, vec2f_t *org, entity_caps caps);
void ent_refresh(void *renderer, double dt, recti32_t *screen);
int32_t ent_setcaps(int32_t ent, entity_caps caps);
int32_t ent_removecaps(int32_t ent, entity_caps caps);
bool ent_hascaps(int32_t ent, entity_caps caps);
void ent_free(entity_t *e);
void ent_setpos(int32_t ent, vec2f_t *pos);
void ent_setvel(int32_t ent, vec2f_t *vel, float ang);
entity_t *ent_byindex(int32_t idx);
void ent_shutdown();

#endif