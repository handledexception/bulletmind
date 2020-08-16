#ifndef _H_MEM_ARENA
#define _H_MEM_ARENA

#include "c99defs.h"

// Basic linear allocator
// https://www.gingerbill.org/article/2019/02/08/memory-allocation-strategies-002/

#ifndef DEFAULT_ALIGNMENT
#define DEFAULT_ALIGNMENT (2 * sizeof(void*))
#endif

#define ARENA_TOTAL_BYTES 8388608 // 8MiB

typedef struct arena_s {
    uint8_t* buffer;
    size_t sz_buffer;
    size_t prev_offset;
    size_t curr_offset;
} arena_t;

extern arena_t mem_arena;

void arena_init(arena_t* arena, void* backing_buffer, size_t sz_backing);
void arena_free_all(arena_t* arena);
void* arena_alloc(arena_t* arena, size_t size, size_t align);

#endif
