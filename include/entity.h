#pragma once
#include <raylib.h>

class Entity {
public:
    virtual void Update(float dt) = 0;
    virtual void Draw(float cellSize, float marginLeft, float marginTop) const = 0;
    virtual Vector2 GetPosition() const = 0;
    virtual ~Entity() {}
};
