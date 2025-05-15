#include "resource_manager.h"

// Определение глобальных переменных
Texture g_background = { 0 };
Texture coal_ = { 0 };
Texture iron_ = { 0 };
Texture tree_ = { 0 };
Texture generator_ = { 0 };


void LoadResources()
{
    g_background = LoadTexture("background.png");
    coal_ = LoadTexture("coal.png");
    iron_ = LoadTexture("iron.png");
    tree_ = LoadTexture("tree.png");
    generator_ = LoadTexture("generator3.png");
}

void UnloadResources()
{
    UnloadTexture(g_background);
    UnloadTexture(coal_);
    UnloadTexture(iron_);
    UnloadTexture(tree_);
    UnloadTexture(generator_);
}