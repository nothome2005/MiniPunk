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

// Словник для індивідуального зміщення координат тексту імені об'єкта
static std::unordered_map<ResourceType, Vector2> objectNameTextOffset = {
    {ResourceType::Tree, {0, 0}},
    {ResourceType::Coal, {0, -300}},
    {ResourceType::None, {0, 0}}
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

    float textX = 0, textY = 0, extraX = 0, extraY = 0;
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
        Vector2 offset = objectNameTextOffset[selectedCell->GetType()];
        textX = (float)(panelCenterX - textSize.x / 2) + offset.x;
        textY = iconY - textSize.y - 8 + offset.y;
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
        // Для генератора можна додати окреме зміщення, якщо потрібно
        Vector2 offset = {0, 0};
        textX = (float)(panelCenterX - textSize.x / 2) + offset.x;
        textY = iconY - textSize.y - 8 + offset.y;
        if (generator_.id != 0) {
            DrawTextureEx(generator_, {(float)(panelCenterX - iconW / 2), iconY}, 0, scale, WHITE);
        }
        drawName = true;
    }

    // Текст малюємо в самому кінці, поверх усіх елементів
    if (drawName && objectPanelFont.texture.id != 0) {
        DrawTextEx(objectPanelFont, name.c_str(), {textX, textY}, fontSize, 2, RED);
        // Для відладки: вивід координат textX/textY
        char coordBuf[64];
        snprintf(coordBuf, sizeof(coordBuf), "x:%.0f y:%.0f", textX, textY);
        DrawText(coordBuf, (int)x + 10, (int)y + 10, 16, YELLOW);
    }
    // Для відладки: малюємо "Test Text <object name>" по центру панелі білим кольором і меншим розміром
    std::string testStr;
    if (selectedCell && selectedCell->GetType() != ResourceType::None) {
        testStr += " ";
        testStr += GetResourceName(selectedCell->GetType());
    } else if (selectedGen) {
        testStr += " Generator";
        extraX = 30.0f;
    }
    int testFontSize = 18;
    Vector2 testTextSize = MeasureTextEx(GetFontDefault(), testStr.c_str(), testFontSize, 2);
    int testTextX = (int)x + ObjectCanva_.width / 2 - (int)(testTextSize.x - extraX);
    int testTextY = (int)y + ObjectCanva_.height / 2 - 50.0f;
    DrawText(testStr.c_str(), testTextX, testTextY, testFontSize, WHITE);
    if (objectPanelFont.texture.id == 0) {
        DrawText("NO FONT!", 10, 40, 30, RED);
    }
}