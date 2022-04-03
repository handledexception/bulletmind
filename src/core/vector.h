#pragma once

#include "core/types.h"
#include "core/memory.h"

#include <assert.h>

#define VEC_NOT_FOUND ((size_t)-1)

#ifdef __cplusplus
extern "C" {
#endif
struct vector {
	void* elems;
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
	if (vec && vec->elems) {
		mem_free(vec->elems);
		vec->elems = NULL;
		vec->num_elems = 0;
		vec->capacity = 0;
	}
}

static inline void vector_reserve(struct vector* vec, size_t elem_size, size_t capacity)
{
	if (capacity == 0 || capacity <= vec->capacity)
		return;
	void* ptr = mem_alloc(elem_size * capacity);
	if (vec->elems) {
		if (vec->num_elems)
			memcpy(ptr, vec->elems, elem_size * vec->num_elems);
		mem_free(vec->elems);
	}
	vec->elems = ptr;
	vec->capacity = capacity;
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

	void* ptr = mem_alloc(elem_size * new_cap);
	if (vec->elems) {
		if (vec->capacity)
			memcpy(ptr, vec->elems, elem_size * vec->capacity);
		mem_free(vec->elems);
	}
	vec->elems = ptr;
	vec->capacity = new_cap;
}

static inline void* vector_elem(const struct vector* vec, size_t elem_size,
				size_t index)
{
	return (void*)(((uint8_t*)vec->elems) + elem_size * index);
}

static inline size_t vector_find(const struct vector* vec, size_t elem_size,
				const void* elem, size_t index)
{
	if (index <= vec->num_elems) {
		for (size_t i = index; i < vec->num_elems; i++) {
			void* cmp = vector_elem(vec, elem_size, i);
			if (memcmp(cmp, elem, elem_size) == 0) {
				return index;
			}
		}
	}
	return -1;
}

static inline void* vector_begin(const struct vector* vec, size_t elem_size)
{
	if (!vec->num_elems)
		return NULL;
	return vector_elem(vec, elem_size, 0);
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

static inline void* vector_push_back_new(struct vector *vec, size_t elem_size)
{
	void *last;
	vector_ensure_capacity(vec, elem_size, ++vec->num_elems);
	last = vector_end(vec, elem_size);
	memset(last, 0, elem_size);
	return last;
}

static inline void vector_pop_back(struct vector* vec, size_t elem_size)
{
	assert(vec->num_elems != 0);
	if (vec->num_elems)
		vector_erase(vec, elem_size, vec->num_elems - 1);
}

#define VECTOR(type)         \
	union {                  \
		struct vector vec;   \
		struct {             \
			type* elems;     \
			size_t num_elems;\
			size_t capacity; \
		};                   \
	}
#define vec_init(v) vector_init(&v.vec)
#define vec_free(v) vector_free(&v.vec)
#define vec_alloc_size(v) (sizeof(*v.elems) * v.num_elems)
#define vec_elem(v, i) vector_elem(&v.vec, sizeof(*v.elems), i)
#define vec_find(v, elem, i) vector_find(&v.vec, sizeof(*v.elems), elem, i)
#define vec_begin(v) vector_begin(&v.vec, sizeof(*v.elems))
#define vec_end(v) vector_end(&v.vec, sizeof(*v.elems))
#define vec_erase(v, i) vector_erase(&v.vec, sizeof(*v.elems), i)
#define vec_push_back(v, elem) vector_push_back(&v.vec, elem, sizeof(*v.elems))
#define vec_pop_back(v) vector_pop_back(&v.vec, sizeof(*v.elems))
#define vec_reserve(v, cap) vector_reserve(&v.vec, sizeof(*v.elems), cap)

// #define vec_resize(v, size) vector_resize(sizeof(*v.elems), &v.vec, size)

// #define vec_copy(dst, src) vector_copy(sizeof(*dst.elems), &dst.vec, &src.vec)

// #define vec_copy_array(dst, src_array, n) \
// 	vector_copy_array(sizeof(*dst.elems), &dst.da, src_array, n)

// #define vec_move(dst, src) vector_move(&dst.da, &src.da)

#ifdef __cplusplus
}
#endif
