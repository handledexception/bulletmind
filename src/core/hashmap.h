#ifndef H_BM_HASHMAP
#define H_BM_HASHMAP

#include "core/vector.h"

typedef u32 (*hash_func_t)(u32 val);

struct bucket {
	u32 key;
	void* val;
};

struct hashmap {
	size_t elem_size;
	size_t capacity;
	struct vector buckets;
	size_t num_buckets;
};

typedef struct hashmap hashmap_t;

u32 hash_string(const char* s, size_t len);

void hashmap_init(struct hashmap* map);
void hashmap_new(hashmap_t* map, size_t capacity);
void hashmap_free(hashmap_t* map);
bool hashmap_find(hashmap_t* map, const char* key, void** elem);
void hashmap_insert(hashmap_t* map, const char* key, const void* elem,
		    size_t elem_size);
void hashmap_remove(hashmap_t* map, const char* key);

void hashmap_ensure_capacity(hashmap_t* map, size_t elem_size, size_t capacity);

#endif
