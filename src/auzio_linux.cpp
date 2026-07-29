#include "auzio_platform.h"
#include "auzio_utils.h"
#include "raylib.h"
#include "auzio_editore.cpp"
#include <sys/mman.h>
#include <stdio.h>

inline V2 ray_toAeVec2(Vector2 raylib_vec2) {
    V2 result = {};
    result.x = raylib_vec2.x;
    result.y = raylib_vec2.y;
    return result;
}

inline Vector2 ray_toRaylibVector2(V2 v2) {
    Vector2 result = {};
    result.x = v2.x;
    result.y = v2.y;
    return result;
}

inline Rectangle ray_toRaylibRect(AP_Rect rect) {
    Rectangle result = {};
    result.height = rect.bottom_right.y - rect.top_left.y;
    result.width = rect.bottom_right.x - rect.top_left.x;
    result.x = rect.top_left.x;
    result.y = rect.top_left.y;
    return result;
}


inline Color ray_toRaylibColor(AP_Color color) {
    Color result = {};
    result.r = color.r;
    result.g = color.g;
    result.b = color.b;
    result.a = color.a;
    return result;
}

struct Ray_ButtonMapping {
    int raylib_btn_code;
    AP_ButtonId editor_button_id;
};

global_var Ray_ButtonMapping raylib_to_editor_key_map[] = {
    // Letters
    { KEY_A, AP_BUTTON_ID_KEY_A },
    { KEY_B, AP_BUTTON_ID_KEY_B },
    { KEY_C, AP_BUTTON_ID_KEY_C },
    { KEY_D, AP_BUTTON_ID_KEY_D },
    { KEY_E, AP_BUTTON_ID_KEY_E },
    { KEY_F, AP_BUTTON_ID_KEY_F },
    { KEY_G, AP_BUTTON_ID_KEY_G },
    { KEY_H, AP_BUTTON_ID_KEY_H },
    { KEY_I, AP_BUTTON_ID_KEY_I },
    { KEY_J, AP_BUTTON_ID_KEY_J },
    { KEY_K, AP_BUTTON_ID_KEY_K },
    { KEY_L, AP_BUTTON_ID_KEY_L },
    { KEY_M, AP_BUTTON_ID_KEY_M },
    { KEY_N, AP_BUTTON_ID_KEY_N },
    { KEY_O, AP_BUTTON_ID_KEY_O },
    { KEY_P, AP_BUTTON_ID_KEY_P },
    { KEY_Q, AP_BUTTON_ID_KEY_Q },
    { KEY_R, AP_BUTTON_ID_KEY_R },
    { KEY_S, AP_BUTTON_ID_KEY_S },
    { KEY_T, AP_BUTTON_ID_KEY_T },
    { KEY_U, AP_BUTTON_ID_KEY_U },
    { KEY_V, AP_BUTTON_ID_KEY_V },
    { KEY_W, AP_BUTTON_ID_KEY_W },
    { KEY_X, AP_BUTTON_ID_KEY_X },
    { KEY_Y, AP_BUTTON_ID_KEY_Y },
    { KEY_Z, AP_BUTTON_ID_KEY_Z },

    // Numbers
    { KEY_ZERO, AP_BUTTON_ID_KEY_0 },
    { KEY_ONE, AP_BUTTON_ID_KEY_1 },
    { KEY_TWO, AP_BUTTON_ID_KEY_2 },
    { KEY_THREE, AP_BUTTON_ID_KEY_3 },
    { KEY_FOUR, AP_BUTTON_ID_KEY_4 },
    { KEY_FIVE, AP_BUTTON_ID_KEY_5 },
    { KEY_SIX, AP_BUTTON_ID_KEY_6 },
    { KEY_SEVEN, AP_BUTTON_ID_KEY_7 },
    { KEY_EIGHT, AP_BUTTON_ID_KEY_8 },
    { KEY_NINE, AP_BUTTON_ID_KEY_9 },

    // Function keys
    { KEY_F1, AP_BUTTON_ID_KEY_F1 },
    { KEY_F2, AP_BUTTON_ID_KEY_F2 },
    { KEY_F3, AP_BUTTON_ID_KEY_F3 },
    { KEY_F4, AP_BUTTON_ID_KEY_F4 },
    { KEY_F5, AP_BUTTON_ID_KEY_F5 },
    { KEY_F6, AP_BUTTON_ID_KEY_F6 },
    { KEY_F7, AP_BUTTON_ID_KEY_F7 },
    { KEY_F8, AP_BUTTON_ID_KEY_F8 },
    { KEY_F9, AP_BUTTON_ID_KEY_F9 },
    { KEY_F10, AP_BUTTON_ID_KEY_F10 },
    { KEY_F11, AP_BUTTON_ID_KEY_F11 },
    { KEY_F12, AP_BUTTON_ID_KEY_F12 },

    // Modifiers
    { KEY_LEFT_SHIFT, AP_BUTTON_ID_KEY_LEFT_SHIFT },
    { KEY_RIGHT_SHIFT, AP_BUTTON_ID_KEY_RIGHT_SHIFT },
    { KEY_LEFT_CONTROL, AP_BUTTON_ID_KEY_LEFT_CONTROL },
    { KEY_RIGHT_CONTROL, AP_BUTTON_ID_KEY_RIGHT_CONTROL },
    { KEY_LEFT_ALT, AP_BUTTON_ID_KEY_LEFT_ALT },
    { KEY_RIGHT_ALT, AP_BUTTON_ID_KEY_RIGHT_ALT },
    { KEY_CAPS_LOCK, AP_BUTTON_ID_KEY_CAPS_LOCK },

    // Common
    { KEY_ESCAPE, AP_BUTTON_ID_KEY_ESCAPE },
    { KEY_BACKSPACE, AP_BUTTON_ID_KEY_BACKSPACE },
    { KEY_DELETE, AP_BUTTON_ID_KEY_DELETE },
    { KEY_ENTER, AP_BUTTON_ID_KEY_ENTER },
    { KEY_TAB, AP_BUTTON_ID_KEY_TAB },
    { KEY_SPACE, AP_BUTTON_ID_KEY_SPACE },

    // Navigation
    { KEY_LEFT, AP_BUTTON_ID_KEY_LEFT },
    { KEY_RIGHT, AP_BUTTON_ID_KEY_RIGHT },
    { KEY_UP, AP_BUTTON_ID_KEY_UP },
    { KEY_DOWN, AP_BUTTON_ID_KEY_DOWN },
    { KEY_HOME, AP_BUTTON_ID_KEY_HOME },
    { KEY_END, AP_BUTTON_ID_KEY_END },
    { KEY_PAGE_UP, AP_BUTTON_ID_KEY_PAGE_UP },
    { KEY_PAGE_DOWN, AP_BUTTON_ID_KEY_PAGE_DOWN },
    { KEY_INSERT, AP_BUTTON_ID_KEY_INSERT },

};

global_var Ray_ButtonMapping raylib_to_editor_mouse_btn_map[] = {
    // Mouse
    { MOUSE_LEFT_BUTTON, AP_BUTTON_ID_MOUSE_LEFT},
    { MOUSE_MIDDLE_BUTTON, AP_BUTTON_ID_MOUSE_MIDDLE},
    { MOUSE_RIGHT_BUTTON, AP_BUTTON_ID_MOUSE_RIGHT},
};

internal void ray_addInputEvent(
    AP_Input *input,
    AP_InputEventType event_type,
    int event_value
) {
    AP_InputEvent *input_event = &input->events[input->event_count++];
    input_event->type = event_type;
    input_event->codepoint = event_value;
}

i32 main(int argc, char **argv) {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(800, 450, "Auzio Editore");

    // TODO: correct "fullscreen"/screensize on init
    // ToggleBorderlessWindowed();
    // TODO: blocking or nonblocking? -> automatic polling or nah
    // DisableEventWaiting();

    // TODO: query device capas? -> VRR + configurable fps limit?
    SetTargetFPS(60);

#if EDITORE_DEBUG
    void *base_address = (void *)TiB((u64)2);
#else
    void *base_address = 0;
#endif

    // TODO: platform independent memory reservation -> depending on if we keep raylib
    AP_Memory memory = {};
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
    const char *jetbrains_mono_regular_path = "assets/fonts/jetbrains/JetBrainsMono-Regular.ttf";

    // TODO: support for emojis + arabics etc.
    // TODO: ligatures
#define UNICODE_FIRST_PLANE_MAX 0xffff
    int cps[UNICODE_FIRST_PLANE_MAX];
    for (int cp_idx = 0; cp_idx < UNICODE_FIRST_PLANE_MAX; ++cp_idx) {
        cps[cp_idx] = cp_idx;
    }

    // TODO: do i need to reload this when changing font_size? -> font changing logic (settings)
    Font current_font = LoadFontEx(
        jetbrains_mono_regular_path, (int)current_font_size,
        cps, UNICODE_FIRST_PLANE_MAX
    );

    float start_x_offset = 5;
    float start_y_offset = 5;

    float input_x = start_x_offset;
    float input_y = start_y_offset;

    AP_RenderBuffer render_buffer = {};

    while (!WindowShouldClose()) {
        AP_Rect screen_rect = createRect((f32)GetScreenWidth(), (f32)GetScreenHeight(), .0f, .0f);

        // NOTE: use some custom string type e.g. with length if possible?
        // TODO: recognizing non-ascii chars -> e.g. äöüß...
        // TODO: holding down key
        // TODO: proper char distance measurement -> should support variable width fonts (MeasureTextEx())

        // TODO: add touch device and stylus support? -> freeline drawing????
        AP_Input input = {};
        input.event_count = 0;

        Vector2 mouse_pos = GetMousePosition();
        Vector2 mouse_delta = GetMouseDelta();
        Vector2 mouse_wheel = GetMouseWheelMoveV();

        input.mouse_pos = ray_toAeVec2(mouse_pos);
        input.mouse_move_delta = ray_toAeVec2(mouse_delta);
        input.mouse_wheel_delta = ray_toAeVec2(mouse_wheel);

        printf("mouse pos: x %f y %f\n", input.mouse_pos.x, input.mouse_pos.y);
        printf("mouse delta: x %f y %f\n", input.mouse_move_delta.x, input.mouse_move_delta.y);
        printf("mouse wheel: x %f y %f\n", input.mouse_wheel_delta.x, input.mouse_wheel_delta.y);


        int pressed_char_code = GetCharPressed();
        while (pressed_char_code != 0) {
            ray_addInputEvent(&input, AP_INPUT_EVENT_CODEPOINT, pressed_char_code);
            pressed_char_code = GetCharPressed();
        }

        for (
            int key_map_idx = 0;
            key_map_idx < (int)ARRAY_COUNT(raylib_to_editor_key_map);
            ++key_map_idx
        ) {
            Ray_ButtonMapping *key_mapping = &raylib_to_editor_key_map[key_map_idx];
            if (IsKeyPressed(key_mapping->raylib_btn_code)) {
                ray_addInputEvent(&input, AP_INPUT_EVENT_BUTTON_DOWN, key_mapping->editor_button_id);
            }

            if (IsKeyReleased(key_mapping->raylib_btn_code)) {
                ray_addInputEvent(&input, AP_INPUT_EVENT_BUTTON_UP, key_mapping->editor_button_id);
            }
        }

        for (
            int mouse_map_idx = 0;
            mouse_map_idx < (int)ARRAY_COUNT(raylib_to_editor_mouse_btn_map);
            ++mouse_map_idx
        ) {
            Ray_ButtonMapping *mouse_btn_mapping = &raylib_to_editor_mouse_btn_map[mouse_map_idx];
            if (IsMouseButtonPressed(mouse_btn_mapping->raylib_btn_code)) {
                ray_addInputEvent(&input, AP_INPUT_EVENT_BUTTON_DOWN, mouse_btn_mapping->editor_button_id);
            }

            if (IsMouseButtonReleased(mouse_btn_mapping->raylib_btn_code)) {
                ray_addInputEvent(&input, AP_INPUT_EVENT_BUTTON_UP, mouse_btn_mapping->editor_button_id);
            }
        }

        ap_editorUpdateAndRender(&memory, &input, &render_buffer, screen_rect);
        // TODO: instead of having to get editor state here -> proper push rendering

        BeginDrawing();
        {
            for (
                int renderable_idx = 0;
                renderable_idx < render_buffer.renderable_count;
                ++renderable_idx
            ) {
                AP_Renderable *renderable = &render_buffer.renderables[renderable_idx];
                switch (renderable->type) {
                    case AP_RENDERABLE_TYPE_CLEAR: {
                        ClearBackground(ray_toRaylibColor(renderable->bg_color));
                    } break;
                    case AP_RENDERABLE_TYPE_CODEPOINTS: {
                        DrawTextCodepoints(
                            current_font, // TODO: implement font_handle + changing of fonts
                            renderable->codepoints, renderable->codepoint_count,
                            ray_toRaylibVector2(renderable->pos),
                            renderable->font_size, renderable->spacing,
                            ray_toRaylibColor(renderable->font_color)
                        );
                    } break;
                    case AP_RENDERABLE_TYPE_RECTANGLE: {
                        DrawRectangleRec(
                            ray_toRaylibRect(renderable->rect),
                            ray_toRaylibColor(renderable->rect_color)
                        );
                    } break;

                    INVALID_DEFAULT_CASE;
                }
            }

            render_buffer.renderable_count = 0;

            // TODO: concept of lines + line numbering
            // check if line has content -> text[i][0] != 0 -> draw line number else not
        }
        EndDrawing();
    }

    return 0;
}
