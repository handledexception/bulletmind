#ifndef H_BM_VECTOR
#define H_BM_VECTOR

#include "core/types.h"
#include "core/memory.h"

#include <assert.h>

struct vector {
	void* elems;
	// size_t elem_size;
	size_t num_elems;
	size_t capacity;
};

static inline void vector_init(struct vector* vec)
{
	vec->elems = NULL;
	vec->num_elems = 0;
	vec->capacity = 0;
}

static inline void vector_free(struct vector* vec)
{
	if (vec) {
		bm_free(vec->elems);
		vec->elems = NULL;
		vec->num_elems = 0;
		vec->capacity = 0;
	}
}

static inline void vector_ensure_capacity(struct vector* vec, size_t elem_size,
					  size_t new_capacity)
{
	if (!vec || new_capacity == 0)
		return;
	if (new_capacity <= vec->capacity)
		return;
	size_t new_cap = (!vec->capacity) ? new_capacity : vec->capacity * 2;
	if (new_cap > new_capacity)
		new_cap = new_capacity;

	void* ptr = bm_malloc(elem_size * new_cap);
	if (vec->elems) {
		if (vec->capacity)
			memcpy(ptr, vec->elems, elem_size * vec->capacity);
		bm_free(vec->elems);
	}
	vec->elems = ptr;
	vec->capacity = new_cap;
}

static inline void* vector_elem(const struct vector* vec, size_t elem_size,
				size_t index)
{
	return (void*)(((uint8_t*)vec->elems) + elem_size * index);
}

static inline void* vector_find(const struct vector* vec, size_t elem_size,
				void* data)
{
	for (size_t i = 0; i < vec->num_elems; i++) {
		void* elem = vector_elem(vec, elem_size, i);
		if (elem && memcmp(elem, data, elem_size) == 0) {
			return elem;
		}
	}
	return NULL;
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

static inline bool vector_is_empty(const struct vector* vec)
{
	return !(vec->elems > 0);
}

static inline void vector_erase(struct vector* vec, size_t elem_size,
				size_t idx)
{
	assert(idx < vec->num_elems);

	if (idx >= vec->num_elems || !--vec->num_elems)
		return;

	memmove(vector_elem(vec, elem_size, idx),
		vector_elem(vec, elem_size, idx + 1),
		elem_size * (vec->num_elems - idx));
}

static inline void vector_push_back(struct vector* vec, const void* elem,
				    size_t elem_size)
{
	vector_ensure_capacity(vec, elem_size, ++vec->num_elems);
	memcpy(vector_end(vec, elem_size), elem, elem_size);
}

static inline void vector_pop_back(struct vector* vec, size_t elem_size)
{
	assert(vec->num_elems != 0);

	if (vec->num_elems)
		vector_erase(vec, elem_size, vec->num_elems - 1);
}

#endif
