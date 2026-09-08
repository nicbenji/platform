internal void *mem_reserve(U64 size) {

}

internal B32 mem_commit(void *ptr, U64 size) {

}

internal void mem_decommit(void *ptr, U64 size) {

}

internal void mem_free(void *ptr, U64 size) {

}

#include <windows.h>

#if BUILD_CLI
int wmain(int argc, WCHAR **argv) {
    return 0;
}
#else
int wWinMain(
    HINSTANCE instance, HINSTANCE prev_instance,
    PWSTR cmd_line, int show_code
) {
    return 0;
}
#endif

