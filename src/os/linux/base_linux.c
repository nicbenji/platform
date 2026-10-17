/* System props */

global_var U64 g_page_size;

internal U64 sys_info_get_page_size(void) {
    return g_page_size;
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

internal File file_open(Str8 file_path, FileFlags flags) {
    File result = file_zero();
    int open_flags = 0;

    if ((flags & FileFlag_Read) && (flags & FileFlag_Write)) {
        open_flags = O_RDWR;
        open_flags |= O_CREAT;
    } else if (flags & FileFlag_Read) {
        open_flags = O_RDONLY;
    } else if (flags & FileFlag_Write) {
        open_flags = O_WRONLY;
        open_flags |= O_CREAT;
    }

    if (flags & FileFlag_Append) {
        open_flags |= O_APPEND;
    }
    if (flags & FileFlag_Truncate) {
        open_flags |= O_TRUNC;
    }
    if (flags & FileFlag_MakeDirs) {
        // TODO: split file path and check for dirs
        // TODO: for each dir check if exists if not create
        // TODO: dir_make(Str8 path)???
    }
    open_flags |= O_CLOEXEC;

    MemArena_Temp scratch = thread_ctx_scratch_begin(0, 0);
    Str8 path_copy = str8_copy(scratch.arena, file_path);

    // TODO: correct file permissions
    int fd = open((const char *)path_copy.begin, open_flags, 0744);
    if (fd != -1) {
        result.u64[0] = (U64)fd;
    }

    thread_ctx_scratch_end(scratch);
    return result;
}

internal void file_close(File file) {
    int fd = (int)file.u64[0];
    close(fd);
}

internal U64 file_get_size(File file) {
    struct stat file_stats;
    int fd = (int)file.u64[0];
    fstat(fd, &file_stats);
    U64 result = (U64)file_stats.st_size;
    return result;
}

internal U64 file_read(File file, U64 start, U64 end, void *buffer) {
    Assert(end >= start);
    int fd = (int)file.u64[0];

    U64 bytes_read = 0;
    U64 left_to_read = end - start;
    for (; left_to_read > 0; ) {
        S64 read_result = pread(
            fd, (U8 *)buffer + bytes_read,
            left_to_read, s64_safe_cast(start + bytes_read)
        );

        if (read_result > 0) {
            bytes_read += (U64)read_result;
            left_to_read -= (U64)read_result;
        } else if (errno != EINTR) {
            // TODO: EOF == 0 vs. diagnostic + error handling <0
            break;
        }
    }
    return bytes_read;
}

internal U64 file_write(File file, U64 start, U64 end, void *buffer) {
    Assert(end >= start);
    int fd = (int)file.u64[0];

    U64 bytes_written = 0;
    U64 left_to_write = end - start;
    for (; bytes_written < left_to_write;) {
        S64 write_result = pwrite(
            fd, (U8 *)buffer + bytes_written,
            left_to_write, s64_safe_cast(start + bytes_written)
        );

        if (write_result >= 0) {
            bytes_written += (U64)write_result;
            left_to_write -= (U64)write_result;
        } else if (errno != EINTR) {
            // TODO: Error handling
            break;
        }
    }
    return bytes_written;
}

/* Dynamic libs */

internal Library dynlib_load(Str8 lib_path) {
    MemArena_Temp scratch = thread_ctx_scratch_begin(0, 0);
    Str8 lib_path_copy = str8_copy(scratch.arena, lib_path);

    void *dynlib = dlopen((const char *)lib_path_copy.begin, RTLD_NOW);
    Library result = { (U64)dynlib };

    thread_ctx_scratch_end(scratch);
    return result;
}

internal VoidProc *dynlib_load_proc(Library lib, Str8 proc_name) {
    MemArena_Temp scratch = thread_ctx_scratch_begin(0, 0);
    Str8 proc_name_copy = str8_copy(scratch.arena, proc_name);

    void *dynlib = (void *)lib.u64[0];
    VoidProc *result = (VoidProc *)dlsym(dynlib, (const char *)proc_name_copy.begin);

    thread_ctx_scratch_end(scratch);
    return result;
}

int main(int argc, char **argv) {
    g_page_size = (U64)sysconf(_SC_PAGESIZE);

    MemoryArena *linux_arena = mem_arena_default();

    Str8 *utf8_args = mem_arena_push_array(linux_arena, Str8, (U64)argc);
    for (int i = 0; i < argc; ++i) {
        utf8_args[i] = str8_from_cstr(argv[i]);
    }

    ThreadContext *main_thread_ctx = thread_ctx();
    thread_ctx_set(main_thread_ctx);

    const char *window_manager = getenv("XDG_SESSION_TYPE");
    if (strcmp(window_manager, "wayland") == 0) {

    } else if (strcmp(window_manager, "x11") == 0) {

    } else {
        // TODO: diagnostic + error msg
        return 69;
    }

    int result = main_entrypoint(argc, utf8_args);
    return result;
}

