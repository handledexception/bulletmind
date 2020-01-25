#ifndef H_MAIN
#define H_MAIN

#define NULL_INDEX -1
#define TEMP_STRING_MAX 4096

#define WINDOW_WIDTH 960
#define WINDOW_HEIGHT 720
#define CAMERA_WIDTH 320
#define CAMERA_HEIGHT 240
#define WINDOW_WIDTH_HALF WINDOW_WIDTH/2
#define WINDOW_HEIGHT_HALF WINDOW_HEIGHT/2
#define TARGET_FPS 60
#define TARGET_FRAMETIME(x) 1.0/x

#define ARRAY_SIZE(x) sizeof(x) / sizeof(x[0])
#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

#endif
