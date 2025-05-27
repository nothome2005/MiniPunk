#include <random>
#include <memory>
#include <vector>
#include <filesystem>
#include <fstream>
#include <string>
#include "raylib.h"
#include "resource_dir.h"
#include "resource_manager.h"
#include "grid.h"
#include "map_generator.h"
#include "generator.h"
#include "player.h"
#include "wolf.h"
#include "game_state.h"
#include "LevelCreator.h"

struct Snowflake {
    Vector2 pos;
    float speedY;
    float speedX;
    float radius;
};

constexpr int SNOWFLAKE_COUNT = 120;

// --- Экраны ---
enum class Screen { MainMenu, LevelSelect, Creator, Game };

// --- Кнопка ---
struct MenuButton {
    Rectangle rect;
    const char* text;
};

// --- Рисование кнопки ---
void DrawMenuButton(const MenuButton& btn, bool hovered) {
    DrawRectangleRec(btn.rect, hovered ? Fade(WHITE, 0.25f) : Fade(WHITE, 0.12f));
    DrawRectangleLinesEx(btn.rect, 2, hovered ? WHITE: GRAY);
    int fontSize = 38;
    int textWidth = MeasureText(btn.text, fontSize);
    DrawText(btn.text, btn.rect.x + (btn.rect.width-textWidth)/2, btn.rect.y + (btn.rect.height-fontSize)/2, fontSize, WHITE);
}

// --- Список пользовательских карт ---
std::vector<std::string> userLevels;
int levelScroll = 0; // индекс первого видимого уровня
int selectedLevel = -1; // индекс выбранного уровня

void LoadUserLevels() {
    userLevels.clear();
    std::filesystem::create_directory("maps");
    for (const auto& entry : std::filesystem::directory_iterator("maps")) {
        if (entry.is_regular_file() && entry.path().extension() == ".lvl") {
            userLevels.push_back(entry.path().stem().string());
        }
    }
    levelScroll = 0;
    selectedLevel = -1;
}

extern bool g_EditorBackToMenu = false;

int main ()
{
    SetConfigFlags(FLAG_WINDOW_HIGHDPI);
    ShowCursor(); // Показываем системный курсор, чтобы пользователь видел мышь
    InitWindow(1024, 1024, "MiniPunk");
    HideCursor(); // Скрываем системный курсор после загрузки ресурсов (если нужен только свой курсор)
    SetTargetFPS(60);
    SearchAndSetResourceDir("resources");

    // --- Меню ---
    Screen screen = Screen::MainMenu;
    MenuButton btnStart   = { {362, 350, 300, 70}, "Start" };
    MenuButton btnCreator = { {362, 440, 300, 70}, "Creator" };
    MenuButton btnExit    = { {362, 530, 300, 70}, "Exit" };
    MenuButton btnBack    = { {362, 800, 300, 60}, "Back" };
    LevelCreator levelCreator;

    // --- Снег ---
    std::random_device rd;
    std::mt19937 g(rd());
    std::vector<Snowflake> snowflakes(SNOWFLAKE_COUNT);
    std::uniform_real_distribution<float> xDist(0, 1024);
    std::uniform_real_distribution<float> yDist(0, 1024);
    std::uniform_real_distribution<float> speedYDist(60, 120);
    std::uniform_real_distribution<float> speedXDist(-20, 20);
    std::uniform_real_distribution<float> radiusDist(1.5f, 3.5f);
    for (auto& s : snowflakes) {
        s.pos = { xDist(g), yDist(g) };
        s.speedY = speedYDist(g);
        s.speedX = speedXDist(g);
        s.radius = radiusDist(g);
    }

    // --- Игровые объекты ---
    constexpr int gridSize = 10;
    constexpr float marginLeft = 50.0f;
    constexpr float marginTop = 150.0f;
    constexpr float marginRight = 250.0f;
    constexpr float marginBottom = 150.0f;
    constexpr float cellSize = (1024.0f - marginLeft - marginRight) / gridSize;
    Grid grid(gridSize, gridSize, marginLeft, marginTop, cellSize);
    LoadResources();
    auto mapCells = MapGenerator::Generate(gridSize, gridSize, 3, 3, 3, 10);
    grid.SetMapCells(mapCells);
    Generator generator(cellSize, marginLeft, marginTop);
    grid.SetGenerator(&generator);
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
    std::uniform_int_distribution<> dist(0, (int)freeCells.size() - 1);
    Vector2 spawn = freeCells[dist(g)];
    Player player((int)spawn.x, (int)spawn.y);
    player.SetGridSize(gridSize, gridSize);
    std::vector<Vector2> wolfCorners = {
        {0, 0}, {0, gridSize-1}, {gridSize-1, 0}, {gridSize-1, gridSize-1}
    };
    std::uniform_int_distribution<> wolfDist(0, 3);
    Vector2 wolfSpawn = wolfCorners[wolfDist(g)];
    WolfMoveStrategy wolfStrategy;
    Wolf wolf((int)wolfSpawn.x, (int)wolfSpawn.y, &wolfStrategy);
    GameState* currentState = nullptr;
    std::unique_ptr<PlayerTurnState> playerState;
    std::unique_ptr<WolfTurnState> wolfState;
    playerState = std::make_unique<PlayerTurnState>(player, wolf, grid, currentState);
    wolfState = std::make_unique<WolfTurnState>(player, wolf, grid, currentState);
    playerState->SetWolfState(wolfState.get());
    wolfState->SetPlayerState(playerState.get());
    currentState = playerState.get();

    while (!WindowShouldClose())
    {
        float delta = GetFrameTime();
        Vector2 mouse = GetMousePosition();
        bool mouseClick = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
        int wheel = GetMouseWheelMove();

        // --- Update snowflakes ---
        for (auto& s : snowflakes) {
            s.pos.y += s.speedY * delta;
            s.pos.x += s.speedX * delta;
            if (s.pos.y > 1024) {
                s.pos.y = -s.radius;
                s.pos.x = xDist(g);
            }
            if (s.pos.x < 0) s.pos.x = 1024;
            if (s.pos.x > 1024) s.pos.x = 0;
        }

        // --- Логика экранов ---
        if (screen == Screen::MainMenu) {
            if (mouseClick) {
                if (CheckCollisionPointRec(mouse, btnStart.rect)) {
                    LoadUserLevels();
                    screen = Screen::LevelSelect;
                }
                else if (CheckCollisionPointRec(mouse, btnCreator.rect)) screen = Screen::Creator;
                else if (CheckCollisionPointRec(mouse, btnExit.rect)) { break; }
            }
        } else if (screen == Screen::LevelSelect) {
            if (mouseClick) {
                Rectangle backRect = {312, 200, 180, 48};
                Rectangle playRect = {512, 200, 180, 48};
                if (CheckCollisionPointRec(mouse, backRect)) {
                    screen = Screen::MainMenu;
                } else if (CheckCollisionPointRec(mouse, playRect) && selectedLevel != -1) {
                    // --- Загрузка выбранного уровня ---
                    std::string levelName = userLevels[selectedLevel];
                    std::string filePath = "maps/" + levelName + ".lvl";
                    std::ifstream in(filePath);
                    int rows = 0, cols = 0;
                    std::string mapName;
                    if (in) {
                        in >> rows >> cols;
                        in.ignore();
                        std::getline(in, mapName);
                        std::vector<int> objTypes(rows * cols);
                        for (int i = 0; i < rows * cols; ++i) in >> objTypes[i];
                        std::vector<MapCell> mapCells(rows * cols);
                        for (int i = 0; i < rows * cols; ++i) {
                            int type, mass;
                            in >> type >> mass;
                            Texture* tex = nullptr;
                            switch ((ResourceType)type) {
                                case ResourceType::Tree: tex = &tree_; break;
                                case ResourceType::Coal: tex = &coal_; break;
                                case ResourceType::Stone: tex = &iron_; break;
                                default: tex = nullptr; break;
                            }
                            // Генератор, игрок, волк через objectTypes
                            if (objTypes[i] == (int)EditorObjectType::Generator) tex = &generator_;
                            if (objTypes[i] == (int)EditorObjectType::Player) tex = &player_idle_texture;
                            if (objTypes[i] == (int)EditorObjectType::Wolf) tex = &wolf_texture;
                            mapCells[i].Set((ResourceType)type, mass, tex);
                        }
                        // --- Найти игрока, волка и генератор ---
                        Vector2 playerPos = {-1, -1};
                        Vector2 wolfPos = {-1, -1};
                        Vector2 generatorPos = {-1, -1};
                        bool foundGen = false;
                        for (int y = 0; y < rows; ++y) {
                            for (int x = 0; x < cols; ++x) {
                                int idx = y * cols + x;
                                if (objTypes[idx] == (int)EditorObjectType::Player) playerPos = {(float)x, (float)y};
                                if (objTypes[idx] == (int)EditorObjectType::Wolf) wolfPos = {(float)x, (float)y};
                                // Левый верхний угол генератора (ищем только один раз)
                                if (!foundGen && x < cols-1 && y < rows-1) {
                                    bool isGen = true;
                                    for (int dy = 0; dy < 2; ++dy)
                                        for (int dx = 0; dx < 2; ++dx)
                                            if (objTypes[(y+dy)*cols + (x+dx)] != (int)EditorObjectType::Generator)
                                                isGen = false;
                                    if (isGen) { generatorPos = {(float)x, (float)y}; foundGen = true; }
                                }
                            }
                        }
                        // --- Пересоздать игровое поле ---
                        grid = Grid(cols, rows, marginLeft, marginTop, cellSize);
                        grid.SetMapCells(mapCells);
                        generator = Generator(cellSize, marginLeft, marginTop);
                        grid.SetGenerator(&generator);
                        player = Player((int)playerPos.x, (int)playerPos.y);
                        player.SetGridSize(cols, rows);
                        wolf = Wolf((int)wolfPos.x, (int)wolfPos.y, &wolfStrategy);
                        playerState = std::make_unique<PlayerTurnState>(player, wolf, grid, currentState);
                        wolfState = std::make_unique<WolfTurnState>(player, wolf, grid, currentState);
                        playerState->SetWolfState(wolfState.get());
                        wolfState->SetPlayerState(playerState.get());
                        currentState = playerState.get();
                        player.ResetTurn();
                        wolf.StartTurn(grid, player);
                        screen = Screen::Game;
                    }
                }
            }
        } else if (screen == Screen::Creator) {
            levelCreator.Update();
            if (g_EditorBackToMenu) {
                g_EditorBackToMenu = false;
                screen = Screen::MainMenu;
            }
        } else if (screen == Screen::Game) {
            if (currentState) currentState->Update(delta);
        }

        // --- Рендеринг ---
        BeginDrawing();
        ClearBackground(BLACK);
        // --- Сначала фон ---
        DrawTexture(g_background, 0, 0, WHITE);

        // --- Рендеринг снегопада ---
        for (const auto& s : snowflakes) {
            DrawCircleV(s.pos, s.radius, WHITE);
        }

        // --- Рендеринг экранов ---
        if (screen == Screen::MainMenu) {
            DrawMenuButton(btnStart, CheckCollisionPointRec(mouse, btnStart.rect));
            DrawMenuButton(btnCreator, CheckCollisionPointRec(mouse, btnCreator.rect));
            DrawMenuButton(btnExit, CheckCollisionPointRec(mouse, btnExit.rect));
        } else if (screen == Screen::LevelSelect) {
            DrawRectangle(312, 200, 180, 48, Fade(WHITE, 0.12f));
            DrawRectangleLines(312, 200, 180, 48, WHITE);
            DrawText("Back", 312 + (180-MeasureText("Back", 38))/2, 200 + (48-38)/2, 38, WHITE);
            DrawRectangle(512, 200, 180, 48, Fade(WHITE, 0.12f));
            DrawRectangleLines(512, 200, 180, 48, WHITE);
            DrawText("Play", 512 + (180-MeasureText("Play", 38))/2, 200 + (48-38)/2, 38, WHITE);
            DrawText("Select level:", 362, 300, 38, WHITE);
            int visibleLevels = 10;
            for (int i = 0; i < visibleLevels; ++i) {
                int idx = levelScroll + i;
                if (idx >= (int)userLevels.size()) break;
                Rectangle levelRect = {362, 350 + i * 40, 300, 38};
                bool hovered = CheckCollisionPointRec(mouse, levelRect);
                DrawRectangleRec(levelRect, hovered ? Fade(WHITE, 0.25f) : Fade(WHITE, 0.12f));
                DrawRectangleLinesEx(levelRect, 2, hovered ? WHITE : GRAY);
                DrawText(userLevels[idx].c_str(), 362 + 10, 350 + i * 40 + 5, 28, WHITE);
                if (hovered && mouseClick) selectedLevel = idx;
            }
            if (selectedLevel != -1) {
                DrawText(("Selected: " + userLevels[selectedLevel]).c_str(), 362, 750, 28, WHITE);
            }
        } else if (screen == Screen::Creator) {
            levelCreator.Draw();
            DrawMenuButton(btnBack, CheckCollisionPointRec(mouse, btnBack.rect));
        } else if (screen == Screen::Game) {
            grid.Draw();
            generator.Draw();
            player.Draw(cellSize, marginLeft, marginTop);
            wolf.Draw(cellSize, marginLeft, marginTop);
        }
        // --- Курсор всегда последним ---
        DrawTexture(cursor_, (int)mouse.x-25, (int)mouse.y-20, WHITE);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}