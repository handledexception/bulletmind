#include "audio.h"
#include "core/memarena.h"

#include <stdio.h>

#include <SDL_mixer.h>

bool audio_init(i32 num_channels, i32 sample_rate, i32 chunk_size)
{
    if (Mix_OpenAudio(sample_rate, MIX_DEFAULT_FORMAT, num_channels, chunk_size) == -1)
        return false;
    printf("Initialized audio: Channels: %d | Sample rate %d | Chunk Size: %d", num_channels, sample_rate, chunk_size);
    return true;
}

bool audio_load_wav(const char* path, audio_chunk_t** data)
{
    audio_chunk_t* chunk = (audio_chunk_t *)arena_alloc(&g_mem_arena, sizeof(audio_chunk_t),
                        DEFAULT_ALIGNMENT);

    Mix_Chunk* wav_file = Mix_LoadWAV(path);
    chunk->allocated = wav_file->allocated;
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
