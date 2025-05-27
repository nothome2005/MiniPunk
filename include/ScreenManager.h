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

// --- Класс для стандартизированных кнопок меню ---
class MenuButtonUI {
public:
    Rectangle rect;
    std::string text;
    int fontSize;
    Color colorNormal;
    Color colorHover;
    Color borderColor;
    Color textColor;
    MenuButtonUI(float x, float y, float w, float h, const char* txt, int fontSize = 38,
                 Color colorNormal = Fade(WHITE, 0.12f),
                 Color colorHover = Fade(WHITE, 0.25f),
                 Color borderColor = WHITE,
                 Color textColor = WHITE)
        : rect{ x, y, w, h }, text(txt), fontSize(fontSize),
          colorNormal(colorNormal), colorHover(colorHover), borderColor(borderColor), textColor(textColor) {}
    bool IsHovered() const {
        return CheckCollisionPointRec(GetMousePosition(), rect);
    }
    bool IsClicked() const {
        return IsHovered() && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    }
    void Draw() const {
        bool hovered = IsHovered();
        DrawRectangleRec(rect, hovered ? colorHover : colorNormal);
        DrawRectangleLinesEx(rect, 2, hovered ? borderColor : GRAY);
        int textW = MeasureText(text.c_str(), fontSize);
        DrawText(text.c_str(), rect.x + (rect.width-textW)/2, rect.y + (rect.height-fontSize)/2, fontSize, textColor);
    }
};

class ScreenManager {
public:
    ScreenManager();
    void Update(float dt);
    void Draw();
private:
    enum class Screen { MainMenu, LevelSelect, Creator, Game, Lose };
    Screen screen = Screen::MainMenu;
    // --- Кнопки меню ---
    std::unique_ptr<MenuButtonUI> btnStart, btnCreator, btnExit, btnBack;
    // --- Для LevelSelect ---
    std::vector<std::string> userLevels;
    int levelScroll = 0;
    int selectedLevel = -1;
    int lastLoadedLevel = -1;
    // --- Для Creator ---
    LevelCreator levelCreator;
    // --- Для Game ---
    constexpr static int gridSize = 10;
    constexpr static float marginLeft = 50.0f;
    constexpr static float marginTop = 150.0f;
    constexpr static float marginRight = 250.0f;
    constexpr static float marginBottom = 150.0f;
    constexpr static float cellSize = (1024.0f - marginLeft - marginRight) / gridSize;
    Grid grid;
    Generator generator;
    Player player;
    WolfMoveStrategy wolfStrategy;
    Wolf wolf;
    GameState* currentState = nullptr;
    std::unique_ptr<PlayerTurnState> playerState;
    std::unique_ptr<WolfTurnState> wolfState;
    // --- Снег ---
    struct Snowflake {
        Vector2 pos;
        float speedY;
        float speedX;
        float radius;
    };
    std::vector<Snowflake> snowflakes;
    std::mt19937 g;
    void LoadUserLevels();
    void UpdateSnowflakes(float dt);
};
