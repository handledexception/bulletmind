#ifndef H_BM_HASHMAP
#define H_BM_HASHMAP

typedef struct hashmap hashmap_t;

void hashmap_create(hashmap_t* map);
void hashmap_destroy(hashmap_t* map);
void hashmap_insert(hashmap_t* map, const char* key, void* elem);
void hashmap_remove(hashmap_t* map, const char* key);
void hashmap_find(hashmap_t* map, const char* key, void** elem);

#endif
