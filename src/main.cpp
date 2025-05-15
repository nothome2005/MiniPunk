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
    SetTargetFPS(60); // Ограничение FPS
    SearchAndSetResourceDir("resources");

    // --- grid setup ---
    constexpr int gridSize = 10;
    constexpr float marginLeft = 50.0f;
    constexpr float marginTop = 150.0f;
    constexpr float marginRight = 250.0f;
    constexpr float marginBottom = 150.0f;
    constexpr float cellSize = (1024.0f - marginLeft - marginRight) / gridSize;
    Grid grid(gridSize, gridSize, marginLeft, marginTop, cellSize);

    LoadResources();

    // --- map generation ---
    auto mapCells = MapGenerator::Generate(gridSize, gridSize, 3, 3, 3, 10);
    grid.SetMapCells(mapCells);

    // --- generator ---
    Generator generator(cellSize, marginLeft, marginTop);
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