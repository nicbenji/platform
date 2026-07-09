#include "auzio.h"
#include "raylib.h"
#include <unistd.h>

i32 main(int argc, char **argv) {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(800, 450, "Auzio Editore");

    // TODO: query device capas?
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
