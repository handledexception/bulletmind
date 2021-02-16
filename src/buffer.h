#pragma once

#include "c99defs.h"
#include "types.h"

// generic data buffer
typedef struct buffer_s {
	u8* data;
	size_t size;
} buffer_t;

buffer_t* buffer_new(size_t size);
buffer_t* buffer_aligned_new(size_t size, size_t alignment);
void buffer_delete(buffer_t* buf);
void buffer_setdata(buffer_t* buf, void* data, size_t size);
u8 buffer_resize(buffer_t* buf, size_t size);
