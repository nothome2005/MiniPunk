#pragma once
#include "entity.h"

class Player : public Entity {
public:
    Player(int x, int y);
    void Move(int dx, int dy, int gridWidth, int gridHeight);
    void SetPosition(int x, int y);
    Vector2 GetPosition() const override;
    void ResetTurn();
    void SetMoved(bool moved);
    bool HasMoved() const;
    void Update(float dt) override; // теперь принимает dt
    void Draw(float cellSize, float marginLeft, float marginTop) const override;

private:
    int x, y;
    bool moved;
    int gridWidth, gridHeight;
};
