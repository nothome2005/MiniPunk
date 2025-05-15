#pragma once
#include "raylib.h"
#include <vector>

// --- ������ ���� ��� ������� (����� ���������) ---
struct SmokeParticle {
    Vector2 pos;
    float radius;
    float alpha;
    float vy;
    float grow;
    float life;
};

// ��������� �������� 4 ������: (4,4), (4,5), (5,4), (5,5) �� 10x10
// ��������� dst ������ ����������� � ������ �����!
// ������ ������ ��� �������� ����������, �������� ��� 4 ������
class Generator {
public:
    Generator(float cellSize, float margin);

    void Update();
    void Draw() const;

    // --- ��������� ---
    void SetSelected(bool sel) { selected = sel; }
    bool IsSelected() const { return selected; }
    // �������� ����� �� ���������� (����������� 4 ������)
    bool IsClicked(float mouseX, float mouseY) const;

    // ���� ��������� ��� ���������� � ������������ �������� �� Y
    static void DrawSelectionOval(float x, float y, float width, float height, float offsetY = 0.0f);

private:
    float cellSize, margin;

    // --- ��� ---
    mutable std::vector<SmokeParticle> smokeParticles;
    float smokeTimer = 0.0f;
    void UpdateSmoke();
    void DrawSmoke() const;

    // --- ��������� (������ ����� ����������) ---
    float fireflyTimer = 0.0f;
    float fireflyAlpha = 0.7f;
    float fireflyAlphaDir = 1.0f; // 1.0 - ����, -1.0 - �������
    void UpdateFirefly();
    void DrawFireflyGlow() const;

    // --- ��������� ---
    bool selected = false;
};