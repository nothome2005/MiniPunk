#pragma once
#include <memory>
#include "grid.h"
#include "player.h"
#include "wolf.h"
#include <vector>

class GameState {
public:
    virtual ~GameState() {}
    virtual void Enter() {}
    virtual void Exit() {}
    virtual void Update(float dt) = 0;
};

class WolfTurnState; // forward

class PlayerTurnState : public GameState {
public:
    PlayerTurnState(Player& player, Wolf& wolf, Grid& grid, GameState*& statePtr)
        : player(player), wolf(wolf), grid(grid), statePtr(statePtr), wolfState(nullptr) {}
    void SetWolfState(GameState* ws) { wolfState = ws; }
    void Update(float dt) override {
        player.Update(dt);
        if (player.HasMoved()) {
            wolf.StartTurn(grid, player);
            player.ResetTurn();
            statePtr = wolfState;
        }
    }
private:
    Player& player;
    Wolf& wolf;
    Grid& grid;
    GameState*& statePtr;
    GameState* wolfState;
};

class WolfTurnState : public GameState {
public:
    WolfTurnState(Player& player, Wolf& wolf, Grid& grid, GameState*& statePtr)
        : player(player), wolf(wolf), grid(grid), statePtr(statePtr), playerState(nullptr) {}
    void SetPlayerState(GameState* ps) { playerState = ps; }
    void Update(float dt) override {
        wolf.Update(dt);
        if (wolf.IsTurnFinished()) {
            statePtr = playerState;
        }
    }
private:
    Player& player;
    Wolf& wolf;
    Grid& grid;
    GameState*& statePtr;
    GameState* playerState;
};
