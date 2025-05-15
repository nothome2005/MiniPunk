#include "grid.h"
#include "raylib.h" // For DrawCircle

Grid::Grid(int rows, int cols, float margin, float cellSize)
    : rows(rows), cols(cols), margin(margin), cellSize(cellSize)
{
    buttons.reserve(rows * cols);
    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {
            float bx = margin + x * cellSize;
            float by = margin + y * cellSize;
            buttons.emplace_back(bx, by, cellSize, x, y);
        }
    }
}

void Grid::Draw() const {
    for (const auto& btn : buttons) btn.Draw();
}

void Grid::Update() {
    for (auto& btn : buttons) {
        if (btn.IsClicked()) {
            TraceLog(LOG_INFO, "Button clicked at grid (%d, %d)", btn.gridX, btn.gridY);
        }
    }
}