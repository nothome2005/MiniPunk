#pragma once
#include "button.h"
#include <vector>

class Grid {
public:
    Grid(int rows, int cols, float margin, float cellSize);

    void Draw() const;
    void Update();

private:
    int rows, cols;
    float margin, cellSize;
    std::vector<Button> buttons;
};