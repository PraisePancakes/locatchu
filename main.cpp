#include <raylib.h>

#include <forge/forge.hpp>
#include <iostream>

int main(int argc, char** argv) {
    InitWindow(500, 500, "locatchu");
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(MAGENTA);
        EndDrawing();
    }

    CloseWindow();
    return 0;
};