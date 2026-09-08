// TODO: large pages
// TODO: asan
// TODO: scratch arena

internal MemoryArena *
mem_arena(void *base_addr, U64 reserve_size, U64 commit_size) {
    Assert(reserve_size >= commit_size);

    // TODO: add MEM_ARENA_HEADER_SIZE explicitly?
    U64 aligned_reserve_size = AlignUpPow2(reserve_size, sys_info_get_page_size());
    U64 aligned_commit_size = AlignUpPow2(commit_size, sys_info_get_page_size());

    // TODO: what to do on failures?
    MemoryArena *result = mem_reserve(base_addr, aligned_reserve_size);
    mem_commit(result, aligned_commit_size);

    if (result == 0) {
        // TODO: diagnostic + proper error handling/message/window
        Assert(!"Allocation failed");
    }

    result->pos = MEM_ARENA_HEADER_SIZE;
    result->reserved = aligned_reserve_size;
    result->committed = aligned_commit_size;

    return result;
}

internal void mem_arena_free(MemoryArena *arena) {
    mem_free(arena, arena->reserved);
}

internal void *
mem_arena_push(MemoryArena *arena, U64 size, U64 align, B32 zero) {
    U64 prev_pos = AlignUpPow2(arena->pos, align);
    arena->pos = prev_pos + size;

    if (arena->pos > arena->committed) {
        U64 new_commit_size = AlignUpPow2(arena->pos, sys_info_get_page_size());
        U64 prev_commit_size = arena->committed;

        // TODO: handle this somehow? grow arena?
        Assert(new_commit_size <= arena->reserved);

        void *commit_block_start = (U8 *)arena + prev_commit_size;
        mem_commit(commit_block_start, new_commit_size - prev_commit_size);

        arena->committed = new_commit_size;
    }

    void *result = (U8 *)arena + arena->pos;
    return result;
}

internal void mem_arena_clear(MemoryArena *arena) {
    mem_arena_pop_to(arena, 0);
}

internal void mem_arena_pop_to(MemoryArena *arena, U64 pos) {
    U64 new_pos = Max(MEM_ARENA_HEADER_SIZE, pos);
    Assert(new_pos <= arena->pos);
    arena->pos = pos;
}

internal MemArena_Temp mem_arena_temp_begin(MemoryArena *arena) {
    MemArena_Temp result = { arena, arena->pos };
    return result;
}

internal void mem_arena_temp_end(MemArena_Temp temp) {
    mem_arena_pop_to(temp.arena, temp.prior_pos);
}
