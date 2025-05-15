#pragma once
#include "raylib.h"
#include <string>
#include <unordered_map>

enum class ResourceType {
    None,
    Tree,
    Coal,
    Stone
};

extern std::unordered_map<ResourceType, float> selectionOvalYOffset;

class MapCell {
public:
    MapCell();
    MapCell(ResourceType type, int mass, Texture* texture);

    void Draw(float x, float y, float size) const;

    ResourceType GetType() const;
    int GetMass() const;
    void Set(ResourceType type, int mass, Texture* texture);

    // --- ��������� ---
    void SetSelected(bool sel) { selected = sel; }
    bool IsSelected() const { return selected; }
    // �������� ����� �� ������
    bool IsClicked(float x, float y, float size) const;

    // ���� ��������� � ������������ �������� �� Y
    static void DrawSelectionOval(float x, float y, float size, float offsetY = 0.0f);

    // ���������� ���������
    static void SetSelectionOvalYOffset(ResourceType type, float offset);

private:
    ResourceType type;
    int mass; // tons
    Texture* texture;
    bool selected = false;
};