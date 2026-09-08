#ifndef BASE_ARENA_H_
#define BASE_ARENA_H_

typedef struct {
    U64 pos;
    U64 reserved;
    U64 committed;
} MemoryArena;

#include <stddef.h>
#define MEM_ARENA_DEFAULT_ALIGN sizeof(max_align_t)

#define MEM_ARENA_HEADER_SIZE sizeof(MemoryArena)

internal MemoryArena *mem_arena(void *base_addr, U64 reserve_size, U64 commit_size);
#define mem_arena_default() mem_arena(0, MiB(64), KiB(64))

internal void mem_arena_free(MemoryArena *arena);

internal void *mem_arena_push(MemoryArena *arena, U64 size, U64 align, B32 zero);

#define mem_arena_push_array(arena, T, count) \
    mem_arena_push(arena, sizeof(T) * (count), Max(AlignOf(T), MEM_ARENA_DEFAULT_ALIGN), false)
#define mem_arena_push_array_zero(arena, T, count) \
    mem_arena_push(arena, sizeof(T) * (count), Max(AlignOf(T), MEM_ARENA_DEFAULT_ALIGN), true)

#define mem_arena_push_struct(arena, T) \
    mem_arena_push_array(arena, T, 1)
#define mem_arena_push_struct_zero(arena, T) \
    mem_arena_push_array_zero(arena, T, 1)

internal void mem_arena_clear(MemoryArena *arena);
internal void mem_arena_pop_to(MemoryArena *arena, U64 pos);

typedef struct {
    MemoryArena *arena;
    U64 prior_pos;
} MemArena_Temp;

internal MemArena_Temp mem_arena_temp_begin(MemoryArena *arena);
internal void mem_arena_temp_end(MemArena_Temp temp);


#endif  // BASE_ARENA_H_
