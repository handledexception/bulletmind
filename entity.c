#include "entity.h"
#include "main.h"
#include "timing.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

double engine_time;

void ent_init()
{
	array_ents = (entity_t *)malloc(sz_arrayents);
	if (array_ents != NULL) { 
		memset(array_ents, 0, sz_arrayents); 
		for (int32_t i = 0; i < MAX_ENTITIES; i++) {
			array_ents[i].caps = NULL_INDEX;
		}
	}
	
	printf("ent_init OK\n");

	ent_spawn(PLAYER | MOVER | SHOOTER | COLLIDER);
	ent_spawn(MOVER | SHOOTER | COLLIDER);
	ent_spawn(SHOOTER | COLLIDER);
	ent_spawn(COLLIDER);
	ent_spawn(MOVER);
	array_ents[2].caps = NULL_INDEX;
	ent_spawn(SHOOTER);
	for (int n = 0; n < 4; n++)
		printf("caps = %d\n", array_ents[n].caps);
	//ent_removecaps(1, PLAYER);
}

int32_t ent_new()
{
	static int32_t ent = 0;
	int32_t start = ent;
	entity_t *e = NULL;
	printf("ent_new = %d\n", ent);
	do {
		ent = (ent + 1) & MASK_ENTITIES;
		if (ent == start) {
			printf("ent_new overflow!\n");
			return NULL_INDEX;
		}
	} while (array_ents[ent].caps != NULL_INDEX);

	e = &array_ents[ent];
	memset(e, 0, sizeof(entity_t));
	
	printf("ent_new with index %d\n", ent);

	return ent;
}

int32_t ent_spawn(entity_caps caps)
{
	int32_t ent = ent_new();
	if (ent == NULL_INDEX) { return NULL_INDEX; }
	
	entity_t *e = &array_ents[ent];
	e->caps |= caps;
	e->time_created = timing_getmillisec();	
	printf("%f - ent_spawn with caps %d\n", e->time_created, caps);
}

int32_t ent_setcaps(int32_t ent, entity_caps caps)
{
	entity_t *e = &array_ents[ent];
	if (e != NULL) {
		e->caps |= caps;
	}	
}

int32_t ent_removecaps(int32_t ent, entity_caps caps)
{
	entity_t *e = &array_ents[ent];
	if (e != NULL) {
		e->caps &= ~caps;
	}
}

void ent_delete(entity_t *e)
{
	if (e != NULL) {
		free(e);
		e = NULL;
	}
}

void ent_setpos(entity_t *e, vec2f_t *pos)
{	
	if (e != NULL) {
		e->pos = *pos;
	}
}

void ent_setdir(entity_t *e, vec2f_t *dir, float ang)
{
	if (e != NULL) {
		e->dir = *dir;
	}
	//e->angle = atan(vec2f_dot(&e->pos, &e->dir));
}

entity_t *ent_find(int32_t id)
{
	if (array_ents != NULL) {
		for (uint32_t e = 0; e < sz_arrayents; e++) {
			if (array_ents[e].id == id) {
				return &array_ents[e];
			}
		}
	}
	printf("could not locate entity by id: %d\n", id);
	return NULL;
}

void ent_shutdown()
{
	if (array_ents) {
		free(array_ents);
		array_ents = NULL;
	}
	printf("ent_shutdown OK\n");
}