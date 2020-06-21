#ifndef _H_BUFFER
#define _H_BUFFER

#include "c99defs.h"

// generic data buffer
typedef struct buffer_s {
    uint8_t *data;
    size_t size;
} buffer_t;

buffer_t* buffer_new(size_t size);
buffer_t* buffer_aligned_new(size_t size, size_t alignment);
void buffer_delete(buffer_t* buf);
void buffer_setdata(buffer_t* buf, void* data, size_t size);
uint8_t buffer_resize(buffer_t* buf, size_t size);

#endif