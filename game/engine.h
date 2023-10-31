#pragma once

typedef struct hashmap hashmap_t;

typedef struct engine {
    hashmap_t* geometry_map;
} engine_t;

engine_t* engine_new(void);
void engine_free(engine_t* eng);
bool engine_add_geometry()
