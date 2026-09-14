#ifndef BASE_THREAD_CONTEXT_H_
#define BASE_THREAD_CONTEXT_H_

typedef struct {
    MemoryArena *scratch_arenas[2];
} ThreadContext;

internal ThreadContext *thread_ctx(void);
internal void thread_ctx_free(ThreadContext *thread_ctx);

// NOTE: Getters and setters rn only for more readable code
internal ThreadContext *thread_ctx_get(void);
internal void thread_ctx_set(ThreadContext *thread_ctx);

/* Scratch arenas */
internal MemoryArena *thread_ctx_get_scratch(MemoryArena **conflicts, U64 count);
#define thread_ctx_scratch_begin(conflicts, count) \
    mem_arena_temp_begin(thread_ctx_get_scratch(conflicts, count))
#define thread_ctx_scratch_end(scratch_arena) \
    mem_arena_temp_end(scratch_arena)

#endif  // BASE_THREAD_CONTEXT_H_
