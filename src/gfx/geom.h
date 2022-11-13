#pragma once

#include "math/types.h"
#include "gfx/gfx.h"

#define GFX_QUAD_NUM_VERTICES 4
static vec3f_t* gfx_quad_positions()
{
	static vec3f_t positions[GFX_QUAD_NUM_VERTICES] = {
		{-1.f, -1.f, 0.f},
		{-1.f, 1.f, 0.f},
		{1.f, 1.f, 0.f},
		{1.f, -1.f, 0.f}
	};

	return (vec3f_t*)&positions[0];
}

static vec2f_t* gfx_quad_uvs()
{
	static vec2f_t uvs[GFX_QUAD_NUM_VERTICES] = {
		{0.f, 1.f}, {0.f, 0.f},
		{1.f, 0.f}, {1.f, 1.f},
	};

	return (vec2f_t*)&uvs[0];
}

#define GFX_QUAD_NUM_INDICES 6
static u32* gfx_quad_indices()
{
    static u32 indices[GFX_QUAD_NUM_INDICES] = {
        0, 1, 2, 2, 3, 0
    };

    return (u32*)&indices[0];
}

#define GFX_CUBE_NUM_VERTICES 24
static vec3f_t* gfx_cube_positions()
{
	static vec3f_t positions[GFX_CUBE_NUM_VERTICES] = {
		// front face
		{-1.0, -1.0, 1.0},
		{ 1.0, -1.0, 1.0},
		{ 1.0,  1.0, 1.0},
		{-1.0,  1.0, 1.0},
		// back face
		{-1.0, -1.0, -1.0},
		{-1.0,  1.0, -1.0},
		{ 1.0,  1.0, -1.0},
		{ 1.0, -1.0, -1.0},
		// top face
		{-1.0, 1.0, -1.0},
		{-1.0, 1.0,  1.0},
		{ 1.0, 1.0,  1.0},
		{ 1.0, 1.0, -1.0},
		// bottom face
		{-1.0, -1.0, -1.0},
		{ 1.0, -1.0, -1.0},
		{ 1.0, -1.0,  1.0},
		{-1.0, -1.0,  1.0},
		// right face
		{1.0, -1.0, -1.0},
		{1.0,  1.0, -1.0},
		{1.0,  1.0,  1.0},
		{1.0, -1.0,  1.0},
		// left face
		{-1.0, -1.0, -1.0},
		{-1.0, -1.0,  1.0},
		{-1.0,  1.0,  1.0},
		{-1.0,  1.0, -1.0},
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
