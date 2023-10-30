#pragma once

typedef struct buffer_s buffer_t;

size_t file_read(const char* path, buffer_t* buffer);
