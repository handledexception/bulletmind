#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct buffer_s buffer_t;

size_t file_read(const char* path, buffer_t* buffer);

#ifdef __cplusplus
}
#endif
