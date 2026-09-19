C_LINKAGE thread_local ThreadContext *thread_ctx_thread_local = 0;

internal ThreadContext *thread_ctx(void) {
    MemoryArena *scratch_arena0 = mem_arena_default();
    MemoryArena *scratch_arena1 = mem_arena_default();

    ThreadContext *result = mem_arena_push_array(scratch_arena0, ThreadContext, 1);
    result->scratch_arenas[0] = scratch_arena0;
    result->scratch_arenas[1] = scratch_arena1;
    return result;
}

internal void thread_ctx_free(ThreadContext *thread_ctx) {
    mem_arena_free(thread_ctx->scratch_arenas[0]);
    mem_arena_free(thread_ctx->scratch_arenas[1]);
}

internal ThreadContext *thread_ctx_current(void) {
    return thread_ctx_thread_local;
}

internal void thread_ctx_set(ThreadContext *thread_ctx) {
    thread_ctx_thread_local = thread_ctx;
}

internal MemoryArena *thread_ctx_get_scratch(MemoryArena **conflicts, U64 count) {
    MemoryArena *result = 0;
    ThreadContext *thread_ctx = thread_ctx_thread_local;

    U64 scratch_count = ArrayCount(thread_ctx->scratch_arenas);
    for (U64 scratch_idx = 0; scratch_idx < scratch_count; ++scratch_idx) {
        MemoryArena *arena = thread_ctx->scratch_arenas[scratch_idx];

        B32 has_conflict = false;
        for (U64 conflict_idx = 0; conflict_idx < count; ++conflict_idx) {
            MemoryArena *conflict = conflicts[conflict_idx];

            if (arena == conflict) {
                has_conflict = true;
                break;
            }
        }

        if (!has_conflict) {
            result = arena;
            break;
        }
    }
    return result;
}
