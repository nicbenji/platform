#include "auzio_platform.h"
#include "auzio_utils.h"
#include "raylib.h"
#include "auzio.cpp"
#include <sys/mman.h>
#include <stdio.h>

struct KeyMapping {
    int raylib_key_code;
    EditorButtonId editor_button_id;
};

global_var KeyMapping raylib_to_editor_key_map[] = {
    // Letters
    { KEY_A, EDITOR_BUTTON_ID_KEY_A },
    { KEY_B, EDITOR_BUTTON_ID_KEY_B },
    { KEY_C, EDITOR_BUTTON_ID_KEY_C },
    { KEY_D, EDITOR_BUTTON_ID_KEY_D },
    { KEY_E, EDITOR_BUTTON_ID_KEY_E },
    { KEY_F, EDITOR_BUTTON_ID_KEY_F },
    { KEY_G, EDITOR_BUTTON_ID_KEY_G },
    { KEY_H, EDITOR_BUTTON_ID_KEY_H },
    { KEY_I, EDITOR_BUTTON_ID_KEY_I },
    { KEY_J, EDITOR_BUTTON_ID_KEY_J },
    { KEY_K, EDITOR_BUTTON_ID_KEY_K },
    { KEY_L, EDITOR_BUTTON_ID_KEY_L },
    { KEY_M, EDITOR_BUTTON_ID_KEY_M },
    { KEY_N, EDITOR_BUTTON_ID_KEY_N },
    { KEY_O, EDITOR_BUTTON_ID_KEY_O },
    { KEY_P, EDITOR_BUTTON_ID_KEY_P },
    { KEY_Q, EDITOR_BUTTON_ID_KEY_Q },
    { KEY_R, EDITOR_BUTTON_ID_KEY_R },
    { KEY_S, EDITOR_BUTTON_ID_KEY_S },
    { KEY_T, EDITOR_BUTTON_ID_KEY_T },
    { KEY_U, EDITOR_BUTTON_ID_KEY_U },
    { KEY_V, EDITOR_BUTTON_ID_KEY_V },
    { KEY_W, EDITOR_BUTTON_ID_KEY_W },
    { KEY_X, EDITOR_BUTTON_ID_KEY_X },
    { KEY_Y, EDITOR_BUTTON_ID_KEY_Y },
    { KEY_Z, EDITOR_BUTTON_ID_KEY_Z },

    // Numbers
    { KEY_ZERO, EDITOR_BUTTON_ID_KEY_0 },
    { KEY_ONE, EDITOR_BUTTON_ID_KEY_1 },
    { KEY_TWO, EDITOR_BUTTON_ID_KEY_2 },
    { KEY_THREE, EDITOR_BUTTON_ID_KEY_3 },
    { KEY_FOUR, EDITOR_BUTTON_ID_KEY_4 },
    { KEY_FIVE, EDITOR_BUTTON_ID_KEY_5 },
    { KEY_SIX, EDITOR_BUTTON_ID_KEY_6 },
    { KEY_SEVEN, EDITOR_BUTTON_ID_KEY_7 },
    { KEY_EIGHT, EDITOR_BUTTON_ID_KEY_8 },
    { KEY_NINE, EDITOR_BUTTON_ID_KEY_9 },

    // Function keys
    { KEY_F1, EDITOR_BUTTON_ID_KEY_F1 },
    { KEY_F2, EDITOR_BUTTON_ID_KEY_F2 },
    { KEY_F3, EDITOR_BUTTON_ID_KEY_F3 },
    { KEY_F4, EDITOR_BUTTON_ID_KEY_F4 },
    { KEY_F5, EDITOR_BUTTON_ID_KEY_F5 },
    { KEY_F6, EDITOR_BUTTON_ID_KEY_F6 },
    { KEY_F7, EDITOR_BUTTON_ID_KEY_F7 },
    { KEY_F8, EDITOR_BUTTON_ID_KEY_F8 },
    { KEY_F9, EDITOR_BUTTON_ID_KEY_F9 },
    { KEY_F10, EDITOR_BUTTON_ID_KEY_F10 },
    { KEY_F11, EDITOR_BUTTON_ID_KEY_F11 },
    { KEY_F12, EDITOR_BUTTON_ID_KEY_F12 },

    // Modifiers
    { KEY_LEFT_SHIFT, EDITOR_BUTTON_ID_KEY_LEFT_SHIFT },
    { KEY_RIGHT_SHIFT, EDITOR_BUTTON_ID_KEY_RIGHT_SHIFT },
    { KEY_LEFT_CONTROL, EDITOR_BUTTON_ID_KEY_LEFT_CONTROL },
    { KEY_RIGHT_CONTROL, EDITOR_BUTTON_ID_KEY_RIGHT_CONTROL },
    { KEY_LEFT_ALT, EDITOR_BUTTON_ID_KEY_LEFT_ALT },
    { KEY_RIGHT_ALT, EDITOR_BUTTON_ID_KEY_RIGHT_ALT },
    { KEY_CAPS_LOCK, EDITOR_BUTTON_ID_KEY_CAPS_LOCK },

    // Common
    { KEY_ESCAPE, EDITOR_BUTTON_ID_KEY_ESCAPE },
    { KEY_BACKSPACE, EDITOR_BUTTON_ID_KEY_BACKSPACE },
    { KEY_DELETE, EDITOR_BUTTON_ID_KEY_DELETE },
    { KEY_ENTER, EDITOR_BUTTON_ID_KEY_ENTER },
    { KEY_TAB, EDITOR_BUTTON_ID_KEY_TAB },
    { KEY_SPACE, EDITOR_BUTTON_ID_KEY_SPACE },

    // Navigation
    { KEY_LEFT, EDITOR_BUTTON_ID_KEY_LEFT },
    { KEY_RIGHT, EDITOR_BUTTON_ID_KEY_RIGHT },
    { KEY_UP, EDITOR_BUTTON_ID_KEY_UP },
    { KEY_DOWN, EDITOR_BUTTON_ID_KEY_DOWN },
    { KEY_HOME, EDITOR_BUTTON_ID_KEY_HOME },
    { KEY_END, EDITOR_BUTTON_ID_KEY_END },
    { KEY_PAGE_UP, EDITOR_BUTTON_ID_KEY_PAGE_UP },
    { KEY_PAGE_DOWN, EDITOR_BUTTON_ID_KEY_PAGE_DOWN },
    { KEY_INSERT, EDITOR_BUTTON_ID_KEY_INSERT },
};

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

    // TODO: platform independent memory reservation -> depending on if we keep raylib
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

    // TODO: support for emojis + arabics etc.
    // TODO: ligatures
#define UNICODE_FIRST_PLANE_MAX 0xffff
    int cps[UNICODE_FIRST_PLANE_MAX];
    for (int cp_idx = 0; cp_idx < UNICODE_FIRST_PLANE_MAX; ++cp_idx) {
        cps[cp_idx] = cp_idx;
    }

    Font current_font = LoadFontEx(
        jetbrains_mono_regular_path, (int)current_font_size,
        cps, UNICODE_FIRST_PLANE_MAX
    );

    float start_x_offset = 5;
    float start_y_offset = 5;

    float input_x = start_x_offset;
    float input_y = start_y_offset;

    while (!WindowShouldClose()) {
        // NOTE: actual text field size
        int edit_field_width = GetScreenWidth();
        int edit_field_height = GetScreenHeight();

        Vector2 mouse_pos = GetMousePosition();
        Vector2 mouse_wheel = GetMouseWheelMoveV();

        // NOTE: use some custom string type e.g. with length if possible?
        // TODO: recognizing non-ascii chars -> e.g. äöüß...
        // TODO: holding down key
        // TODO: proper char distance measurement -> should support variable width fonts (MeasureTextEx())

        EditorInput input = {};
        input.event_count = 0;

        int pressed_char_code = GetCharPressed();
        while (pressed_char_code != 0) {
            EditorInputEvent *input_event = &input.events[input.event_count++];
            input_event->type = EDITOR_INPUT_EVENT_CODEPOINT;
            input_event->codepoint = pressed_char_code;

            pressed_char_code = GetCharPressed();
        }

        for (
            int key_map_idx = 0;
            key_map_idx < (int)ARRAY_COUNT(raylib_to_editor_key_map);
            ++key_map_idx
        ) {
            KeyMapping *key_mapping = &raylib_to_editor_key_map[key_map_idx];
            if (IsKeyPressed(key_mapping->raylib_key_code)) {
                EditorInputEvent *input_event = &input.events[input.event_count++];
                input_event->type = EDITOR_INPUT_EVENT_BUTTON_DOWN;
                input_event->button_id = key_mapping->editor_button_id;
            }

            if (IsKeyReleased(key_mapping->raylib_key_code)) {
                EditorInputEvent *input_event = &input.events[input.event_count++];
                input_event->type = EDITOR_INPUT_EVENT_BUTTON_UP;
                input_event->button_id = key_mapping->editor_button_id;
            }
        }

        editorUpdate(&memory, &input);
        // TODO: instead of having to get editor state here -> proper push rendering
        EditorState *editor_state = (EditorState *)memory.permanent_memory;

        BeginDrawing();
        {
            ClearBackground(BLACK);

            Vector2 text_pos = {input_x, input_y};
            DrawTextCodepoints(
                current_font, editor_state->codepoints, editor_state->codepoint_count, text_pos,
                current_font_size, current_font_spacing, current_font_color
            );

            // TODO: concept of lines + line numbering
            // check if line has content -> text[i][0] != 0 -> draw line number else not
        }
        EndDrawing();
    }

    return 0;
}
