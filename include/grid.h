#pragma once
#include "button.h"
#include "map_cell.h"
#include "generator.h"
#include <vector>

class Grid {
public:
    // Исправленный конструктор: принимает marginLeft, marginTop, cellSize
    Grid(int rows, int cols, float marginLeft, float marginTop, float cellSize);

    void Draw() const;
    void Update();

    void SetMapCells(const std::vector<MapCell>& mapCells);

    // Генератор
    void SetGenerator(Generator* gen);

    // --- Новый публичный доступ к mapCells и generator ---
    const std::vector<MapCell>& GetMapCells() const { return mapCells; }
    Generator* GetGenerator() const { return generator; }

    // --- Сделать DeselectAll публичным ---
    void DeselectAll();

private:
    int rows, cols;
    float marginLeft, marginTop, cellSize;
    std::vector<Button> buttons;
    std::vector<MapCell> mapCells;
    Generator* generator = nullptr;
};