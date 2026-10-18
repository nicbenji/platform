#include "base/base_inc.h"
#include "base/base_inc.c"

#include "os/window_manager/window_manager_inc.h"
#include "os/window_manager/window_manager_inc.c"

internal int main_entrypoint(int argc, Str8 *argv) {
    MemoryArena *arena = mem_arena_default();
    WM_Window window_handle = wm_open_main_window(str8_lit("Test App"), 1280, 720);

    for (B32 running = true; running;) {
        WM_EventList events = wm_get_events(arena, window_handle);
        for (WM_Event *curr = events.first; curr != 0; curr = curr->next) {
            switch (curr->kind) {
                case WM_EventKind_WindowClose: {
                    running = false;
                } break;

                default: {
                } break;
            }
        }
    }

    return 0;
}
