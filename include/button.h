#pragma once
#include "raylib.h"

class Button {
public:
    Button(float x, float y, float size, int gridX, int gridY);

    void Draw() const;
    // Returns true if the button is clicked this frame
    bool IsClicked() const;

    // Expose rect for debug/visualization
    Rectangle rect;

    // Grid coordinates
    int gridX;
    int gridY;
};