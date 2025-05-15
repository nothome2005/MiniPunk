#include "map_cell.h"
#include <utility>

std::unordered_map<ResourceType, float> selectionOvalYOffset = {
    {ResourceType::Tree, 0.3f},
    {ResourceType::Coal, 0.1f},
    {ResourceType::Stone, 0.25f},
    {ResourceType::None, 0.25f}
};

MapCell::MapCell() : type(ResourceType::None), mass(0), texture(nullptr), selected(false) {}

MapCell::MapCell(ResourceType type, int mass, Texture* texture)
    : type(type), mass(mass), texture(texture), selected(false) {}

void MapCell::Draw(float x, float y, float size) const {
    if (selected) {
        float offset = 0.0f;
        auto it = selectionOvalYOffset.find(type);
        if (it != selectionOvalYOffset.end())
            offset = it->second * size;
        DrawSelectionOval(x, y, size, offset);
    }
    if (type != ResourceType::None && texture) {
        float yOffset = 0.0f;
        if (type == ResourceType::Tree) {
            yOffset = -size * 0.12f; // Поднять дерево вверх на 12% высоты клетки
        }
        DrawTexturePro(
            *texture,
            Rectangle{0, 0, (float)texture->width, (float)texture->height},
            Rectangle{x, y + yOffset, size, size},
            Vector2{0, 0},
            0.0f,
            WHITE
        );
    }
}

ResourceType MapCell::GetType() const { return type; }
int MapCell::GetMass() const { return mass; }
void MapCell::Set(ResourceType t, int m, Texture* tex) {
    type = t;
    mass = m;
    texture = tex;
    selected = false;
}

bool MapCell::IsClicked(float x, float y, float size) const {
    Vector2 mouse = GetMousePosition();
    return CheckCollisionPointRec(mouse, Rectangle{x, y, size, size}) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

void MapCell::DrawSelectionOval(float x, float y, float size, float offsetY) {
    float cx = x + size / 2.0f;
    float cy = y + size / 2.0f + offsetY;
    float rx = size / 2.0f;
    float ry = size / 3.0f / 2.0f;
    int thick = 6;
    for (int i = 0; i < thick; ++i) {
        DrawEllipseLines((int)cx, (int)cy, rx - i, ry - i * 0.7f, RED);
        DrawEllipseLines((int)cx, (int)cy, rx + i, ry + i * 0.7f, RED);
    }
}

void MapCell::SetSelectionOvalYOffset(ResourceType type, float offset) {
    selectionOvalYOffset[type] = offset;
}