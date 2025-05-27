#include "LevelCreator.h"
#include "map_cell.h"
#include <algorithm>
#include <cassert>
#include <filesystem>
#include <fstream>

LevelCreator::LevelCreator()
    : rows(10), cols(10), marginLeft(50.0f), marginTop(150.0f), cellSize((1024.0f - 50.0f - 250.0f) / 10),
      currentX(0), currentY(0),
      mapCells(rows * cols),
      objectTypes(rows * cols, EditorObjectType::None),
      grid(rows, cols, marginLeft, marginTop, cellSize),
      nameEditRect({50, 50, 300, 40}), nameEditActive(false),
      btnIncRect({400, 50, 100, 40}), btnDecRect({520, 50, 100, 40}),
      btnSaveRect({650, 50, 100, 40}), mapName{}
{
    SyncGrid();
}

void LevelCreator::SyncGrid() {
    grid.SetMapCells(mapCells);
}

void LevelCreator::Update() {
    HandleUI();
    if (IsKeyPressed(KEY_UP))    currentY = std::max(0, currentY - 1);
    if (IsKeyPressed(KEY_DOWN))  currentY = std::min(rows - 1, currentY + 1);
    if (IsKeyPressed(KEY_LEFT))  currentX = std::max(0, currentX - 1);
    if (IsKeyPressed(KEY_RIGHT)) currentX = std::min(cols - 1, currentX + 1);
    currentX = std::max(0, std::min(currentX, cols - 1));
    currentY = std::max(0, std::min(currentY, rows - 1));
    for (int k = KEY_ONE; k <= KEY_SIX; ++k) {
        if (IsKeyPressed((KeyboardKey)k)) PlaceResource(k);
    }
    // Клавиша 0 очищает выбранную клетку
    if (IsKeyPressed(KEY_ZERO)) {
        int idx = currentY * cols + currentX;
        if (idx >= 0 && idx < (int)objectTypes.size()) {
            if (objectTypes[idx] == EditorObjectType::Generator) {
                // Очищаем всю 2x2 область генератора, если попали в любую из его клеток
                int gx = currentX, gy = currentY;
                // ищем левый верхний угол генератора
                for (int dy = 0; dy <= 1; ++dy) for (int dx = 0; dx <= 1; ++dx) {
                    int tx = currentX - dx, ty = currentY - dy;
                    if (tx >= 0 && ty >= 0 && tx < cols-1 && ty < rows-1) {
                        bool isGen = true;
                        for (int sy = 0; sy < 2; ++sy)
                            for (int sx = 0; sx < 2; ++sx)
                                if (objectTypes[(ty+sy)*cols + (tx+sx)] != EditorObjectType::Generator)
                                    isGen = false;
                        if (isGen) { gx = tx; gy = ty; }
                    }
                }
                for (int dy = 0; dy < 2; ++dy)
                    for (int dx = 0; dx < 2; ++dx)
                        objectTypes[(gy+dy)*cols + (gx+dx)] = EditorObjectType::None;
            } else {
                objectTypes[idx] = EditorObjectType::None;
            }
            mapCells[idx].Set(ResourceType::None, 0, nullptr);
            SyncGrid();
        }
    }
}

void LevelCreator::PlaceResource(int key) {
    int idx = currentY * cols + currentX;
    if (idx < 0 || idx >= (int)objectTypes.size()) return;
    ResourceType type = ResourceType::None;
    Texture* tex = nullptr;
    EditorObjectType objType = EditorObjectType::None;
    switch (key) {
        case KEY_ONE:   type = ResourceType::Tree;   tex = &tree_;      objType = EditorObjectType::Tree; break;
        case KEY_TWO:   type = ResourceType::Coal;   tex = &coal_;      objType = EditorObjectType::Coal; break;
        case KEY_THREE: type = ResourceType::Stone;  tex = &iron_;      objType = EditorObjectType::Stone; break;
        case KEY_FOUR:  type = ResourceType::None;   tex = &player_idle_texture; objType = EditorObjectType::Player; break;
        case KEY_FIVE:  type = ResourceType::None;   tex = &wolf_texture; objType = EditorObjectType::Wolf; break;
        case KEY_SIX:   type = ResourceType::None;   tex = &generator_; objType = EditorObjectType::Generator; break;
    }
    if (objType == EditorObjectType::Generator) {
        // Разрешаем ставить только в ЛЕВЫЙ ВЕРХНИЙ угол 2x2 блока
        if (currentX < cols-1 && currentY < rows-1) {
            bool canPlace = true;
            for (int dy = 0; dy < 2; ++dy)
                for (int dx = 0; dx < 2; ++dx)
                    if (objectTypes[(currentY+dy)*cols + (currentX+dx)] == EditorObjectType::Generator)
                        canPlace = false;
            if (currentX > 0 && objectTypes[currentY*cols + (currentX-1)] == EditorObjectType::Generator) canPlace = false;
            if (currentY > 0 && objectTypes[(currentY-1)*cols + currentX] == EditorObjectType::Generator) canPlace = false;
            if (!canPlace) return;
            for (int dy = 0; dy < 2; ++dy)
                for (int dx = 0; dx < 2; ++dx)
                    objectTypes[(currentY+dy)*cols + (currentX+dx)] = EditorObjectType::Generator;
        }
    } else if (objType == EditorObjectType::Player) {
        // Только один игрок на карте: очищаем все клетки от игрока
        for (auto& t : objectTypes) if (t == EditorObjectType::Player) t = EditorObjectType::None;
        objectTypes[idx] = EditorObjectType::Player;
    } else {
        objectTypes[idx] = objType;
    }
    if (objType == EditorObjectType::Tree || objType == EditorObjectType::Coal || objType == EditorObjectType::Stone)
        mapCells[idx].Set(type, 1, tex);
    else
        mapCells[idx].Set(ResourceType::None, 0, nullptr);
    SyncGrid();
}

void LevelCreator::Draw() const {
    // 1. Нарисовать сетку (фон клеток и линии)
    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {
            int idx = y * cols + x;
            float cx = marginLeft + x * cellSize;
            float cy = marginTop + y * cellSize;
            // Фон клетки
            Color cellColor = (x == currentX && y == currentY) ? WHITE : Color{30, 50, 80, 120};
            DrawRectangle(cx, cy, cellSize, cellSize, Fade(cellColor, (x == currentX && y == currentY) ? 0.10f : 0.18f));
            // Контур клетки
            DrawRectangleLines(cx, cy, cellSize, cellSize, (x == currentX && y == currentY) ? WHITE : Color{30, 50, 80, 180});
            // Содержимое клетки
            if (idx >= 0 && idx < (int)mapCells.size())
                mapCells[idx].Draw(cx, cy, cellSize);
        }
    }
    // --- Игрок ---
    bool playerDrawn = false;
    for (int y = 0; y < rows && !playerDrawn; ++y) {
        for (int x = 0; x < cols && !playerDrawn; ++x) {
            int idx = y * cols + x;
            if (idx >= 0 && idx < (int)objectTypes.size() && objectTypes[idx] == EditorObjectType::Player) {
                float cx = marginLeft + x * cellSize;
                float cy = marginTop + y * cellSize;
                // Масштаб игрока: вписать в клетку
                float scale = std::min(cellSize / (float)player_idle_texture.width, cellSize / (float)player_idle_texture.height);
                float texW = player_idle_texture.width * scale;
                float texH = player_idle_texture.height * scale;
                float offsetX = cx + (cellSize - texW) / 2.0f;
                float offsetY = cy + (cellSize - texH) / 2.0f;
                DrawTextureEx(player_idle_texture, {offsetX, offsetY}, 0, scale, WHITE);
                playerDrawn = true;
            }
        }
    }
    // --- Генератор (2x2, одна большая иконка) ---
    for (int y = 0; y < rows-1; ++y) {
        for (int x = 0; x < cols-1; ++x) {
            int idx = y * cols + x;
            if (idx >= 0 && idx < (int)objectTypes.size() && objectTypes[idx] == EditorObjectType::Generator) {
                // Проверяем, что 2x2 клетки подряд - генератор
                bool isGen = true;
                for (int dy = 0; dy < 2; ++dy)
                    for (int dx = 0; dx < 2; ++dx)
                        if (objectTypes[(y+dy)*cols + (x+dx)] != EditorObjectType::Generator) isGen = false;
                if (!isGen) continue;
                float cx = marginLeft + x * cellSize;
                float cy = marginTop + y * cellSize;
                float size = cellSize * 2.0f;
                float scale = size / std::max((float)generator_.width, (float)generator_.height);
                float texW = generator_.width * scale;
                float texH = generator_.height * scale;
                float offsetX = cx + (size - texW) / 2.0f;
                float offsetY = cy + (size - texH) / 2.0f;
                DrawTextureEx(generator_, {offsetX, offsetY}, 0, scale, WHITE);
            }
        }
    }
    // --- Волк ---
    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {
            int idx = y * cols + x;
            if (idx >= 0 && idx < (int)objectTypes.size() && objectTypes[idx] == EditorObjectType::Wolf) {
                float cx = marginLeft + x * cellSize;
                float cy = marginTop + y * cellSize;
                float scaleX = cellSize / (float)wolf_texture.width;
                float scaleY = cellSize / (float)wolf_texture.height;
                float scale = std::min(scaleX, scaleY);
                float texW = wolf_texture.width * scale;
                float texH = wolf_texture.height * scale;
                float offsetX = cx + (cellSize - texW) / 2.0f;
                float offsetY = cy + (cellSize - texH) / 2.0f;
                DrawTextureEx(wolf_texture, {offsetX, offsetY}, 0, scale, WHITE);
            }
        }
    }
    // 2. Нарисовать линии сетки поверх
    for (int y = 0; y <= rows; ++y) {
        float yPos = marginTop + y * cellSize;
        DrawLineEx({marginLeft, yPos}, {marginLeft + cols * cellSize, yPos}, 2.0f, Color{30, 50, 80, 180});
    }
    for (int x = 0; x <= cols; ++x) {
        float xPos = marginLeft + x * cellSize;
        DrawLineEx({xPos, marginTop}, {xPos, marginTop + rows * cellSize}, 2.0f, Color{30, 50, 80, 180});
    }
    // 3. Инвентарь/подсказка
    float invY = 900;
    float invX = 100;
    float iconSize = 64;
    float gap = 24;
    struct InvItem { Texture* tex; int num; } items[] = {
        {&tree_, 1}, {&coal_, 2}, {&iron_, 3}, {&player_idle_texture, 4}, {&wolf_texture, 5}, {&generator_, 6}
    };
    for (int i = 0; i < 6; ++i) {
        float x = invX + i * (iconSize + gap);
        DrawRectangle(x, invY, iconSize, iconSize, Fade(WHITE, 0.15f));
        DrawRectangleLines(x, invY, iconSize, iconSize, WHITE);
        // Масштаб для всех иконок одинаковый (вписывание по большей стороне)
        float scale = std::min(iconSize / (float)items[i].tex->width, iconSize / (float)items[i].tex->height);
        float texW = items[i].tex->width * scale;
        float texH = items[i].tex->height * scale;
        float offsetX = x + (iconSize - texW) / 2.0f;
        float offsetY = invY + (iconSize - texH) / 2.0f;
        DrawTextureEx(*items[i].tex, {offsetX, offsetY}, 0, scale, WHITE);
        DrawText(TextFormat("%d", items[i].num), x + iconSize - 24, invY + iconSize - 32, 32, WHITE);
    }
    // 4. UI редактора
    DrawUI();
}

void LevelCreator::DrawUI() const {
    // Поле для ввода названия
    DrawRectangleRec(nameEditRect, nameEditActive ? Fade(WHITE, 0.25f) : Fade(WHITE, 0.12f));
    DrawRectangleLinesEx(nameEditRect, 2, nameEditActive ? YELLOW : GRAY);
    DrawText("Map name:", nameEditRect.x, nameEditRect.y - 32, 24, WHITE);
    DrawText(mapName, nameEditRect.x + 8, nameEditRect.y + 8, 28, WHITE);
    // Кнопки +/-
    DrawRectangleRec(btnIncRect, Fade(WHITE, 0.18f));
    DrawRectangleLinesEx(btnIncRect, 2, WHITE);
    DrawText("+", btnIncRect.x + 36, btnIncRect.y + 4, 32, WHITE);
    DrawRectangleRec(btnDecRect, Fade(WHITE, 0.18f));
    DrawRectangleLinesEx(btnDecRect, 2, WHITE);
    DrawText("-", btnDecRect.x + 36, btnDecRect.y + 4, 32, WHITE);
    DrawText(TextFormat("Size: %dx%d", cols, rows), btnIncRect.x, btnIncRect.y - 32, 24, WHITE);
    // Кнопка Save
    Vector2 mouse = GetMousePosition();
    bool hovered = CheckCollisionPointRec(mouse, btnSaveRect);
    DrawRectangleRec(btnSaveRect, Fade(WHITE, 0.18f));
    DrawRectangleLinesEx(btnSaveRect, 2, hovered ? WHITE : GRAY);
    int tw = MeasureText("Save", 32);
    DrawText("Save", btnSaveRect.x + (btnSaveRect.width-tw)/2, btnSaveRect.y + 8, 32, WHITE);
    // Кнопка Back справа от Save
    Rectangle btnBackRect = {btnSaveRect.x + btnSaveRect.width + 20, btnSaveRect.y, 140, btnSaveRect.height};
    bool backHovered = CheckCollisionPointRec(mouse, btnBackRect);
    DrawRectangleRec(btnBackRect, Fade(WHITE, 0.18f));
    DrawRectangleLinesEx(btnBackRect, 2, backHovered ? WHITE : GRAY);
    int twb = MeasureText("Back", 32);
    DrawText("Back", btnBackRect.x + (btnBackRect.width-twb)/2, btnBackRect.y + 8, 32, WHITE);
}

void LevelCreator::HandleUI() {
    Vector2 mouse = GetMousePosition();
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (CheckCollisionPointRec(mouse, nameEditRect)) {
            nameEditActive = true;
        } else {
            nameEditActive = false;
        }
        if (CheckCollisionPointRec(mouse, btnIncRect)) {
            ResizeGrid(std::min(20, cols + 1));
        }
        if (CheckCollisionPointRec(mouse, btnDecRect)) {
            ResizeGrid(std::max(5, cols - 1));
        }
        if (CheckCollisionPointRec(mouse, btnSaveRect)) {
            // Сохраняем карту
            std::filesystem::create_directory("maps");
            std::string fname = std::string("maps/") + (mapName[0] ? mapName : "unnamed") + ".lvl";
            SaveToFile(fname.c_str());
        }
        // Back button справа от Save
        Rectangle btnBackRect = {btnSaveRect.x + btnSaveRect.width + 20, btnSaveRect.y, 140, btnSaveRect.height};
        if (CheckCollisionPointRec(mouse, btnBackRect)) {
            extern bool g_EditorBackToMenu;
            g_EditorBackToMenu = true;
        }
    }
    if (nameEditActive) {
        int key = GetCharPressed();
        while (key > 0) {
            int len = (int)strlen(mapName);
            if ((key >= 32) && (key <= 125) && len < 31) {
                mapName[len] = (char)key;
                mapName[len+1] = 0;
            }
            key = GetCharPressed();
        }
        if (IsKeyPressed(KEY_BACKSPACE)) {
            int len = (int)strlen(mapName);
            if (len > 0) mapName[len-1] = 0;
        }
    }
}

bool LevelCreator::SaveToFile(const char* filename) const {
    std::ofstream out(filename);
    if (!out) return false;
    out << rows << ' ' << cols << '\n';
    out << mapName << '\n';
    // objectTypes
    for (int i = 0; i < rows * cols; ++i) {
        out << (int)objectTypes[i] << ' ';
    }
    out << '\n';
    // mapCells (ресурсы)
    for (int i = 0; i < rows * cols; ++i) {
        out << (int)mapCells[i].GetType() << ' ' << mapCells[i].GetMass() << ' ';
    }
    out << '\n';
    return true;
}

void LevelCreator::ResizeGrid(int newSize) {
    if (newSize == cols) return;
    cols = rows = newSize;
    cellSize = (1024.0f - marginLeft - 250.0f) / cols;
    mapCells.assign(rows * cols, MapCell());
    objectTypes.assign(rows * cols, EditorObjectType::None);
    grid = Grid(rows, cols, marginLeft, marginTop, cellSize);
    SyncGrid();
    currentX = std::min(currentX, cols-1);
    currentY = std::min(currentY, rows-1);
}
