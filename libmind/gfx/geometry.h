#pragma once

#include "math/types.h"
#include "gfx/gfx.h"

#define GFX_QUAD_NUM_VERTICES 4
static vec3f_t* gfx_quad_positions()
{
	static vec3f_t positions[GFX_QUAD_NUM_VERTICES] = {{-1.f, -1.f, 0.f},
							   {-1.f, 1.f, 0.f},
							   {1.f, 1.f, 0.f},
							   {1.f, -1.f, 0.f}};

	return (vec3f_t*)&positions[0];
}

static vec2f_t* gfx_quad_uvs()
{
	static vec2f_t uvs[GFX_QUAD_NUM_VERTICES] = {
		{0.f, 1.f},
		{0.f, 0.f},
		{1.f, 0.f},
		{1.f, 1.f},
	};

	return (vec2f_t*)&uvs[0];
}

#define GFX_QUAD_NUM_INDICES 6
static u32* gfx_quad_indices()
{
	static u32 indices[GFX_QUAD_NUM_INDICES] = {0, 1, 2, 2, 3, 0};

	return (u32*)&indices[0];
}

#define GFX_CUBE_NUM_VERTICES 24
static vec3f_t* gfx_cube_positions()
{
	static vec3f_t positions[GFX_CUBE_NUM_VERTICES] = {
		// front face
		{-1.0, -1.0, 1.0},
		{1.0, -1.0, 1.0},
		{1.0, 1.0, 1.0},
		{-1.0, 1.0, 1.0},
		// back face
		{-1.0, -1.0, -1.0},
		{-1.0, 1.0, -1.0},
		{1.0, 1.0, -1.0},
		{1.0, -1.0, -1.0},
		// top face
		{-1.0, 1.0, -1.0},
		{-1.0, 1.0, 1.0},
		{1.0, 1.0, 1.0},
		{1.0, 1.0, -1.0},
		// bottom face
		{-1.0, -1.0, -1.0},
		{1.0, -1.0, -1.0},
		{1.0, -1.0, 1.0},
		{-1.0, -1.0, 1.0},
		// right face
		{1.0, -1.0, -1.0},
		{1.0, 1.0, -1.0},
		{1.0, 1.0, 1.0},
		{1.0, -1.0, 1.0},
		// left face
		{-1.0, -1.0, -1.0},
		{-1.0, -1.0, 1.0},
		{-1.0, 1.0, 1.0},
		{-1.0, 1.0, -1.0},
	};

	return (vec3f_t*)&positions[0];
}

static vec2f_t* gfx_cube_uvs()
{
	static vec2f_t uvs[GFX_CUBE_NUM_VERTICES] = {
		// front face
		{0.f, 1.f},
		{0.f, 0.f},
		{1.f, 0.f},
		{1.f, 1.f},
		// back face
		{0.f, 1.f},
		{0.f, 0.f},
		{1.f, 0.f},
		{1.f, 1.f},
		// top face
		{0.f, 1.f},
		{0.f, 0.f},
		{1.f, 0.f},
		{1.f, 1.f},
		// bottom face
		{0.f, 1.f},
		{0.f, 0.f},
		{1.f, 0.f},
		{1.f, 1.f},
		// right face
		{0.f, 1.f},
		{0.f, 0.f},
		{1.f, 0.f},
		{1.f, 1.f},
		// left face
		{0.f, 1.f},
		{0.f, 0.f},
		{1.f, 0.f},
		{1.f, 1.f},
	};

	return (vec2f_t*)&uvs[0];
}

#define GFX_CUBE_NUM_INDICES 36
static u32* gfx_cube_indices()
{
	static u32 indices[GFX_CUBE_NUM_INDICES] = {
		0,  1,  2,  0,  2,  3,  // front
		4,  5,  6,  4,  6,  7,  // back
		8,  9,  10, 8,  10, 11, // top
		12, 13, 14, 12, 14, 15, // bottom
		16, 17, 18, 16, 18, 19, // right
		20, 21, 22, 20, 22, 23, // left
	};

	return (u32*)&indices[0];
}

#define GFX_CUBE_NUM_FACES 6
#define GFX_CUBE_NUM_VERTS (GFX_CUBE_NUM_FACES * 4)
void gfx_compute_cube(const vec3f_t sz, vec3f_t* pos, u32* ind, bool rh_coords)
{
	static const vec3f_t face_normals[GFX_CUBE_NUM_FACES] = {
		{0, 0, 1},  {0, 0, -1}, {1, 0, 0},
		{-1, 0, 0}, {0, 1, 0},  {0, -1, 0},
	};
	int pos_count = 0;
	int ind_count = 0;
	vec3f_t half_sz = vec3_mulf(sz, 0.5f);
	for (int i = 0; i < GFX_CUBE_NUM_FACES; i++) {
		vec3f_t norm = face_normals[i];
		vec3f_t basis;
		if (i >= 4)
			basis = vec3_set(0.0f, 0.0f, 1.0f);
		else
			basis = vec3_set(0.0f, 1.0f, 0.0f);
		vec3f_t side1 = vec3_cross(norm, basis);
		vec3f_t side2 = vec3_cross(norm, side1);

		ind[ind_count++] = pos_count + 0;
		ind[ind_count++] = pos_count + 1;
		ind[ind_count++] = pos_count + 2;
		ind[ind_count++] = pos_count + 0;
		ind[ind_count++] = pos_count + 2;
		ind[ind_count++] = pos_count + 3;

		vec3f_t v1 = vec3_sub(norm, side1);
		v1 = vec3_sub(v1, side2);
		v1 = vec3_mul(v1, half_sz);
		pos[pos_count++] = vec3_copy(v1);
		vec3f_t v2 = vec3_sub(norm, side1);
		v2 = vec3_add(v2, side2);
		v2 = vec3_mul(v2, half_sz);
		pos[pos_count++] = vec3_copy(v2);
		vec3f_t v3 = vec3_add(side1, side2);
		v3 = vec3_add(v3, norm);
		v3 = vec3_mul(v3, half_sz);
		pos[pos_count++] = vec3_copy(v3);
		vec3f_t v4 = vec3_add(norm, side1);
		v4 = vec3_sub(v4, side2);
		v4 = vec3_mul(v4, half_sz);
		pos[pos_count++] = vec3_copy(v4);
	}
}

// Swap data at index "i" with "j" of pointer "p" (type T)
#define SWAP_PTR(p, T, i, j)        \
	do {                        \
		T* a = &p[i];       \
		T* b = &p[j];       \
		T c = *(T*)(&p[j]); \
		*b = *a;            \
		*a = c;             \
	} while (0)

static inline void reverse_indices_winding(u32* indices, size_t size)
{
	for (size_t i = 0; i < size; i += 3) {
		SWAP_PTR(indices, u32, i, i + 2);
	}
}
