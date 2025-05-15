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
    mapCells.resize(rows * cols);
}

void Grid::SetMapCells(const std::vector<MapCell>& cells) {
    mapCells = cells;
}

void Grid::SetGenerator(Generator* gen) {
    generator = gen;
}

void Grid::DeselectAll() {
    for (auto& cell : mapCells) cell.SetSelected(false);
    if (generator) generator->SetSelected(false);
}

void Grid::Draw() const {
    for (size_t i = 0; i < buttons.size(); ++i) {
        int gx = buttons[i].gridX, gy = buttons[i].gridY;
        bool isCenter = (gx == 4 || gx == 5) && (gy == 4 || gy == 5);
        if (i < mapCells.size() && !(generator && isCenter))
            mapCells[i].Draw(buttons[i].rect.x, buttons[i].rect.y, buttons[i].rect.width);
        buttons[i].Draw();
    }
    if (generator) generator->Draw();
}

void Grid::Update() {
    if (generator) generator->Update();

    Vector2 mouse = GetMousePosition();

    // Проверка генератора
    if (generator && generator->IsClicked(mouse.x, mouse.y)) {
        if (generator->IsSelected()) {
            generator->SetSelected(false);
        } else {
            DeselectAll();
            generator->SetSelected(true);
        }
        return;
    }

    // Проверка по клеткам
    for (size_t i = 0; i < buttons.size(); ++i) {
        if (i < mapCells.size() && mapCells[i].GetType() != ResourceType::None) {
            float x = buttons[i].rect.x;
            float y = buttons[i].rect.y;
            float size = buttons[i].rect.width;
            if (mapCells[i].IsClicked(x, y, size)) {
                if (mapCells[i].IsSelected()) {
                    mapCells[i].SetSelected(false);
                } else {
                    DeselectAll();
                    mapCells[i].SetSelected(true);
                }
                return;
            }
        }
    }
}