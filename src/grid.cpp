#include "grid.h"
#include "raylib.h" 

extern bool g_objectsSelectable;

static const Color DARK_BLUE_CELL = { 20, 30, 60, 255 };

Grid::Grid(int rows, int cols, float marginLeft, float marginTop, float cellSize)
    : rows(rows), cols(cols), marginLeft(marginLeft), marginTop(marginTop), cellSize(cellSize)
{
    buttons.reserve(rows * cols);
    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {
            float bx = marginLeft + x * cellSize;
            float by = marginTop + y * cellSize;
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
    
    static const Color FROSTY_DARK_BLUE_TRANSPARENT = { 30, 50, 80, 120 };
    for (int y = 0; y <= rows; ++y) {
        float yPos = marginTop + y * cellSize;
        DrawLineEx(
            { marginLeft, yPos },
            { marginLeft + cols * cellSize, yPos },
            2.0f, FROSTY_DARK_BLUE_TRANSPARENT
        );
    }
    for (int x = 0; x <= cols; ++x) {
        float xPos = marginLeft + x * cellSize;
        DrawLineEx(
            { xPos, marginTop },
            { xPos, marginTop + rows * cellSize },
            2.0f, FROSTY_DARK_BLUE_TRANSPARENT
        );
    }

    for (size_t i = 0; i < buttons.size(); ++i) {
        int gx = buttons[i].gridX, gy = buttons[i].gridY;
        bool isCenter = (gx == 4 || gx == 5) && (gy == 4 || gy == 5);
        if (i < mapCells.size() && !(generator && isCenter))
            mapCells[i].Draw(buttons[i].rect.x, buttons[i].rect.y, buttons[i].rect.width);
    }
    if (generator) generator->Draw();
}

void Grid::Update() {
    if (generator) generator->Update();

    if (!g_objectsSelectable) return;

    Vector2 mouse = GetMousePosition();

    if (generator && generator->IsClicked(mouse.x, mouse.y)) {
        if (generator->IsSelected()) {
            generator->SetSelected(false);
        } else {
            DeselectAll();
            generator->SetSelected(true);
        }
        return;
    }

    // Обробка натискання на клітинку
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