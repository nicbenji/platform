#include "auzio_platform.h"
#include "raylib.h"
#include <sys/mman.h>
#include <stdio.h>

internal EditorScanKeyCode raylib_to_editor_scan_key_code(int raylib_key_code) {
    switch (raylib_key_code) {
        // Letters
        case KEY_A: return EDITOR_SCAN_KEY_A;
        case KEY_B: return EDITOR_SCAN_KEY_B;
        case KEY_C: return EDITOR_SCAN_KEY_C;
        case KEY_D: return EDITOR_SCAN_KEY_D;
        case KEY_E: return EDITOR_SCAN_KEY_E;
        case KEY_F: return EDITOR_SCAN_KEY_F;
        case KEY_G: return EDITOR_SCAN_KEY_G;
        case KEY_H: return EDITOR_SCAN_KEY_H;
        case KEY_I: return EDITOR_SCAN_KEY_I;
        case KEY_J: return EDITOR_SCAN_KEY_J;
        case KEY_K: return EDITOR_SCAN_KEY_K;
        case KEY_L: return EDITOR_SCAN_KEY_L;
        case KEY_M: return EDITOR_SCAN_KEY_M;
        case KEY_N: return EDITOR_SCAN_KEY_N;
        case KEY_O: return EDITOR_SCAN_KEY_O;
        case KEY_P: return EDITOR_SCAN_KEY_P;
        case KEY_Q: return EDITOR_SCAN_KEY_Q;
        case KEY_R: return EDITOR_SCAN_KEY_R;
        case KEY_S: return EDITOR_SCAN_KEY_S;
        case KEY_T: return EDITOR_SCAN_KEY_T;
        case KEY_U: return EDITOR_SCAN_KEY_U;
        case KEY_V: return EDITOR_SCAN_KEY_V;
        case KEY_W: return EDITOR_SCAN_KEY_W;
        case KEY_X: return EDITOR_SCAN_KEY_X;
        case KEY_Y: return EDITOR_SCAN_KEY_Y;
        case KEY_Z: return EDITOR_SCAN_KEY_Z;

        // Numbers
        case KEY_ZERO: return EDITOR_SCAN_KEY_0;
        case KEY_ONE: return EDITOR_SCAN_KEY_1;
        case KEY_TWO: return EDITOR_SCAN_KEY_2;
        case KEY_THREE: return EDITOR_SCAN_KEY_3;
        case KEY_FOUR: return EDITOR_SCAN_KEY_4;
        case KEY_FIVE: return EDITOR_SCAN_KEY_5;
        case KEY_SIX: return EDITOR_SCAN_KEY_6;
        case KEY_SEVEN: return EDITOR_SCAN_KEY_7;
        case KEY_EIGHT: return EDITOR_SCAN_KEY_8;
        case KEY_NINE: return EDITOR_SCAN_KEY_9;

        // Function keys
        case KEY_F1: return EDITOR_SCAN_KEY_F1;
        case KEY_F2: return EDITOR_SCAN_KEY_F2;
        case KEY_F3: return EDITOR_SCAN_KEY_F3;
        case KEY_F4: return EDITOR_SCAN_KEY_F4;
        case KEY_F5: return EDITOR_SCAN_KEY_F5;
        case KEY_F6: return EDITOR_SCAN_KEY_F6;
        case KEY_F7: return EDITOR_SCAN_KEY_F7;
        case KEY_F8: return EDITOR_SCAN_KEY_F8;
        case KEY_F9: return EDITOR_SCAN_KEY_F9;
        case KEY_F10: return EDITOR_SCAN_KEY_F10;
        case KEY_F11: return EDITOR_SCAN_KEY_F11;
        case KEY_F12: return EDITOR_SCAN_KEY_F12;

        // Modifiers
        case KEY_LEFT_SHIFT: return EDITOR_SCAN_KEY_LEFT_SHIFT;
        case KEY_RIGHT_SHIFT: return EDITOR_SCAN_KEY_RIGHT_SHIFT;
        case KEY_LEFT_CONTROL: return EDITOR_SCAN_KEY_LEFT_CONTROL;
        case KEY_RIGHT_CONTROL: return EDITOR_SCAN_KEY_RIGHT_CONTROL;
        case KEY_LEFT_ALT: return EDITOR_SCAN_KEY_LEFT_ALT;
        case KEY_RIGHT_ALT: return EDITOR_SCAN_KEY_RIGHT_ALT;
        case KEY_CAPS_LOCK: return EDITOR_SCAN_KEY_CAPS_LOCK;

        // Common
        case KEY_ESCAPE: return EDITOR_SCAN_KEY_ESCAPE;
        case KEY_BACKSPACE: return EDITOR_SCAN_KEY_BACKSPACE;
        case KEY_DELETE: return EDITOR_SCAN_KEY_DELETE;
        case KEY_ENTER: return EDITOR_SCAN_KEY_ENTER;
        case KEY_TAB: return EDITOR_SCAN_KEY_TAB;
        case KEY_SPACE: return EDITOR_SCAN_KEY_SPACE;

        // Navigation
        case KEY_LEFT: return EDITOR_SCAN_KEY_LEFT;
        case KEY_RIGHT: return EDITOR_SCAN_KEY_RIGHT;
        case KEY_UP: return EDITOR_SCAN_KEY_UP;
        case KEY_DOWN: return EDITOR_SCAN_KEY_DOWN;
        case KEY_HOME: return EDITOR_SCAN_KEY_HOME;
        case KEY_END: return EDITOR_SCAN_KEY_END;
        case KEY_PAGE_UP: return EDITOR_SCAN_KEY_PAGE_UP;
        case KEY_PAGE_DOWN: return EDITOR_SCAN_KEY_PAGE_DOWN;
        case KEY_INSERT: return EDITOR_SCAN_KEY_INSERT;

        default: return EDITOR_SCAN_KEY_UNKNOWN;
    }
}

i32 main(int argc, char **argv) {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(800, 450, "Auzio Editore");

    // TODO: correct "fullscreen"/screensize on init
    // ToggleBorderlessWindowed();

    // TODO: query device capas? -> VRR + configurable fps limit?
    SetTargetFPS(60);

#if EDITORE_DEBUG
    void *base_address = (void *)TiB((u64)2);
#else
    void *base_address = 0;
#endif

    EditorMemory memory = {};
    memory.permanent_memory_size = MiB(64);
    memory.scratch_memory_size = GiB(1);
    uint64_t total_size = memory.permanent_memory_size + memory.scratch_memory_size;
    memory.permanent_memory = mmap(
        base_address, total_size,
        PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS,
        -1, 0
    );
    if (memory.permanent_memory == MAP_FAILED) {
        //Diagnostic
        return 1;
    }
    memory.scratch_memory = (uint8_t *)memory.permanent_memory 
        + memory.permanent_memory_size;

    // TODO: some nice default font?/font of some system setting?
    // TODO: monospace and variable space fonts
    // TODO: font settings -> config file (what format??? .c-file?)? settings via cli functionality e.g. temporary?
    float current_font_size = 40;
    float current_font_spacing = .0f;
    Color current_font_color = WHITE;
    const char *jetbrains_mono_regular_path = "assets/fonts/jetbrains/JetBrainsMono-Regular.ttf";
    Font current_font = LoadFontEx(jetbrains_mono_regular_path, (int)current_font_size, 0, 0);

    float start_x_offset = 5;
    float start_y_offset = 5;

    float input_x = start_x_offset;
    float input_y = start_y_offset;

    b32 edit_field_hovered = true;
    b32 edit_field_active = false;

#define MAX_CHARS 4096
    // NOTE: ensure buffer bounds -> most likely alloc on heap later
    // look into how much to keep in buffer, caching, etc.
    int codepoints[MAX_CHARS];
    int codepoint_count = 0;

    while (!WindowShouldClose()) {
        // NOTE: actual text field size
        int edit_field_width = GetScreenWidth();
        int edit_field_height = GetScreenHeight();

        if (edit_field_hovered) {
            SetMouseCursor(MOUSE_CURSOR_IBEAM);
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                edit_field_active = true;
            }
        }

        // NOTE: use some custom string type e.g. with length if possible?
        // TODO: recognizing non-ascii chars -> e.g. äöüß...
        // TODO: holding down key
        // TODO: proper char distance measurement -> should support variable width fonts (MeasureTextEx())
        if (edit_field_active) {
            int scan_key_code = GetKeyPressed();
            while (scan_key_code != 0) {
                int pressed_char_code = GetCharPressed();
                if (pressed_char_code != 0) {
                    codepoints[codepoint_count++] = pressed_char_code;
                }

                switch (scan_key_code) {
                    // TODO: ctrl + backspace / ctrl + del semantics
                    case KEY_BACKSPACE: {
                        --codepoint_count;
                    } break;
                    case KEY_ENTER: {
                        char new_line = '\n';
                        codepoints[codepoint_count++] = new_line;
                    } break;
                }

                scan_key_code = GetKeyPressed();
            }
            ASSERT(codepoint_count <= MAX_CHARS);
        }

        BeginDrawing();
        {
            ClearBackground(BLACK);

            Vector2 textPos = {input_x, input_y};
            DrawTextCodepoints(
                current_font, codepoints, codepoint_count, textPos,
                current_font_size, current_font_spacing, current_font_color
            );

            // TODO: concept of lines + line numbering
            // check if line has content -> text[i][0] != 0 -> draw line number else not
        }
        EndDrawing();
    }

    return 0;
}
