#ifndef H_BM_HASHMAP
#define H_BM_HASHMAP

#include "core/vector.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef u32 (*hash_func_t)(u32 val);

typedef struct hash_key {
	void* data;
	u32 size;
	u32 seed;
} hash_key_t;

typedef struct hash_bucket {
	u32 key;
	void* val;
} hash_bucket_t;

typedef struct hashmap {
	VECTOR(hash_bucket_t) buckets;
} hashmap_t;

u32 hash_ptr(void* p);
u32 hash_string(const char* s, size_t len);
u32 hash_key(hash_key_t key);

void hashmap_init(hashmap_t* map);
hashmap_t* hashmap_new(void);
void hashmap_free(hashmap_t* map);
size_t hashmap_size(hashmap_t* map);
bool hashmap_find(hashmap_t* map, hash_key_t key, void** elem);
void hashmap_insert(hashmap_t* map, hash_key_t key, void* const elem, size_t elem_size);
void hashmap_remove(hashmap_t* map, hash_key_t key);

// #define HASHMAP(type) \
// 	union { \
// 		hashmap_t map; \
// 		struct { \
// 			VECTOR(struct hash_bucket_##type) elems; \
// 		}; \
// 	}
// #define map_init(m) hashmap_init(&m.map); 
// #define map_free(m) hashmap_free(&m.map)
// #define map_size(m) hashmap_size(&m.map)
// #define map_find(m, k, e) hashmap_find(&m.map, k, &e)
// #define map_insert(m, k, e) hashmap_insert(&m.map, k, e, sizeof(*(&m.map.buckets[0].elems.val)))
// #define map_elem(m, k) hashmap_
#ifdef __cplusplus
}
#endif

#endif
