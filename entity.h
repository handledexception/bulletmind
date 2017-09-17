#ifndef H_ENTITY
#define H_ENTITY

#include "c99defs.h"
#include "vector.h"

#define MAX_ENTITIES 4
#define MASK_ENTITIES (MAX_ENTITIES - 1)
#define MAX_ENTITY_CAPS 64

typedef enum {
	MOVER = 4,
	SHOOTER = 16,
	DESTROYABLE = 32,
	COLLIDER = 64,
	PLAYSOUND = 256,
	PLAYER = 512
} entity_caps;

typedef struct {
	int32_t id;
	//char name[256];
	int32_t caps;
		
	vec2f_t pos;
	vec2f_t dir;
	float angle;
	rectf_t *bbox;
		
	double time_created;
} entity_t;

entity_t *array_ents;
static size_t sz_arrayents = sizeof(entity_t) * MAX_ENTITIES;

void ent_init();
int32_t ent_new();
int32_t ent_spawn(entity_caps caps);

void ent_delete(entity_t *e);
void ent_setpos(entity_t *e, vec2f_t *pos);
void ent_setdir(entity_t *e, vec2f_t *dir, float ang);
entity_t *ent_find(int32_t id);
void ent_shutdown();

#endif