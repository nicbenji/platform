#include "base/base_inc.h"
#include "base/base_inc.c"

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>

internal Str8 bgen_read_entire_file(MemoryArena *arena, const char *file_path) {
    Str8 result = {0};
    U64 prev_pos = arena->pos;

    int fd = open(file_path, O_RDONLY);
    if (fd < 0) {
        // TODO: diagnostic
        return result;
    }

    struct stat file_stats;
    if (fstat(fd, &file_stats) < 0) {
        // TODO: diagnostic
        goto err;
    }
    result.length = (U64)file_stats.st_size;
    result.begin = mem_arena_push_array(arena, U8, result.length);

    S64 bytes_read = read(fd, result.begin, result.length);
    if (bytes_read < result.length) {
        // FIXME: pop_to should be prev_pos + bytes_read? but that does not work if bytes_read < 0
        mem_arena_pop_to(arena, prev_pos);
        result.length = bytes_read;
    }

    err:;
    close(fd);
    return result;
}

internal int main_entrypoint(int argc, char **argv) {
    MemoryArena *bgen_arena = mem_arena_default();
    Str8 func_signatures = bgen_read_entire_file(bgen_arena, "wl_client.signatures");
    printf("%.*s\n", str8_make_printable(func_signatures));

    mem_arena_free(bgen_arena);

    return 0;
}
