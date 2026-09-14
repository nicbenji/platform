#include "base/base_inc.h"
#include "base/base_inc.c"

internal int main_entrypoint(int argc, Str8 *argv) {
    MemoryArena *bgen_arena = mem_arena_default();
    Str8 func_signatures = file_read_all(bgen_arena, str8_literal("wl_client.signatures"));
    printf("%.*s\n", str8_make_printable(func_signatures));

    mem_arena_free(bgen_arena);

    return 0;
}
