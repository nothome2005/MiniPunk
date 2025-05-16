#include "raylib.h"
#include "resource_dir.h"
#include "resource_manager.h"
#include "grid.h"
#include "map_generator.h"
#include "generator.h"
#include "object_panel.h"

// --- ObjectCanva_ animation state ---
float objectCanvaX = 1200.0f; // Start hidden (off-screen)
const float objectCanvaTargetX = 800.0f; // Where to stop when shown
const float objectCanvaHiddenX = 1200.0f; // Where to hide (off-screen)
const float objectCanvaY = 200.0f; // Fixed Y position
const float objectCanvaSpeed = 1600.0f; // px/sec

extern Texture2D ObjectCanva_;
extern Texture2D cursor_;

int main ()
{
    SetConfigFlags(FLAG_WINDOW_HIGHDPI);

    InitWindow(1200, 1024, "MiniPunk");
    SetTargetFPS(60); // Ограничение FPS
    SearchAndSetResourceDir("resources");

    HideCursor(); // Скрыть системный курсор

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

        // --- ObjectCanva_ animation logic ---
        bool anySelected = false;
        // Используем публичный геттер для mapCells
        for (const auto& cell : grid.GetMapCells()) {
            if (cell.IsSelected()) { anySelected = true; break; }
        }
        // Используем публичный геттер для generator
        if (grid.GetGenerator() && grid.GetGenerator()->IsSelected()) anySelected = true;

        float targetX = anySelected ? objectCanvaTargetX : objectCanvaHiddenX;
        if (objectCanvaX < targetX) {
            objectCanvaX += objectCanvaSpeed * GetFrameTime();
            if (objectCanvaX > targetX) objectCanvaX = targetX;
        } else if (objectCanvaX > targetX) {
            objectCanvaX -= objectCanvaSpeed * GetFrameTime();
            if (objectCanvaX < targetX) objectCanvaX = targetX;
        }

        BeginDrawing();
        ClearBackground(BLACK);

        DrawTexture(g_background, 0, 0, WHITE);

        grid.Draw();

        // --- Draw Object Panel ---
        DrawObjectPanel(objectCanvaX, objectCanvaY, grid);

        // --- Draw custom cursor ---
        Vector2 mouse = GetMousePosition();
        // Смещаем изображение так, чтобы остриё курсора совпадало с позицией мыши (левый верхний угол)
        DrawTexture(cursor_, (int)mouse.x-25, (int)mouse.y-20, WHITE);

        EndDrawing();
    }

    UnloadResources();
    CloseWindow();
    return 0;
}