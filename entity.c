#include "entity.h"
#include "timing.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
/*
int arry[5] = { 2, 4, 6, 8, 10 };
memset(&arry, 0, sizeof(int) * 5);
arry[0] = 1;
size_t szarray = ARRAY_SIZE(arry);
for (int i = 0; i < szarray; i++) {
if (arry[i] == 0) {
arry[i] = 2;
break;
}
}
*/

void ent_init()
{
	array_ents = (entity_t *)malloc(sz_arrayents);
	if (array_ents != NULL) { memset(array_ents, 0, sz_arrayents); }	
}

entity_t *ent_new(int32_t id, entity_caps cap, vec2f_t pos, vec2f_t dir, float ang, rectf_t bbox)
{
	entity_t *e = (entity_t *)malloc(sizeof(entity_t));
	if (e != NULL) {
		e->id = id;
		e->caps |= cap;
		e->pos = pos;
		e->dir = dir;
		e->angle = ang;
		e->bbox = bbox;
		e->time_created = timing_getmillisec();
	}
	return e;
}

void ent_delete(entity_t *e)
{
	if (e != NULL) {
		free(e);
		e = NULL;
	}
}

void ent_setpos(int32_t id, vec2f_t pos)
{	
	ent_find(id)->pos = pos;
}

void ent_setdir(int32_t id, vec2f_t dir, float ang)
{
	entity_t *e = ent_find(id);
	e->dir = dir;
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