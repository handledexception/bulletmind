#ifndef H_BM_GUI
#define H_BM_GUI

#include "core/types.h"

#define BM_WINDOW_POS_UNDEFINED_MASK 0x1fff0000u
#define BM_WINDOW_POS_UNDEFINED_DO_MASK(x) (BM_WINDOW_POS_UNDEFINED_MASK | (x))
#define BM_WINDOW_POS_UNDEFINED BM_WINDOW_POS_UNDEFINED_DO_MASK(0)
#define BM_WINDOW_POS_IS_UNDEFINED(x) \
	(((x)&0xFFFF0000) == BM_WINDOW_POS_UNDEFINED_MASK)

#define BM_WINDOW_POS_CENTERED_MASK 0x2fff0000u
#define BM_WINDOW_POS_CENTERED_DO_MASK(x) (BM_WINDOW_POS_CENTERED_MASK | (x))
#define BM_WINDOW_POS_CENTERED BM_WINDOW_POS_CENTERED_DO_MASK(0)
#define BM_WINDOW_POS_IS_CENTERED(x) \
	(((x)&0xFFFF0000) == BM_WINDOW_POS_CENTERED_MASK)

typedef struct gui_window gui_window_t;
typedef struct gui_window_data gui_window_data_t;
typedef struct gui_subsystem gui_platform_t;
typedef struct gui_display gui_display_t;

typedef struct {
	u32 format;
	s32 w;
	s32 h;
	s32 refresh_rate;
	void* data;
} gui_display_mode_t;

typedef enum {
	BM_ORIENTATION_DEFAULT,
	BM_ORIENTATION_LANDSCAPE = BM_ORIENTATION_DEFAULT,

	BM_ORIENTATION_CW_90,
	BM_ORIENTATION_PORTRAIT = BM_ORIENTATION_CW_90,

	BM_ORIENTATION_CW_180,
	BM_ORIENTATION_LANDSCAPE_FLIP = BM_ORIENTATION_CW_180,

	BM_ORIENTATION_CCW_90,
	BM_ORIENTATION_PORTRAIT_FLIP = BM_ORIENTATION_CCW_90,

	BM_ORIENTATION_UNKNOWN
} gui_display_orientation_t;

struct gui_window {
	s32 id;
	s32 flags;
	char* title;
	s32 x;
	s32 y;
	s32 w;
	s32 h;
	s32 min_w;
	s32 min_h;
	s32 max_w;
	s32 max_h;
	rect_t bounds;
	bool destroy_me;
	gui_window_data_t* data;
	gui_window_t* parent;
};

struct gui_subsystem {
	const char* name;
	bool (*create_window)(gui_platform_t* gs, gui_window_t* window);
	void (*destroy_window)(gui_platform_t* gs, gui_window_t* window);
	void (*show_window)(gui_platform_t* gs, gui_window_t* window,
			    bool shown);
	void* (*get_handle)(gui_platform_t* gs, gui_window_t* window);
};

struct gui_display {
	s32 id;
	s32 index;
	char* name;
	rect_t bounds;
	gui_display_mode_t mode;
	gui_display_orientation_t orientation;
};

extern bool gui_init(void);
extern void gui_shutdown(void);

#ifdef BM_WINDOWS
extern bool gui_init_win32(gui_platform_t* gs);
#elif BM_MACOS
extern bool gui_init_macos(gui_platform_t* gs);
#elif BM_LINUX
extern bool gui_init_linux(gui_platform_t* gs);
#endif

extern gui_display_t* gui_create_display(s32 index);
extern void gui_destroy_display(gui_display_t* display);

extern gui_window_t* gui_create_window(const char* title, s32 x, s32 y, s32 w,
				       s32 h, s32 flags, gui_window_t* parent);
extern void gui_destroy_window(gui_window_t* window);
extern void gui_show_window(gui_window_t* window, bool shown);
extern void* gui_get_window_handle(gui_window_t* window);

#endif
