#ifndef H_BUFFER
#define H_BUFFER

#include "c99defs.h"
#include <stdlib.h>

// generic data buffer
typedef struct {
	char *data;
	size_t size;
} buffer_t;

buffer_t *buffer_new(size_t size);
void buffer_delete(buffer_t *buf);
void buffer_setdata(buffer_t *buf, void *data, size_t size);
uint8_t buffer_resize(buffer_t *buf, size_t size);

#endif