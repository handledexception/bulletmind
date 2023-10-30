#include "core/file_io.h"
#include "core/buffer.h"

#include <stdio.h>
#include <stdlib.h>

size_t file_read(const char* path, buffer_t* buffer)
{
    if (path == NULL || buffer == NULL)
        return 0;

    FILE* f = fopen(path, "r");
    fseek(f, 0, SEEK_END);
    long file_size = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (buffer->size < file_size)
        buffer_resize(buffer, file_size);
    size_t bytes_read = fread(&buffer->data[0], 1, file_size, f);
    fclose(f);

    return bytes_read;
}
