#include "generator.h"
#include "resource_manager.h"
#include "raylib.h"
#include <algorithm>
#include <random>

float generatorSelectionOvalYOffset = 0.5f;

Generator::Generator(float cellSize, float marginLeft, float marginTop)
    : cellSize(cellSize), marginLeft(marginLeft), marginTop(marginTop)
{}

void Generator::Update() {
    UpdateSmoke();
    UpdateFirefly();
}

void Generator::UpdateSmoke() {
    smokeTimer += GetFrameTime();
    if (smokeTimer > 0.1f) {
        smokeTimer -= 0.1f;
        float x = marginLeft + (4.0f + 1.0f) * cellSize;
        float y = marginTop + 4.0f * cellSize - cellSize * 0.1f;
        float dx = ((float)GetRandomValue(-10, 10)) / 100.0f * cellSize;
        SmokeParticle p;
        p.pos = { x + dx, y };
        p.radius = cellSize * 0.18f + ((float)GetRandomValue(0, 8)) / 100.0f * cellSize;
        p.alpha = 0.5f + ((float)GetRandomValue(0, 30)) / 100.0f;
        p.vy = -cellSize * (0.25f + ((float)GetRandomValue(0, 10)) / 100.0f);
        p.grow = cellSize * 0.08f;
        p.life = 1.0f;
        smokeParticles.push_back(p);
    }
    for (auto& p : smokeParticles) {
        p.pos.y += p.vy * GetFrameTime();
        p.radius += p.grow * GetFrameTime();
        p.alpha -= 0.5f * GetFrameTime();
        p.life -= GetFrameTime();
    }
    smokeParticles.erase(
        std::remove_if(smokeParticles.begin(), smokeParticles.end(),
            [](const SmokeParticle& p) { return p.alpha <= 0.0f || p.life <= 0.0f; }),
        smokeParticles.end()
    );
}

void Generator::UpdateFirefly() {
    float speed = 0.5f;
    fireflyAlpha += fireflyAlphaDir * speed * GetFrameTime();
    if (fireflyAlpha > 0.7f) {
        fireflyAlpha = 0.7f;
        fireflyAlphaDir = -1.0f;
    }
    if (fireflyAlpha < 0.3f) {
        fireflyAlpha = 0.3f;
        fireflyAlphaDir = 1.0f;
    }
}

void Generator::Draw() const {

    DrawSmoke();
    DrawFireflyGlow();

    // --- ������� ���� �������� ���������� ---
    if (selected) {
        // ���� ��������� �� ����������� � ����� ���� (4,5)-(5,5)
        float x = marginLeft + 4 * cellSize;
        float y = marginTop + 5 * cellSize;
        float w = 2 * cellSize;
        float h = (2 * cellSize) / 3.0f;
        float offsetY = generatorSelectionOvalYOffset * h;
        DrawSelectionOval(x, y, w, h, offsetY);
    }

    // ������� �������� ����������, ����� 4 ������� (2x2)
    Rectangle src = {
        0.0f, 0.0f,
        (float)generator_.width,
        (float)generator_.height
    };
    Rectangle dst = {
        marginLeft + 4 * cellSize,
        marginTop + 4 * cellSize,
        2 * cellSize,
        2 * cellSize
    };
    DrawTexturePro(generator_, src, dst, Vector2{0,0}, 0.0f, WHITE);
}

void Generator::DrawSmoke() const {
    for (const auto& p : smokeParticles) {
        Color c = { 120, 120, 120, (unsigned char)(p.alpha * 255) };
        DrawCircleV(p.pos, p.radius, c);
    }
}

void Generator::DrawFireflyGlow() const {
    // Glow ������� ����������: ����� ���� ������� �����
    float x = marginLeft + (4.0f + 1.0f) * cellSize;
    float y = marginTop + 4.0f * cellSize + cellSize * 0.25f;
    float r = cellSize * 0.18f;

    
    const int layers = 3;
    float baseAlpha = fireflyAlpha;
    for (int i = layers - 1; i >= 0; --i) {
        float k = (float)i / (float)(layers - 1);
        float radius = r * (1.5f + k * 1.5f);
        float alpha = baseAlpha * (0.10f + 0.25f * (1.0f - k));
        Color c = { 255, 160, 40, (unsigned char)(alpha * 255) };
        DrawCircleV({x, y}, radius, c);
    }
}

bool Generator::IsClicked(float mouseX, float mouseY) const {
    float x = marginLeft + 4 * cellSize;
    float y = marginTop + 4 * cellSize;
    Rectangle rect = { x, y, 2 * cellSize, 2 * cellSize };
    return CheckCollisionPointRec({mouseX, mouseY}, rect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

void Generator::DrawSelectionOval(float x, float y, float width, float height, float offsetY) {
    float cx = x + width / 2.0f;
    float cy = y + height / 2.0f + offsetY;
    float rx = width / 2.0f;
    float ry = height / 2.0f;
    int thick = 6;
    for (int i = 0; i < thick; ++i) {
        DrawEllipseLines((int)cx, (int)cy, rx - i, ry - i * 0.7f, RED);
        DrawEllipseLines((int)cx, (int)cy, rx + i, ry + i * 0.7f, RED);
    }
}

void Generator::SetSelectionOvalYOffset(float offset) {
    generatorSelectionOvalYOffset = offset;
}