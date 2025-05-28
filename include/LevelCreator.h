#pragma once
#include "grid.h"
#include <vector>
#include "raylib.h"
#include "resource_manager.h"
#include "map_cell.h"
#include <string>
#include <fstream>
#include <filesystem>
#include "button.h"

// Дополнительные типы для редактора
enum class EditorObjectType {
    None,
    Tree,
    Coal,
    Stone,
    Player,
    Wolf,
    Generator
};

extern bool g_EditorBackToMenu;

class LevelCreator {
public:
    LevelCreator();
    void Update();
    void Draw() const;
    // --- Новое: сохранение ---
    bool SaveToFile(const char* filename) const;
    int GetRows() const { return rows; }
    int GetCols() const { return cols; }
    const char* GetMapName() const { return mapName; }
    const std::vector<MapCell>& GetMapCells() const { return mapCells; }
    const std::vector<EditorObjectType>& GetObjectTypes() const { return objectTypes; }
private:
    int currentX, currentY;
    int rows, cols;
    float marginLeft, marginTop, cellSize;
    void PlaceResource(int key);
    std::vector<MapCell> mapCells;
    std::vector<EditorObjectType> objectTypes;
    Grid grid;
    void SyncGrid();
    // --- UI ---
    char mapName[32] = "";
    bool nameEditActive = false;
    Rectangle nameEditRect = { 700, 160, 220, 40 };
    std::unique_ptr<Button> btnInc, btnDec, btnSave, btnBack;
    void DrawUI() const;
    void HandleUI();
    void ResizeGrid(int newSize);
};
