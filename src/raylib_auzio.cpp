#include "auzio.h"
#include "raylib.h"
#include <unistd.h>

global_var b32 global_running;

i32 main(int argc, char **argv) {
    global_running = true;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(780, 540, "Auzio Editore");

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
