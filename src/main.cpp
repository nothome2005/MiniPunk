#include "raylib.h"
#include "resource_dir.h"
#include "resource_manager.h"
#include "grid.h"

int main ()
{
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

    InitWindow(1024, 1024, "MiniPunk");
    SearchAndSetResourceDir("resources");

    // --- grid setup ---
    constexpr int gridSize = 10;
    constexpr float margin = 150.0f;
    constexpr float cellSize = (1024.0f - 2 * margin) / gridSize;
    Grid grid(gridSize, gridSize, margin, cellSize);

    LoadResources();
    while (!WindowShouldClose())
    {
        grid.Update();

        BeginDrawing();
        ClearBackground(BLACK);

        DrawTexture(g_background, 0, 0, WHITE);

        grid.Draw();

        EndDrawing();
    }

    // cleanup
    UnloadResources();

    // destroy the window and cleanup the OpenGL context
    CloseWindow();
    return 0;
}