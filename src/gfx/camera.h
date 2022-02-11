/*
 * Copyright (c) 2021 Paul Hindt
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef H_BM_GFX_CAMERA
#define H_BM_GFX_CAMERA

#include "math/types.h"
#include "core/rect.h"

typedef struct {
    vec3f_t pos;
    vec3f_t dir;
    vec3f_t up;
    mat4f_t view_matrix;
    mat4f_t proj_matrix;
    rect_t viewport;
    f32 fov;
    f32 z_near;
    f32 z_far;
} camera_t;

void gfx_camera_ortho(camera_t* cam, vec3f_t* pos, vec3f_t* dir, vec3f_t* up, rect_t* viewport, f32 z_near, f32 z_far);
void gfx_camera_persp(camera_t* cam, vec3f_t* pos, vec3f_t* dir, vec3f_t* up, rect_t* viewport, f32 fov, f32 z_near, f32 z_far);

#endif
