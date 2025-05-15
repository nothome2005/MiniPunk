#pragma once
#include "raylib.h"
#include <vector>

// --- Эффект дыма без текстур (через примитивы) ---
struct SmokeParticle {
    Vector2 pos;
    float radius;
    float alpha;
    float vy;
    float grow;
    float life;
};

// Генератор занимает 4 клетки: (4,4), (4,5), (5,4), (5,5) на 10x10
// Положение dst всегда фиксировано в центре сетки!
// Теперь рисует всю текстуру генератора, подгоняя под 4 клетки
class Generator {
public:
    Generator(float cellSize, float margin);

    void Update();
    void Draw() const;

    // --- Выделение ---
    void SetSelected(bool sel) { selected = sel; }
    bool IsSelected() const { return selected; }
    // Проверка клика по генератору (центральные 4 клетки)
    bool IsClicked(float mouseX, float mouseY) const;

    // Овал выделения для генератора с возможностью смещения по Y
    static void DrawSelectionOval(float x, float y, float width, float height, float offsetY = 0.0f);

private:
    float cellSize, margin;

    // --- Дым ---
    mutable std::vector<SmokeParticle> smokeParticles;
    float smokeTimer = 0.0f;
    void UpdateSmoke();
    void DrawSmoke() const;

    // --- Светлячок (только снизу генератора) ---
    float fireflyTimer = 0.0f;
    float fireflyAlpha = 0.7f;
    float fireflyAlphaDir = 1.0f; // 1.0 - ярче, -1.0 - тусклее
    void UpdateFirefly();
    void DrawFireflyGlow() const;

    // --- Выделение ---
    bool selected = false;
};