#include "memarena.h"

#include <assert.h>

bool is_power_of_two(uintptr_t x)
{
    return (x & (x - 1)) == 0;
}

uintptr_t align_forward(uintptr_t ptr, size_t align)
{
    uintptr_t p, a, modulo;

    assert(is_power_of_two(align));
    if (!is_power_of_two(align))
        return 0;

    p = ptr;
    a = (uintptr_t)align;
    modulo = p & (a - 1);

    if (modulo != 0)
        p += a - modulo;

    return p;
}

void arena_init(arena_t* arena, void* backing_buffer, size_t sz_backing)
{
    arena->buffer = (uint8_t*)backing_buffer;
    arena->sz_buffer = sz_backing;
    arena->curr_offset = 0;
    arena->prev_offset = 0;
}

void arena_free_all(arena_t* arena)
{
    arena->curr_offset = 0;
    arena->prev_offset = 0;
}

void* arena_alloc(arena_t* arena, size_t size, size_t align)
{
    uintptr_t curr_ptr = (uintptr_t)arena->buffer + (uintptr_t)arena->curr_offset;
    uintptr_t offset = align_forward(curr_ptr, align);
    if (offset == 0)
        return NULL;

    offset -= (uintptr_t)arena->buffer;

    if (offset + size <= arena->sz_buffer) {
        void* ptr = &arena[offset];
        arena->prev_offset = offset;
        arena->curr_offset = offset + size;
        memset(ptr, 0, size);

        return ptr;
    }

    return NULL;
}