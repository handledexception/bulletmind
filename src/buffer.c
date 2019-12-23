#include "buffer.h"

#include <string.h>

buffer_t *buffer_new(size_t size)
{
    // todo: alignment
    buffer_t *buf = (buffer_t *)malloc(sizeof(buffer_t));
    buf->size = size;
    buf->data = (char *)malloc(sizeof(char) * size);

    return buf;
}

void buffer_delete(buffer_t *buf)
{
    if(buf) {
        if (buf->data) { free(buf->data); }
        buf->data = NULL;
        buf->size = 0;
        free(buf);
    }
}

void buffer_setdata(buffer_t *buf, void *data, size_t size)
{
    if (buf) {
        free(buf->data);
        buf->data = (char *)data;
        buf->size = size;
    }
}

uint8_t buffer_resize(buffer_t *buf, size_t size)
{
    if (buf) {
        // clearing the buffer
        if (size <= 0) {
            free(buf->data);
            buf->data = NULL;
            buf->size = size;
            return 0;
        } else
        // shrink the buffer
        if (size <= buf->size && buf->data) {
            char *shrink = realloc(buf->data, size);
            if (shrink != NULL) { buf->data = shrink; }
            else {
                free(shrink);
                return -1;
            }
            buf->size = size;
        } else
        // grow the buffer
        if (size > buf->size) {
            char *grow = (char *)malloc(sizeof(char) * size);
            memcpy(grow, buf->data, size);
            buf->data = grow;
            buf->size = size;
        }
    }

    return 0;
}
