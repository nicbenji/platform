#include "base/base_inc.h"
#include "base/base_inc.c"

#include "os/window_manager/window_manager_inc.h"
#include "os/window_manager/window_manager_inc.c"

internal void render_wierd_gradient(WM_Backbuffer *buffer, int x_offset, int y_offset) {
    int stride = buffer->width * buffer->bytes_per_pixel;
    U8 *row = buffer->memory;
    for (int y = 0; y < buffer->height; ++y) {
        U32 *pixel = (U32 *)row;
        for (int x = 0; x < buffer->width; ++x) {
            U8 blue = (U8)(x + x_offset);
            U8 green = (U8)(y + y_offset);

            // BB GG RR xx
            *pixel = ((green << 8) | blue);
            ++pixel;
        }

        row += stride;
    }
}

internal int main_entrypoint(int argc, Str8 *argv) {
    MemoryArena *arena = mem_arena_default();
    WM_Window window = wm_open_main_window(str8_lit("Test App"), 1280, 720);

    int x_off = 0, y_off = 0;
    for (B32 running = true; running;) {
        WM_EventList events = wm_get_events(arena, window);
        for (WM_Event *curr = events.first; curr != 0; curr = curr->next) {
            switch (curr->kind) {
                case WM_EventKind_WindowClose: {
                    running = false;
                } break;

                default: {
                } break;
            }
        }
        WM_Backbuffer *frame_buffer = wm_get_backbuffer(window);
        render_wierd_gradient(frame_buffer, x_off, y_off);

        wm_present_backbuffer(window);

        ++x_off;
        y_off += 2;
    }

    return 0;
}
