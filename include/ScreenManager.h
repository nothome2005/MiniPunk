#pragma once
#include <memory>
#include <vector>
#include <string>
#include <random>
#include "raylib.h"
#include "LevelCreator.h"
#include "player.h"
#include "wolf.h"
#include "grid.h"
#include "generator.h"
#include "game_state.h"
#include "resource_manager.h"
#include "LevelLoader.h"
#include "button.h"

class ScreenManager {
public:
    ScreenManager();
    void Update(float dt);
    void Draw();
private:
    enum class Screen { MainMenu, LevelSelect, Creator, Game, Lose, Win };
    Screen screen = Screen::MainMenu;
    bool shouldExit = false;  // Flag to indicate we should exit the game
    std::unique_ptr<Button> btnStart, btnCreator, btnExit, btnBack;
    LevelCreator levelCreator;
    constexpr static int gridSize = 10;
    constexpr static float marginLeft = 50.0f, marginTop = 150.0f, marginRight = 250.0f, marginBottom = 150.0f;
    constexpr static float cellSize = (1024.0f - marginLeft - marginRight) / gridSize;
    Grid grid; Generator generator; Player player; WolfMoveStrategy wolfStrategy; Wolf wolf;
    GameState* currentState = nullptr;
    std::unique_ptr<PlayerTurnState> playerState;
    std::unique_ptr<WolfTurnState> wolfState;
    int generatorX = -1, generatorY = -1; // ���������� ������ �������� ���� ����������
    struct Snowflake { Vector2 pos; float speedY, speedX, radius; };
    std::vector<Snowflake> snowflakes; std::mt19937 g;
    LevelLoader levelLoader;
    void UpdateSnowflakes(float dt);
    void DrawEndScreen(bool win);
};
