#pragma once

#include "core/types.h"

#define BM_NUM_AUDIO_CHANNELS 2
#define BM_AUDIO_SAMPLE_RATE 44100
#define BM_AUDIO_CHUNK_SIZE 4096

typedef struct audio_chunk_s {
	bool allocated;
	u8* data;
	u32 size;
	u8 volume; /* Per-sample volume, 0-128 */
} audio_chunk_t;

typedef struct audio_state_s {
	void* sfx;
	void* music;
	bool music_playing;
	u8 music_volume;
} audio_state_t;

bool audio_init(s32 num_channels, s32 sample_rate, s32 chunk_size);
bool audio_load_sound(const char* path, audio_chunk_t** data);
void audio_shutdown();
