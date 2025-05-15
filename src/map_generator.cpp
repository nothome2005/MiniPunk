#include "map_generator.h"
#include "resource_manager.h"
#include <random>
#include <algorithm>
#include <vector>

namespace {
// Проверка, что позиция (x, y) не занята и вокруг нее нет ресурсов
bool CanPlace(const std::vector<MapCell>& cells, int rows, int cols, int x, int y) {
    // Центр карты (4,4), (4,5), (5,4), (5,5) для 10x10
    if ((x == 4 || x == 5) && (y == 4 || y == 5))
        return false;

    // Проверяем все клетки вокруг (включая саму)
    for (int dy = -1; dy <= 1; ++dy) {
        for (int dx = -1; dx <= 1; ++dx) {
            int nx = x + dx;
            int ny = y + dy;
            if (nx < 0 || ny < 0 || nx >= cols || ny >= rows)
                continue;
            if (cells[ny * cols + nx].GetType() != ResourceType::None)
                return false;
        }
    }
    return true;
}
}

std::vector<MapCell> MapGenerator::Generate(int rows, int cols, int treeCount, int coalCount, int stoneCount, int massPerCell) {
    int total = rows * cols;
    std::vector<MapCell> cells(total);

    // Список всех позиций, кроме центральных 4 клеток
    std::vector<std::pair<int, int>> positions;
    for (int y = 0; y < rows; ++y)
        for (int x = 0; x < cols; ++x)
            if (!((x == 4 || x == 5) && (y == 4 || y == 5)))
                positions.emplace_back(x, y);

    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(positions.begin(), positions.end(), g);

    auto try_place = [&](ResourceType type, int count, Texture* tex) {
        int placed = 0;
        for (size_t i = 0; i < positions.size() && placed < count; ++i) {
            int x = positions[i].first;
            int y = positions[i].second;
            if (CanPlace(cells, rows, cols, x, y)) {
                cells[y * cols + x].Set(type, massPerCell, tex);
                placed++;
            }
        }
        // Можно добавить предупреждение, если placed < count
    };

    try_place(ResourceType::Tree, treeCount, &tree_);
    try_place(ResourceType::Coal, coalCount, &coal_);
    try_place(ResourceType::Stone, stoneCount, &iron_);

    return cells;
}