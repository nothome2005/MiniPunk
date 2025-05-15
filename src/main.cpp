#include "raylib.h"
#include "resource_dir.h"
#include "resource_manager.h"
#include "grid.h"
#include "map_generator.h"
#include "generator.h"

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

    // --- map generation ---
    auto mapCells = MapGenerator::Generate(gridSize, gridSize, 3, 3, 3, 10);
    grid.SetMapCells(mapCells);

    // --- generator ---
    Generator generator(cellSize, margin);
    grid.SetGenerator(&generator);

    while (!WindowShouldClose())
    {
        grid.Update();

        BeginDrawing();
        ClearBackground(BLACK);

        DrawTexture(g_background, 0, 0, WHITE);

        grid.Draw();

        EndDrawing();
    }

    UnloadResources();
    CloseWindow();
    return 0;
}