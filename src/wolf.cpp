#include "wolf.h"
#include "resource_manager.h"
#include <queue>

// --- WolfMoveStrategy реализация ---
std::pair<int, int> WolfMoveStrategy::GetNextMove(const Grid& grid, const Player& player, int x, int y) {
    int px = (int)player.GetPosition().x;
    int py = (int)player.GetPosition().y;
    int gridW = grid.GetCols();
    int gridH = grid.GetRows();
    const auto& cells = grid.GetMapCells();
    // Y приоритет
    if (y < py) {
        int ny = y + 1;
        if (ny < gridH && cells[ny * gridW + x].GetType() == ResourceType::None && !((x == 4 || x == 5) && (ny == 4 || ny == 5)))
            return {0, 1};
    } else if (y > py) {
        int ny = y - 1;
        if (ny >= 0 && cells[ny * gridW + x].GetType() == ResourceType::None && !((x == 4 || x == 5) && (ny == 4 || ny == 5)))
            return {0, -1};
    }
    // X приоритет
    if (x < px) {
        int nx = x + 1;
        if (nx < gridW && cells[y * gridW + nx].GetType() == ResourceType::None && !((nx == 4 || nx == 5) && (y == 4 || y == 5)))
            return {1, 0};
    } else if (x > px) {
        int nx = x - 1;
        if (nx >= 0 && cells[y * gridW + nx].GetType() == ResourceType::None && !((nx == 4 || nx == 5) && (y == 4 || y == 5)))
            return {-1, 0};
    }
    return {0, 0};
}

Wolf::Wolf(int x, int y, IMoveStrategy* strategy)
    : x(x), y(y), stepsPlanned(0), stepIndex(0), moveTimer(0), turnFinished(true), moveStrategy(strategy) {}

void Wolf::StartTurn(const Grid& grid, const Player& player) {
    PlanSteps(grid, player);
    stepIndex = 0;
    moveTimer = 0;
    turnFinished = false;
    if (stepsPlanned == 0) {
        turnFinished = true;
    }
}

void Wolf::Update(float dt) {
    if (turnFinished) return;
    moveTimer += dt;
    if (moveTimer >= 0.5f && stepIndex < stepsPlanned) {
        x += dxQueue[stepIndex];
        y += dyQueue[stepIndex];
        moveTimer = 0;
        ++stepIndex;
        if (stepIndex >= stepsPlanned) {
            turnFinished = true;
        }
    }
}

bool Wolf::IsTurnFinished() const {
    return turnFinished;
}

void Wolf::PlanSteps(const Grid& grid, const Player& player) {
    int cx = x, cy = y;
    stepsPlanned = 0;
    for (int s = 0; s < 2; ++s) {
        auto move = moveStrategy->GetNextMove(grid, player, cx, cy);
        int dx = move.first, dy = move.second;
        if (dx != 0 || dy != 0) {
            dxQueue[s] = dx;
            dyQueue[s] = dy;
            cx += dx;
            cy += dy;
            ++stepsPlanned;
        } else {
            break;
        }
    }
}

void Wolf::Draw(float cellSize, float marginLeft, float marginTop) const {
    float px = marginLeft + x * cellSize;
    float py = marginTop + y * cellSize;
    float scaleX = cellSize / (float)wolf_texture.width;
    float scaleY = cellSize / (float)wolf_texture.height;
    float scale = scaleX < scaleY ? scaleX : scaleY;
    float texW = wolf_texture.width * scale;
    float texH = wolf_texture.height * scale;
    float offsetX = px + (cellSize - texW) / 2.0f;
    float offsetY = py + (cellSize - texH);
    DrawTextureEx(wolf_texture, {offsetX, offsetY}, 0, scale, WHITE);
}

Vector2 Wolf::GetPosition() const {
    return {(float)x, (float)y};
}
