#include "ScreenManager.h"
#include <random>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include "map_generator.h"
#include "LevelCreator.h"


extern bool g_EditorBackToMenu;

ScreenManager::ScreenManager()
    : grid(gridSize, gridSize, marginLeft, marginTop, cellSize),
      generator(cellSize, marginLeft, marginTop),
      player(0, 0),
      wolf(0, 0, &wolfStrategy),
      g(std::random_device{}())
{
    btnStart = std::make_unique<MenuButtonUI>(362, 350, 300, 70, "Start");
    btnCreator = std::make_unique<MenuButtonUI>(362, 440, 300, 70, "Creator");
    btnExit = std::make_unique<MenuButtonUI>(362, 530, 300, 70, "Exit");
    btnBack = std::make_unique<MenuButtonUI>(362, 800, 300, 60, "Back");
    // --- Снег ---
    std::uniform_real_distribution<float> xDist(0, 1024);
    std::uniform_real_distribution<float> yDist(0, 1024);
    std::uniform_real_distribution<float> speedYDist(60, 120);
    std::uniform_real_distribution<float> speedXDist(-20, 20);
    std::uniform_real_distribution<float> radiusDist(1.5f, 3.5f);
    snowflakes.resize(120);
    for (auto& s : snowflakes) {
        s.pos = { xDist(g), yDist(g) };
        s.speedY = speedYDist(g);
        s.speedX = speedXDist(g);
        s.radius = radiusDist(g);
    }
    // --- Игровые объекты ---
    auto mapCells = MapGenerator::Generate(gridSize, gridSize, 3, 3, 3, 10);
    grid.SetMapCells(mapCells);
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
    player = Player((int)spawn.x, (int)spawn.y);
    player.SetGridSize(gridSize, gridSize);
    std::vector<Vector2> wolfCorners = {
        {0, 0}, {0, gridSize-1}, {gridSize-1, 0}, {gridSize-1, gridSize-1}
    };
    std::uniform_int_distribution<> wolfDist(0, 3);
    Vector2 wolfSpawn = wolfCorners[wolfDist(g)];
    wolf = Wolf((int)wolfSpawn.x, (int)wolfSpawn.y, &wolfStrategy);
    playerState = std::make_unique<PlayerTurnState>(player, wolf, grid, currentState);
    wolfState = std::make_unique<WolfTurnState>(player, wolf, grid, currentState);
    playerState->SetWolfState(wolfState.get());
    wolfState->SetPlayerState(playerState.get());
    currentState = playerState.get();
    LoadUserLevels();
}

void ScreenManager::LoadUserLevels() {
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

void ScreenManager::UpdateSnowflakes(float dt) {
    std::uniform_real_distribution<float> xDist(0, 1024);
    for (auto& s : snowflakes) {
        s.pos.y += s.speedY * dt;
        s.pos.x += s.speedX * dt;
        if (s.pos.y > 1024) {
            s.pos.y = -s.radius;
            s.pos.x = xDist(g);
        }
        if (s.pos.x < 0) s.pos.x = 1024;
        if (s.pos.x > 1024) s.pos.x = 0;
    }
}

void ScreenManager::Update(float dt) {
    UpdateSnowflakes(dt);
    Vector2 mouse = GetMousePosition();
    bool mouseClick = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    int wheel = GetMouseWheelMove();
    switch (screen) {
    case Screen::MainMenu:
        if (mouseClick) {
            if (btnStart->IsHovered()) {
                LoadUserLevels();
                screen = Screen::LevelSelect;
            } else if (btnCreator->IsHovered()) {
                screen = Screen::Creator;
            } else if (btnExit->IsHovered()) {
                CloseWindow();
            }
        }
        break;
    case Screen::LevelSelect: {
        MenuButtonUI btnBack(312, 200, 180, 48, "Back");
        MenuButtonUI btnPlay(512, 200, 180, 48, "Play");
        if (mouseClick) {
            if (btnBack.IsHovered()) {
                screen = Screen::MainMenu;
            } else if (btnPlay.IsHovered() && selectedLevel != -1) {
                lastLoadedLevel = selectedLevel;
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
                    lastLoadedLevel = selectedLevel;
                    screen = Screen::Game;
                }
            }
        }
        int visibleLevels = 10;
        for (int i = 0; i < visibleLevels; ++i) {
            int idx = levelScroll + i;
            if (idx >= (int)userLevels.size()) break;
            MenuButtonUI levelBtn(362, 350 + i * 40, 300, 38, userLevels[idx].c_str(), 28);
            if (levelBtn.IsHovered() && mouseClick) selectedLevel = idx;
        }
        break;
    }
    case Screen::Creator:
        levelCreator.Update();
        if (g_EditorBackToMenu) {
            g_EditorBackToMenu = false;
            screen = Screen::MainMenu;
        }
        break;
    case Screen::Game:
        if (currentState) currentState->Update(dt);
        if ((int)player.GetPosition().x == (int)wolf.GetPosition().x && (int)player.GetPosition().y == (int)wolf.GetPosition().y) {
            screen = Screen::Lose;
        }
        break;
    case Screen::Lose:
        // Ничего не обновляем
        break;
    }
}

void ScreenManager::Draw() {
    ClearBackground(BLACK);
    DrawTexture(g_background, 0, 0, WHITE);
    for (const auto& s : snowflakes) {
        DrawCircleV(s.pos, s.radius, WHITE);
    }
    Vector2 mouse = GetMousePosition();
    switch (screen) {
    case Screen::MainMenu:
        btnStart->Draw();
        btnCreator->Draw();
        btnExit->Draw();
        break;
    case Screen::LevelSelect: {
        MenuButtonUI btnBack(312, 200, 180, 48, "Back");
        MenuButtonUI btnPlay(512, 200, 180, 48, "Play");
        btnBack.Draw();
        btnPlay.Draw();
        DrawText("Select level:", 362, 300, 38, WHITE);
        int visibleLevels = 10;
        for (int i = 0; i < visibleLevels; ++i) {
            int idx = levelScroll + i;
            if (idx >= (int)userLevels.size()) break;
            MenuButtonUI levelBtn(362, 350 + i * 40, 300, 38, userLevels[idx].c_str(), 28);
            levelBtn.Draw();
        }
        if (selectedLevel != -1) {
            DrawText(("Selected: " + userLevels[selectedLevel]).c_str(), 362, 750, 28, WHITE);
        }
        break;
    }
    case Screen::Creator:
        levelCreator.Draw();
        break;
    case Screen::Game:
        grid.Draw();
        generator.Draw();
        player.Draw(cellSize, marginLeft, marginTop);
        wolf.Draw(cellSize, marginLeft, marginTop);
        break;
    case Screen::Lose: {
        ClearBackground(BLACK);
        int fontSize = 80;
        const char* loseText = "You Lose";
        int textW = MeasureText(loseText, fontSize);
        DrawText(loseText, 512 - textW/2, 250, fontSize, RED);
        MenuButtonUI btnRestart(362, 450, 300, 70, "Restart");
        MenuButtonUI btnMenu(362, 540, 300, 70, "Menu");
        btnRestart.Draw();
        btnMenu.Draw();
        // --- Обработка кликов по кнопкам ---
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (btnRestart.IsHovered() && lastLoadedLevel != -1) {
                selectedLevel = lastLoadedLevel;
                screen = Screen::LevelSelect;
            } else if (btnMenu.IsHovered()) {
                screen = Screen::MainMenu;
            }
        }
        break;
    }
    }
    DrawTexture(cursor_, (int)mouse.x-25, (int)mouse.y-20, WHITE);
}
