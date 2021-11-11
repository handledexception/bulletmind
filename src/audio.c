#include "audio.h"

#include "core/logger.h"
#include "core/memory.h"

#include <stdio.h>

#include <SDL_mixer.h>

bool audio_init(s32 num_channels, s32 sample_rate, s32 chunk_size)
{
	// if (Mix_OpenAudioDevice(48000, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024,
	//                       "Speakers (ASUS Xonar DGX Audio Device)", SDL_AUDIO_ALLOW_ANY_CHANGE) == -1)
	if (Mix_OpenAudio(sample_rate, MIX_DEFAULT_FORMAT, num_channels,
			  chunk_size) == -1)
		return false;
	Mix_AllocateChannels(32);
	logger(LOG_INFO,
	       "Initialized audio: Channels: %d | Sample rate %d | Chunk Size: %d",
	       num_channels, sample_rate, chunk_size);

	return true;
}

bool audio_load_sound(const char* path, audio_chunk_t** data)
{

	Mix_Chunk* wav_file = Mix_LoadWAV(path);
	if (!wav_file)
		return false;
	audio_chunk_t* chunk = (audio_chunk_t*)arena_alloc(
		&g_mem_arena, sizeof(audio_chunk_t), DEFAULT_ALIGNMENT);
	// audio_chunk_t* chunk = (audio_chunk_t*)malloc(sizeof(audio_chunk_t));
	chunk->allocated = (bool)wav_file->allocated;
	chunk->data = wav_file->abuf;
	chunk->size = wav_file->alen;
	chunk->volume = wav_file->volume;
	*data = chunk;
	return true;
}

void audio_shutdown()
{
	Mix_CloseAudio();
}
