#include "base/base_inc.h"
#include "base/base_inc.c"

#include "os/window_manager/window_manager_inc.h"
#include "os/window_manager/window_manager_inc.c"

internal int main_entrypoint(int argc, Str8 *argv) {
    MemoryArena *arena = mem_arena_default();
    Wm_Window window_handle
        = wm_open_main_window(arena, str8_lit("Test App"), 1280, 720);

    for (; !wm_window_should_close(window_handle);) {
        wm_process_events(window_handle);
    }

    return 0;
}
