#include <random>
#include <memory>
#include "raylib.h"
#include "resource_dir.h"
#include "resource_manager.h"
#include "grid.h"
#include "map_generator.h"
#include "generator.h"
#include "object_panel.h"
#include "player.h"
#include "wolf.h"
#include "game_state.h"

float objectCanvaX = 1200.0f; 
const float objectCanvaTargetX = 800.0f;
const float objectCanvaHiddenX = 1200.0f; 
const float objectCanvaY = 200.0f;
const float objectCanvaSpeed = 1600.0f;

extern Texture2D ObjectCanva_;
extern Texture2D cursor_;

// √лобальна€ переменна€, управл€юща€ возможностью выбора объектов
bool g_objectsSelectable = false;

int main ()
{
    SetConfigFlags(FLAG_WINDOW_HIGHDPI);

    if (g_objectsSelectable) {
        InitWindow(1200, 1024, "MiniPunk-Selectable");
    }
    else
    {
        InitWindow(1024, 1024, "MiniPunk-NonSelect");
    }
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

    // --- создание игрока в случайной свободной клетке ---
    std::vector<Vector2> freeCells;
    const auto& cells = grid.GetMapCells();
    for (int y = 0; y < gridSize; ++y) {
        for (int x = 0; x < gridSize; ++x) {
            int idx = y * gridSize + x;
            if (cells[idx].GetType() == ResourceType::None) {
                freeCells.push_back({(float)x, (float)y});
            }
        }
    }
    std::random_device rd;
    std::mt19937 g(rd());
    std::uniform_int_distribution<> dist(0, (int)freeCells.size() - 1);
    Vector2 spawn = freeCells[dist(g)];
    Player player((int)spawn.x, (int)spawn.y);

    // --- волк спавнитс€ в случайном углу ---
    std::vector<Vector2> wolfCorners = {
        {0, 0}, {0, gridSize-1}, {gridSize-1, 0}, {gridSize-1, gridSize-1}
    };
    std::uniform_int_distribution<> wolfDist(0, 3);
    Vector2 wolfSpawn = wolfCorners[wolfDist(g)];
    WolfMoveStrategy wolfStrategy;
    Wolf wolf((int)wolfSpawn.x, (int)wolfSpawn.y, &wolfStrategy);

    // --- State pattern ---
    GameState* currentState = nullptr;
    PlayerTurnState playerState(player, wolf, grid, currentState);
    WolfTurnState wolfState(player, wolf, grid, currentState);
    playerState.SetWolfState(&wolfState);
    wolfState.SetPlayerState(&playerState);
    currentState = &playerState;

    while (!WindowShouldClose())
    {
        float delta = GetFrameTime();
        if (!g_objectsSelectable) {
            grid.DeselectAll();
        }
        grid.Update();

        currentState->Update(delta);

        float targetX = objectCanvaTargetX;
        float anySelected = false;
        if (g_objectsSelectable) {
            for (const auto& cell : grid.GetMapCells()) {
                if (cell.IsSelected()) { anySelected = true; break; }
            }
            if (grid.GetGenerator() && grid.GetGenerator()->IsSelected()) anySelected = true;
        }
        targetX = anySelected ? objectCanvaTargetX : objectCanvaHiddenX;
        if (objectCanvaX < targetX) {
            objectCanvaX += objectCanvaSpeed * delta;
            if (objectCanvaX > targetX) objectCanvaX = targetX;
        } else if (objectCanvaX > targetX) {
            objectCanvaX -= objectCanvaSpeed * delta;
            if (objectCanvaX < targetX) objectCanvaX = targetX;
        }

        BeginDrawing();
        ClearBackground(BLACK);

        DrawTexture(g_background, 0, 0, WHITE);
        grid.Draw();
        player.Draw(cellSize, marginLeft, marginTop);
        wolf.Draw(cellSize, marginLeft, marginTop);
        DrawObjectPanel(objectCanvaX, objectCanvaY, grid);
        Vector2 mouse = GetMousePosition();
        DrawTexture(cursor_, (int)mouse.x-25, (int)mouse.y-20, WHITE);
        EndDrawing();
    }

    UnloadResources();
    CloseWindow();
    return 0;
}