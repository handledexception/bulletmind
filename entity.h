#ifndef H_ENTITY
#define H_ENTITY

#include "c99defs.h"
#include "vector.h"

#define MAX_ENTITIES 1024
#define MAX_ENTITY_CAPS 64

typedef enum {
	MOVER = 4,
	SHOOTER = 16,
	DESTROYABLE = 32,
	COLLIDER = 64,
	PLAYSOUND = 256
} entity_caps;

typedef struct {
	int32_t id;
	char name[256];
	int32_t caps;
	
	vec2f_t pos;

	union {
		vec2f_t dir;
		float angle;
	};
	
	rectf_t bbox;
	double time_created;

} entity_t;

entity_t *array_ents;
static size_t sz_arrayents = sizeof(entity_t) * MAX_ENTITIES;

void ent_init();
entity_t *ent_new(int32_t id, entity_caps cap, vec2f_t pos, vec2f_t dir, float ang, rectf_t bbox);
void ent_delete(entity_t *e);
void ent_setpos(int32_t id, vec2f_t pos);
entity_t *ent_find(int32_t id);

#endif