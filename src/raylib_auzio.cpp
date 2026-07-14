#include "auzio.h"
#include "raylib.h"
#include <stdio.h>
#include <unistd.h>

i32 main(int argc, char **argv) {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(800, 450, "Auzio Editore");

    // TODO: correct "fullscreen"/screensize on init
    // ToggleBorderlessWindowed();

    // TODO: query device capas? -> VRR + configurable fps limit?
    SetTargetFPS(60);

    // TODO: some nice default font?/font of some system setting?
    // TODO: monospace and variable space fonts
    // TODO: font settings -> config file (what .fmt???)? settings via cli functionality e.g. temporary?
    float font_size = 40;
    float spacing = 0.0f;
    const char *jetbrains_mono_regular_path = "assets/fonts/jetbrains/JetBrainsMono-Regular.ttf";

    Font current_font = LoadFontEx(jetbrains_mono_regular_path, (int)font_size, 0, 0);

    // height based on font-size
    float cursor_size = font_size + 5;
    float start_x_offset = 5;
    float start_y_offset = 5;
    Rectangle cursor_rect = {start_x_offset, start_x_offset, 1, cursor_size};

    float input_x = start_x_offset;
    float input_y = start_y_offset;

    b32 edit_field_active = false;

#define MAX_CHARS 128
    // NOTE: ensure buffer bounds -> most likely alloc on heap later
    // look into how much to keep in buffer, caching, etc.
    char text[MAX_CHARS + 1] = {};
    int textIdx = 0;

    while (!WindowShouldClose()) {
        // NOTE: actual text field size
        int edit_field_width = GetScreenWidth();
        int edit_field_height = GetScreenHeight();

        SetMouseCursor(MOUSE_CURSOR_IBEAM);

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            edit_field_active = true;
        }

        // NOTE: use some custom string type e.g. with length if possible?
        // TODO: recognizing non-ascii chars -> e.g. äöüß...
        // TODO: holding down key
        // TODO: proper char distance measurement -> should support variable width fonts (MeasureTextEx())
        if (edit_field_active) {
            int pressed_char_code = GetCharPressed();
            while (pressed_char_code != 0) {
                char pressed_char = (char)pressed_char_code;
                text[textIdx++] = pressed_char;
                Vector2 char_dims = MeasureTextEx(current_font, &pressed_char, font_size, spacing);
                cursor_rect.x += char_dims.x;

                pressed_char_code = GetCharPressed();
            }
            ASSERT(textIdx < MAX_CHARS);
            text[textIdx] = 0;

            int key_code = GetKeyPressed();
            while (key_code != 0) {
                switch (key_code) {
                    // TODO: ctrl + backspace / ctrl + del semantics
                    case KEY_BACKSPACE: {
                        --textIdx;
                        char *char_to_remove = &text[textIdx];
                        Vector2 char_dims = MeasureTextEx(current_font, char_to_remove, font_size, spacing);

                        if (*char_to_remove == '\n') {
                            char prev_line[MAX_CHARS + 1] = {};
                            // FIXME: this logic seems to be a complete gibberish way of doing this
                            for (
                                int i = 0, charIdx = textIdx - 1;
                                text[charIdx] != '\n' && i < (int)ARRAY_COUNT(prev_line);
                                --charIdx, ++i
                            ) {
                                prev_line[i] = text[charIdx]; // this string is reversed, it is just used to get the line length
                            }
                            Vector2 prev_line_dims = MeasureTextEx(current_font, prev_line, font_size, spacing);
                            float end_of_prev_line = prev_line_dims.x + start_x_offset;
                            cursor_rect.x = end_of_prev_line;
                            cursor_rect.y -= 0.5f * char_dims.y;
                        } else {
                            cursor_rect.x -= char_dims.x;
                        }

                        char_to_remove = 0;
                    } break;
                    case KEY_ENTER: {
                        char new_line = '\n';
                        text[textIdx++] = new_line;
                        Vector2 char_dims = MeasureTextEx(current_font, &new_line, font_size, spacing);

                        cursor_rect.x = start_x_offset;
                        cursor_rect.y += .5f * char_dims.y;

                        text[textIdx] = 0;
                    } break;
                }

                key_code = GetKeyPressed();
            }

        }

        BeginDrawing();
        {
            ClearBackground(BLACK);

            if (edit_field_active) {
                // NOTE: blinking/animated cursor?
                DrawRectangleRec(cursor_rect, WHITE);
            }

            Vector2 textPos = {input_x, input_y};
            DrawTextEx(current_font, text, textPos, font_size, spacing, WHITE);

            // TODO: concept of lines + line numbering
            // check if line has content -> text[i][0] != 0 -> draw line number else not
        }
        EndDrawing();
    }

    return 0;
}
