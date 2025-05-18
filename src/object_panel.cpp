#include "object_panel.h"
#include "resource_manager.h"
#include "raylib.h"
#include <string>
#include <unordered_map>

static std::string GetResourceName(ResourceType type) {
    switch (type) {
        case ResourceType::Tree: return "Tree";
        case ResourceType::Coal: return "Coal";
        case ResourceType::Stone: return "Metal";
        default: return "Unknown";
    }
}

// Словник для індивідуального зміщення тексту панелі по X
static std::unordered_map<ResourceType, float> objectPanelTextOffsetX = {
    {ResourceType::Tree, -50.0f},
    {ResourceType::Coal, -40.0f},
    {ResourceType::Stone, -50.0f},
    {ResourceType::None, -60.0f} // Можна використати для генератора
};

// Отримати текстуру ресурсу за типом
static Texture* GetResourceTexture(ResourceType type) {
    switch (type) {
        case ResourceType::Tree: return &tree_;
        case ResourceType::Coal: return &coal_;
        case ResourceType::Stone: return &iron_;
        default: return nullptr;
    }
}

void DrawObjectPanel(float x, float y, const Grid& grid) {
    DrawTexture(ObjectCanva_, (int)x, (int)y, WHITE);

    const MapCell* selectedCell = nullptr;
    for (const auto& cell : grid.GetMapCells()) {
        if (cell.IsSelected()) {
            selectedCell = &cell;
            break;
        }
    }
    const Generator* selectedGen = (grid.GetGenerator() && grid.GetGenerator()->IsSelected()) ? grid.GetGenerator() : nullptr;

    int panelCenterX = (int)x + ObjectCanva_.width / 2 - 25;
    int panelCenterY = (int)y + ObjectCanva_.height / 3;

    float textX = 0, textY = 0;
    std::string name;
    int fontSize = 28;
    Vector2 textSize = {0, 0};
    bool drawName = false;

    if (selectedCell && selectedCell->GetType() != ResourceType::None) {
        Texture* tex = GetResourceTexture(selectedCell->GetType());
        float scale = 0.125f;
        int iconW = tex && tex->id != 0 ? (int)(tex->width * scale) : 0;
        int iconH = tex && tex->id != 0 ? (int)(tex->height * scale) : 0;
        float iconY = (float)(panelCenterY - iconH / 2 - 20);
        name = GetResourceName(selectedCell->GetType());
        textSize = MeasureTextEx(objectPanelFont, name.c_str(), fontSize, 2);
        float offsetX = objectPanelTextOffsetX[selectedCell->GetType()];
        textX = (float)(panelCenterX - textSize.x / 2) + offsetX;
        textY = iconY - textSize.y - 8;
        if (tex && tex->id != 0) {
            DrawTextureEx(*tex, {(float)(panelCenterX - iconW / 2), iconY}, 0, scale, WHITE);
        }
        drawName = true;
    } else if (selectedGen) {
        float scale = 0.12f;
        int iconW = (int)(generator_.width * scale);
        int iconH = (int)(generator_.height * scale);
        float iconY = (float)(panelCenterY - iconH / 2 - 20);
        name = "Generator";
        textSize = MeasureTextEx(objectPanelFont, name.c_str(), fontSize, 2);
        float offsetX = objectPanelTextOffsetX[ResourceType::None]; // Для генератора
        textX = (float)(panelCenterX - textSize.x / 2) + offsetX;
        textY = iconY - textSize.y - 8;
        if (generator_.id != 0) {
            DrawTextureEx(generator_, {(float)(panelCenterX - iconW / 2), iconY}, 0, scale, WHITE);
        }
        drawName = true;
    }

    std::string testStr;
    ResourceType testType = ResourceType::None;
    if (selectedCell && selectedCell->GetType() != ResourceType::None) {
        testStr += " ";
        testStr += GetResourceName(selectedCell->GetType());
        testType = selectedCell->GetType();
    } else if (selectedGen) {
        testStr += " Generator";
        testType = ResourceType::None;
    }
    int testFontSize = 18;
    Vector2 testTextSize = MeasureTextEx(objectPanelFont, testStr.c_str(), testFontSize, 2);
    float testOffsetX = objectPanelTextOffsetX[testType];
    int testTextX = (int)x + (ObjectCanva_.width - (int)testTextSize.x) / 2 + (int)testOffsetX;
    int testTextY = 300;
    DrawText(testStr.c_str(), testTextX, testTextY, testFontSize, WHITE);
}