#ifndef _H_BINARY
#define _H_BINARY

#include "c99defs.h"

typedef enum {
    SEEK_ORIGIN_BEGIN,
    SEEK_ORIGIN_CURRENT,
    SEEK_ORIGIN_END,
} seek_origin_t;

typedef struct stream_s {
    uint8_t* data;
    size_t size;
    size_t position;
} stream_t;

bool bin_stream_init(stream_t** stream, uint8_t* data, const size_t size);
void bin_stream_shutdown(stream_t* stream);
bool bin_stream_seek(stream_t* stream, const seek_origin_t origin, const int32_t offset);
bool bin_stream_write(stream_t* stream, uint8_t* data, const size_t size, size_t* bytes_written);
// void bin_write_i8(stream_t* stream, const int8_t value, const seek_origin_t origin);
// void binary_writer_write_i16(stream_t* stream, const int16_t value);
// void binary_writer_write_i32(stream_t* stream, const int32_t value);
// void binary_writer_write_i64(stream_t* stream, const int64_t value);

#endif
