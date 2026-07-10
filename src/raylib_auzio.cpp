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

    Font default_font = GetFontDefault();

    // height based on font-size
    float font_size = 20;
    float start_x_offset = 10;
    float start_y_offset = 10;
    Rectangle cursor_rect = {start_x_offset, start_x_offset, 1, font_size};

    int input_x = (int)start_x_offset;
    int input_y = (int)start_y_offset;

    b32 left_mouse_was_down = false;

#define MAX_CHARS 128
    // NOTE: ensure buffer bounds -> most likely alloc on heap later
    char text[MAX_CHARS + 1];
    int textIdx = 0;

    while (!WindowShouldClose()) {
        SetMouseCursor(MOUSE_CURSOR_IBEAM);

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            left_mouse_was_down = true;
        }

        // NOTE: use some custom string type e.g. with length if possible?
        // TODO: recognizing non-ascii chars -> e.g. äöüß...
        // TODO: holding down key
        int pressed_char = GetCharPressed();
        while (pressed_char != 0) {
            text[textIdx++] = (char)pressed_char;
            pressed_char = GetCharPressed();
        }
        ASSERT(textIdx < MAX_CHARS);
        text[textIdx] = 0;

        if (IsKeyPressed(KEY_BACKSPACE)) {
            --textIdx;
            text[textIdx] = 0;
        }

        BeginDrawing();
        {
            ClearBackground(BLACK);

            if (left_mouse_was_down) {
                // TODO: blinking cursor?
                DrawRectangleRec(cursor_rect, WHITE);
            }

            DrawText(text, input_x, input_y, 40, LIGHTGRAY);
        }
        EndDrawing();
    }

    return 0;
}
