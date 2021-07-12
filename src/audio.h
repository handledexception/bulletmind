#pragma once

#include "core/types.h"

#define BM_NUM_AUDIO_CHANNELS 2
#define BM_AUDIO_SAMPLE_RATE 48000
#define BM_AUDIO_CHUNK_SIZE 4096

typedef struct audio_chunk_s {
	bool allocated;
	u8 *data;
	size_t size;
	u8 volume; /* Per-sample volume, 0-128 */
} audio_chunk_t;

bool audio_init(i32 num_channels, i32 sample_rate, i32 chunk_size);
bool audio_load_wav(const char *path, audio_chunk_t **data);
void audio_shutdown();
