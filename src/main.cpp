#include "raylib.h"
#include "resource_dir.h"
#include "resource_manager.h"
#include "grid.h"
#include "map_generator.h"
#include "generator.h"
#include "object_panel.h"


float objectCanvaX = 1200.0f; 
const float objectCanvaTargetX = 800.0f;
const float objectCanvaHiddenX = 1200.0f; 
const float objectCanvaY = 200.0f;
const float objectCanvaSpeed = 1600.0f;

extern Texture2D ObjectCanva_;
extern Texture2D cursor_;

int main ()
{
    SetConfigFlags(FLAG_WINDOW_HIGHDPI);

    InitWindow(1200, 1024, "MiniPunk");
    SetTargetFPS(60);
    SearchAndSetResourceDir("resources");

    HideCursor();

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

    
    Generator generator(cellSize, marginLeft, marginTop);
    grid.SetGenerator(&generator);

    while (!WindowShouldClose())
    {
        grid.Update();

        
        bool anySelected = false;
        
        for (const auto& cell : grid.GetMapCells()) {
            if (cell.IsSelected()) { anySelected = true; break; }
        }
        
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

      
        DrawObjectPanel(objectCanvaX, objectCanvaY, grid);


        Vector2 mouse = GetMousePosition();
        DrawTexture(cursor_, (int)mouse.x-25, (int)mouse.y-20, WHITE);

        EndDrawing();
    }

    UnloadResources();
    CloseWindow();
    return 0;
}