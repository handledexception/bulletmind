#pragma once

typedef enum {
	IMG_TYPE_TARGA,
	IMG_TYPE_BMP,
	IMG_TYPE_PNG,
	IMG_TYPE_JPEG,
	IMG_TYPE_RAW,
} imgtype_t;

typedef enum {
	BGR24,
	RGB24,
	RGBA32,
	BGRA32,
	ARGB32,
} pix_fmt_t;
