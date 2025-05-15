#include "button.h"
#include <string>

Button::Button(float x, float y, float size, int gridX, int gridY)
    : rect{ x, y, size, size }, gridX(gridX), gridY(gridY)
{}

void Button::Draw() const {
    // Transparent fill, only border
    DrawRectangleLinesEx(rect, 2, BLACK);
    // No text or indicators
}

bool Button::IsClicked() const {
    return CheckCollisionPointRec(GetMousePosition(), rect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}