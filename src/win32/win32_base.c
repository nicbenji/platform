#include <windows.h>

/* System props */

global_var U64 global_page_size;

internal U64 sys_info_get_page_size(void) {
    return global_page_size;
}

/* Memory */

internal void *mem_reserve(U64 size) {
    void *result = VirtualAlloc(0, size, MEM_RESERVE, PAGE_READWRITE);
    return result;
}

internal B32 mem_commit(void *ptr, U64 size) {
    B32 result = (VirtualAlloc(ptr, size, MEM_COMMIT, PAGE_READWRITE) != 0);
    return result;
}

internal void mem_decommit(void *ptr, U64 size) {
    VirtualFree(ptr, size, MEM_DECOMMIT);
}

internal void mem_free(void *ptr, U64 size) {
    VirtualFree(ptr, 0, MEM_RELEASE);
}

/* Files */

internal FileHandle file_open(Str8 file_path, FileAccessFlags flags) {
    FileHandle result = {0};
    MemArena_Temp scratch = thread_ctx_scratch_begin(0, 0);
    Str16 file_path16 = str16_from_str8(scratch.arena, file_path);

    DWORD desired_access = 0;
    DWORD share_mode = 0;
    DWORD creation_disposition = 0;
    if (flags & FileAccessFlag_Read) {
        desired_access |= GENERIC_READ;
    }
    if (flags & FileAccessFlag_Write) {
        desired_access |= GENERIC_WRITE;
    }
    // TODO: API decision -> default write behavior is append/replace???
    creation_disposition = OPEN_ALWAYS;

    HANDLE file_handle = CreateFileW(
        (const WCHAR *)file_path16.begin,
        desired_access, share_mode, 0,
        creation_disposition, FILE_ATTRIBUTE_NORMAL, 0
    );

    if (file_handle != INVALID_HANDLE_VALUE) {
        result.u64[0] = (U64)file_handle;
    }

    thread_ctx_scratch_end(scratch);
    return result;
}

internal void file_close(FileHandle file) {
    HANDLE file_handle = (HANDLE)file.u64[0];
    CloseHandle(file_handle);
}

internal U64 file_get_size(FileHandle file) {
    U64 result = 0;
    HANDLE file_handle = (HANDLE)file.u64[0];
    LARGE_INTEGER file_size;
    if (GetFileSizeEx(file_handle, &file_size)) {
        result = s64_safe_cast(file_size.QuadPart);
    };
    return result;
}

internal U64 file_read(FileHandle file, U64 start, U64 end, void *buffer) {
    Assert(end >= start);
    HANDLE file_handle = (HANDLE)file.u64[0];

    U64 offset = start;
    U8 *buf_ptr = buffer;
    while (offset != end) {
        U32 read_amount = (U32)Min(U32_MAX, end - offset);
        DWORD read_result;
        OVERLAPPED overlapped = {
            .Offset = (U32)offset,
            .OffsetHigh = (U32)(offset >> 32)
        };

        if (
            !ReadFile(file_handle, buf_ptr, read_amount, &read_result, &overlapped)
            || read_result == 0
        ) {
            break;
        }

        offset += read_result;
        buf_ptr += read_result;
    }
    U64 bytes_read = offset - start;
    return bytes_read;
}

/* Dynamic libs */

internal LibraryHandle dynlib_load(Str8 lib_path) {
    MemArena_Temp scratch = thread_ctx_scratch_begin(0, 0);
    Str16 lib_path16 = str16_from_str8(scratch.arena, lib_path);

    HMODULE dynlib = LoadLibraryW((const WCHAR *)lib_path16.begin);
    LibraryHandle result = { (U64)dynlib };

    thread_ctx_scratch_end(scratch);
    return result;
}

internal VoidProc *dynlib_load_proc(LibraryHandle lib, Str8 proc_name) {
    MemArena_Temp scratch = thread_ctx_scratch_begin(0, 0);
    Str8 proc_name_copy = str8_copy(scratch.arena, proc_name);

    HMODULE dynlib = (HMODULE)lib.u64[0];
    VoidProc *result = (VoidProc *)GetProcAddress(dynlib, (const char *)proc_name_copy.begin);

    thread_ctx_scratch_end(scratch);
    return result;
}

/* Win32 Entrypoint */

internal Str8 w32_wstr_to_str8(MemoryArena *arena, WCHAR *wstr) {
    U64 length = wcslen(wstr);
    Str16 utf16_str = str16(wstr, length);
    Str8 result = str8_from_str16(arena, utf16_str);
    return result;
}

int w32_entrypoint(int argc, WCHAR **wargv) {
    SYSTEM_INFO sys_info;
    GetSystemInfo(&sys_info);
    global_page_size = sys_info.dwPageSize;

    MemoryArena *w32_arena = mem_arena_default();

    // TODO: platform-independent cli args?
    Str8 *utf8_args = mem_arena_push_array(w32_arena, Str8, argc);
    for (int i = 0; i < argc; ++i) {
        utf8_args[i] = w32_wstr_to_str8(w32_arena, wargv[i]);
    }

    ThreadContext *main_thread_ctx = thread_ctx();
    thread_ctx_set(main_thread_ctx);

    int result = main_entrypoint(argc, utf8_args);
    return result;
}

#if BUILD_CLI
int wmain(int argc, WCHAR **wargv) {
    int result = w32_entrypoint(argc, wargv);
    return result;
}
#else
int wWinMain(
    HINSTANCE instance, HINSTANCE prev_instance,
    PWSTR cmd_line, int show_code
) {
    int result = w32_entrypoint(__argc, __wargv);
    return result;
}
#endif

