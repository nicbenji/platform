#include <windows.h>

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

int w32_entrypoint(int argc, WCHAR **wargv) {
    // TODO: convert to char ** or platform-independent cli args
    return entrypoint(argc, wargv);
}


#if BUILD_CLI
int wmain(int argc, WCHAR **wargv) {
    return w32_entrypoint(argc, wargv);
}
#else
int wWinMain(
    HINSTANCE instance, HINSTANCE prev_instance,
    PWSTR cmd_line, int show_code
) {
    return w32_entrypoint(__argc, __wargv);
}
#endif

