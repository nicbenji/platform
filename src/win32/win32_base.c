#include <windows.h>

global_var U64 global_page_size;

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

internal U64 sys_info_get_page_size(void) {
    return global_page_size;
}

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

