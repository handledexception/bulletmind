#include "core/types.h"
#include "core/hashmap.h"
#include "core/memory.h"

#include <MurmurHash2.h>

static inline u32 murmur_u32(u32 val)
{
	return MurmurHash2((void*)&val, sizeof(u32), 31);
}

u32 hash_string(const char* s, size_t len)
{
	u32 hash = 0;
	for (size_t i = 0; i < len; i++) {
		hash = murmur_u32(s[i]) ^ s[i];
	}
	return hash;
}

void hashmap_init(struct hashmap* map)
{
	map->buckets.elems = NULL;
	map->buckets.capacity = 0;
	map->buckets.num_elems = 0;
	map->capacity = 0;
	map->elem_size = 0;
	map->num_buckets = 0;
}

void hashmap_new(hashmap_t* map, size_t capacity)
{
	hashmap_init(map);
	vector_init(&map->buckets);
	vector_ensure_capacity(&map->buckets, sizeof(struct bucket), capacity);
}

void hashmap_free(hashmap_t* map)
{
	if (map) {
		vector_free(&map->buckets);
		hashmap_free(map);
	}
}

bool hashmap_find(hashmap_t* map, const char* key, void** elem)
{
	assert(map->elem_size > 0);

	bool found = false;
	for (size_t i = 0; i < map->buckets.num_elems; i++) {
		u32 key_hash = hash_string(key, strlen(key));
		struct bucket* buk = (struct bucket*)vector_elem(
			&map->buckets, sizeof(struct bucket), i);
		if (buk && buk->key == key_hash) {
			*elem = buk->val;
			found = true;
			break;
		}
	}
	return true;
}

void hashmap_insert(hashmap_t* map, const char* key, const void* elem,
		    size_t elem_size)
{
	if (map) {
		if (map->elem_size != elem_size)
			map->elem_size = elem_size;
	}
	u32 hash = 0;
	if (key != NULL)
		hash = hash_string(key, strlen(key));
	struct bucket buk;
	buk.key = hash;
	buk.val = bm_malloc(elem_size);
	memcpy(buk.val, elem, elem_size);
	vector_push_back(&map->buckets, &buk, sizeof(struct bucket));
}

void hashmap_remove(hashmap_t* map, const char* key) {}
