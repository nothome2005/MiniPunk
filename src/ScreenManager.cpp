#include "ScreenManager.h"
#include "LevelLoader.h"
#include <random>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include "map_generator.h"
#include "LevelCreator.h"
#include <functional>

extern bool g_EditorBackToMenu;

ScreenManager::ScreenManager()
    : grid(gridSize, gridSize, marginLeft, marginTop, cellSize),
      generator(cellSize, marginLeft, marginTop),
      player(0, 0),
      wolf(0, 0, &wolfStrategy),
      g(std::random_device{}()),
      shouldExit(false)
{
    btnStart = std::make_unique<Button>(362, 350, 300, 70, "Start");
    btnCreator = std::make_unique<Button>(362, 440, 300, 70, "Creator");
    btnExit = std::make_unique<Button>(362, 530, 300, 70, "Exit");
    btnBack = std::make_unique<Button>(362, 800, 300, 60, "Back");
    btnStart->SetOnClick([this]() { levelLoader.Reload(); screen = Screen::LevelSelect; });
    btnCreator->SetOnClick([this]() { screen = Screen::Creator; });
    btnExit->SetOnClick([this]() { shouldExit = true; });
    btnBack->SetOnClick([this]() { screen = Screen::MainMenu; });
    std::uniform_real_distribution<float> xDist(0, 1024), yDist(0, 1024), speedYDist(60, 120), speedXDist(-20, 20), radiusDist(1.5f, 3.5f);
    snowflakes.resize(120);
    for (auto& s : snowflakes) s = {{xDist(g), yDist(g)}, speedYDist(g), speedXDist(g), radiusDist(g)};
    grid.SetMapCells(MapGenerator::Generate(gridSize, gridSize, 3, 3, 3, 10));
    grid.SetGenerator(&generator);
    std::vector<Vector2> freeCells;
    const auto& cells = grid.GetMapCells();
    for (int y = 0; y < gridSize; ++y)
        for (int x = 0; x < gridSize; ++x)
            if (cells[y * gridSize + x].GetType() == ResourceType::None)
                freeCells.push_back({(float)x, (float)y});
    Vector2 spawn = freeCells[std::uniform_int_distribution<>(0, (int)freeCells.size() - 1)(g)];
    player = Player((int)spawn.x, (int)spawn.y);
    player.SetGridSize(gridSize, gridSize);
    Vector2 wolfSpawn = std::vector<Vector2>{{0,0},{0,gridSize-1},{gridSize-1,0},{gridSize-1,gridSize-1}}[std::uniform_int_distribution<>(0,3)(g)];
    wolf = Wolf((int)wolfSpawn.x, (int)wolfSpawn.y, &wolfStrategy);
    playerState = std::make_unique<PlayerTurnState>(player, wolf, grid, currentState);
    wolfState = std::make_unique<WolfTurnState>(player, wolf, grid, currentState);
    playerState->SetWolfState(wolfState.get());
    wolfState->SetPlayerState(playerState.get());
    currentState = playerState.get();
    generatorX = 4; generatorY = 4;
}

void ScreenManager::UpdateSnowflakes(float dt) {
    std::uniform_real_distribution<float> xDist(0, 1024);
    for (auto& s : snowflakes) {
        s.pos.y += s.speedY * dt;
        s.pos.x += s.speedX * dt;
        if (s.pos.y > 1024) { s.pos.y = -s.radius; s.pos.x = xDist(g); }
        if (s.pos.x < 0) s.pos.x = 1024;
        if (s.pos.x > 1024) s.pos.x = 0;
    }
}

void ScreenManager::DrawEndScreen(bool win) {
    ClearBackground(BLACK);
    int fontSize = 80;
    const char* text = win ? "You won" : "You Lose";
    Color color = win ? GREEN : RED;
    int textW = MeasureText(text, fontSize);
    DrawText(text, 512 - textW/2, 250, fontSize, color);
    Button btnRestart(362, 450, 300, 70, "Restart");
    Button btnMenu(362, 540, 300, 70, "Menu");
    btnRestart.SetOnClick([this]() {
        if (levelLoader.GetLastLoadedLevel() != -1) {
            levelLoader.SetSelectedLevel(levelLoader.GetLastLoadedLevel());
            screen = Screen::LevelSelect;
        }
    });
    btnMenu.SetOnClick([this]() { screen = Screen::MainMenu; });
    btnRestart.Update();
    btnMenu.Update();
    btnRestart.Draw();
    btnMenu.Draw();
}

void ScreenManager::Update(float dt) {
    UpdateSnowflakes(dt);

    if (shouldExit) {
        CloseWindow();
        return;
    }

    Vector2 mouse = GetMousePosition();
    switch (screen) {
    case Screen::MainMenu:
        btnStart->Update();
        btnCreator->Update();
        btnExit->Update();
        break;
    case Screen::LevelSelect: {
        Button btnBack(312, 200, 180, 48, "Back");
        Button btnPlay(512, 200, 180, 48, "Play");
        btnBack.SetOnClick([this]() { screen = Screen::MainMenu; });
        btnPlay.SetOnClick([this]() {
            if (levelLoader.GetSelectedLevel() != -1) {
                levelLoader.SetLastLoadedLevel(levelLoader.GetSelectedLevel());
                std::string levelName = levelLoader.GetUserLevels()[levelLoader.GetSelectedLevel()];
                std::ifstream in("maps/" + levelName + ".lvl");
                int rows = 0, cols = 0; std::string mapName;
                if (in) {
                    in >> rows >> cols; in.ignore(); std::getline(in, mapName);
                    std::vector<int> objTypes(rows * cols); for (int& t : objTypes) in >> t;
                    std::vector<MapCell> mapCells(rows * cols);
                    for (int i = 0; i < rows * cols; ++i) {
                        int type, mass; in >> type >> mass;
                        Texture* tex = nullptr;
                        switch ((ResourceType)type) {
                            case ResourceType::Tree: tex = &tree_; break;
                            case ResourceType::Coal: tex = &coal_; break;
                            case ResourceType::Stone: tex = &iron_; break;
                            default: tex = nullptr; break;
                        }
                        if (objTypes[i] == (int)EditorObjectType::Generator) tex = &generator_;
                        if (objTypes[i] == (int)EditorObjectType::Player) tex = &player_idle_texture;
                        if (objTypes[i] == (int)EditorObjectType::Wolf) tex = &wolf_texture;
                        mapCells[i].Set((ResourceType)type, mass, tex);
                    }
                    Vector2 playerPos{-1,-1}, wolfPos{-1,-1};
                    generatorX = -1; generatorY = -1;
                    for (int y = 0; y < rows-1; ++y) for (int x = 0; x < cols-1; ++x) {
                        int idx = y * cols + x;
                        if (objTypes[idx] == (int)EditorObjectType::Generator &&
                            objTypes[idx+1] == (int)EditorObjectType::Generator &&
                            objTypes[idx+cols] == (int)EditorObjectType::Generator &&
                            objTypes[idx+cols+1] == (int)EditorObjectType::Generator) {
                            generatorX = x; generatorY = y;
                        }
                        if (objTypes[idx] == (int)EditorObjectType::Player) playerPos = {(float)x, (float)y};
                        if (objTypes[idx] == (int)EditorObjectType::Wolf) wolfPos = {(float)x, (float)y};
                    }
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
                    levelLoader.SetLastLoadedLevel(levelLoader.GetSelectedLevel());
                    screen = Screen::Game;
                }
            }
        });
        btnBack.Update();
        btnPlay.Update();
        int visibleLevels = 10;
        for (int i = 0; i < visibleLevels; ++i) {
            int idx = levelLoader.GetScroll() + i;
            if (idx >= (int)levelLoader.GetUserLevels().size()) break;
            Button levelBtn(362, 350 + i * 40, 300, 38, levelLoader.GetUserLevels()[idx], 28);
            levelBtn.SetOnClick([this, idx]() { levelLoader.SetSelectedLevel(idx); });
            levelBtn.Update();
        }
        break;
    }
    case Screen::Creator:
        levelCreator.Update();
        if (g_EditorBackToMenu) { g_EditorBackToMenu = false; screen = Screen::MainMenu; }
        break;
    case Screen::Game:
        if (currentState) currentState->Update(dt);
        if ((int)player.GetPosition().x == (int)wolf.GetPosition().x && (int)player.GetPosition().y == (int)wolf.GetPosition().y)
            screen = Screen::Lose;
        if (generatorX != -1 && generatorY != -1) {
            Vector2 p = player.GetPosition();
            int px = (int)p.x, py = (int)p.y;
            for (int dx = -1; dx <= 2; ++dx) {
                for (int dy = -1; dy <= 2; ++dy) {
                    if (dx >= 0 && dx < 2 && dy >= 0 && dy < 2) continue;
                    int cx = generatorX + dx, cy = generatorY + dy;
                    if (px == cx && py == cy) {
                        screen = Screen::Win;
                    }
                }
            }
        }
        break;
    case Screen::Lose:
        break;
    case Screen::Win:
        break;
    }
}

void ScreenManager::Draw() {
    if (shouldExit) return;

    ClearBackground(BLACK);
    DrawTexture(g_background, 0, 0, WHITE);
    for (const auto& s : snowflakes) DrawCircleV(s.pos, s.radius, WHITE);
    Vector2 mouse = GetMousePosition();
    switch (screen) {
    case Screen::MainMenu:
        btnStart->Draw(); btnCreator->Draw(); btnExit->Draw(); break;
    case Screen::LevelSelect: {
        Button btnBack(312, 200, 180, 48, "Back");
        Button btnPlay(512, 200, 180, 48, "Play");
        btnBack.Draw(); btnPlay.Draw();
        DrawText("Select level:", 362, 300, 38, WHITE);
        int visibleLevels = 10;
        for (int i = 0; i < visibleLevels; ++i) {
            int idx = levelLoader.GetScroll() + i;
            if (idx >= (int)levelLoader.GetUserLevels().size()) break;
            Button levelBtn(362, 350 + i * 40, 300, 38, levelLoader.GetUserLevels()[idx], 28);
            levelBtn.Draw();
        }
        if (levelLoader.GetSelectedLevel() != -1)
            DrawText(("Selected: " + levelLoader.GetUserLevels()[levelLoader.GetSelectedLevel()]).c_str(), 362, 750, 28, WHITE);
        break;
    }
    case Screen::Creator:
        levelCreator.Draw(); break;
    case Screen::Game:
        grid.Draw(); generator.Draw(); player.Draw(cellSize, marginLeft, marginTop); wolf.Draw(cellSize, marginLeft, marginTop); break;
    case Screen::Lose:
        DrawEndScreen(false);
        break;
    case Screen::Win:
        DrawEndScreen(true);
        break;
    }
    DrawTexture(cursor_, (int)mouse.x-25, (int)mouse.y-20, WHITE);
}
