#include "buffer.h"
#include "memalign.h"

buffer_t* buffer_new(size_t size) {
    buffer_t* buf = (buffer_t*)malloc(sizeof(buffer_t));
    buf->size = size;
    buf->data = (uint8_t*)malloc(size);
    memset(buf->data, 0, size);
    return buf;
}

buffer_t* buffer_aligned_new(size_t size, size_t alignment) {
    const size_t aligned_size = size + (alignment-1) + sizeof(void*);
    buffer_t* buf = (buffer_t*)malloc(sizeof(buffer_t));
    
    buf->data = aligned_malloc(size, alignment);

    memset(buf->data, 0, aligned_size);

    buf->size = aligned_size;

    aligned_free(buf->data);

    return buf;
}

void buffer_delete(buffer_t* buf) {
    if(buf) {
        if (buf->data) { free(buf->data); }
        buf->data = NULL;
        buf->size = 0;
        free(buf);
    }
}

void buffer_setdata(buffer_t* buf, void* data, size_t size) {
    if (buf) {
        if (buf->data) {
            free(buf->data);
            buf->data = (uint8_t*)realloc(buf->data, size);
            memcpy(buf->data, data, size);
            buf->size = size;
        }
        else {
            buf->data = (uint8_t*)malloc(size);
            buf->size = size;
        }
    }
}

uint8_t buffer_resize(buffer_t* buf, size_t size) {
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
            uint8_t* shrink = realloc(buf->data, size);
            if (shrink != NULL) { buf->data = shrink; }
            else {
                free(shrink);
                return -1;
            }
            buf->size = size;
        } else
        // grow the buffer
        if (size > buf->size) {
            uint8_t* grow = (uint8_t*)malloc(sizeof(uint8_t) * size);
            memcpy(grow, buf->data, size);
            buf->data = grow;
            buf->size = size;
        }
    }

    return 0;
}
