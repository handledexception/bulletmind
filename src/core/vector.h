#ifndef H_BM_HASHMAP
#define H_BM_HASHMAP

#include "core/types.h"
#include "core/memory.h"

struct vector {
	void* elems;
	size_t num_elems;
	size_t capacity;
};

static inline void vector_create(struct vector* dst)
{
	dst->elems = NULL;
	dst->num_elems = 0;
	dst->capacity = 0;
}

static inline void vector_destroy(struct vector* dst)
{
	bm_free(dst->elems);
	dst->elems = NULL;
	dst->num_elems = 0;
	dst->capacity = 0;
}

static inline void vector_ensure_capacity(struct vector* dst, size_t elem_size, size_t new_size)
{
	if (!dst)
		return;
	if (new_size <= dst->capacity)
		return;

	size_t new_cap = dst->capacity;
	if (new_cap == 0)
		new_cap = new_size;
	else
		new_cap *= 2;

	void *ptr = bm_malloc(elem_size * new_cap);
	if (dst->elems) {
		if (dst->capacity)
			memcpy(ptr, dst->elems, elem_size * dst->capacity);

		bm_free(dst->elems);
	}
	dst->elems = ptr;
	dst->capacity = new_cap;
}

static inline void* vector_elem(const struct vector* vec, size_t elem_size, size_t index)
{
	return (void *)(((uint8_t *)vec->elems) + elem_size * index);
}

static inline void* vector_end(const struct vector* vec, size_t elem_size)
{
	if (!vec->num_elems)
		return NULL;
	return vector_elem(vec, elem_size, vec->num_elems - 1);
}

static inline size_t vector_size(const struct vector* vec, size_t elem_size)
{
	return elem_size * vec->num_elems;
}

void vector_push_back(struct vector* dst, const void* elem, size_t elem_size)
{
	vector_ensure_capacity(dst, elem_size, ++dst->num_elems);
	memcpy(vector_end(dst, elem_size), elem, elem_size);
}

#endif
