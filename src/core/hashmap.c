#include "core/types.h"
#include "core/hashmap.h"
#include "core/memory.h"

#include <MurmurHash2.h>

static inline u32 murmur_u32(u32 val)
{
	return MurmurHash2((void*)&val, sizeof(u32), 31);
}

u32 hash_ptr(void* p)
{
	u32 hash = 0;
	u32* addr = (u32*)p;
	hash = murmur_u32((u32)*addr);
	return hash;
}

u32 hash_key(hash_key_t key)
{
	u32 hash = 0;
	u8* k = (u8*)key.data;
	for (size_t i = 0; i < key.size; i++) {
		hash = murmur_u32(k[i]) ^ k[i];
	}
	return hash;
}

void hashmap_init(struct hashmap* map)
{
	vec_init(map->buckets);
}

hashmap_t* hashmap_new(void)
{
	hashmap_t* map = (hashmap_t*)BM_ALLOC(sizeof(*map));
	hashmap_init(map);
	return map;
}

void hashmap_free(hashmap_t* map)
{
	if (map) {
		vec_free(map->buckets);
		BM_FREE(map);
		map = NULL;
	}
}

size_t hashmap_size(hashmap_t* map)
{
	if (map) {
		return map->buckets.num_elems;
	} else {
		return 0;
	}
}

bool hashmap_find(hashmap_t* map, hash_key_t key, void** elem)
{
	bool found = false;
	for (size_t i = 0; i < map->buckets.num_elems; i++) {
		u32 key_hash = hash_key(key);
		hash_bucket_t* buk = &map->buckets.elems[i];
		if (buk && buk->key == key_hash) {
			*elem = buk->val;
			found = true;
			break;
		}
	}
	return found;
}

void hashmap_insert(hashmap_t* map, hash_key_t key, const void* elem,
		    size_t elem_size)
{
	if (!map || !elem)
		return;

	hash_bucket_t buk;
	buk.key = hash_key(key);
	buk.val = BM_ALLOC(elem_size);
	memcpy(buk.val, elem, elem_size);
	vec_push_back(map->buckets, &buk);
}

void hashmap_remove(hashmap_t* map, hash_key_t key) {}
