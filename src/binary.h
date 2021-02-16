#pragma once

#include "c99defs.h"
#include "types.h"

typedef enum {
	SEEK_ORIGIN_BEGIN,
	SEEK_ORIGIN_CURRENT,
	SEEK_ORIGIN_END,
} seek_origin_t;

typedef struct stream_s {
	u8* data;
	size_t size;
	size_t position;
} stream_t;

bool bin_stream_init(stream_t** stream, u8* data, const size_t size);
void bin_stream_shutdown(stream_t* stream);
bool bin_stream_seek(stream_t* stream, const seek_origin_t origin, const i32 offset);
bool bin_stream_write(stream_t* stream, u8* data, const size_t size, size_t* bytes_written);
// void bin_write_i8(stream_t* stream, const i8 value, const seek_origin_t origin);
// void binary_writer_write_i16(stream_t* stream, const i16 value);
// void binary_writer_write_i32(stream_t* stream, const i32 value);
// void binary_writer_write_i64(stream_t* stream, const i64 value);
