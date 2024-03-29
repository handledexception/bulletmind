#pragma once

#include "core/types.h"

struct gui_window_s;

typedef struct {
	pix_fmt_t format;
	s32 width;
	s32 height;
	f32 fps_num;
	f32 fps_den;
} display_mode_t;

struct gui_window_s {
	u32 id;
	char* title;
	u32 flags;
	display_mode_t display_mode;

	s32 pos_x;
	s32 pos_y;
	s32 width;
	s32 height;
	s32 min_w, min_h;
	s32 max_w, max_h;
	rect_t bounds;
};

typedef struct gui_window_s gui_window_t;

extern gui_window_t* gui_create_window(const char* title, s32 x, s32 y, s32 w,
				       s32 h, u32 flags);
extern void gui_set_window_title(gui_window_t* window, const char* title);
extern void gui_show_window(gui_window_t* window);
extern void gui_hide_window(gui_window_t* window);
extern void gui_maximize_window(gui_window_t* window);
extern void gui_minimize_window(gui_window_t* window);
extern void gui_fullscreen_window(gui_window_t* window);
extern void gui_windowed_window(gui_window_t* window);
extern void gui_set_window_position(gui_window_t* window, s32 x, s32 y);
extern void gui_set_window_size(gui_window_t* window, s32 w, s32 h);
