#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

#include <dlfcn.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#include "linux_inc.h"
#include "linux_inc.c"

/* System props */

global_var U64 global_page_size;

internal U64 sys_info_get_page_size(void) {
    return global_page_size;
}

/* Memory */

internal void *mem_reserve(U64 size) {
    void *result = mmap(0, size, PROT_NONE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    if (result == MAP_FAILED) {
        result = 0;
    }
    return result;
}

internal B32 mem_commit(void *ptr, U64 size) {
    mprotect(ptr, size, PROT_READ|PROT_WRITE);
    return 1;
}

internal void mem_decommit(void *ptr, U64 size) {
    madvise(ptr, size, MADV_DONTNEED);
    mprotect(ptr, size, PROT_NONE);
}

internal void mem_free(void *ptr, U64 size) {
    munmap(ptr, size);
}

/* Files */

internal FileHandle file_open(Str8 file_path, FileAccessFlags flags) {
    FileHandle result = {0};
    int open_flags = 0;

    if ((flags & FileAccessFlag_Read) && (flags & FileAccessFlag_Write)) {
        open_flags = O_RDWR;
    } else if (flags & FileAccessFlag_Read) {
        open_flags = O_RDONLY;
    } else if (flags & FileAccessFlag_Write) {
        open_flags = O_WRONLY;
    }

    open_flags |= O_CLOEXEC;

    MemArena_Temp scratch = thread_ctx_scratch_begin(0, 0);
    Str8 path_copy = str8_copy(scratch.arena, file_path);

    int fd = open((const char *)path_copy.begin, open_flags);
    if (fd != -1) {
        result.u64[0] = (U64)fd;
    }

    thread_ctx_scratch_end(scratch);
    return result;
}

internal void file_close(FileHandle file) {
    int fd = (int)file.u64[0];
    close(fd);
}

internal U64 file_get_size(FileHandle file) {
    struct stat file_stats;
    int fd = (int)file.u64[0];
    fstat(fd, &file_stats);
    U64 result = s64_safe_cast(file_stats.st_size);
    return result;
}

internal U64 file_read(FileHandle file, U64 start, U64 end, void *buffer) {
    Assert(end >= start);
    int fd = (int)file.u64[0];

    U64 bytes_read = 0;
    U64 left_to_read = end - start;
    while (left_to_read > 0) {
        S64 read_result = pread(
            fd, (U8 *)buffer + bytes_read,
            left_to_read, s64_safe_cast(start + bytes_read)
        );

        if (read_result > 0) {
            bytes_read += (U64)read_result;
            left_to_read -= (U64)read_result;
        } else {
            // TODO: EOF == 0 vs. diagnostic + error handling <0
            // e.g. EINTR??
            break;
        }
    }
    return bytes_read;
}

/* Dynamic libs */

internal LibraryHandle dynlib_load(Str8 lib_path) {
    MemArena_Temp scratch = thread_ctx_scratch_begin(0, 0);
    Str8 lib_path_copy = str8_copy(scratch.arena, lib_path);

    void *dynlib = dlopen((const char *)lib_path_copy.begin, RTLD_NOW);
    LibraryHandle result = { (U64)dynlib };

    thread_ctx_scratch_end(scratch);
    return result;
}

internal VoidProc *dynlib_load_proc(LibraryHandle lib, Str8 proc_name) {
    MemArena_Temp scratch = thread_ctx_scratch_begin(0, 0);
    Str8 proc_name_copy = str8_copy(scratch.arena, proc_name);

    void *dynlib = (void *)lib.u64[0];
    VoidProc *result = (VoidProc *)dlsym(dynlib, (const char *)proc_name_copy.begin);

    thread_ctx_scratch_end(scratch);
    return result;
}


int main(int argc, char **argv) {
    global_page_size = (U64)sysconf(_SC_PAGESIZE);

    Str8 utf8_args[argc];
    for (int i = 0; i < argc; ++i) {
        utf8_args[i] = str8_from_cstr(argv[i]);
    }

    ThreadContext *main_thread_ctx = thread_ctx();
    thread_ctx_set(main_thread_ctx);

    const char *window_manager = getenv("XDG_SESSION_TYPE");
    if (strcmp(window_manager, "wayland") == 0) {

        LibraryHandle wl_client_lib 
            = dynlib_load(str8_literal("libwayland-client.so.0"));

        Wl_Functions wl;
        if (!wl_load_functions(wl_client_lib, &wl)) {
            // TODO: diagnostic + error msg
            return 420;
        }

    } else if (strcmp(window_manager, "x11") == 0) {


    } else {
        // TODO: diagnostic + error msg
        return 69;
    }

    int result = main_entrypoint(argc, utf8_args);
    return result;
}

